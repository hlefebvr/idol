//
// Created by Henri on 12/06/2026.
//

#ifndef IDOL_HEADER_CPLEX_H
#define IDOL_HEADER_CPLEX_H

// LP status codes
#define idol_CPX_STAT_OPTIMAL              1
#define idol_CPX_STAT_UNBOUNDED            2
#define idol_CPX_STAT_INFEASIBLE           3
#define idol_CPX_STAT_INForUNBD            4
#define idol_CPX_STAT_OPTIMAL_INFEAS       5
#define idol_CPX_STAT_NUM_BEST             6
#define idol_CPX_STAT_FEASIBLE             23
#define idol_CPX_STAT_ABORT_TIME_LIM       11
#define idol_CPX_STAT_ABORT_OBJ_LIM        12
#define idol_CPX_STAT_ABORT_DUAL_OBJ_LIM   22
#define idol_CPX_STAT_ABORT_PRIM_OBJ_LIM   21

// MIP status codes
#define idol_CPXMIP_OPTIMAL              101
#define idol_CPXMIP_OPTIMAL_TOL          102
#define idol_CPXMIP_INFEASIBLE           103
#define idol_CPXMIP_SOL_LIM              104
#define idol_CPXMIP_NODE_LIM_FEAS        105
#define idol_CPXMIP_NODE_LIM_INFEAS      106
#define idol_CPXMIP_TIME_LIM_FEAS        107
#define idol_CPXMIP_TIME_LIM_INFEAS      108
#define idol_CPXMIP_FAIL_FEAS            109
#define idol_CPXMIP_FAIL_INFEAS          110
#define idol_CPXMIP_MEM_LIM_FEAS         111
#define idol_CPXMIP_MEM_LIM_INFEAS       112
#define idol_CPXMIP_ABORT_FEAS           113
#define idol_CPXMIP_ABORT_INFEAS         114
#define idol_CPXMIP_OPTIMAL_INFEAS       115
#define idol_CPXMIP_FAIL_FEAS_NO_TREE    116
#define idol_CPXMIP_FAIL_INFEAS_NO_TREE  117
#define idol_CPXMIP_UNBOUNDED            118
#define idol_CPXMIP_INForUNBD            119
#define idol_CPXMIP_FEASIBLE             127

// Objective sense
#define idol_CPX_MIN   1
#define idol_CPX_MAX  -1

// Variable types
#define idol_CPX_CONTINUOUS  'C'
#define idol_CPX_BINARY      'B'
#define idol_CPX_INTEGER     'I'

// Bound change type
#define idol_CPX_LOWER  'L'
#define idol_CPX_UPPER  'U'

// Infinity
#define idol_CPX_INFBOUND  1e+30

// Parameters
#define idol_CPX_PARAM_EPAGAP    2008
#define idol_CPX_PARAM_EPGAP     2009
#define idol_CPX_PARAM_EPINT     2010
#define idol_CPX_PARAM_EPOPT     1014
#define idol_CPX_PARAM_EPRHS     1016
#define idol_CPX_PARAM_OBJULIM   1148
#define idol_CPX_PARAM_OBJLLIM   1149
#define idol_CPX_PARAM_LPMETHOD  1006
#define idol_CPX_PARAM_PREIND    1030
#define idol_CPX_PARAM_SCRIND    1035
#define idol_CPX_PARAM_TILIM     1039
#define idol_CPX_PARAM_THREADS   1067

