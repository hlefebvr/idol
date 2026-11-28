#include "idol/mixed-integer/optimizers/wrappers/Cplex/Optimizers_Cplex.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/CplexCallbackI.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempQCtr.h"
#include <algorithm>
#include <cstdlib>
#include <dlfcn.h>
#include <filesystem>
#include <regex>

using idol::Optimizers::Cplex;

#define CPLEX_LOAD(name) do { name = reinterpret_cast<decltype(name)>(dlsym(m_handle, #name)); \
    if (!name) throw idol::Exception(std::string("Missing CPLEX symbol ") + #name); } while (false)

std::unique_ptr<Cplex::DynamicLib> Cplex::m_dynamic_lib;

std::string Cplex::DynamicLib::find_library() {
    namespace fs = std::filesystem;
    if (const char* value = std::getenv("IDOL_CPLEX_PATH")) {
        if (fs::is_regular_file(value)) return value;
    }
    std::vector<fs::path> homes;
    for (const char* key : {"CPLEX_STUDIO_DIR2211", "CPLEX_STUDIO_DIR", "CPLEX_HOME"}) {
        if (const char* value = std::getenv(key)) homes.emplace_back(value);
    }
    homes.emplace_back("/Users/henri/Research/Solvers/IBM/CPLEX_Studio2211");
    homes.emplace_back("/Users/henri/Research/solvers/IBM/CPLEX_Studio2211");
    for (const auto& home : homes) {
        if (!fs::exists(home)) continue;
        for (const auto& root : {home / "cplex" / "bin", home / "bin", home}) {
            if (!fs::exists(root)) continue;
            for (const auto& entry : fs::recursive_directory_iterator(root)) {
                if (!entry.is_regular_file()) continue;
                const auto name = entry.path().filename().string();
#if defined(__APPLE__)
                if (std::regex_match(name, std::regex(R"(libcplex[0-9]+\.dylib)"))) return entry.path();
#elif defined(_WIN32)
                if (std::regex_match(name, std::regex(R"(cplex[0-9]+\.dll)", std::regex::icase))) return entry.path();
#else
                if (std::regex_match(name, std::regex(R"(libcplex[0-9]+\.so)"))) return entry.path();
#endif
            }
        }
    }
    return {};
}

Cplex::DynamicLib::DynamicLib() {
    const auto path = find_library();
    if (path.empty()) return;
    m_handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!m_handle) return;
    CPLEX_LOAD(CPXversion); CPLEX_LOAD(CPXopenCPLEX); CPLEX_LOAD(CPXcloseCPLEX); CPLEX_LOAD(CPXcreateprob);
    CPLEX_LOAD(CPXfreeprob); CPLEX_LOAD(CPXgeterrorstring); CPLEX_LOAD(CPXaddcols); CPLEX_LOAD(CPXaddrows);
    CPLEX_LOAD(CPXaddqconstr); CPLEX_LOAD(CPXaddsos); CPLEX_LOAD(CPXlpopt); CPLEX_LOAD(CPXmipopt); CPLEX_LOAD(CPXqpopt); CPLEX_LOAD(CPXbaropt);
    CPLEX_LOAD(CPXgetstat); CPLEX_LOAD(CPXsolninfo); CPLEX_LOAD(CPXgetobjval); CPLEX_LOAD(CPXgetbestobjval); CPLEX_LOAD(CPXgetmiprelgap);
    CPLEX_LOAD(CPXgetx); CPLEX_LOAD(CPXgetpi); CPLEX_LOAD(CPXgetdj); CPLEX_LOAD(CPXgetray); CPLEX_LOAD(CPXdualfarkas);
    CPLEX_LOAD(CPXchgbds); CPLEX_LOAD(CPXchgctype); CPLEX_LOAD(CPXchgobj); CPLEX_LOAD(CPXchgrhs);
    CPLEX_LOAD(CPXchgsense); CPLEX_LOAD(CPXchgcoef); CPLEX_LOAD(CPXchgobjoffset); CPLEX_LOAD(CPXchgobjsen);
    CPLEX_LOAD(CPXchgqpcoef);
    CPLEX_LOAD(CPXchgprobtype);
    CPLEX_LOAD(CPXdelcols); CPLEX_LOAD(CPXdelrows); CPLEX_LOAD(CPXdelqconstrs); CPLEX_LOAD(CPXdelsos);
    CPLEX_LOAD(CPXsetintparam); CPLEX_LOAD(CPXsetdblparam); CPLEX_LOAD(CPXwriteprob); CPLEX_LOAD(CPXreadcopyprob);
    CPLEX_LOAD(CPXgetnumcols); CPLEX_LOAD(CPXgetnumrows); CPLEX_LOAD(CPXgetnumqconstrs); CPLEX_LOAD(CPXgetnumsos);
    CPLEX_LOAD(CPXgetnumindconstrs); CPLEX_LOAD(CPXgetnumpwl); CPLEX_LOAD(CPXgetnumobjs); CPLEX_LOAD(CPXgetnumquad);
    CPLEX_LOAD(CPXgetobj); CPLEX_LOAD(CPXgetlb); CPLEX_LOAD(CPXgetub); CPLEX_LOAD(CPXgetrhs); CPLEX_LOAD(CPXgetrngval);
    CPLEX_LOAD(CPXgetctype); CPLEX_LOAD(CPXgetsense); CPLEX_LOAD(CPXgetobjoffset); CPLEX_LOAD(CPXgetobjsen);
    CPLEX_LOAD(CPXgetcolname); CPLEX_LOAD(CPXgetrowname); CPLEX_LOAD(CPXgetsosname); CPLEX_LOAD(CPXgetqconstrname); CPLEX_LOAD(CPXgetrows);
    CPLEX_LOAD(CPXgetquad); CPLEX_LOAD(CPXgetqconstr); CPLEX_LOAD(CPXgetsos);
    CPLEX_LOAD(CPXgetsolnpoolnumsolns); CPLEX_LOAD(CPXgetsolnpoolx);
    CPLEX_LOAD(CPXsetusercutcallbackfunc); CPLEX_LOAD(CPXsetlazyconstraintcallbackfunc);
    CPLEX_LOAD(CPXsetheuristiccallbackfunc); CPLEX_LOAD(CPXsetinfocallbackfunc);
    CPLEX_LOAD(CPXgetcallbackinfo); CPLEX_LOAD(CPXgetcallbacknodex); CPLEX_LOAD(CPXcutcallbackadd);
    CPLEX_LOAD(CPXsetterminate);
}

