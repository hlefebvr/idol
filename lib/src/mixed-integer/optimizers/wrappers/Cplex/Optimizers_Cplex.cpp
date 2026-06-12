//
// Created by Henri on 12/06/2026.
//

#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/general/utils/exceptions/Exception.h"

#include <dlfcn.h>
#include <filesystem>
#include <regex>

#define CPLEX_ERR_BUF_SIZE 512

#define CPLEX_CATCH(env, cmd)                                           \
{                                                                       \
    int _err = (lib.cmd);                                               \
    if (_err) {                                                         \
        char _buf[CPLEX_ERR_BUF_SIZE];                                  \
        lib.CPXgeterrorstring(env, _err, _buf);                         \
        throw Exception(std::string("CPLEX error [") + std::to_string(_err) + "]: " + _buf); \
    }                                                                   \
}

#define CPLEX_SYM_LOAD(name) {                                          \
    name = (idol_##name##_t) dlsym(m_handle, #name);                    \
    const char* _err = dlerror();                                        \
    if (_err)                                                            \
        throw std::runtime_error(std::string("Missing CPLEX symbol ") + #name + ": " + _err); \
}

std::unique_ptr<idol::Optimizers::Cplex::DynamicLib> idol::Optimizers::Cplex::m_dynamic_lib;

// ---------------------------------------------------------------------------
// DynamicLib
// ---------------------------------------------------------------------------

std::string idol::Optimizers::Cplex::DynamicLib::find_library() {

    namespace fs = std::filesystem;

    if (const char* env = std::getenv("IDOL_CPLEX_PATH")) {
        std::string path(env);
        if (fs::exists(path)) {
            return path;
        }
        std::cerr << "WARNING: IDOL_CPLEX_PATH is set, but the file does not exist: " << path << std::endl;
    }

    std::vector<fs::path> candidates;
    if (const char* env = std::getenv("CPLEX_STUDIO_DIR")) {
        const fs::path home(env);
        std::vector<fs::path> subdirs = {
            home / "cplex" / "bin" / "arm64_osx",
            home / "cplex" / "bin" / "x86-64_linux",
            home / "cplex" / "bin" / "x86-64_osx",
            home / "cplex" / "lib" / "arm64_osx" / "static_pic",
            home / "cplex" / "lib" / "x86-64_linux" / "static_pic",
        };

        for (const auto& lib_dir : subdirs) {
            if (!fs::exists(lib_dir) || !fs::is_directory(lib_dir)) {
                continue;
            }

#if defined(_WIN32)
            std::regex pattern(R"(cplex\d+\.dll)", std::regex::icase);
#elif defined(__APPLE__)
            std::regex pattern(R"(libcplex\d+\.dylib)", std::regex::icase);
#else
            std::regex pattern(R"(libcplex\d+\.so(\.\d+)*)", std::regex::icase);
#endif

            for (const auto& entry : fs::directory_iterator(lib_dir)) {
                if (!entry.is_regular_file()) continue;
                const auto fname = entry.path().filename().string();
                if (std::regex_match(fname, pattern)) {
                    candidates.push_back(entry.path());
                }
            }
        }
    }

    if (candidates.empty()) {
        return "";
    }

    return *std::max_element(candidates.begin(), candidates.end(),
        [](const fs::path& a, const fs::path& b) {
            std::regex re(R"(\d+)");
            std::smatch ma, mb;
            std::string sa = a.filename().string();
            std::string sb = b.filename().string();
            std::regex_search(sa, ma, re);
            std::regex_search(sb, mb, re);
            return std::stoi(ma.str()) < std::stoi(mb.str());
        });
}

idol::Optimizers::Cplex::DynamicLib::DynamicLib() {

    const auto cplex_path = find_library();
    if (cplex_path.empty()) {
        return;
    }

    dlerror();
    m_handle = dlopen(cplex_path.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    if (!m_handle) {
        std::cerr << "WARNING: dlopen failed for CPLEX: " << dlerror() << std::endl;
        return;
    }

    CPLEX_SYM_LOAD(CPXopenCPLEX);
    CPLEX_SYM_LOAD(CPXcloseCPLEX);
    CPLEX_SYM_LOAD(CPXcreateprob);
    CPLEX_SYM_LOAD(CPXfreeprob);
    CPLEX_SYM_LOAD(CPXaddcols);
    CPLEX_SYM_LOAD(CPXaddrows);
    CPLEX_SYM_LOAD(CPXchgbds);
    CPLEX_SYM_LOAD(CPXchgctype);
    CPLEX_SYM_LOAD(CPXchgobj);
    CPLEX_SYM_LOAD(CPXchgobjoffset);
    CPLEX_SYM_LOAD(CPXchgobjsen);
    CPLEX_SYM_LOAD(CPXchgrhs);
    CPLEX_SYM_LOAD(CPXchgsense);
    CPLEX_SYM_LOAD(CPXchgcoef);
    CPLEX_SYM_LOAD(CPXdelcols);
    CPLEX_SYM_LOAD(CPXdelrows);
    CPLEX_SYM_LOAD(CPXlpopt);
    CPLEX_SYM_LOAD(CPXmipopt);
    CPLEX_SYM_LOAD(CPXgetstat);
    CPLEX_SYM_LOAD(CPXgetobjsen);
    CPLEX_SYM_LOAD(CPXgetobjval);
    CPLEX_SYM_LOAD(CPXgetbestobjval);
    CPLEX_SYM_LOAD(CPXgetmiprelgap);
    CPLEX_SYM_LOAD(CPXgetx);
    CPLEX_SYM_LOAD(CPXgetpi);
    CPLEX_SYM_LOAD(CPXgetdj);
    CPLEX_SYM_LOAD(CPXgetray);
    CPLEX_SYM_LOAD(CPXdualfarkas);
    CPLEX_SYM_LOAD(CPXgetnumcols);
    CPLEX_SYM_LOAD(CPXgetnumrows);
    CPLEX_SYM_LOAD(CPXsetintparam);
    CPLEX_SYM_LOAD(CPXsetdblparam);
    CPLEX_SYM_LOAD(CPXwriteprob);
    CPLEX_SYM_LOAD(CPXgeterrorstring);
    CPLEX_SYM_LOAD(CPXversionnumber);
    CPLEX_SYM_LOAD(CPXgetsolnpoolnumsolns);
    CPLEX_SYM_LOAD(CPXgetsolnpoolobjval);
    CPLEX_SYM_LOAD(CPXgetsolnpoolx);
    CPLEX_SYM_LOAD(CPXpopulate);
    CPLEX_SYM_LOAD(CPXaddsos);
    CPLEX_SYM_LOAD(CPXdelsos);
    CPLEX_SYM_LOAD(CPXgetnumsos);
    CPLEX_SYM_LOAD(CPXaddqconstr);
    CPLEX_SYM_LOAD(CPXdelqconstrs);
    CPLEX_SYM_LOAD(CPXgetnumqconstrs);
    CPLEX_SYM_LOAD(CPXcopyquad);
    CPLEX_SYM_LOAD(CPXqpopt);
}

idol::Optimizers::Cplex::DynamicLib::~DynamicLib() {
    if (m_handle) {
        dlclose(m_handle);
    }
}

idol::Optimizers::Cplex::DynamicLib& idol::Optimizers::Cplex::get_dynamic_lib(bool t_load_library) {
    if (!m_dynamic_lib) {
        m_dynamic_lib = std::make_unique<DynamicLib>();
    }
    if (t_load_library && !m_dynamic_lib->is_available()) {
        throw Exception("CPLEX library is not available");
    }
    return *m_dynamic_lib;
}

// ---------------------------------------------------------------------------
// Conversion helpers
// ---------------------------------------------------------------------------

char idol::Optimizers::Cplex::cplex_var_type(int t_type) const {
    if (m_continuous_relaxation) return idol_CPX_CONTINUOUS;
    switch (t_type) {
        case Continuous: return idol_CPX_CONTINUOUS;
        case Integer:    return idol_CPX_INTEGER;
        case Binary:     return idol_CPX_BINARY;
        default:;
    }
    throw Exception("Unsupported variable type: " + std::to_string(t_type));
}

char idol::Optimizers::Cplex::cplex_ctr_type(int t_type) {
    switch (t_type) {
        case Equal:          return 'E';
        case LessOrEqual:    return 'L';
        case GreaterOrEqual: return 'G';
        default:;
    }
    throw Exception("Unsupported constraint type: " + std::to_string(t_type));
}

int idol::Optimizers::Cplex::cplex_obj_sense(int t_sense) {
    switch (t_sense) {
        case Minimize: return idol_CPX_MIN;
        case Maximize: return idol_CPX_MAX;
        default:;
    }
    throw Exception("Unsupported objective sense: " + std::to_string(t_sense));
}

double idol::Optimizers::Cplex::cplex_numeric(double t_value) {
    if (is_pos_inf(t_value)) return  idol_CPX_INFBOUND;
    if (is_neg_inf(t_value)) return -idol_CPX_INFBOUND;
    return t_value;
}

std::pair<idol::SolutionStatus, idol::SolutionReason>
idol::Optimizers::Cplex::cplex_status(int t_status) const {
    switch (t_status) {
        // LP
        case idol_CPX_STAT_OPTIMAL:        return { Optimal,    Proved };
        case idol_CPX_STAT_OPTIMAL_INFEAS: return { SubOptimal, Proved };
        case idol_CPX_STAT_INFEASIBLE:     return { Infeasible, Proved };
        case idol_CPX_STAT_UNBOUNDED:      return { Unbounded,  Proved };
        case idol_CPX_STAT_INForUNBD:      return { InfOrUnbnd, Proved };
        case idol_CPX_STAT_FEASIBLE:       return { Feasible,   NotSpecified };
        case idol_CPX_STAT_NUM_BEST:       return { Fail,       NotSpecified };
        case idol_CPX_STAT_ABORT_TIME_LIM: return { Infeasible, TimeLimit };
        case idol_CPX_STAT_ABORT_OBJ_LIM:
        case idol_CPX_STAT_ABORT_DUAL_OBJ_LIM:
        case idol_CPX_STAT_ABORT_PRIM_OBJ_LIM: return { Feasible, ObjLimit };
        // MIP
        case idol_CPXMIP_OPTIMAL:          return { Optimal,    Proved };
        case idol_CPXMIP_OPTIMAL_TOL:      return { Optimal,    Proved };
        case idol_CPXMIP_OPTIMAL_INFEAS:   return { SubOptimal, Proved };
        case idol_CPXMIP_INFEASIBLE:       return { Infeasible, Proved };
        case idol_CPXMIP_UNBOUNDED:        return { Unbounded,  Proved };
        case idol_CPXMIP_INForUNBD:        return { InfOrUnbnd, Proved };
        case idol_CPXMIP_SOL_LIM:          return { Feasible,   SolutionLimit };
        case idol_CPXMIP_TIME_LIM_FEAS:    return { Feasible,   TimeLimit };
        case idol_CPXMIP_TIME_LIM_INFEAS:  return { Infeasible, TimeLimit };
        case idol_CPXMIP_NODE_LIM_FEAS:    return { Feasible,   NotSpecified };
        case idol_CPXMIP_NODE_LIM_INFEAS:  return { Infeasible, NotSpecified };
        case idol_CPXMIP_FEASIBLE:         return { Feasible,   NotSpecified };
        case idol_CPXMIP_FAIL_FEAS:
        case idol_CPXMIP_FAIL_FEAS_NO_TREE:
        case idol_CPXMIP_MEM_LIM_FEAS:
        case idol_CPXMIP_ABORT_FEAS:       return { Feasible,   NotSpecified };
        case idol_CPXMIP_FAIL_INFEAS:
        case idol_CPXMIP_FAIL_INFEAS_NO_TREE:
        case idol_CPXMIP_MEM_LIM_INFEAS:
        case idol_CPXMIP_ABORT_INFEAS:     return { Infeasible, NotSpecified };
        default:;
    }
    throw Exception("Unsupported CPLEX status: " + std::to_string(t_status));
}

// ---------------------------------------------------------------------------
// Constructor / Destructor
// ---------------------------------------------------------------------------

idol::Optimizers::Cplex::Cplex(const Model& t_model, bool t_continuous_relaxation)
    : OptimizerWithLazyUpdates(t_model),
      m_continuous_relaxation(t_continuous_relaxation) {

    auto& lib = get_dynamic_lib();

    int status = 0;
    m_env = lib.CPXopenCPLEX(&status);
    if (!m_env || status) {
        throw Exception("Could not open CPLEX environment.");
    }

    m_model = lib.CPXcreateprob(m_env, &status, "idol-model");
    if (!m_model || status) {
        lib.CPXcloseCPLEX(&m_env);
        throw Exception("Could not create CPLEX problem.");
    }
}

idol::Optimizers::Cplex::~Cplex() {
    auto& lib = get_dynamic_lib();
    if (m_model) {
        lib.CPXfreeprob(m_env, &m_model);
        m_model = nullptr;
    }
    if (m_env) {
        lib.CPXcloseCPLEX(&m_env);
        m_env = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Build / Optimize / Write
// ---------------------------------------------------------------------------

void idol::Optimizers::Cplex::hook_build() {
    // Linear objective coefficients are passed via hook_add(Var), so we only need
    // hook_update_objective for quadratic objectives. Leave the flag set so the
    // update mechanism calls hook_update_objective before the first optimization.
    if (!parent().get_obj_expr().has_quadratic()) {
        hook_update_objective_sense();
        set_objective_as_updated();
    }
    set_rhs_as_updated();
}

void idol::Optimizers::Cplex::hook_optimize() {
    set_solution_index(0);

    auto& lib = get_dynamic_lib();
    const auto& model = parent();

    // Determine if the problem has any integer/binary variables
    bool has_integers = false;
    if (!m_continuous_relaxation) {
        for (const auto& var : model.vars()) {
            const auto type = model.get_var_type(var);
            if (type == Integer || type == Binary) {
                has_integers = true;
                break;
            }
        }
    }

    const bool has_sos   = !m_continuous_relaxation && lib.CPXgetnumsos(m_env, m_model) > 0;
    const bool has_qobj  = parent().get_obj_expr().has_quadratic();
    const bool has_qctrs = lib.CPXgetnumqconstrs(m_env, m_model) > 0;

    if (has_integers || has_sos) {
        CPLEX_CATCH(m_env, CPXmipopt(m_env, m_model));
    } else if (has_qobj || has_qctrs) {
        CPLEX_CATCH(m_env, CPXqpopt(m_env, m_model));
    } else {
        CPLEX_CATCH(m_env, CPXlpopt(m_env, m_model));

        if (get_param_infeasible_or_unbounded_info()) {
            const int stat = lib.CPXgetstat(m_env, m_model);

            // Presolve can return InfOrUnbnd without distinguishing — re-solve without it
            if (stat == idol_CPX_STAT_INForUNBD) {
                lib.CPXsetintparam(m_env, idol_CPX_PARAM_PREIND, 0);
                CPLEX_CATCH(m_env, CPXlpopt(m_env, m_model));
                lib.CPXsetintparam(m_env, idol_CPX_PARAM_PREIND, 1);
            }

            // CPXdualfarkas requires the dual simplex to have proven infeasibility
            const int stat2 = lib.CPXgetstat(m_env, m_model);
            if (stat2 == idol_CPX_STAT_INFEASIBLE) {
                lib.CPXsetintparam(m_env, idol_CPX_PARAM_PREIND, 0);
                lib.CPXsetintparam(m_env, idol_CPX_PARAM_LPMETHOD, 2);
                CPLEX_CATCH(m_env, CPXlpopt(m_env, m_model));
                lib.CPXsetintparam(m_env, idol_CPX_PARAM_PREIND, 1);
                lib.CPXsetintparam(m_env, idol_CPX_PARAM_LPMETHOD, 0);
            }
        }
    }
}

void idol::Optimizers::Cplex::hook_write(const std::string& t_name) {
    auto& lib = get_dynamic_lib();
    CPLEX_CATCH(m_env, CPXwriteprob(m_env, m_model, t_name.c_str(), nullptr));
}

// ---------------------------------------------------------------------------
// hook_add
// ---------------------------------------------------------------------------

int idol::Optimizers::Cplex::hook_add(const Var& t_var, bool t_add_column) {

    auto& lib = get_dynamic_lib();
    const auto& model = parent();
    const auto lb  = cplex_numeric(model.get_var_lb(t_var));
    const auto ub  = cplex_numeric(model.get_var_ub(t_var));
    const auto obj = model.get_var_obj(t_var);
    const auto type = cplex_var_type(model.get_var_type(t_var));
    const auto& name = t_var.name();
    char* cname = const_cast<char*>(name.c_str());

    // Column is appended at position n_cols
    const int col_index = lib.CPXgetnumcols(m_env, m_model);

    double zero_obj = obj;
    int    empty_beg = 0;

    CPLEX_CATCH(m_env, CPXaddcols(m_env, m_model, 1, 0,
        &zero_obj, &empty_beg, nullptr, nullptr,
        &lb, &ub, &cname));

    // Set variable type for MIP
    if (type != idol_CPX_CONTINUOUS) {
        CPLEX_CATCH(m_env, CPXchgctype(m_env, m_model, 1, &col_index, &type));
    }

    // Fill in column coefficients for existing constraints
    if (t_add_column) {
        const auto& column = model.get_var_column(t_var);
        for (const auto& [ctr, coeff] : column) {
            if (!has_lazy(ctr)) continue;
            const int row_idx = lazy(ctr).impl();
            CPLEX_CATCH(m_env, CPXchgcoef(m_env, m_model, row_idx, col_index, cplex_numeric(coeff)));
        }
    }

    return col_index;
}

int idol::Optimizers::Cplex::hook_add(const Ctr& t_ctr) {

    auto& lib = get_dynamic_lib();
    const auto& model = parent();
    const auto& row  = model.get_ctr_row(t_ctr);
    const char  sense = cplex_ctr_type(model.get_ctr_type(t_ctr));
    const double rhs  = cplex_numeric(model.get_ctr_rhs(t_ctr));
    const auto& name  = t_ctr.name();
    char* rname = const_cast<char*>(name.c_str());

    const int row_index = lib.CPXgetnumrows(m_env, m_model);

    std::vector<int>    cols;
    std::vector<double> vals;
    cols.reserve(row.size());
    vals.reserve(row.size());

    for (const auto& [var, coeff] : row) {
        cols.push_back(lazy(var).impl());
        vals.push_back(cplex_numeric(coeff));
    }

    int beg = 0;
    int nz  = (int) cols.size();

    CPLEX_CATCH(m_env, CPXaddrows(m_env, m_model,
        0, 1, nz,
        &rhs, &sense,
        &beg, cols.data(), vals.data(),
        nullptr, &rname));

    return row_index;
}

int idol::Optimizers::Cplex::hook_add(const QCtr& t_ctr) {

    auto& lib = get_dynamic_lib();
    const auto& model = parent();
    const auto& expr  = model.get_qctr_expr(t_ctr);
    const char  sense = cplex_ctr_type(model.get_qctr_type(t_ctr));
    const double rhs  = cplex_numeric(expr.affine().constant());
    const auto& name  = t_ctr.name();

    std::vector<int>    linind;
    std::vector<double> linval;
    for (const auto& [var, coeff] : expr.affine().linear()) {
        linind.push_back(lazy(var).impl());
        linval.push_back(cplex_numeric(coeff));
    }

    // CPXaddqconstr has no 1/2 factor: each triplet contributes coeff * x[i] * x[j] directly
    std::vector<int>    quadrow, quadcol;
    std::vector<double> quadval;
    for (const auto& [pair, coeff] : expr) {
        quadrow.push_back(lazy(pair.first).impl());
        quadcol.push_back(lazy(pair.second).impl());
        quadval.push_back(cplex_numeric(coeff));
    }

    const int qc_index = lib.CPXgetnumqconstrs(m_env, m_model);

    CPLEX_CATCH(m_env, CPXaddqconstr(m_env, m_model,
        (int) linind.size(), (int) quadrow.size(),
        rhs, sense,
        linind.empty()  ? nullptr : linind.data(),
        linval.empty()  ? nullptr : linval.data(),
        quadrow.empty() ? nullptr : quadrow.data(),
        quadcol.empty() ? nullptr : quadcol.data(),
        quadval.empty() ? nullptr : quadval.data(),
        name.c_str()));

    return qc_index;
}

int idol::Optimizers::Cplex::hook_add(const SOSCtr& t_ctr) {

    auto& lib = get_dynamic_lib();
    const auto& model   = parent();
    const auto& vars    = model.get_sosctr_vars(t_ctr);
    const auto& weights = model.get_sosctr_weights(t_ctr);
    const int n = (int) vars.size();

    std::vector<int> indices(n);
    for (int i = 0; i < n; ++i) {
        indices[i] = lazy(vars[i]).impl();
    }

    const char type     = model.is_sos1(t_ctr) ? '1' : '2'; // CPX_TYPE_SOS1/2
    int        beginning = 0;
    const int  sos_index = lib.CPXgetnumsos(m_env, m_model);

    CPLEX_CATCH(m_env, CPXaddsos(m_env, m_model,
        1, n,
        &type, &beginning,
        indices.data(),
        const_cast<double*>(weights.data()),
        nullptr));

    return sos_index;
}

// ---------------------------------------------------------------------------
// hook_update (individual objects)
// ---------------------------------------------------------------------------

void idol::Optimizers::Cplex::hook_update(const Var& t_var) {

    auto& lib = get_dynamic_lib();
    const auto& model = parent();
    const int index = lazy(t_var).impl();

    const double lb  = cplex_numeric(model.get_var_lb(t_var));
    const double ub  = cplex_numeric(model.get_var_ub(t_var));
    const double obj = cplex_numeric(model.get_var_obj(t_var));
    const char   type = cplex_var_type(model.get_var_type(t_var));

    const char lb_flag = idol_CPX_LOWER;
    const char ub_flag = idol_CPX_UPPER;

    CPLEX_CATCH(m_env, CPXchgbds(m_env, m_model, 1, &index, &lb_flag, &lb));
    CPLEX_CATCH(m_env, CPXchgbds(m_env, m_model, 1, &index, &ub_flag, &ub));
    CPLEX_CATCH(m_env, CPXchgobj(m_env, m_model, 1, &index, &obj));

    if (!m_continuous_relaxation && type != idol_CPX_CONTINUOUS) {
        CPLEX_CATCH(m_env, CPXchgctype(m_env, m_model, 1, &index, &type));
    }
}

void idol::Optimizers::Cplex::hook_update(const Ctr& t_ctr) {

    auto& lib = get_dynamic_lib();
    const auto& model = parent();
    const int   index = lazy(t_ctr).impl();
    const double rhs  = cplex_numeric(model.get_ctr_rhs(t_ctr));
    const char   sense = cplex_ctr_type(model.get_ctr_type(t_ctr));

    CPLEX_CATCH(m_env, CPXchgrhs   (m_env, m_model, 1, &index, &rhs));
    CPLEX_CATCH(m_env, CPXchgsense (m_env, m_model, 1, &index, &sense));
}

// ---------------------------------------------------------------------------
// hook_update (bulk)
// ---------------------------------------------------------------------------

void idol::Optimizers::Cplex::hook_update_objective_sense() {
    auto& lib = get_dynamic_lib();
    const int sense = cplex_obj_sense(parent().get_obj_sense());
    CPLEX_CATCH(m_env, CPXchgobjsen(m_env, m_model, sense));
}

void idol::Optimizers::Cplex::hook_update_objective() {

    auto& lib = get_dynamic_lib();
    const auto& model = parent();
    const auto& objective = model.get_obj_expr();

    const int n = lib.CPXgetnumcols(m_env, m_model);

    // Zero out all existing linear coefficients, then set new ones
    if (n > 0) {
        std::vector<int>    indices(n);
        std::vector<double> zeros(n, 0.0);
        for (int j = 0; j < n; ++j) indices[j] = j;
        CPLEX_CATCH(m_env, CPXchgobj(m_env, m_model, n, indices.data(), zeros.data()));
    }

    for (const auto& [var, coeff] : objective.affine().linear()) {
        const int    j = lazy(var).impl();
        const double v = cplex_numeric(coeff);
        CPLEX_CATCH(m_env, CPXchgobj(m_env, m_model, 1, &j, &v));
    }

    const double constant = cplex_numeric(objective.affine().constant());
    CPLEX_CATCH(m_env, CPXchgobjoffset(m_env, m_model, constant));

    // Handle quadratic objective via CPXcopyquad (fully replaces Q matrix).
    // Only called when the new or previous objective has quadratic terms, because
    // CPXcopyquad silently promotes the problem type to QP, which breaks CPXlpopt.
    // CPLEX stores (1/2) x^T Q x, so:
    //   off-diagonal (i != j): Q[i][j] = coeff
    //   diagonal     (i == j): Q[i][i] = 2 * coeff
    if (n > 0 && (objective.has_quadratic() || m_has_quad_obj)) {
        std::vector<int> qmatbeg(n, 0), qmatcnt(n, 0);
        std::vector<int>    qmatind;
        std::vector<double> qmatval;

        if (objective.has_quadratic()) {
            std::map<int, std::vector<std::pair<int, double>>> col_data;

            for (const auto& [pair, coeff] : objective) {
                int i = lazy(pair.first).impl();
                int j = lazy(pair.second).impl();
                if (i < j) std::swap(i, j);          // lower triangular: i >= j
                const double v = (i == j) ? 2.0 * cplex_numeric(coeff) : cplex_numeric(coeff);
                col_data[j].emplace_back(i, v);
            }

            int offset = 0;
            for (int j = 0; j < n; ++j) {
                qmatbeg[j] = offset;
                auto it = col_data.find(j);
                if (it != col_data.end()) {
                    auto& terms = it->second;
                    std::sort(terms.begin(), terms.end());
                    for (const auto& [row, val] : terms) {
                        qmatind.push_back(row);
                        qmatval.push_back(val);
                    }
                    qmatcnt[j] = (int) terms.size();
                    offset += (int) terms.size();
                }
            }
        }

        // A dummy non-null pointer is needed when no non-zeros are present
        int    dummy_ind = 0;
        double dummy_val = 0.0;
        CPLEX_CATCH(m_env, CPXcopyquad(m_env, m_model,
            qmatbeg.data(), qmatcnt.data(),
            qmatind.empty() ? &dummy_ind : qmatind.data(),
            qmatval.empty() ? &dummy_val : qmatval.data()));

        m_has_quad_obj = objective.has_quadratic();
    }

    hook_update_objective_sense();
}

void idol::Optimizers::Cplex::hook_update_rhs() {

    auto& lib = get_dynamic_lib();
    const auto& model = parent();

    for (const auto& ctr : model.ctrs()) {
        const int   index = lazy(ctr).impl();
        const double rhs  = cplex_numeric(model.get_ctr_rhs(ctr));
        CPLEX_CATCH(m_env, CPXchgrhs(m_env, m_model, 1, &index, &rhs));
    }
}

void idol::Optimizers::Cplex::hook_update_matrix(const Ctr& t_ctr, const Var& t_var, double t_constant) {
    auto& lib = get_dynamic_lib();
    const int row = lazy(t_ctr).impl();
    const int col = lazy(t_var).impl();
    CPLEX_CATCH(m_env, CPXchgcoef(m_env, m_model, row, col, cplex_numeric(t_constant)));
}

void idol::Optimizers::Cplex::hook_update() {
    // CPLEX C API applies changes immediately; no explicit flush needed.
}

// ---------------------------------------------------------------------------
// hook_remove
// ---------------------------------------------------------------------------

void idol::Optimizers::Cplex::hook_remove(const Var& t_var) {

    auto& lib = get_dynamic_lib();
    const int index = lazy(t_var).impl();
    CPLEX_CATCH(m_env, CPXdelcols(m_env, m_model, index, index));

    for (auto& lz : lazy_vars()) {
        if (!lz.has_impl()) continue;
        if (lz.impl() > index) lz.impl()--;
    }
}

void idol::Optimizers::Cplex::hook_remove(const Ctr& t_ctr) {

    auto& lib = get_dynamic_lib();
    const int index = lazy(t_ctr).impl();
    CPLEX_CATCH(m_env, CPXdelrows(m_env, m_model, index, index));

    for (auto& lz : lazy_ctrs()) {
        if (!lz.has_impl()) continue;
        if (lz.impl() > index) lz.impl()--;
    }
}

void idol::Optimizers::Cplex::hook_remove(const QCtr& t_ctr) {

    auto& lib = get_dynamic_lib();
    const int index = lazy(t_ctr).impl();
    CPLEX_CATCH(m_env, CPXdelqconstrs(m_env, m_model, index, index));

    for (auto& lz : lazy_qctrs()) {
        if (!lz.has_impl()) continue;
        if (lz.impl() > index) lz.impl()--;
    }
}

void idol::Optimizers::Cplex::hook_remove(const SOSCtr& t_ctr) {

    auto& lib = get_dynamic_lib();
    const int index = lazy(t_ctr).impl();
    CPLEX_CATCH(m_env, CPXdelsos(m_env, m_model, index, index));

    for (auto& lz : lazy_sosctrs()) {
        if (!lz.has_impl()) continue;
        if (lz.impl() > index) lz.impl()--;
    }
}

// ---------------------------------------------------------------------------
// Solution query
// ---------------------------------------------------------------------------

idol::SolutionStatus idol::Optimizers::Cplex::get_status() const {
    auto& lib = get_dynamic_lib();
    const int status = lib.CPXgetstat(m_env, m_model);
    return cplex_status(status).first;
}

idol::SolutionReason idol::Optimizers::Cplex::get_reason() const {
    auto& lib = get_dynamic_lib();
    const int status = lib.CPXgetstat(m_env, m_model);
    return cplex_status(status).second;
}

double idol::Optimizers::Cplex::get_best_obj() const {

    const auto status = get_status();

    if (status == Unbounded)               return -Inf;
    if (status == Infeasible || status == InfOrUnbnd) return +Inf;

    auto& lib = get_dynamic_lib();

    if (m_solution_index > 0) {
        double val = 0.0;
        lib.CPXgetsolnpoolobjval(m_env, m_model, (int) m_solution_index - 1, &val);
        return val;
    }

    double val = 0.0;
    CPLEX_CATCH(m_env, CPXgetobjval(m_env, m_model, &val));
    return val;
}

double idol::Optimizers::Cplex::get_best_bound() const {

    const auto status = get_status();

    if (status == Unbounded)   return -Inf;
    if (status == Infeasible)  return +Inf;

    auto& lib = get_dynamic_lib();

    double val = 0.0;
    if (lib.CPXgetbestobjval(m_env, m_model, &val) != 0) {
        // LP: bound equals obj value
        lib.CPXgetobjval(m_env, m_model, &val);
    }
    return val;
}

double idol::Optimizers::Cplex::get_var_primal(const Var& t_var) const {

    if (const auto status = get_status(); status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Primal solution not available.");
    }

    auto& lib = get_dynamic_lib();
    const int index = lazy(t_var).impl();
    double val = 0.0;

    if (m_solution_index > 0) {
        lib.CPXgetsolnpoolx(m_env, m_model, (int) m_solution_index - 1, &val, index, index);
        return val;
    }

    CPLEX_CATCH(m_env, CPXgetx(m_env, m_model, &val, index, index));
    return val;
}

double idol::Optimizers::Cplex::get_var_reduced_cost(const Var& t_var) const {

    if (const auto status = get_status(); status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Reduced cost not available.");
    }

    auto& lib = get_dynamic_lib();
    const int index = lazy(t_var).impl();
    double val = 0.0;
    CPLEX_CATCH(m_env, CPXgetdj(m_env, m_model, &val, index, index));
    return val;
}

double idol::Optimizers::Cplex::get_var_ray(const Var& t_var) const {

    if (get_status() != Unbounded) {
        throw Exception("Ray not available.");
    }

    auto& lib = get_dynamic_lib();
    const int n = lib.CPXgetnumcols(m_env, m_model);
    std::vector<double> ray(n);
    CPLEX_CATCH(m_env, CPXgetray(m_env, m_model, ray.data()));
    return ray[lazy(t_var).impl()];
}

double idol::Optimizers::Cplex::get_ctr_dual(const Ctr& t_ctr) const {

    if (const auto status = get_status(); status != Optimal && status != Feasible && status != SubOptimal) {
        throw Exception("Dual solution not available.");
    }

    auto& lib = get_dynamic_lib();
    const int index = lazy(t_ctr).impl();
    double val = 0.0;
    CPLEX_CATCH(m_env, CPXgetpi(m_env, m_model, &val, index, index));
    return val;
}

double idol::Optimizers::Cplex::get_ctr_farkas(const Ctr& t_ctr) const {

    if (get_status() != Infeasible) {
        throw Exception("Farkas certificate not available.");
    }

    auto& lib = get_dynamic_lib();
    const int n_rows = lib.CPXgetnumrows(m_env, m_model);
    std::vector<double> y(n_rows);
    double proof = 0.0;
    CPLEX_CATCH(m_env, CPXdualfarkas(m_env, m_model, y.data(), &proof));
    return y[lazy(t_ctr).impl()];
}

double idol::Optimizers::Cplex::get_relative_gap() const {
    auto& lib = get_dynamic_lib();
    double gap = 0.0;
    if (lib.CPXgetmiprelgap(m_env, m_model, &gap) != 0) {
        throw Exception("Relative gap not available.");
    }
    return gap;
}

double idol::Optimizers::Cplex::get_absolute_gap() const {
    throw Exception("CPLEX: absolute gap not directly available.");
}

unsigned int idol::Optimizers::Cplex::get_n_solutions() const {

    const auto status = get_status();
    if (status == Unbounded || status == Infeasible || status == InfOrUnbnd) return 0;

    auto& lib = get_dynamic_lib();
    const int pool = lib.CPXgetsolnpoolnumsolns(m_env, m_model);
    // Primary solution is always index 0, pool solutions start at 1
    return 1 + (pool > 0 ? pool : 0);
}

unsigned int idol::Optimizers::Cplex::get_solution_index() const {
    return m_solution_index;
}

void idol::Optimizers::Cplex::set_solution_index(unsigned int t_index) {
    m_solution_index = t_index;
}

// ---------------------------------------------------------------------------
// Parameter setters
// ---------------------------------------------------------------------------

void idol::Optimizers::Cplex::set_param_logs(bool t_value) {
    auto& lib = get_dynamic_lib();
    lib.CPXsetintparam(m_env, idol_CPX_PARAM_SCRIND, t_value ? 1 : 0);
    Optimizer::set_param_logs(t_value);
}

void idol::Optimizers::Cplex::set_param_presolve(bool t_value) {
    auto& lib = get_dynamic_lib();
    lib.CPXsetintparam(m_env, idol_CPX_PARAM_PREIND, t_value ? 1 : 0);
    Optimizer::set_param_presolve(t_value);
}

void idol::Optimizers::Cplex::set_param_time_limit(double t_time_limit) {
    auto& lib = get_dynamic_lib();
    lib.CPXsetdblparam(m_env, idol_CPX_PARAM_TILIM, t_time_limit);
    Optimizer::set_param_time_limit(t_time_limit);
}

void idol::Optimizers::Cplex::set_param_threads(unsigned int t_thread_limit) {
    auto& lib = get_dynamic_lib();
    lib.CPXsetintparam(m_env, idol_CPX_PARAM_THREADS, (CPXINT) t_thread_limit);
    Optimizer::set_param_threads(t_thread_limit);
}

void idol::Optimizers::Cplex::set_param_best_obj_stop(double t_best_obj_stop) {
    auto& lib = get_dynamic_lib();
    // For minimization: stop when incumbent ≤ t_best_obj_stop → CPX_PARAM_OBJLLIM
    // For maximization: stop when incumbent ≥ t_best_obj_stop → CPX_PARAM_OBJULIM
    if (parent().get_obj_sense() == Minimize) {
        lib.CPXsetdblparam(m_env, idol_CPX_PARAM_OBJLLIM, cplex_numeric(t_best_obj_stop));
    } else {
        lib.CPXsetdblparam(m_env, idol_CPX_PARAM_OBJULIM, cplex_numeric(t_best_obj_stop));
    }
    Optimizer::set_param_best_obj_stop(t_best_obj_stop);
}

void idol::Optimizers::Cplex::set_param_best_bound_stop(double t_best_bound_stop) {
    // CPLEX C API has no native parameter for stopping based on the dual bound.
    Optimizer::set_param_best_bound_stop(t_best_bound_stop);
}

void idol::Optimizers::Cplex::set_param_infeasible_or_unbounded_info(bool /*t_value*/) {
    // CPLEX detects infeasibility/unboundedness automatically; nothing to toggle here.
    Optimizer::set_param_infeasible_or_unbounded_info(true);
}

void idol::Optimizers::Cplex::set_tol_mip_relative_gap(double t_relative_gap_tolerance) {
    Optimizer::set_tol_mip_relative_gap(t_relative_gap_tolerance);
    auto& lib = get_dynamic_lib();
    lib.CPXsetdblparam(m_env, idol_CPX_PARAM_EPGAP, t_relative_gap_tolerance);
}

void idol::Optimizers::Cplex::set_tol_mip_absolute_gap(double t_absolute_gap_tolerance) {
    Optimizer::set_tol_mip_absolute_gap(t_absolute_gap_tolerance);
    auto& lib = get_dynamic_lib();
    lib.CPXsetdblparam(m_env, idol_CPX_PARAM_EPAGAP, t_absolute_gap_tolerance);
}

void idol::Optimizers::Cplex::set_tol_feasibility(double t_tol_feasibility) {
    Optimizer::set_tol_feasibility(t_tol_feasibility);
    auto& lib = get_dynamic_lib();
    lib.CPXsetdblparam(m_env, idol_CPX_PARAM_EPRHS, t_tol_feasibility);
}

void idol::Optimizers::Cplex::set_tol_optimality(double t_tol_optimality) {
    Optimizer::set_tol_optimality(t_tol_optimality);
    auto& lib = get_dynamic_lib();
    lib.CPXsetdblparam(m_env, idol_CPX_PARAM_EPOPT, t_tol_optimality);
}

void idol::Optimizers::Cplex::set_tol_integer(double t_tol_integer) {
    Optimizer::set_tol_integer(t_tol_integer);
    auto& lib = get_dynamic_lib();
    lib.CPXsetdblparam(m_env, idol_CPX_PARAM_EPINT, t_tol_integer);
}

void idol::Optimizers::Cplex::set_param(const std::string& /*t_param*/, int /*t_value*/) {
    throw Exception("CPLEX: setting parameters by name is not supported. Use numeric parameter IDs.");
}

void idol::Optimizers::Cplex::set_param(const std::string& /*t_param*/, double /*t_value*/) {
    throw Exception("CPLEX: setting parameters by name is not supported. Use numeric parameter IDs.");
}

void idol::Optimizers::Cplex::set_max_n_solution_in_pool(unsigned int t_value) {
    auto& lib = get_dynamic_lib();
    // CPX_PARAM_POPULATELIM controls the number of pool solutions gathered by CPXpopulate
    const int CPX_PARAM_POPULATELIM = 2029;
    lib.CPXsetintparam(m_env, CPX_PARAM_POPULATELIM, (CPXINT) t_value);
}

// ---------------------------------------------------------------------------
// Availability / version
// ---------------------------------------------------------------------------

bool idol::Optimizers::Cplex::is_available() {
    auto& lib = get_dynamic_lib(false);
    return lib.is_available();
}

std::string idol::Optimizers::Cplex::get_version() {
    auto& lib = get_dynamic_lib();
    int status = 0;
    CPXENVptr env = lib.CPXopenCPLEX(&status);
    if (!env) return "CPLEX (version unknown)";
    int version = 0;
    lib.CPXversionnumber(env, &version);
    lib.CPXcloseCPLEX(&env);
    const int major = version / 1000000;
    const int minor = (version % 1000000) / 10000;
    const int micro = (version % 10000) / 100;
    const int patch = version % 100;
    return std::to_string(major) + "." + std::to_string(minor) + "."
         + std::to_string(micro) + "." + std::to_string(patch);
}