extern "C" {

    typedef int CPXINT;
    typedef struct cpxenv        *CPXENVptr;
    typedef struct cpxenv const  *CPXCENVptr;
    typedef struct cpxlp         *CPXLPptr;
    typedef const struct cpxlp   *CPXCLPptr;

    CPXENVptr idol_CPXopenCPLEX (int *status_p);
    int idol_CPXcloseCPLEX (CPXENVptr *env_p);
    CPXLPptr idol_CPXcreateprob (CPXCENVptr env, int *status_p, char const *probname_str);
    int idol_CPXfreeprob (CPXCENVptr env, CPXLPptr *lp_p);

    int idol_CPXaddcols (CPXCENVptr env, CPXLPptr lp, int ccnt, int nzcnt,
                         double const *obj, int const *cmatbeg,
                         int const *cmatind, double const *cmatval,
                         double const *lb, double const *ub, char **colname);

    int idol_CPXaddrows (CPXCENVptr env, CPXLPptr lp, int ccnt, int rcnt,
                         int nzcnt, double const *rhs, char const *sense,
                         int const *rmatbeg, int const *rmatind,
                         double const *rmatval, char **colname, char **rowname);

    int idol_CPXchgbds (CPXCENVptr env, CPXLPptr lp, int cnt, int const *indices,
                        char const *lu, double const *bd);

    int idol_CPXchgctype (CPXCENVptr env, CPXLPptr lp, int cnt,
                          int const *indices, char const *xctype);

    int idol_CPXchgobj (CPXCENVptr env, CPXLPptr lp, int cnt, int const *indices,
                        double const *values);

    int idol_CPXchgobjoffset (CPXCENVptr env, CPXLPptr lp, double offset);

    int idol_CPXchgobjsen (CPXCENVptr env, CPXLPptr lp, int maxormin);

    int idol_CPXchgrhs (CPXCENVptr env, CPXLPptr lp, int cnt, int const *indices,
                        double const *values);

    int idol_CPXchgsense (CPXCENVptr env, CPXLPptr lp, int cnt,
                          int const *indices, char const *sense);

    int idol_CPXchgcoef (CPXCENVptr env, CPXLPptr lp, int i, int j, double newvalue);

    int idol_CPXdelcols (CPXCENVptr env, CPXLPptr lp, int begin, int end);
    int idol_CPXdelrows (CPXCENVptr env, CPXLPptr lp, int begin, int end);

    int idol_CPXlpopt (CPXCENVptr env, CPXLPptr lp);
    int idol_CPXmipopt (CPXCENVptr env, CPXLPptr lp);

    int idol_CPXgetstat (CPXCENVptr env, CPXCLPptr lp);
    int idol_CPXgetobjsen (CPXCENVptr env, CPXCLPptr lp);
    int idol_CPXgetobjval (CPXCENVptr env, CPXCLPptr lp, double *objval_p);
    int idol_CPXgetbestobjval (CPXCENVptr env, CPXCLPptr lp, double *objval_p);
    int idol_CPXgetmiprelgap (CPXCENVptr env, CPXCLPptr lp, double *gap_p);

    int idol_CPXgetx (CPXCENVptr env, CPXCLPptr lp, double *x, int begin, int end);
    int idol_CPXgetpi (CPXCENVptr env, CPXCLPptr lp, double *pi, int begin, int end);
    int idol_CPXgetdj (CPXCENVptr env, CPXCLPptr lp, double *dj, int begin, int end);
    int idol_CPXgetray (CPXCENVptr env, CPXCLPptr lp, double *z);
    int idol_CPXdualfarkas (CPXCENVptr env, CPXCLPptr lp, double *y, double *proof_p);

    int idol_CPXgetnumcols (CPXCENVptr env, CPXCLPptr lp);
    int idol_CPXgetnumrows (CPXCENVptr env, CPXCLPptr lp);

    int idol_CPXsetintparam (CPXENVptr env, int whichparam, CPXINT newvalue);
    int idol_CPXsetdblparam (CPXENVptr env, int whichparam, double newvalue);

    int idol_CPXwriteprob (CPXCENVptr env, CPXCLPptr lp,
                           char const *filename_str, char const *filetype);

    int idol_CPXgeterrorstring (CPXCENVptr env, int errcode, char *buffer_str);

    int idol_CPXversionnumber (CPXCENVptr env, int *version_p);

    int idol_CPXgetsolnpoolnumsolns (CPXCENVptr env, CPXCLPptr lp);
    int idol_CPXgetsolnpoolobjval (CPXCENVptr env, CPXCLPptr lp, int soln, double *objval_p);
    int idol_CPXgetsolnpoolx (CPXCENVptr env, CPXCLPptr lp, int soln, double *x, int begin, int end);
    int idol_CPXpopulate (CPXCENVptr env, CPXLPptr lp);

    // SOS constraints  (sostype contains characters '1' or '2', i.e. CPX_TYPE_SOS1/2)
    int idol_CPXaddsos (CPXCENVptr env, CPXLPptr lp, int numsos, int numsosnz,
                        char const *sostype, int const *sosbeg, int const *sosind,
                        double const *soswt, char **sosname);
    int idol_CPXdelsos (CPXCENVptr env, CPXLPptr lp, int begin, int end);
    int idol_CPXgetnumsos (CPXCENVptr env, CPXCLPptr lp);

    // Quadratic constraints
    int idol_CPXaddqconstr (CPXCENVptr env, CPXLPptr lp, int linnzcnt, int quadnzcnt,
                            double rhs, char sense,
                            int const *linind, double const *linval,
                            int const *quadrow, int const *quadcol, double const *quadval,
                            char const *lname_str);
    int idol_CPXdelqconstrs (CPXCENVptr env, CPXLPptr lp, int begin, int end);
    int idol_CPXgetnumqconstrs (CPXCENVptr env, CPXCLPptr lp);

    // Quadratic objective
    int idol_CPXcopyquad (CPXCENVptr env, CPXLPptr lp, int const *qmatbeg, int const *qmatcnt,
                          int const *qmatind, double const *qmatval);

    // QP solver
    int idol_CPXqpopt (CPXCENVptr env, CPXLPptr lp);

}

#endif //IDOL_HEADER_CPLEX_H