Cplex::DynamicLib::~DynamicLib() { if (m_handle) dlclose(m_handle); }

Cplex::DynamicLib& Cplex::get_dynamic_lib(bool load) {
    if (!m_dynamic_lib) m_dynamic_lib = std::make_unique<DynamicLib>();
    if (load && !m_dynamic_lib->is_available()) throw idol::Exception("CPLEX 22.1.1 runtime library is not available. Set IDOL_CPLEX_PATH.");
    return *m_dynamic_lib;
}

bool Cplex::is_available() { return get_dynamic_lib(false).is_available(); }

std::string Cplex::get_version() {
    auto& lib = get_dynamic_lib(); int error = 0; CPXENVptr env = lib.CPXopenCPLEX(&error);
    if (!env || error) throw idol::Exception("Could not open CPLEX environment.");
    std::string result = lib.CPXversion(env); lib.CPXcloseCPLEX(&env); return result;
}

void Cplex::check(int error) const {
    if (!error) return;
    char buffer[4096] = {}; auto& lib = get_dynamic_lib(); lib.CPXgeterrorstring(m_env, error, buffer);
    throw idol::NumericalException("CPLEX exception [" + std::to_string(error) + "]: " + buffer);
}

Cplex::Cplex(const idol::Model& model, bool relaxation)
    : OptimizerWithLazyUpdates(model), m_continuous_relaxation(relaxation) {
    auto& lib = get_dynamic_lib(); int error = 0; m_env = lib.CPXopenCPLEX(&error);
    if (!m_env || error) throw idol::Exception("Could not open CPLEX environment.");
    m_model = lib.CPXcreateprob(m_env, &error, "idol-model"); check(error);
}

Cplex::~Cplex() { if (!m_dynamic_lib) return; auto& lib = get_dynamic_lib(); if (m_model) lib.CPXfreeprob(m_env,&m_model); if (m_env) lib.CPXcloseCPLEX(&m_env); }

char Cplex::cplex_var_type(int type) const {
    if (m_continuous_relaxation) return idol_CPX_CONTINUOUS;
    if (type == idol::Continuous) return idol_CPX_CONTINUOUS;
    if (type == idol::Integer) return idol_CPX_INTEGER;
    if (type == idol::Binary) return idol_CPX_BINARY;
    throw idol::Exception("Unsupported variable type.");
}
char Cplex::cplex_ctr_type(int type) {
    if (type == idol::LessOrEqual) return idol_CPX_LE; if (type == idol::GreaterOrEqual) return idol_CPX_GE;
    if (type == idol::Equal) return idol_CPX_EQ; throw idol::Exception("Unsupported constraint type.");
}
double Cplex::cplex_numeric(double value) { return idol::is_pos_inf(value) ? idol_CPX_INFBOUND : idol::is_neg_inf(value) ? -idol_CPX_INFBOUND : value; }

void Cplex::hook_build() { check(get_dynamic_lib().CPXchgobjsen(m_env,m_model,idol_CPX_MIN)); set_rhs_as_updated(); }

