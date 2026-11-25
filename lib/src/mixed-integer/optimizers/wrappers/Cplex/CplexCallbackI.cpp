#include "idol/mixed-integer/optimizers/wrappers/Cplex/CplexCallbackI.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include <algorithm>

using idol::CplexCallbackI;

CplexCallbackI::CplexCallbackI(Optimizers::Cplex& parent) : m_parent(parent) {}
void CplexCallbackI::add_callback(Callback* callback) { m_callbacks.emplace_back(callback); }
void CplexCallbackI::set_context(void* data, int where, CallbackEvent event, const double* solution) {
    m_cbdata = data; m_wherefrom = where; m_event = event; m_callback_solution = solution;
}
void CplexCallbackI::clear_context() { m_cbdata = nullptr; m_wherefrom = 0; m_callback_solution = nullptr; }
void CplexCallbackI::call(CallbackEvent event) { m_event = event; for (auto& cb : m_callbacks) execute(*cb, event); }

void CplexCallbackI::register_callbacks() {
    auto& lib = Optimizers::Cplex::get_dynamic_lib();
    m_terminate = 0;
    m_parent.check(lib.CPXsetintparam(m_parent.m_env, idol_CPXPARAM_MIP_Strategy_Search, idol_CPX_MIPSEARCH_TRADITIONAL));
    m_parent.check(lib.CPXsetusercutcallbackfunc(m_parent.m_env, user_cut, this));
    m_parent.check(lib.CPXsetlazyconstraintcallbackfunc(m_parent.m_env, lazy_cut, this));
    m_parent.check(lib.CPXsetheuristiccallbackfunc(m_parent.m_env, heuristic, this));
    m_parent.check(lib.CPXsetinfocallbackfunc(m_parent.m_env, info, this));
    m_parent.check(lib.CPXsetterminate(m_parent.m_env, &m_terminate));
}
void CplexCallbackI::rethrow_exception() { if (m_exception) std::rethrow_exception(m_exception); }

int CplexCallbackI::user_cut(CPXCENVptr, void* data, int where, void* handle, int* action) {
    auto& self = *static_cast<CplexCallbackI*>(handle);
    try { self.set_context(data, where, InvalidSolution); self.call(InvalidSolution); self.clear_context(); }
    catch (...) { self.m_exception = std::current_exception(); return 1; }
    *action = self.m_added_cut ? idol_CPX_CALLBACK_SET : idol_CPX_CALLBACK_DEFAULT; self.m_added_cut = false; return 0;
}
int CplexCallbackI::lazy_cut(CPXCENVptr, void* data, int where, void* handle, int* action) {
    auto& self = *static_cast<CplexCallbackI*>(handle);
    try { self.set_context(data, where, IncumbentSolution); self.call(IncumbentSolution); self.clear_context(); }
    catch (...) { self.m_exception = std::current_exception(); return 1; }
    *action = self.m_added_cut ? idol_CPX_CALLBACK_SET : idol_CPX_CALLBACK_DEFAULT; self.m_added_cut = false; return 0;
}
int CplexCallbackI::heuristic(CPXCENVptr, void*, int, void* handle, double* obj, double* x, int* check, int* action) {
    auto& self = *static_cast<CplexCallbackI*>(handle);
    if (!self.m_heuristic_solution) { *action = idol_CPX_CALLBACK_DEFAULT; return 0; }
    const auto& point = *self.m_heuristic_solution;
    for (const auto& var : self.m_parent.parent().vars()) x[self.m_parent.lazy(var).impl()] = point.get(var);
    *obj = point.objective_value(); *check = 1; *action = idol_CPX_CALLBACK_SET; self.m_heuristic_solution.reset(); return 0;
}
int CplexCallbackI::info(CPXCENVptr env, void* data, int where, void* handle) {
    auto& self = *static_cast<CplexCallbackI*>(handle); auto& lib = Optimizers::Cplex::get_dynamic_lib(); long long nodes = 0;
    lib.CPXgetcallbackinfo(env, data, where, idol_CPX_CALLBACK_INFO_BEST_INTEGER, &self.m_best_obj);
    lib.CPXgetcallbackinfo(env, data, where, idol_CPX_CALLBACK_INFO_BEST_REMAINING, &self.m_best_bound);
    lib.CPXgetcallbackinfo(env, data, where, idol_CPX_CALLBACK_INFO_NODE_COUNT_LONG, &nodes);
    self.m_node_count = static_cast<unsigned int>(nodes); return 0;
}

const idol::Model& CplexCallbackI::original_model() const { return m_parent.parent(); }
void CplexCallbackI::add_cut(const TempCtr& cut, bool lazy) {
    if (!m_cbdata) throw Exception("CPLEX cut submission is not available outside a cut callback.");
    if (lazy && m_event != IncumbentSolution) throw Exception("Cannot add a lazy cut at a fractional point.");
    if (!lazy && m_event != InvalidSolution) throw Exception("Cannot add a user cut at an integer point.");
    std::vector<int> indices; std::vector<double> values;
    for (const auto& [var, value] : cut.lhs()) { indices.push_back(m_parent.lazy(var).impl()); values.push_back(value); }
    const char sense = Optimizers::Cplex::cplex_ctr_type(cut.type());
    m_parent.check(Optimizers::Cplex::get_dynamic_lib().CPXcutcallbackadd(
        m_parent.m_env, m_cbdata, m_wherefrom, static_cast<int>(indices.size()), cut.rhs(), sense,
        indices.data(), values.data(), idol_CPX_USECUT_FORCE));
    m_added_cut = true;
}
void CplexCallbackI::add_user_cut(const TempCtr& cut) { add_cut(cut, false); }
void CplexCallbackI::add_lazy_cut(const TempCtr& cut) { add_cut(cut, true); }
void CplexCallbackI::submit_heuristic_solution(PrimalPoint point) { m_heuristic_solution = std::move(point); }
idol::PrimalPoint CplexCallbackI::primal_solution() const {
    PrimalPoint result; result.set_status(Feasible); result.set_reason(NotSpecified);
    std::vector<double> values(m_parent.parent().vars().size());
    if (m_callback_solution) std::copy_n(m_callback_solution, values.size(), values.begin());
    else m_parent.check(Optimizers::Cplex::get_dynamic_lib().CPXgetcallbacknodex(
        m_parent.m_env, m_cbdata, m_wherefrom, values.data(), 0, static_cast<int>(values.size()) - 1));
    for (const auto& var : m_parent.parent().vars()) result.set(var, values[m_parent.lazy(var).impl()]);
    return result;
}
const idol::Timer& CplexCallbackI::time() const { return m_parent.time(); }
double CplexCallbackI::best_obj() const { return m_best_obj; }
double CplexCallbackI::best_bound() const { return m_best_bound; }
unsigned int CplexCallbackI::node_count() const { return m_node_count; }
void CplexCallbackI::terminate() { m_terminate = 1; }
