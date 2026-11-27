#ifndef IDOL_HEADER_CPLEX_H
#define IDOL_HEADER_CPLEX_H

#include <cstdint>

#if defined(_WIN32)
#define IDOL_CPXPUBLIC __stdcall
#else
#define IDOL_CPXPUBLIC
#endif

extern "C" {

typedef long long CPXLONG;
struct cpxenv;
struct cpxlp;
typedef cpxenv* CPXENVptr;
typedef const cpxenv* CPXCENVptr;
typedef cpxlp* CPXLPptr;
typedef const cpxlp* CPXCLPptr;

typedef int (IDOL_CPXPUBLIC *idol_cpx_heuristic_callback)(CPXCENVptr, void*, int, void*, double*, double*, int*, int*);
typedef int (IDOL_CPXPUBLIC *idol_cpx_cut_callback)(CPXCENVptr, void*, int, void*, int*);
typedef int (IDOL_CPXPUBLIC *idol_cpx_info_callback)(CPXCENVptr, void*, int, void*);

}

#define idol_CPX_INFBOUND 1.0e20
#define idol_CPX_MIN 1
#define idol_CPX_MAX -1
#define idol_CPX_CONTINUOUS 'C'
#define idol_CPX_BINARY 'B'
#define idol_CPX_INTEGER 'I'
#define idol_CPX_LE 'L'
#define idol_CPX_GE 'G'
#define idol_CPX_EQ 'E'
#define idol_CPX_TYPE_SOS1 '1'
#define idol_CPX_TYPE_SOS2 '2'

#define idol_CPX_STAT_OPTIMAL 1
#define idol_CPX_STAT_UNBOUNDED 2
#define idol_CPX_STAT_INFEASIBLE 3
#define idol_CPX_STAT_INForUNBD 4
#define idol_CPX_STAT_OPTIMAL_INFEAS 5
#define idol_CPX_STAT_ABORT_IT_LIM 10
#define idol_CPX_STAT_ABORT_TIME_LIM 11
#define idol_CPX_STAT_ABORT_OBJ_LIM 12
#define idol_CPX_STAT_ABORT_USER 13
#define idol_CPX_STAT_ABORT_PRIM_OBJ_LIM 21
#define idol_CPX_STAT_ABORT_DUAL_OBJ_LIM 22
#define idol_CPX_STAT_FEASIBLE 23

#define idol_CPXMIP_OPTIMAL 101
#define idol_CPXMIP_OPTIMAL_TOL 102
#define idol_CPXMIP_INFEASIBLE 103
#define idol_CPXMIP_SOL_LIM 104
#define idol_CPXMIP_NODE_LIM_FEAS 105
#define idol_CPXMIP_NODE_LIM_INFEAS 106
#define idol_CPXMIP_TIME_LIM_FEAS 107
#define idol_CPXMIP_TIME_LIM_INFEAS 108
#define idol_CPXMIP_FAIL_FEAS 109
#define idol_CPXMIP_FAIL_INFEAS 110
#define idol_CPXMIP_MEM_LIM_FEAS 111
#define idol_CPXMIP_MEM_LIM_INFEAS 112
#define idol_CPXMIP_ABORT_FEAS 113
#define idol_CPXMIP_ABORT_INFEAS 114
#define idol_CPXMIP_OPTIMAL_INFEAS 115
#define idol_CPXMIP_FAIL_FEAS_NO_TREE 116
#define idol_CPXMIP_FAIL_INFEAS_NO_TREE 117
#define idol_CPXMIP_POPULATESOL_LIM 128
#define idol_CPXMIP_FEASIBLE 127
#define idol_CPXMIP_OPTIMAL_POPULATED 129
#define idol_CPXMIP_OPTIMAL_POPULATED_TOL 130
#define idol_CPXMIP_DETTIME_LIM_FEAS 131
#define idol_CPXMIP_DETTIME_LIM_INFEAS 132
#define idol_CPXMIP_ABORT_RELAXATION_UNBOUNDED 133
#define idol_CPXMIP_UNBOUNDED 118
#define idol_CPXMIP_INForUNBD 119

#define idol_CPXPARAM_ScreenOutput 1035
#define idol_CPXPARAM_TimeLimit 1039
#define idol_CPXPARAM_Threads 1067
#define idol_CPXPARAM_LPMethod 1062
#define idol_CPXPARAM_Preprocessing_Presolve 1030
#define idol_CPXPARAM_Simplex_Tolerances_Feasibility 1016
#define idol_CPXPARAM_Simplex_Tolerances_Optimality 1014
#define idol_CPXPARAM_MIP_Tolerances_AbsMIPGap 2008
#define idol_CPXPARAM_MIP_Tolerances_MIPGap 2009
#define idol_CPXPARAM_MIP_Tolerances_Integrality 2010
#define idol_CPXPARAM_MIP_Pool_Capacity 2103
#define idol_CPXPARAM_MIP_Strategy_Search 2109
#define idol_CPXPARAM_MIP_Limits_LowerObjStop 2233
#define idol_CPXPARAM_MIP_Limits_UpperObjStop 2234
#define idol_CPX_MIPSEARCH_TRADITIONAL 1
#define idol_CPX_ALG_DUAL 2
#define idol_CPX_ALG_PRIMAL 1
#define idol_CPXPROB_LP 0
#define idol_CPXPROB_MILP 1
#define idol_CPXPROB_QP 5
#define idol_CPXPROB_MIQP 7

#define idol_CPX_CALLBACK_DEFAULT 0
#define idol_CPX_CALLBACK_FAIL 1
#define idol_CPX_CALLBACK_SET 2
#define idol_CPX_CALLBACK_ABORT_CUT_LOOP 3
#define idol_CPX_CALLBACK_INFO_BEST_INTEGER 101
#define idol_CPX_CALLBACK_INFO_BEST_REMAINING 102
#define idol_CPX_CALLBACK_INFO_NODE_COUNT_LONG 140
#define idol_CPX_CALLBACK_MIP_INCUMBENT_HEURSOLN 117
#define idol_CPX_CALLBACK_MIP_INCUMBENT_USERSOLN 118
#define idol_CPX_CALLBACK_MIP_INCUMBENT_MIPSTART 119
#define idol_CPX_CALLBACK_MIP_INCUMBENT_NODESOLN 111
#define idol_CPX_USECUT_FORCE 0
#endif