int Cplex::hook_add(const idol::Var& var, bool add_column) {
    const auto& model = parent(); const auto& column = model.get_var_column(var);
    std::vector<int> ind; std::vector<double> val;
    if (add_column) for (const auto& [ctr,coef] : column) if (has_lazy(ctr)) { ind.push_back(lazy(ctr).impl()); val.push_back(coef); }
    const int beg = 0; double obj=model.get_var_obj(var),lb=cplex_numeric(model.get_var_lb(var)),ub=cplex_numeric(model.get_var_ub(var));
    char* name=const_cast<char*>(var.name().c_str());
    check(get_dynamic_lib().CPXaddcols(m_env,m_model,1,(int)ind.size(),&obj,&beg,ind.data(),val.data(),&lb,&ub,&name));
    const char type=cplex_var_type(model.get_var_type(var)); if(type!=idol_CPX_CONTINUOUS){ int i=model.get_var_index(var); check(get_dynamic_lib().CPXchgctype(m_env,m_model,1,&i,&type)); m_is_mip=true; }
    return model.get_var_index(var);
}

int Cplex::hook_add(const idol::Ctr& ctr) {
    const auto& row=parent().get_ctr_row(ctr); std::vector<int> ind; std::vector<double> val;
    for(const auto& [var,coef]:row){ind.push_back(lazy(var).impl());val.push_back(coef);} int beg=0;
    double rhs=parent().get_ctr_rhs(ctr); char sense=cplex_ctr_type(parent().get_ctr_type(ctr)); char* name=const_cast<char*>(ctr.name().c_str());
    check(get_dynamic_lib().CPXaddrows(m_env,m_model,0,1,(int)ind.size(),&rhs,&sense,&beg,ind.data(),val.data(),nullptr,&name));
    return parent().get_ctr_index(ctr);
}

int Cplex::hook_add(const idol::QCtr& ctr) {
    const auto& expr=parent().get_qctr_expr(ctr); std::vector<int> li,qi,qj; std::vector<double> lv,qv;
    for(const auto& [v,c]:expr.affine().linear()){li.push_back(lazy(v).impl());lv.push_back(c);}
    for(const auto& [p,c]:expr){qi.push_back(lazy(p.first).impl());qj.push_back(lazy(p.second).impl());qv.push_back(c);}
    const double rhs=-expr.affine().constant(); const char sense=cplex_ctr_type(parent().get_qctr_type(ctr));
    check(get_dynamic_lib().CPXaddqconstr(m_env,m_model,(int)li.size(),(int)qv.size(),rhs,sense,li.data(),lv.data(),qi.data(),qj.data(),qv.data(),ctr.name().c_str()));
    return parent().get_qctr_index(ctr);
}

int Cplex::hook_add(const idol::SOSCtr& ctr) {
    const auto& vars=parent().get_sosctr_vars(ctr); const auto& weights=parent().get_sosctr_weights(ctr); std::vector<int> ind;
    for(const auto& v:vars) ind.push_back(lazy(v).impl()); int beg=0; char type=parent().is_sos1(ctr)?idol_CPX_TYPE_SOS1:idol_CPX_TYPE_SOS2; char* name=const_cast<char*>(ctr.name().c_str());
    check(get_dynamic_lib().CPXaddsos(m_env,m_model,1,(int)vars.size(),&type,&beg,ind.data(),weights.data(),&name)); return parent().get_sosctr_index(ctr);
}

