#ifndef IDOL_OPTIMIZERS_CPLEX_H
#define IDOL_OPTIMIZERS_CPLEX_H

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "idol/general/optimizers/OptimizerWithLazyUpdates.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/headers/header_cplex.h"

namespace idol { class CplexCallbackI; }
namespace idol::Optimizers { class Cplex; }

class idol::Optimizers::Cplex : public OptimizerWithLazyUpdates<int, int, int, int> {
    friend class ::idol::CplexCallbackI;
public:
    class DynamicLib;
private:
    static std::unique_ptr<DynamicLib> m_dynamic_lib;
    CPXENVptr m_env = nullptr;
    CPXLPptr m_model = nullptr;
    bool m_continuous_relaxation;
    bool m_is_mip = false;
    bool m_has_quadratic_objective = false;
    bool m_lazy_cut = false;
    unsigned int m_solution_index = 0;
    std::optional<std::vector<double>> m_primal_ray;
    std::optional<std::vector<double>> m_farkas;
    std::unique_ptr<CplexCallbackI> m_cplex_callback;
    char cplex_var_type(int) const;
    static char cplex_ctr_type(int);
    static double cplex_numeric(double);
    [[nodiscard]] std::pair<SolutionStatus, SolutionReason> cplex_status(int) const;
    void check(int) const;
    void create_callback_if_not_exists();
    void save_infeasibility_information();
protected:
    void hook_build() override;
    void hook_optimize() override;
    void hook_write(const std::string&) override;
    int hook_add(const Var&, bool) override;
    int hook_add(const Ctr&) override;
    int hook_add(const QCtr&) override;
    int hook_add(const SOSCtr&) override;
    void hook_update(const Var&) override;
    void hook_update(const Ctr&) override;
    void hook_update_matrix(const Ctr&, const Var&, double) override;
    void hook_update_objective() override;
    void hook_update_rhs() override;
    void hook_update() override;
    void hook_remove(const Var&) override;
    void hook_remove(const Ctr&) override;
    void hook_remove(const QCtr&) override;
    void hook_remove(const SOSCtr&) override;
    [[nodiscard]] SolutionStatus get_status() const override;
    [[nodiscard]] SolutionReason get_reason() const override;
    [[nodiscard]] double get_best_obj() const override;
    [[nodiscard]] double get_best_bound() const override;
    [[nodiscard]] double get_var_primal(const Var&) const override;
    [[nodiscard]] double get_var_reduced_cost(const Var&) const override;
    [[nodiscard]] double get_var_ray(const Var&) const override;
    [[nodiscard]] double get_ctr_dual(const Ctr&) const override;
    [[nodiscard]] double get_ctr_farkas(const Ctr&) const override;
    [[nodiscard]] double get_relative_gap() const override;
    [[nodiscard]] double get_absolute_gap() const override;
    [[nodiscard]] unsigned int get_n_solutions() const override;
    [[nodiscard]] unsigned int get_solution_index() const override;
    void set_solution_index(unsigned int) override;
public:
    Cplex(const Model&, bool);
    ~Cplex() override;
    [[nodiscard]] std::string name() const override { return "Cplex"; }
    [[nodiscard]] bool lazy_cuts() const { return m_lazy_cut; }
    void set_lazy_cuts(bool);
    void add_callback(Callback*);
    void set_max_n_solution_in_pool(unsigned int);
    void set_param_time_limit(double) override;
    void set_param_threads(unsigned int) override;
    void set_param_best_obj_stop(double) override;
    void set_param_best_bound_stop(double) override;
    void set_param_presolve(bool) override;
    void set_param_infeasible_or_unbounded_info(bool) override;
    void set_param_logs(bool) override;
    void set_tol_mip_relative_gap(double) override;
    void set_tol_mip_absolute_gap(double) override;
    void set_tol_feasibility(double) override;
    void set_tol_optimality(double) override;
    void set_tol_integer(double) override;
    static bool is_available();
    static std::string get_version();
    static DynamicLib& get_dynamic_lib(bool = true);
    static Model read_from_file(Env&, const std::string&);
    CplexCallbackI& get_cplex_callback_interface();
};

