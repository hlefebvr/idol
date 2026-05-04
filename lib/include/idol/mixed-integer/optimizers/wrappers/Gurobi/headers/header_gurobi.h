//
// Created by Henri on 18/01/2026.
//

#ifndef IDOL_HEADER_GUROBI_H
#define IDOL_HEADER_GUROBI_H

// Gurobi status
#define idol_GRB_LOADED              1
#define idol_GRB_OPTIMAL             2
#define idol_GRB_INFEASIBLE          3
#define idol_GRB_INF_OR_UNBD         4
#define idol_GRB_UNBOUNDED           5
#define idol_GRB_CUTOFF              6
#define idol_GRB_ITERATION_LIMIT     7
#define idol_GRB_NODE_LIMIT          8
#define idol_GRB_TIME_LIMIT          9
#define idol_GRB_SOLUTION_LIMIT     10
#define idol_GRB_INTERRUPTED        11
#define idol_GRB_NUMERIC            12
#define idol_GRB_SUBOPTIMAL         13
#define idol_GRB_INPROGRESS         14
#define idol_GRB_USER_OBJ_LIMIT     15
#define idol_GRB_WORK_LIMIT         16
#define idol_GRB_MEM_LIMIT          17
#define idol_GRB_LOCALLY_OPTIMAL    18
#define idol_GRB_LOCALLY_INFEASIBLE 19

// Gurobi numeric constants
#define idol_GRB_INFINITY  1e100
#define idol_GRB_UNDEFINED 1e101
#define idol_GRB_MAXINT    2000000000

// Callback code
#define idol_GRB_CB_POLLING   0
#define idol_GRB_CB_PRESOLVE  1
#define idol_GRB_CB_SIMPLEX   2
#define idol_GRB_CB_MIP       3
#define idol_GRB_CB_MIPSOL    4
#define idol_GRB_CB_MIPNODE   5
#define idol_GRB_CB_MESSAGE   6
#define idol_GRB_CB_BARRIER   7
#define idol_GRB_CB_MULTIOBJ  8
#define idol_GRB_CB_IIS       9
#define idol_GRB_CB_PDHG     10
#define idol_GRB_CB_NLBAR    11
#define idol_GRB_CB_MAX      11

// Callback what
#define idol_GRB_CB_MIP_OBJBST         3000
#define idol_GRB_CB_MIP_OBJBND         3001
#define idol_GRB_CB_MIPSOL_SOL            4001
#define idol_GRB_CB_MIPSOL_OBJ            4002
#define idol_GRB_CB_MIPNODE_STATUS         5001
#define idol_GRB_CB_MIPNODE_REL            5002

extern "C" {

    typedef struct _GRBmodel GRBmodel;
    typedef struct _GRBenv GRBenv;

    // Gurobi functions
    void idol_GRBversion(int *majorP, int *minorP, int *technicalP);
    int idol_GRBloadenvinternal(GRBenv **envP, const char *logfilename, int major, int minor, int tech);
    int idol_GRBloadenv(GRBenv **envP, const char *logfilename);
    int idol_GRBgetintattr(GRBmodel *model, const char *attrname, int *valueP);
    const char* idol_GRBgeterrormsg(GRBenv *env);
    void idol_GRBfreeenv(GRBenv *env);
    GRBenv* idol_GRBgetenv(GRBmodel *model);
    int idol_GRBnewmodel(GRBenv *env, GRBmodel **modelP, const char *Pname, int numvars, double *obj, double *lb, double *ub, char *vtype, char **varnames);
    int idol_GRBfreemodel(GRBmodel *model);
    int idol_GRBaddvar(GRBmodel *model, int numnz, int *vind, double *vval, double obj, double lb, double ub, char vtype, const char *varname);
    int idol_GRBaddconstr(GRBmodel *model, int numnz, int *cind, double *cval, char sense, double rhs, const char *constrname);
    int idol_GRBaddqconstr(GRBmodel *model, int numlnz, int *lind, double *lval, int numqnz, int *qrow, int *qcol, double *qval, char sense, double rhs, const char *QCname);
    int idol_GRBsetdblattrelement(GRBmodel *model, const char *attrname, int element, double newvalue);
    int idol_GRBsetcharattrelement(GRBmodel *model, const char *attrname, int element, char newvalue);
    int idol_GRBsetobjective(GRBmodel *model, int sense, double constant, int lnz, int *lind, double *lval, int qnz, int *qrow, int *qcol, double *qval);
    int idol_GRBdelvars(GRBmodel *model, int len, int *ind);
    int idol_GRBdelconstrs(GRBmodel *model, int len, int *ind);
    int idol_GRBdelsos(GRBmodel *model, int len, int *ind);
    int idol_GRBoptimize(GRBmodel *model);
    int idol_GRBwrite(GRBmodel *model, const char *filename);
    int idol_GRBsetintattr(GRBmodel *model, const char *attrname, int newvalue);
    int idol_GRBchgcoeffs(GRBmodel *model, int cnt, int *cind, int *vind, double *val);
    int idol_GRBupdatemodel(GRBmodel *model);
    int idol_GRBsetintparam(GRBenv *env, const char *paramname, int value);
    int idol_GRBsetdblparam(GRBenv *env, const char *paramname, double value);
    int idol_GRBsetstrparam(GRBenv *env, const char *paramname, const char *value);
    int idol_GRBsetcallbackfunc(GRBmodel *model, int (*cb)(GRBmodel *model, void *cbdata, int where, void *usrdata), void  *usrdata);
    int idol_GRBgetintparam(GRBenv *env, const char *paramname, int *valueP);
    int idol_GRBgetdblparam(GRBenv *env, const char *paramname, double *valueP);
    int idol_GRBgetstrparam(GRBenv *env, const char *paramname, char *valueP);
    int idol_GRBgetdblattr(GRBmodel *model, const char *attrname, double *valueP);
    int idol_GRBgetdblattrarray(GRBmodel *model, const char *attrname, int first, int len, double *values);
    int idol_GRBgetdblattrelement(GRBmodel *model, const char *attrname, int element, double *valueP);
    int idol_GRBsetdblattr(GRBmodel *model, const char *attrname, double newvalue);
    int idol_GRBdelqconstrs(GRBmodel *model, int len, int *ind);
    int idol_GRBaddsos(GRBmodel *model, int numsos, int nummembers, int *types, int *beg, int *ind, double *weight);
    int idol_GRBcbget(void *cbdata, int where, int what, void *resultP);
    int idol_GRBcbsolution(void *cbdata, const double *solution, double *objvalP);
    int idol_GRBcbcut(void *cbdata, int cutlen, const int *cutind, const double *cutval, char cutsense, double cutrhs);
    int idol_GRBcblazy(void *cbdata, int lazylen, const int *lazyind, const double *lazyval, char lazysense, double lazyrhs);
    int idol_GRBreadmodel(GRBenv *env, const char *filename, GRBmodel **modelP);
    int idol_GRBgetcharattrarray(GRBmodel *model, const char *attrname, int start, int len, char *values);
    int idol_GRBgetstrattrarray(GRBmodel *model, const char *attrname, int start, int len, char **values);
    int idol_GRBgetconstrs(GRBmodel *model, int *numnzP, int *cbeg, int *cind, double *cval, int start, int len);
    void idol_GRBterminate(GRBmodel *model);

}

#endif //IDOL_HEADER_GUROBI_H