void Cplex::hook_update(const idol::Var& var) {
    int i=lazy(var).impl(); double values[2]={cplex_numeric(parent().get_var_lb(var)),cplex_numeric(parent().get_var_ub(var))}; char lu[2]={'L','U'}; int indices[2]={i,i};
    check(get_dynamic_lib().CPXchgbds(m_env,m_model,2,indices,lu,values)); double obj=parent().get_var_obj(var); check(get_dynamic_lib().CPXchgobj(m_env,m_model,1,&i,&obj));
    const bool was_mip=m_is_mip;char type=cplex_var_type(parent().get_var_type(var));
    if (type != idol_CPX_CONTINUOUS || m_is_mip) check(get_dynamic_lib().CPXchgctype(m_env,m_model,1,&i,&type));
    m_is_mip = !m_continuous_relaxation && std::any_of(parent().vars().begin(), parent().vars().end(),
        [this](const auto& candidate) { return parent().get_var_type(candidate) != idol::Continuous; });
    if(was_mip&&!m_is_mip)check(get_dynamic_lib().CPXchgprobtype(m_env,m_model,m_has_quadratic_objective?idol_CPXPROB_QP:idol_CPXPROB_LP));
}
void Cplex::hook_update(const idol::Ctr& ctr) { int i=lazy(ctr).impl(); double rhs=parent().get_ctr_rhs(ctr); char sense=cplex_ctr_type(parent().get_ctr_type(ctr)); check(get_dynamic_lib().CPXchgrhs(m_env,m_model,1,&i,&rhs)); check(get_dynamic_lib().CPXchgsense(m_env,m_model,1,&i,&sense)); }
void Cplex::hook_update_matrix(const idol::Ctr& ctr,const idol::Var& var,double value){check(get_dynamic_lib().CPXchgcoef(m_env,m_model,lazy(ctr).impl(),lazy(var).impl(),value));}
void Cplex::hook_update_objective(){const auto& obj=parent().get_obj_expr();std::vector<int> ind;std::vector<double> val;for(const auto& v:parent().vars()){ind.push_back(lazy(v).impl());val.push_back(obj.affine().linear().get(v));}auto& lib=get_dynamic_lib();check(lib.CPXchgobj(m_env,m_model,(int)ind.size(),ind.data(),val.data()));check(lib.CPXchgobjoffset(m_env,m_model,obj.affine().constant()));if(obj.has_quadratic()||m_has_quadratic_objective){for(const auto& first:parent().vars())for(const auto& second:parent().vars())if(lazy(first).impl()<=lazy(second).impl()){const auto pair=idol::CommutativePair<idol::Var>(first,second);double coefficient=obj.get(pair);if(lazy(first).impl()==lazy(second).impl())coefficient*=2.;check(lib.CPXchgqpcoef(m_env,m_model,lazy(first).impl(),lazy(second).impl(),coefficient));}if(!obj.has_quadratic())check(lib.CPXchgprobtype(m_env,m_model,m_is_mip?idol_CPXPROB_MILP:idol_CPXPROB_LP));}m_has_quadratic_objective=obj.has_quadratic();}
void Cplex::hook_update_rhs(){for(const auto& c:parent().ctrs())hook_update(c);}
void Cplex::hook_update(){}
void Cplex::hook_remove(const idol::Var& var){int i=lazy(var).impl();check(get_dynamic_lib().CPXdelcols(m_env,m_model,i,i));for(auto& x:lazy_vars())if(x.has_impl()&&x.impl()>i)--x.impl();m_is_mip=!m_continuous_relaxation&&std::any_of(parent().vars().begin(),parent().vars().end(),[this](const auto& candidate){return parent().get_var_type(candidate)!=idol::Continuous;});if(!m_is_mip)check(get_dynamic_lib().CPXchgprobtype(m_env,m_model,m_has_quadratic_objective?idol_CPXPROB_QP:idol_CPXPROB_LP));}
void Cplex::hook_remove(const idol::Ctr& ctr){int i=lazy(ctr).impl();check(get_dynamic_lib().CPXdelrows(m_env,m_model,i,i));for(auto& x:lazy_ctrs())if(x.has_impl()&&x.impl()>i)--x.impl();}
void Cplex::hook_remove(const idol::QCtr& ctr){int i=lazy(ctr).impl();check(get_dynamic_lib().CPXdelqconstrs(m_env,m_model,i,i));for(auto& x:lazy_qctrs())if(x.has_impl()&&x.impl()>i)--x.impl();}
void Cplex::hook_remove(const idol::SOSCtr& ctr){int i=lazy(ctr).impl();check(get_dynamic_lib().CPXdelsos(m_env,m_model,i,i));for(auto& x:lazy_sosctrs())if(x.has_impl()&&x.impl()>i)--x.impl();}

