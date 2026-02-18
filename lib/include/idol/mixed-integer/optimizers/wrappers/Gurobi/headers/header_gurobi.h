//
// Created by Henri on 18/01/2026.
//

#ifndef IDOL_HEADER_GUROBI_H
#define IDOL_HEADER_GUROBI_H

// Gurobi status
#define GRB_LOADED              1
#define GRB_OPTIMAL             2
#define GRB_INFEASIBLE          3
#define GRB_INF_OR_UNBD         4
#define GRB_UNBOUNDED           5
#define GRB_CUTOFF              6
#define GRB_ITERATION_LIMIT     7
#define GRB_NODE_LIMIT          8
#define GRB_TIME_LIMIT          9
#define GRB_SOLUTION_LIMIT     10
#define GRB_INTERRUPTED        11
#define GRB_NUMERIC            12
#define GRB_SUBOPTIMAL         13
#define GRB_INPROGRESS         14
#define GRB_USER_OBJ_LIMIT     15
#define GRB_WORK_LIMIT         16
#define GRB_MEM_LIMIT          17
#define GRB_LOCALLY_OPTIMAL    18
#define GRB_LOCALLY_INFEASIBLE 19

// Gurobi numeric constants
#define GRB_INFINITY  1e100
#define GRB_UNDEFINED 1e101
#define GRB_MAXINT    2000000000

// Callback code
#define GRB_CB_POLLING   0
#define GRB_CB_PRESOLVE  1
#define GRB_CB_SIMPLEX   2
#define GRB_CB_MIP       3
#define GRB_CB_MIPSOL    4
#define GRB_CB_MIPNODE   5
#define GRB_CB_MESSAGE   6
#define GRB_CB_BARRIER   7
#define GRB_CB_MULTIOBJ  8
#define GRB_CB_IIS       9
#define GRB_CB_PDHG     10
#define GRB_CB_NLBAR    11
#define GRB_CB_MAX      11

// Callback what
#define GRB_CB_MIP_OBJBST         3000
#define GRB_CB_MIP_OBJBND         3001
#define GRB_CB_MIPSOL_SOL            4001
#define GRB_CB_MIPSOL_OBJ            4002
#define GRB_CB_MIPNODE_STATUS         5001
#define GRB_CB_MIPNODE_REL            5002

extern "C" {

    typedef struct _GRBmodel GRBmodel;
    typedef struct _GRBenv GRBenv;

    // Gurobi functions
    void GRBversion(int *majorP, int *minorP, int *technicalP);
    int GRBloadenvinternal(GRBenv **envP, const char *logfilename, int major, int minor, int tech);
    int GRBgetintattr(GRBmodel *model, const char *attrname, int *valueP);
    const char* GRBgeterrormsg(GRBenv *env);
    void GRBfreeenv(GRBenv *env);
    GRBenv* GRBgetenv(GRBmodel *model);
    int GRBnewmodel(GRBenv *env, GRBmodel **modelP, const char *Pname, int numvars, double *obj, double *lb, double *ub, char *vtype, char **varnames);
    int GRBfreemodel(GRBmodel *model);
    int GRBaddvar(GRBmodel *model, int numnz, int *vind, double *vval, double obj, double lb, double ub, char vtype, const char *varname);
    int GRBaddconstr(GRBmodel *model, int numnz, int *cind, double *cval, char sense, double rhs, const char *constrname);
    int GRBaddqconstr(GRBmodel *model, int numlnz, int *lind, double *lval, int numqnz, int *qrow, int *qcol, double *qval, char sense, double rhs, const char *QCname);
    int GRBsetdblattrelement(GRBmodel *model, const char *attrname, int element, double newvalue);
    int GRBsetcharattrelement(GRBmodel *model, const char *attrname, int element, char newvalue);
    int GRBsetobjective(GRBmodel *model, int sense, double constant, int lnz, int *lind, double *lval, int qnz, int *qrow, int *qcol, double *qval);
    int GRBdelvars(GRBmodel *model, int len, int *ind);
    int GRBdelconstrs(GRBmodel *model, int len, int *ind);
    int GRBdelsos(GRBmodel *model, int len, int *ind);
    int GRBoptimize(GRBmodel *model);
    int GRBwrite(GRBmodel *model, const char *filename);
    int GRBsetintattr(GRBmodel *model, const char *attrname, int newvalue);
    int GRBchgcoeffs(GRBmodel *model, int cnt, int *cind, int *vind, double *val);
    int GRBupdatemodel(GRBmodel *model);
    int GRBsetintparam(GRBenv *env, const char *paramname, int value);
    int GRBsetdblparam(GRBenv *env, const char *paramname, double value);
    int GRBsetstrparam(GRBenv *env, const char *paramname, const char *value);
    int GRBsetcallbackfunc(GRBmodel *model, int (*cb)(GRBmodel *model, void *cbdata, int where, void *usrdata), void  *usrdata);
    int GRBgetintparam(GRBenv *env, const char *paramname, int *valueP);
    int GRBgetdblparam(GRBenv *env, const char *paramname, double *valueP);
    int GRBgetstrparam(GRBenv *env, const char *paramname, char *valueP);
    int GRBgetdblattr(GRBmodel *model, const char *attrname, double *valueP);
    int GRBgetdblattrarray(GRBmodel *model, const char *attrname, int first, int len, double *values);
    int GRBgetdblattrelement(GRBmodel *model, const char *attrname, int element, double *valueP);
    int GRBsetdblattr(GRBmodel *model, const char *attrname, double newvalue);
    int GRBdelqconstrs(GRBmodel *model, int len, int *ind);
    int GRBaddsos(GRBmodel *model, int numsos, int nummembers, int *types, int *beg, int *ind, double *weight);
    int GRBcbget(void *cbdata, int where, int what, void *resultP);
    int GRBcbsolution(void *cbdata, const double *solution, double *objvalP);
    int GRBcbcut(void *cbdata, int cutlen, const int *cutind, const double *cutval, char cutsense, double cutrhs);
    int GRBcblazy(void *cbdata, int lazylen, const int *lazyind, const double *lazyval, char lazysense, double lazyrhs);
    void GRBterminate(GRBmodel *model);

}

#endif //IDOL_HEADER_GUROBI_H