class idol::Optimizers::Cplex::DynamicLib {
    void* m_handle = nullptr;
    static std::string find_library();
public:
    using version_t=const char*(IDOL_CPXPUBLIC*)(CPXCENVptr); using open_t=CPXENVptr(IDOL_CPXPUBLIC*)(int*);
    using close_t=int(IDOL_CPXPUBLIC*)(CPXENVptr*); using create_t=CPXLPptr(IDOL_CPXPUBLIC*)(CPXCENVptr,int*,const char*);
    using free_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr*); using error_t=char*(IDOL_CPXPUBLIC*)(CPXCENVptr,int,char*);
    version_t CPXversion=nullptr; open_t CPXopenCPLEX=nullptr; close_t CPXcloseCPLEX=nullptr; create_t CPXcreateprob=nullptr;
    free_t CPXfreeprob=nullptr; error_t CPXgeterrorstring=nullptr;
    using addcols_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,int,const double*,const int*,const int*,const double*,const double*,const double*,char**);
    using addrows_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,int,int,const double*,const char*,const int*,const int*,const double*,char**,char**);
    using addqconstr_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,int,double,int,const int*,const double*,const int*,const int*,const double*,const char*);
    using addsos_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,int,const char*,const int*,const int*,const double*,char**);
    addcols_t CPXaddcols=nullptr; addrows_t CPXaddrows=nullptr; addqconstr_t CPXaddqconstr=nullptr; addsos_t CPXaddsos=nullptr;
    using opt_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr); opt_t CPXlpopt=nullptr, CPXmipopt=nullptr, CPXqpopt=nullptr, CPXbaropt=nullptr;
    using stat_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr); stat_t CPXgetstat=nullptr;
    using solninfo_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,int*,int*,int*,int*); solninfo_t CPXsolninfo=nullptr;
    using getdbl_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,double*); getdbl_t CPXgetobjval=nullptr,CPXgetbestobjval=nullptr,CPXgetmiprelgap=nullptr;
    using getvec_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,double*,int,int); getvec_t CPXgetx=nullptr,CPXgetpi=nullptr,CPXgetdj=nullptr;
    using getray_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,double*); getray_t CPXgetray=nullptr;
    using farkas_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,double*,double*); farkas_t CPXdualfarkas=nullptr;
    using chgbds_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,const int*,const char*,const double*);
    using chgctype_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,const int*,const char*);
    using chgvec_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,const int*,const double*);
    using chgsense_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,const int*,const char*);
    using chgcoef_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,int,double);
    using chgoffset_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,double); using chgsen_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int);
    chgbds_t CPXchgbds=nullptr; chgctype_t CPXchgctype=nullptr; chgvec_t CPXchgobj=nullptr,CPXchgrhs=nullptr;
    chgsense_t CPXchgsense=nullptr; chgcoef_t CPXchgcoef=nullptr; chgoffset_t CPXchgobjoffset=nullptr; chgsen_t CPXchgobjsen=nullptr;
    chgcoef_t CPXchgqpcoef=nullptr;
    using chgprob_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int); chgprob_t CPXchgprobtype=nullptr;
    using del_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,int,int); del_t CPXdelcols=nullptr,CPXdelrows=nullptr,CPXdelqconstrs=nullptr,CPXdelsos=nullptr;
    using setint_t=int(IDOL_CPXPUBLIC*)(CPXENVptr,int,int); using setdbl_t=int(IDOL_CPXPUBLIC*)(CPXENVptr,int,double);
    setint_t CPXsetintparam=nullptr; setdbl_t CPXsetdblparam=nullptr;
    using write_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,const char*,const char*); write_t CPXwriteprob=nullptr;
    using read_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXLPptr,const char*,const char*); read_t CPXreadcopyprob=nullptr;
    using getnum_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr); getnum_t CPXgetnumcols=nullptr,CPXgetnumrows=nullptr,CPXgetnumqconstrs=nullptr,CPXgetnumsos=nullptr;
    getnum_t CPXgetnumindconstrs=nullptr,CPXgetnumpwl=nullptr,CPXgetnumobjs=nullptr,CPXgetnumquad=nullptr;
    using getarray_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,double*,int,int);
    using getchararray_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,char*,int,int);
    using getscalar_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,double*);
    using getname_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,char**,char*,int,int*,int,int);
    using getrows_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,int*,int*,int*,double*,int,int*,int,int);
    getarray_t CPXgetobj=nullptr,CPXgetlb=nullptr,CPXgetub=nullptr,CPXgetrhs=nullptr,CPXgetrngval=nullptr;
    getchararray_t CPXgetctype=nullptr,CPXgetsense=nullptr; getscalar_t CPXgetobjoffset=nullptr;
    getnum_t CPXgetobjsen=nullptr; getname_t CPXgetcolname=nullptr,CPXgetrowname=nullptr,CPXgetsosname=nullptr; getrows_t CPXgetrows=nullptr;
    using getonename_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,char*,int,int*,int); getonename_t CPXgetqconstrname=nullptr;
    using getquad_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,int*,int*,int*,double*,int,int*,int,int);
    using getqctr_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,int*,int*,double*,char*,int*,double*,int,int*,int*,int*,double*,int,int*,int);
    using getsos_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,int*,char*,int*,int*,double*,int,int*,int,int);
    getquad_t CPXgetquad=nullptr; getqctr_t CPXgetqconstr=nullptr; getsos_t CPXgetsos=nullptr;
    using poolnum_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr); poolnum_t CPXgetsolnpoolnumsolns=nullptr;
    using poolx_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,CPXCLPptr,int,double*,int,int); poolx_t CPXgetsolnpoolx=nullptr;
    using setcutcb_t=int(IDOL_CPXPUBLIC*)(CPXENVptr,idol_cpx_cut_callback,void*);
    using setheurcb_t=int(IDOL_CPXPUBLIC*)(CPXENVptr,idol_cpx_heuristic_callback,void*);
    using setinfocb_t=int(IDOL_CPXPUBLIC*)(CPXENVptr,idol_cpx_info_callback,void*);
    setcutcb_t CPXsetusercutcallbackfunc=nullptr,CPXsetlazyconstraintcallbackfunc=nullptr;
    setheurcb_t CPXsetheuristiccallbackfunc=nullptr;
    setinfocb_t CPXsetinfocallbackfunc=nullptr;
    using cbinfo_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,void*,int,int,void*);
    using cbx_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,void*,int,double*,int,int);
    using cutadd_t=int(IDOL_CPXPUBLIC*)(CPXCENVptr,void*,int,int,double,int,const int*,const double*,int);
    cbinfo_t CPXgetcallbackinfo=nullptr; cbx_t CPXgetcallbacknodex=nullptr; cutadd_t CPXcutcallbackadd=nullptr;
    using terminate_t=int(IDOL_CPXPUBLIC*)(CPXENVptr,volatile int*); terminate_t CPXsetterminate=nullptr;
    DynamicLib(); ~DynamicLib(); [[nodiscard]] bool is_available() const { return m_handle; }
};

#endif