std::pair<idol::SolutionStatus,idol::SolutionReason> Cplex::cplex_status(int s) const {
    using namespace idol; const auto limited=[this](SolutionReason reason){int primal_feasible=0;get_dynamic_lib().CPXsolninfo(m_env,m_model,nullptr,nullptr,&primal_feasible,nullptr);return std::pair{primal_feasible?Feasible:Unknown,reason};};switch(s){case idol_CPX_STAT_OPTIMAL:case idol_CPXMIP_OPTIMAL:case idol_CPXMIP_OPTIMAL_TOL:case idol_CPXMIP_OPTIMAL_POPULATED:case idol_CPXMIP_OPTIMAL_POPULATED_TOL:return{Optimal,Proved};
    case idol_CPX_STAT_INFEASIBLE:case idol_CPXMIP_INFEASIBLE:return{Infeasible,Proved};case idol_CPX_STAT_UNBOUNDED:case idol_CPXMIP_UNBOUNDED:return{Unbounded,Proved};case idol_CPX_STAT_INForUNBD:case idol_CPXMIP_INForUNBD:return{InfOrUnbnd,Proved};case idol_CPXMIP_ABORT_RELAXATION_UNBOUNDED:return{Unknown,NotSpecified};
    case idol_CPXMIP_TIME_LIM_FEAS:case idol_CPXMIP_DETTIME_LIM_FEAS:return{Feasible,TimeLimit};case idol_CPXMIP_TIME_LIM_INFEAS:case idol_CPXMIP_DETTIME_LIM_INFEAS:return{Unknown,TimeLimit};case idol_CPXMIP_NODE_LIM_FEAS:return{Feasible,NodeLimit};case idol_CPXMIP_NODE_LIM_INFEAS:return{Unknown,NodeLimit};case idol_CPXMIP_SOL_LIM:case idol_CPXMIP_POPULATESOL_LIM:return{Feasible,SolutionLimit};case idol_CPXMIP_MEM_LIM_FEAS:return{Feasible,MemoryLimit};case idol_CPXMIP_MEM_LIM_INFEAS:return{Unknown,MemoryLimit};case idol_CPXMIP_FAIL_FEAS:case idol_CPXMIP_FAIL_FEAS_NO_TREE:return{Feasible,Numerical};case idol_CPX_STAT_NUM_BEST:return{Feasible,Numerical};case idol_CPX_STAT_OPTIMAL_INFEAS:case idol_CPXMIP_OPTIMAL_INFEAS:case idol_CPXMIP_FAIL_INFEAS:case idol_CPXMIP_FAIL_INFEAS_NO_TREE:return{Unknown,Numerical};case idol_CPX_STAT_FEASIBLE:case idol_CPXMIP_FEASIBLE:case idol_CPXMIP_ABORT_FEAS:return{Feasible,NotSpecified};case idol_CPXMIP_ABORT_INFEAS:return{Unknown,NotSpecified};case idol_CPX_STAT_ABORT_TIME_LIM:case idol_CPX_STAT_ABORT_DETTIME_LIM:return limited(TimeLimit);case idol_CPX_STAT_ABORT_IT_LIM:return limited(IterLimit);case idol_CPX_STAT_ABORT_USER:return limited(Interrupted);case idol_CPX_STAT_ABORT_PRIM_OBJ_LIM:return{Feasible,ObjLimit};case idol_CPX_STAT_ABORT_OBJ_LIM:case idol_CPX_STAT_ABORT_DUAL_OBJ_LIM:return limited(ObjLimit);default:return{Unknown,NotSpecified};}
}
void Cplex::hook_optimize(){m_solution_index=0;m_primal_ray.reset();m_farkas.reset();auto& l=get_dynamic_lib();if(m_cplex_callback)m_cplex_callback->register_callbacks();const int error=m_is_mip?l.CPXmipopt(m_env,m_model):parent().qctrs().size()?l.CPXbaropt(m_env,m_model):parent().get_obj_expr().has_quadratic()?l.CPXqpopt(m_env,m_model):l.CPXlpopt(m_env,m_model);if(m_cplex_callback)m_cplex_callback->rethrow_exception();check(error);if(get_param_infeasible_or_unbounded_info())save_infeasibility_information();}
void Cplex::save_infeasibility_information() {
    auto& l = get_dynamic_lib();
    const auto s = get_status();
    if (m_has_quadratic_objective) { return; }

    // CPXgetray / CPXdualfarkas only work on LPs. For a MIP whose LP relaxation
    // is unbounded (resp. infeasible), temporarily flip the problem type to LP,
    // re-solve as LP to extract the certificate, then restore MILP. Integer
    // variable types are preserved by CPLEX across CPXchgprobtype.
    const bool was_mip = m_is_mip;

    if (s == Unbounded) {
        if (was_mip) { check(l.CPXchgprobtype(m_env, m_model, idol_CPXPROB_LP)); }
        check(l.CPXsetintparam(m_env, idol_CPXPARAM_LPMethod, idol_CPX_ALG_PRIMAL));
        check(l.CPXlpopt(m_env, m_model));
        m_primal_ray.emplace(parent().vars().size());
        if (l.CPXgetray(m_env, m_model, m_primal_ray->data()) != 0) {
            m_primal_ray.reset();
        }
        if (was_mip) { check(l.CPXchgprobtype(m_env, m_model, idol_CPXPROB_MILP)); }
    } else if (s == Infeasible) {
        if (was_mip) { check(l.CPXchgprobtype(m_env, m_model, idol_CPXPROB_LP)); }
        check(l.CPXsetintparam(m_env, idol_CPXPARAM_LPMethod, idol_CPX_ALG_DUAL));
        check(l.CPXlpopt(m_env, m_model));
        // The LP relaxation of an infeasible MIP may itself be feasible; guard
        // CPXdualfarkas against that case so we return without a certificate
        // rather than aborting.
        if (cplex_status(l.CPXgetstat(m_env, m_model)).first == Infeasible) {
            m_farkas.emplace(parent().ctrs().size());
            double proof = 0;
            if (l.CPXdualfarkas(m_env, m_model, m_farkas->data(), &proof) != 0) {
                m_farkas.reset();
            }
        }
        if (was_mip) { check(l.CPXchgprobtype(m_env, m_model, idol_CPXPROB_MILP)); }
    }
}
void Cplex::hook_write(const std::string& name){check(get_dynamic_lib().CPXwriteprob(m_env,m_model,name.c_str(),nullptr));}
idol::SolutionStatus Cplex::get_status()const{return cplex_status(get_dynamic_lib().CPXgetstat(m_env,m_model)).first;}
idol::SolutionReason Cplex::get_reason()const{return cplex_status(get_dynamic_lib().CPXgetstat(m_env,m_model)).second;}
double Cplex::get_best_obj()const{auto s=get_status();if(s==idol::Unbounded)return-idol::Inf;if(s==idol::Infeasible||s==idol::InfOrUnbnd||s==idol::Unknown||s==idol::Loaded)return idol::Inf;double v;check(get_dynamic_lib().CPXgetobjval(m_env,m_model,&v));return v;}
double Cplex::get_best_bound()const{auto s=get_status();if(s==idol::Unbounded||s==idol::InfOrUnbnd)return-idol::Inf;if(s==idol::Infeasible)return idol::Inf;if(!m_is_mip)return s==idol::Optimal?get_best_obj():-idol::Inf;double v;check(get_dynamic_lib().CPXgetbestobjval(m_env,m_model,&v));return v;}
double Cplex::get_var_primal(const idol::Var& v)const{const auto s=get_status();if(s!=idol::Optimal&&s!=idol::Feasible)throw idol::Exception("Primal solution not available.");double x;if(m_is_mip&&m_solution_index){check(get_dynamic_lib().CPXgetsolnpoolx(m_env,m_model,m_solution_index,&x,lazy(v).impl(),lazy(v).impl()));}else check(get_dynamic_lib().CPXgetx(m_env,m_model,&x,lazy(v).impl(),lazy(v).impl()));return x;}
double Cplex::get_var_reduced_cost(const idol::Var& v)const{double x;check(get_dynamic_lib().CPXgetdj(m_env,m_model,&x,lazy(v).impl(),lazy(v).impl()));return x;}
double Cplex::get_var_ray(const idol::Var& v)const{if(!m_primal_ray)throw idol::Exception("Ray not available.");return m_primal_ray->at(lazy(v).impl());}
double Cplex::get_ctr_dual(const idol::Ctr& c)const{double x;check(get_dynamic_lib().CPXgetpi(m_env,m_model,&x,lazy(c).impl(),lazy(c).impl()));return x;}
double Cplex::get_ctr_farkas(const idol::Ctr& c)const{if(!m_farkas)throw idol::Exception("Farkas certificate not available.");return m_farkas->at(lazy(c).impl());}
double Cplex::get_relative_gap()const{return relative_gap(get_best_bound(),get_best_obj());}
double Cplex::get_absolute_gap()const{return std::abs(get_best_obj()-get_best_bound());}
unsigned int Cplex::get_n_solutions()const{auto s=get_status();if(s!=idol::Optimal&&s!=idol::Feasible)return 0;if(!m_is_mip)return 1;return get_dynamic_lib().CPXgetsolnpoolnumsolns(m_env,m_model);}
unsigned int Cplex::get_solution_index()const{return m_solution_index;} void Cplex::set_solution_index(unsigned int i){m_solution_index=i;}

void Cplex::set_param_time_limit(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_TimeLimit,std::min(v,1e75)));Optimizer::set_param_time_limit(v);} void Cplex::set_param_threads(unsigned v){check(get_dynamic_lib().CPXsetintparam(m_env,idol_CPXPARAM_Threads,(int)v));Optimizer::set_param_threads(v);}
void Cplex::set_param_best_obj_stop(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_MIP_Limits_LowerObjStop,v));Optimizer::set_param_best_obj_stop(v);} void Cplex::set_param_best_bound_stop(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_MIP_Limits_UpperObjStop,v));Optimizer::set_param_best_bound_stop(v);}
void Cplex::set_param_presolve(bool v){check(get_dynamic_lib().CPXsetintparam(m_env,idol_CPXPARAM_Preprocessing_Presolve,v));Optimizer::set_param_presolve(v);}
void Cplex::set_param_infeasible_or_unbounded_info(bool v){
    // CPLEX cannot populate infeasibility/unboundedness info while presolve is
    // active, so we disable it when enabling info and restore the user's idol
    // presolve setting when disabling info. Without the restore, a transient
    // enable/disable (e.g., during CG's ray-generation re-solve) would leave
    // CPLEX presolve off even though m_param_presolve still reports on.
    auto& lib = get_dynamic_lib();
    if (v) {
        check(lib.CPXsetintparam(m_env, idol_CPXPARAM_Preprocessing_Presolve, 0));
    } else {
        check(lib.CPXsetintparam(m_env, idol_CPXPARAM_Preprocessing_Presolve, get_param_presolve() ? 1 : 0));
    }
    Optimizer::set_param_infeasible_or_unbounded_info(v);
}
void Cplex::set_param_logs(bool v){check(get_dynamic_lib().CPXsetintparam(m_env,idol_CPXPARAM_ScreenOutput,v));Optimizer::set_param_logs(v);} void Cplex::set_max_n_solution_in_pool(unsigned v){check(get_dynamic_lib().CPXsetintparam(m_env,idol_CPXPARAM_MIP_Pool_Capacity,(int)v));}
void Cplex::set_tol_mip_relative_gap(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_MIP_Tolerances_MIPGap,v));Optimizer::set_tol_mip_relative_gap(v);} void Cplex::set_tol_mip_absolute_gap(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_MIP_Tolerances_AbsMIPGap,v));Optimizer::set_tol_mip_absolute_gap(v);}
void Cplex::set_tol_feasibility(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_Simplex_Tolerances_Feasibility,v));Optimizer::set_tol_feasibility(v);} void Cplex::set_tol_optimality(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_Simplex_Tolerances_Optimality,v));Optimizer::set_tol_optimality(v);} void Cplex::set_tol_integer(double v){check(get_dynamic_lib().CPXsetdblparam(m_env,idol_CPXPARAM_MIP_Tolerances_Integrality,v));Optimizer::set_tol_integer(v);}

void Cplex::create_callback_if_not_exists(){if(!m_cplex_callback)m_cplex_callback=std::make_unique<idol::CplexCallbackI>(*this);} void Cplex::add_callback(idol::Callback* c){create_callback_if_not_exists();m_cplex_callback->add_callback(c);} void Cplex::set_lazy_cuts(bool v){m_lazy_cut=v;create_callback_if_not_exists();}
idol::CplexCallbackI& Cplex::get_cplex_callback_interface(){if(!m_cplex_callback)throw idol::Exception("Cplex callback is not created.");return *m_cplex_callback;}

idol::Model Cplex::read_from_file(idol::Env& env,const std::string& filename){
    auto& lib=get_dynamic_lib();int error=0;CPXENVptr cpx_env=lib.CPXopenCPLEX(&error);
    if(!cpx_env||error)throw idol::Exception("Could not open CPLEX environment for file import.");
    CPXLPptr problem=lib.CPXcreateprob(cpx_env,&error,"idol-import");
    const auto cleanup=[&](){if(problem)lib.CPXfreeprob(cpx_env,&problem);if(cpx_env)lib.CPXcloseCPLEX(&cpx_env);};
    if(error||!problem){cleanup();throw idol::Exception("Could not create CPLEX model for file import.");}
    error=lib.CPXreadcopyprob(cpx_env,problem,filename.c_str(),nullptr);
    if(error){cleanup();throw idol::Exception("CPLEX could not read model file: "+filename);}
    if(lib.CPXgetnumobjs(cpx_env,problem)>1||lib.CPXgetnumindconstrs(cpx_env,problem)>0||lib.CPXgetnumpwl(cpx_env,problem)>0){cleanup();throw idol::Exception("CPLEX file contains multi-objective, indicator, or piecewise-linear constructs unsupported by idol.");}
    const int n=lib.CPXgetnumcols(cpx_env,problem),m=lib.CPXgetnumrows(cpx_env,problem);idol::Model result(env);result.reserve_vars(n);result.reserve_ctrs(m);
    std::vector<double> obj(n),lb(n),ub(n);std::vector<char> type(n,idol_CPX_CONTINUOUS);if(n){lib.CPXgetobj(cpx_env,problem,obj.data(),0,n-1);lib.CPXgetlb(cpx_env,problem,lb.data(),0,n-1);lib.CPXgetub(cpx_env,problem,ub.data(),0,n-1);lib.CPXgetctype(cpx_env,problem,type.data(),0,n-1);}
    auto names=[&](auto getter,int count){if(!count)return std::vector<std::string>{};std::vector<char*> ptrs(count);int surplus=0;getter(cpx_env,problem,ptrs.data(),nullptr,0,&surplus,0,count-1);std::vector<char> store(std::max(1,-surplus));getter(cpx_env,problem,ptrs.data(),store.data(),(int)store.size(),&surplus,0,count-1);std::vector<std::string> out;out.reserve(count);for(char* p:ptrs)out.emplace_back(p?p:"");return out;};
    const auto var_names=names(lib.CPXgetcolname,n);
    const double sign=lib.CPXgetobjsen(cpx_env,problem)==idol_CPX_MAX?-1.:1.;
    for(int j=0;j<n;++j){idol::VarType t;if(type[j]==idol_CPX_CONTINUOUS)t=idol::Continuous;else if(type[j]==idol_CPX_BINARY)t=idol::Binary;else if(type[j]==idol_CPX_INTEGER)t=idol::Integer;else{cleanup();throw idol::Exception("CPLEX file contains a variable type unsupported by idol.");}result.add_var(lb[j],ub[j],t,sign*obj[j],var_names[j]);}
    double offset=0;lib.CPXgetobjoffset(cpx_env,problem,&offset);result.set_obj_const(sign*offset);
    if(lib.CPXgetnumquad(cpx_env,problem)>0){int qnz=0,qsurplus=0;std::vector<int>qbeg(n);lib.CPXgetquad(cpx_env,problem,&qnz,qbeg.data(),nullptr,nullptr,0,&qsurplus,0,n-1);std::vector<int>qind(std::max(1,-qsurplus));std::vector<double>qval(qind.size());lib.CPXgetquad(cpx_env,problem,&qnz,qbeg.data(),qind.data(),qval.data(),(int)qind.size(),&qsurplus,0,n-1);auto expression=result.get_obj_expr();for(int column=0;column<n;++column){const int end=column+1<n?qbeg[column+1]:qnz;for(int k=qbeg[column];k<end;++k)expression+=sign*.5*qval[k]*result.get_var_by_index(column)*result.get_var_by_index(qind[k]);}result.set_obj_expr(std::move(expression));}
    std::vector<double> rhs(m),range(m);std::vector<char>sense(m);if(m){lib.CPXgetrhs(cpx_env,problem,rhs.data(),0,m-1);lib.CPXgetrngval(cpx_env,problem,range.data(),0,m-1);lib.CPXgetsense(cpx_env,problem,sense.data(),0,m-1);}const auto row_names=names(lib.CPXgetrowname,m);
    int nz=0,surplus=0;std::vector<int>beg(m);if(m)lib.CPXgetrows(cpx_env,problem,&nz,beg.data(),nullptr,nullptr,0,&surplus,0,m-1);std::vector<int>ind(std::max(1,-surplus));std::vector<double>val(ind.size());if(m)lib.CPXgetrows(cpx_env,problem,&nz,beg.data(),ind.data(),val.data(),(int)ind.size(),&surplus,0,m-1);
    for(int i=0;i<m;++i){idol::LinExpr<idol::Var> expr;const int end=i+1<m?beg[i+1]:nz;for(int k=beg[i];k<end;++k)expr+=val[k]*result.get_var_by_index(ind[k]);if(sense[i]==idol_CPX_LE)result.add_ctr(expr<=rhs[i],row_names[i]);else if(sense[i]==idol_CPX_GE)result.add_ctr(expr>=rhs[i],row_names[i]);else if(sense[i]==idol_CPX_EQ)result.add_ctr(expr==rhs[i],row_names[i]);else if(sense[i]=='R'){const double other=rhs[i]+range[i];result.add_ctr(expr>=std::min(rhs[i],other),row_names[i]+"_lb");result.add_ctr(expr<=std::max(rhs[i],other),row_names[i]+"_ub");}else{cleanup();throw idol::Exception("CPLEX file contains an unsupported row sense.");}}
    const int nq=lib.CPXgetnumqconstrs(cpx_env,problem);for(int q=0;q<nq;++q){int lnz=0,qnz=0,ls=0,qs=0;double qr=0;char qtype=0;lib.CPXgetqconstr(cpx_env,problem,&lnz,&qnz,&qr,&qtype,nullptr,nullptr,0,&ls,nullptr,nullptr,nullptr,0,&qs,q);std::vector<int>li(std::max(1,-ls)),qi(std::max(1,-qs)),qj(qi.size());std::vector<double>lv(li.size()),qv(qi.size());lib.CPXgetqconstr(cpx_env,problem,&lnz,&qnz,&qr,&qtype,li.data(),lv.data(),(int)li.size(),&ls,qi.data(),qj.data(),qv.data(),(int)qi.size(),&qs,q);int name_surplus=0;lib.CPXgetqconstrname(cpx_env,problem,nullptr,0,&name_surplus,q);std::vector<char>name(std::max(1,-name_surplus));lib.CPXgetqconstrname(cpx_env,problem,name.data(),(int)name.size(),&name_surplus,q);idol::QuadExpr<idol::Var> expr;for(int k=0;k<lnz;++k)expr+=lv[k]*result.get_var_by_index(li[k]);for(int k=0;k<qnz;++k)expr+=qv[k]*result.get_var_by_index(qi[k])*result.get_var_by_index(qj[k]);expr-=qr;if(qtype==idol_CPX_LE)result.add_qctr(std::move(expr),idol::LessOrEqual,name.data());else if(qtype==idol_CPX_GE)result.add_qctr(std::move(expr),idol::GreaterOrEqual,name.data());else if(qtype==idol_CPX_EQ)result.add_qctr(std::move(expr),idol::Equal,name.data());else{cleanup();throw idol::Exception("CPLEX file contains an unsupported quadratic row sense.");}}
    const int ns=lib.CPXgetnumsos(cpx_env,problem);if(ns){int snz=0,ss=0;std::vector<char>st(ns);std::vector<int>sbeg(ns);lib.CPXgetsos(cpx_env,problem,&snz,st.data(),sbeg.data(),nullptr,nullptr,0,&ss,0,ns-1);std::vector<int>si(std::max(1,-ss));std::vector<double>sw(si.size());lib.CPXgetsos(cpx_env,problem,&snz,st.data(),sbeg.data(),si.data(),sw.data(),(int)si.size(),&ss,0,ns-1);const auto sos_names=names(lib.CPXgetsosname,ns);for(int s=0;s<ns;++s){const int end=s+1<ns?sbeg[s+1]:snz;std::vector<idol::Var>vars;std::vector<double>weights;for(int k=sbeg[s];k<end;++k){vars.push_back(result.get_var_by_index(si[k]));weights.push_back(sw[k]);}if(st[s]!=idol_CPX_TYPE_SOS1&&st[s]!=idol_CPX_TYPE_SOS2){cleanup();throw idol::Exception("CPLEX file contains an unsupported SOS type.");}result.add_sosctr(st[s]==idol_CPX_TYPE_SOS1,std::move(vars),std::move(weights),sos_names[s]);}}
    cleanup();return result;
}
