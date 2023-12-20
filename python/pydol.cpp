//
// Created by henri on 15.12.23.
//

#ifndef IDOL_PY_MODELING_CPP
#define IDOL_PY_MODELING_CPP

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <idol/modeling.h>
#include <idol/solvers.h>
#include <idol/problems/generalized-assignment-problem/GAP_Instance.h>

#include <utility>

#define DEF_DEFAULT_PARAMETERS(optimizer) \
.def("with_logs", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_logs) \
.def("with_time_limit", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_time_limit) \
.def("with_thread_limit", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_thread_limit) \
.def("with_iteration_limit", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_iteration_limit) \
.def("with_best_bound_stop", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_best_bound_stop) \
.def("with_best_obj_stop", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_best_obj_stop) \
.def("with_relative_gap_tolerance", ( optimizer& (optimizer::*)(double) ) &optimizer::with_relative_gap_tolerance)  \
.def("with_absolute_gap_tolerance", ( optimizer& (optimizer::*)(double) ) &optimizer::with_absolute_gap_tolerance) \
.def("with_presolve", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_presolve) \
.def("with_infeasible_or_unbounded_info", ( optimizer& (optimizer::*)(bool) ) &optimizer::with_infeasible_or_unbounded_info)

#define COUT(obj) \
std::stringstream ss; ss << obj; return ss.str();

#define DEF_COUT(obj) \
.def("__repr__", [](const obj& t_self) { COUT(t_self) }) \
.def("__str__", [](const obj& t_self) { COUT(t_self) })

namespace py = pybind11;

using namespace idol;

PYBIND11_MODULE(pydol, m) {

    m.doc() = "pydol is a python wrapper for the C++ library idol.";

    py::enum_<VarType>(m, "VarType")
            .value("INTEGER", VarType::Integer)
            .value("CONTINUOUS", VarType::Continuous)
            .value("BINARY", VarType::Binary)
            .export_values();

    py::enum_<CtrType>(m, "CtrType")
            .value("LESS_OR_EQUAL", CtrType::LessOrEqual)
            .value("GREATER_OR_EQUAL", CtrType::GreaterOrEqual)
            .value("EQUAL", CtrType::Equal)
            .export_values();

    py::enum_<ObjectiveSense>(m, "ObjectiveSense")
            .value("MAXIMIZE", ObjectiveSense::Maximize)
            .value("MINIMIZE", ObjectiveSense::Minimize)
            .export_values();

    py::class_<Env>(m, "Env")
            .def(py::init<>())
            .def("__getitem__", [](const Env& t_env, const Var& t_var){ return t_env[t_var]; })
            ;

    py::class_<Param>(m, "Param")
            .def(py::init<const Var&>())
            .def(py::init<const Ctr&>())
            ;

    py::class_<Ctr>(m, "Ctr")
            .def(
                py::init<Env&, TempCtr, std::string>(),
                    "Constructor",
                    py::arg("t_env"),
                    py::arg("t_temp_ctr"),
                    py::arg("t_name") = ""
            )
            .def(
                    py::init<Env&, CtrType, Constant, std::string>(),
                    "Constructor",
                    py::arg("t_env"),
                    py::arg("t_type"),
                    py::arg("t_constant"),
                    py::arg("t_name") = ""
            )

            .def("set", [](Ctr& t_self, const Annotation<Ctr, unsigned int>& t_annotation, unsigned int t_value){ t_self.set(t_annotation, t_value); })
            .def("get", [](Ctr& t_self, const Annotation<Ctr, unsigned int>& t_annotation) { return t_self.get(t_annotation); })

            DEF_COUT(Ctr)
            .def("get_name", &Ctr::name)

            .def("get_id", (unsigned int (Ctr::*)() const) &Ctr::id)
            .def("get_name", (const std::string& (Ctr::*)() const) &Ctr::name)
            ;

    py::class_<Var>(m, "Var")
            .def(py::init<Env&, double, double, VarType, std::string>())

            DEF_COUT(Var)
            .def("get_name", &Var::name)

            .def("get_id", (unsigned int (Var::*)() const) &Var::id)
            .def("get_name", (const std::string& (Var::*)() const) &Var::name)

            // Parametrization
            .def("__inv__", [](const Var& t_self) { return !t_self; })

            // Products
            .def("__mul__", [](const Var& t_a, double t_b){ return t_a * t_b; })
            .def("__rmul__", [](const Var& t_a, double t_b){ return t_a * t_b; })
            .def("__mul__", [](const Var& t_a, const Constant& t_b){ return t_a * t_b; })
            .def("__rmul__", [](const Var& t_a, const Constant& t_b){ return t_a * t_b; })

            // Additions
            .def("__add__", [](const Var& t_a, const Var& t_b){ return t_a + t_b; })
            .def("__add__", [](const Var& t_a, const LinExpr<Var>& t_b) { return t_a + t_b; })
            .def("__radd__", [](const Var& t_a, const LinExpr<Var>& t_b) { return t_a + t_b; })
            .def("__add__", [](const Var& t_a, const QuadExpr<Var, Var>& t_b) { return t_a + t_b; })
            .def("__radd__", [](const Var& t_a, const QuadExpr<Var, Var>& t_b) { return t_a + t_b; })
            .def("__add__", [](const Var& t_a, const Expr<Var, Var>& t_b) { return t_a + t_b; })
            .def("__radd__", [](const Var& t_a, const Expr<Var, Var>& t_b) { return t_a + t_b; })
            .def("__add__", [](const Var& t_a, const Constant& t_b) { return t_a + t_b; })
            .def("__radd__", [](const Var& t_a, const Constant& t_b) { return t_a + t_b; })
            .def("__add__", [](const Var& t_a, double t_b) { return t_a + t_b; })
            .def("__radd__", [](const Var& t_a, double t_b) { return t_a + t_b; })
        ;

    py::class_<Constant>(m, "Constant")
            .def(py::init<>())
            .def(py::init<double>())
            ;

    py::class_<LinExpr<Var>>(m, "LinExpr")
            .def(py::init<>())
            DEF_COUT(LinExpr<Var>)

            // Products
            .def("__mul__", [](const LinExpr<Var>& t_a, double t_b){ return t_a * t_b; })
            .def("__rmul__", [](const LinExpr<Var>& t_a, double t_b){ return t_a * t_b; })

            // Additions
            .def("__add__", [](const LinExpr<Var>& t_a, const LinExpr<Var>& t_b){ return t_a + t_b; })
            .def("__add__", [](const LinExpr<Var>& t_a, const Var& t_b){ return t_a + t_b; })
            .def("__radd__", [](const LinExpr<Var>& t_a, const Var& t_b){ return t_a + t_b; })
            .def("__add__", [](const LinExpr<Var>& t_a, double t_b){ return t_a + t_b; })
            .def("__radd__", [](const LinExpr<Var>& t_a, double t_b){ return t_a + t_b; })

            .def("__le__", [](LinExpr<Var> t_lhs, double t_rhs){ return std::move(t_lhs) <= t_rhs; })
            .def("__le__", [](LinExpr<Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](LinExpr<Var> t_lhs, Var t_rhs){ return std::move(t_lhs) <= t_rhs; })
            .def("__le__", [](LinExpr<Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](LinExpr<Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](LinExpr<Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })

            .def("__ge__", [](LinExpr<Var> t_lhs, double t_rhs){ return std::move(t_lhs) >= t_rhs; })
            .def("__ge__", [](LinExpr<Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](LinExpr<Var> t_lhs, Var t_rhs){ return std::move(t_lhs) >= t_rhs; })
            .def("__ge__", [](LinExpr<Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](LinExpr<Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](LinExpr<Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })

            .def("__eq__", [](LinExpr<Var> t_lhs, double t_rhs){ return std::move(t_lhs) == t_rhs; })
            .def("__eq__", [](LinExpr<Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](LinExpr<Var> t_lhs, Var t_rhs){ return std::move(t_lhs) == t_rhs; })
            .def("__eq__", [](LinExpr<Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](LinExpr<Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](LinExpr<Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            ;

    py::class_<QuadExpr<Var, Var>>(m, "QuadExpr")
            .def(py::init<>())

            .def("__le__", [](QuadExpr<Var> t_lhs, double t_rhs){ return std::move(t_lhs) <= t_rhs; })
            .def("__le__", [](QuadExpr<Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](QuadExpr<Var> t_lhs, Var t_rhs){ return std::move(t_lhs) <= t_rhs; })
            .def("__le__", [](QuadExpr<Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](QuadExpr<Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](QuadExpr<Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })

            .def("__ge__", [](QuadExpr<Var> t_lhs, double t_rhs){ return std::move(t_lhs) >= t_rhs; })
            .def("__ge__", [](QuadExpr<Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](QuadExpr<Var> t_lhs, Var t_rhs){ return std::move(t_lhs) >= t_rhs; })
            .def("__ge__", [](QuadExpr<Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](QuadExpr<Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](QuadExpr<Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })

            .def("__eq__", [](QuadExpr<Var> t_lhs, double t_rhs){ return std::move(t_lhs) == t_rhs; })
            .def("__eq__", [](QuadExpr<Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](QuadExpr<Var> t_lhs, Var t_rhs){ return std::move(t_lhs) == t_rhs; })
            .def("__eq__", [](QuadExpr<Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](QuadExpr<Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](QuadExpr<Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            ;

    py::class_<Expr<Var, Var>>(m, "Expr")
            .def(py::init<>())
            .def(py::init<double>())
            .def(py::init<const Var&>())
            .def(py::init<const Param&>())
            .def(py::init<const Constant&>())
            .def(py::init<const LinExpr<Var>&>())
            .def(py::init<const QuadExpr<Var>&>())

            .def("__add__", [](Expr<Var, Var> t_a, Expr<Var, Var> t_b) { return std::move(t_a) + std::move(t_b); })
            .def("__add__", [](Expr<Var, Var> t_a, LinExpr<Var> t_b) { return std::move(t_a) + std::move(t_b); })
            .def("__add__", [](Expr<Var, Var> t_a, QuadExpr<Var, Var> t_b) { return std::move(t_a) + std::move(t_b); })
            .def("__add__", [](Expr<Var, Var> t_a, Constant t_b) { return std::move(t_a) + std::move(t_b); })
            .def("__add__", [](Expr<Var, Var> t_a, double t_b) { return std::move(t_a) + t_b; })
            .def("__radd__", [](Expr<Var, Var> t_a, double t_b) { return std::move(t_a) + t_b; })
            .def("__add__", [](Expr<Var, Var> t_a, const Var& t_b) { return std::move(t_a) + t_b; })

            .def("__le__", [](Expr<Var, Var> t_lhs, double t_rhs){ return std::move(t_lhs) <= t_rhs; })
            .def("__le__", [](Expr<Var, Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](Expr<Var, Var> t_lhs, Var t_rhs){ return std::move(t_lhs) <= t_rhs; })
            .def("__le__", [](Expr<Var, Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](Expr<Var, Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })
            .def("__le__", [](Expr<Var, Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) <= std::move(t_rhs); })

            .def("__ge__", [](Expr<Var, Var> t_lhs, double t_rhs){ return std::move(t_lhs) >= t_rhs; })
            .def("__ge__", [](Expr<Var, Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](Expr<Var, Var> t_lhs, Var t_rhs){ return std::move(t_lhs) >= t_rhs; })
            .def("__ge__", [](Expr<Var, Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](Expr<Var, Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })
            .def("__ge__", [](Expr<Var, Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) >= std::move(t_rhs); })

            .def("__eq__", [](Expr<Var, Var> t_lhs, double t_rhs){ return std::move(t_lhs) == t_rhs; })
            .def("__eq__", [](Expr<Var, Var> t_lhs, Constant t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](Expr<Var, Var> t_lhs, Var t_rhs){ return std::move(t_lhs) == t_rhs; })
            .def("__eq__", [](Expr<Var, Var> t_lhs, LinExpr<Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](Expr<Var, Var> t_lhs, QuadExpr<Var, Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            .def("__eq__", [](Expr<Var, Var> t_lhs, Expr<Var, Var> t_rhs){ return std::move(t_lhs) == std::move(t_rhs); })
            ;

    py::class_<TempCtr>(m, "TempCtr")
            .def(py::init<>())
            .def(py::init<Row, CtrType>())

            .def("get_row", (const Row& (TempCtr::*)() const) &TempCtr::row)
            .def("get_row", (Row& (TempCtr::*)()) &TempCtr::row)
            .def("get_type", (CtrType (TempCtr::*)() const) &TempCtr::type)
            .def("set_type", (void (TempCtr::*)(CtrType)) &TempCtr::set_type)
        ;

    py::class_<Model>(m, "Model")
            .def(py::init<Env&>())
            .def(py::init<Env&, ObjectiveSense>())

            DEF_COUT(Model)

            // Variables
            .def("add_var",
                 (Var (Model::*)(double, double, VarType, std::string)) &Model::add_var,
                 "Adds a new variable to the model",
                 py::arg("t_lb"),
                 py::arg("t_ub"),
                 py::arg("t_type"),
                 py::arg("t_name") = ""
            )
            .def("add_var",
                 (Var (Model::*)(double, double, VarType, Column, std::string)) &Model::add_var,
                 "Adds a new variable to the model",
                 py::arg("t_lb"),
                 py::arg("t_ub"),
                 py::arg("t_type"),
                 py::arg("t_column"),
                 py::arg("t_name") = ""
            )
            .def("add", (void (Model::*)(const Var&)) &Model::add)
            .def("add", (void (Model::*)(const Var&, TempVar)) &Model::add)
            .def("has", (bool (Model::*)(const Var&) const) &Model::has)
            .def("remove", (void (Model::*)(const Var&)) &Model::remove)
            .def("get_vars", (ConstIteratorForward<std::vector<Var>> (Model::*)() const) &Model::vars)

            // Constraints
            .def("add_ctr",
                     (Ctr (Model::*)(TempCtr, std::string)) &Model::add_ctr,
                     py::arg("t_temp_ctr"),
                     py::arg("t_name") = ""
                 )
            .def("add_ctr",
                 [](Model& t_self, const Row& t_row, CtrType t_type, std::string t_name) { return t_self.add_ctr(Row(t_row), t_type, std::move(t_name)); },
                 py::arg("t_row"),
                 py::arg("t_type"),
                 py::arg("t_name") = ""
            )
            .def("add", (void (Model::*)(const Ctr&)) &Model::add)
            .def("add", (void (Model::*)(const Ctr&, TempCtr)) &Model::add)
            .def("has", (bool (Model::*)(const Ctr&) const) &Model::has)
            .def("remove", (void (Model::*)(const Ctr&)) &Model::remove)
            .def("get_ctrs", (ConstIteratorForward<std::vector<Ctr>> (Model::*)() const) &Model::ctrs)

            .def("copy", (Model (Model::*)() const) &Model::copy)
            .def("get_id", (const std::string& (Model::*)() const) &Model::id)
            .def("get_env", (Env& (Model::*)() const) &Model::env)

            .def("get_obj_sense", (ObjectiveSense (Model::*)()) &Model::get_obj_sense)
            .def("get_obj_expr", (const Expr<Var, Var>& (Model::*)() const) &Model::get_obj_expr)
            .def("get_mat_coeff", (const Constant& (Model::*)(const Ctr&, const Var&) const) &Model::get_mat_coeff)
            .def("get_status", (SolutionStatus (Model::*)() const) &Model::get_status)
            .def("get_reason", (SolutionReason (Model::*)() const) &Model::get_reason)
            .def("get_best_obj", (double (Model::*)() const) &Model::get_best_obj)
            .def("get_best_bound", (double (Model::*)() const) &Model::get_best_bound)
            .def("set_obj_sense", (void (Model::*)(ObjectiveSense)) &Model::set_obj_sense)
            .def("set_obj_expr", (void (Model::*)(const Expr<Var, Var>&)) &Model::set_obj_expr)
            .def("set_rhs_expr", (void (Model::*)(const LinExpr<Ctr>&)) &Model::set_rhs_expr)
            .def("set_obj_const", (void (Model::*)(const Constant&)) &Model::set_obj_const)
            .def("set_mat_coeff", (void (Model::*)(const Ctr&, const Var&, const Constant&)) &Model::set_mat_coeff)
            .def("get_ctr_by_index", (Ctr (Model::*)(unsigned int)) &Model::get_ctr_by_index)
            .def("get_var_by_index", (Var (Model::*)(unsigned int)) &Model::get_var_by_index)
            .def("get_ctr_index", (unsigned int (Model::*)(const Ctr&)) &Model::get_ctr_index)
            .def("get_var_index", (unsigned int (Model::*)(const Var&)) &Model::get_var_index)
            .def("get_ctr_type", (CtrType (Model::*)(const Ctr&)) &Model::get_ctr_type)
            .def("get_var_type", (VarType (Model::*)(const Var&)) &Model::get_var_type)
            .def("get_ctr_row", (const Row& (Model::*)(const Ctr&)) &Model::get_ctr_row)
            .def("get_ctr_dual", (double (Model::*)(const Ctr&)) &Model::get_ctr_dual)
            .def("get_ctr_farkas", (double (Model::*)(const Ctr&)) &Model::get_ctr_farkas)
            .def("set_ctr_rhs", (void (Model::*)(const Ctr&, const Constant&)) &Model::set_ctr_rhs)
            .def("set_ctr_type", (void (Model::*)(const Ctr&, CtrType)) &Model::set_ctr_type)
            .def("set_ctr_row", (void (Model::*)(const Ctr&, const Row&)) &Model::set_ctr_row)
            .def("get_var_lb", (double (Model::*)(const Var&)) &Model::get_var_lb)
            .def("get_var_ub", (double (Model::*)(const Var&)) &Model::get_var_ub)
            .def("get_var_primal", (double (Model::*)(const Var&)) &Model::get_var_primal)
            .def("get_var_ray", (double (Model::*)(const Var&)) &Model::get_var_ray)
            .def("get_var_column", (const Column& (Model::*)(const Var&)) &Model::get_var_column)
            .def("set_var_type", (void (Model::*)(const Var&, VarType)) &Model::set_var_type)
            .def("set_var_lb", (void (Model::*)(const Var&, double)) &Model::set_var_lb)
            .def("set_var_ub", (void (Model::*)(const Var&, double)) &Model::set_var_ub)
            .def("set_var_obj", (void (Model::*)(const Var&, const Constant&)) &Model::set_var_obj)
            .def("set_var_column", (void (Model::*)(const Var&, const Column&)) &Model::set_var_column)
            .def("get_n_solutions", (unsigned int (Model::*)() const) &Model::get_n_solutions)
            .def("get_solutionIndex", (unsigned int (Model::*)() const) &Model::get_solution_index)
            .def("set_solutionIndex", (void (Model::*)(unsigned int) const) &Model::set_solution_index)
            .def("fix", (Model (Model::*)(unsigned int) const) &Model::fix)

            .def("use", (void (Model::*)(const OptimizerFactory&)) &Model::use)
            .def("unuse", (void (Model::*)()) &Model::unuse)
            .def("optimize", (void (Model::*)()) &Model::optimize)
            .def("write", (void (Model::*)(const std::string&) const) &Model::write)
        ;

    py::class_<Solution::Primal>(m, "PrimalSolution")
            .def(py::init<>())
            .def("__getitem__", [](const Solution::Primal& t_primal, const Var& t_var) { return t_primal.get(t_var); })
            .def("set_status", (void (Solution::Primal::*)(SolutionStatus)) &Solution::Primal::set_status)
            .def("get_status", (SolutionStatus (Solution::Primal::*)() const) &Solution::Primal::status)
            .def("set_reason", (void (Solution::Primal::*)(SolutionReason)) &Solution::Primal::set_reason)
            .def("get_reason", (SolutionReason (Solution::Primal::*)() const) &Solution::Primal::reason)
            .def("set_objective_value", (void (Solution::Primal::*)(double)) &Solution::Primal::set_objective_value)
            .def("get_objective_value", (double (Solution::Primal::*)() const) &Solution::Primal::objective_value)
            .def("has_objective_value", (bool (Solution::Primal::*)() const) &Solution::Primal::has_objective_value)
            .def("reset_objective_value", (void (Solution::Primal::*)() const) &Solution::Primal::reset_objective_value)
            .def("set", (void (Solution::Primal::*)(const Var&, double)) &Solution::Primal::set)
            .def("get", (double (Solution::Primal::*)(const Var&)) &Solution::Primal::get)
            .def("size", (unsigned int (Solution::Primal::*)() const) &Solution::Primal::size)
            .def("__repr__", [](const Solution::Primal& t_self) { COUT(t_self) })
            .def("__str__", [](const Solution::Primal& t_self) { COUT(t_self) })
        ;

    py::class_<Solution::Dual>(m, "DualSolution")
            .def(py::init<>())
            .def("__getitem__", [](const Solution::Dual& t_primal, const Ctr& t_ctr) { return t_primal.get(t_ctr); })
            .def("set_status", (void (Solution::Dual::*)(SolutionStatus)) &Solution::Dual::set_status)
            .def("get_status", (SolutionStatus (Solution::Dual::*)() const) &Solution::Dual::status)
            .def("set_reason", (void (Solution::Dual::*)(SolutionReason)) &Solution::Dual::set_reason)
            .def("get_reason", (SolutionReason (Solution::Dual::*)() const) &Solution::Dual::reason)
            .def("set_objective_value", (void (Solution::Dual::*)(double)) &Solution::Dual::set_objective_value)
            .def("get_objective_value", (double (Solution::Dual::*)() const) &Solution::Dual::objective_value)
            .def("has_objective_value", (bool (Solution::Dual::*)() const) &Solution::Dual::has_objective_value)
            .def("reset_objective_value", (void (Solution::Dual::*)() const) &Solution::Dual::reset_objective_value)
            .def("set", (void (Solution::Dual::*)(const Ctr&, double)) &Solution::Dual::set)
            .def("get", (double (Solution::Dual::*)(const Ctr&)) &Solution::Dual::get)
            .def("size", (unsigned int (Solution::Dual::*)() const) &Solution::Dual::size)
            .def("__repr__", [](const Solution::Dual& t_self) { COUT(t_self) })
            .def("__str__", [](const Solution::Dual& t_self) { COUT(t_self) })
            ;

    py::class_<Annotation<Ctr>>(m, "CtrAnnotation")
            .def(py::init<Env&, std::string, unsigned int>())
            .def(py::init<Env&, std::string>())
            ;

    m.def("save_primal", (Solution::Primal (*)(const Model&)) &save_primal);
    m.def("save_ray", (Solution::Primal (*)(const Model&)) &save_ray);
    m.def("save_dual", (Solution::Dual (*)(const Model&)) &save_dual);
    m.def("save_farkas", (Solution::Dual (*)(const Model&)) &save_farkas);

    m.attr("MASTER_ID") = MasterId;

    // Optimizers

    auto optimizers = m.def_submodule("Optimizers");

    py::class_<OptimizerFactory>(optimizers, "OptimizerFactory");

    py::class_<GLPK, OptimizerFactory>(optimizers, "GLPK")
            .def(py::init<>())
            DEF_DEFAULT_PARAMETERS(GLPK)
            .def_static("ContinuousRelaxation", &GLPK::ContinuousRelaxation)
            ;

    py::class_<HiGHS, OptimizerFactory>(optimizers, "HiGHS")
            .def(py::init<>())
            DEF_DEFAULT_PARAMETERS(HiGHS)
            .def_static("ContinuousRelaxation", &HiGHS::ContinuousRelaxation)
            ;

    py::class_<Mosek, OptimizerFactory>(optimizers, "Mosek")
            .def(py::init<>())
            DEF_DEFAULT_PARAMETERS(Mosek)
            .def_static("ContinuousRelaxation", &Mosek::ContinuousRelaxation)
            ;

    py::class_<Gurobi, OptimizerFactory>(optimizers, "Gurobi")
            .def(py::init<>())
            DEF_DEFAULT_PARAMETERS(Gurobi)
            .def_static("ContinuousRelaxation", &Gurobi::ContinuousRelaxation)
            ;

    auto algorithms = m.def_submodule("Algorithms");

    auto branching_rules = m.def_submodule("BranchingRules");

    py::class_<BranchingRuleFactory<DefaultNodeInfo>>(branching_rules, "BranchingRule");

    py::class_<MostInfeasible::Strategy<DefaultNodeInfo>, BranchingRuleFactory<DefaultNodeInfo>>(branching_rules, "MostInfeasible")
            .def(py::init<>())
            ;

    auto node_selection = m.def_submodule("NodeSelectionRules");

    py::class_<NodeSelectionRuleFactory<DefaultNodeInfo>>(node_selection, "NodeSelectionRule");

    py::class_<BestBound::Strategy<DefaultNodeInfo>, NodeSelectionRuleFactory<DefaultNodeInfo>>(node_selection, "BestBound")
            .def(py::init([](){ return BestBound::Strategy<DefaultNodeInfo>(BestBound()); }))
            ;

    py::class_<BranchAndBound<DefaultNodeInfo>, OptimizerFactory>(algorithms, "BranchAndBound")
            .def(py::init<>())
            .def("with_node_optimizer", (BranchAndBound<DefaultNodeInfo>& (BranchAndBound<DefaultNodeInfo>::*)(const OptimizerFactory&)) &BranchAndBound<DefaultNodeInfo>::with_node_optimizer)
            .def("with_branching_rule", (BranchAndBound<DefaultNodeInfo>& (BranchAndBound<DefaultNodeInfo>::*)(const BranchingRuleFactory<DefaultNodeInfo>&)) &BranchAndBound<DefaultNodeInfo>::with_branching_rule)
            .def("with_node_selection_rule", (BranchAndBound<DefaultNodeInfo>& (BranchAndBound<DefaultNodeInfo>::*)(const NodeSelectionRuleFactory<DefaultNodeInfo>&)) &BranchAndBound<DefaultNodeInfo>::with_node_selection_rule)

            DEF_DEFAULT_PARAMETERS(BranchAndBound<DefaultNodeInfo>)

            .def("__add__", [](BranchAndBound<DefaultNodeInfo>& t_self, const OptimizerFactory& t_optimizer){ return t_self + t_optimizer; })
            .def("__add__", [](BranchAndBound<DefaultNodeInfo>& t_self, const BranchingRuleFactory<DefaultNodeInfo>& t_branching_rule){ return t_self.with_branching_rule(t_branching_rule); })
            .def("__add__", [](BranchAndBound<DefaultNodeInfo>& t_self, const NodeSelectionRuleFactory<DefaultNodeInfo>& t_branching_rule){ return t_self.with_node_selection_rule(t_branching_rule); })
            ;

    py::class_<DantzigWolfe::SubProblem>(algorithms, "DantzigWolfeSubProblem")
            .def(py::init<>())
            .def("add_optimizer", (DantzigWolfe::SubProblem& (DantzigWolfe::SubProblem::*)(const OptimizerFactory&)) &DantzigWolfe::SubProblem::add_optimizer)
            ;

    py::class_<DantzigWolfeDecomposition, OptimizerFactory>(algorithms, "DantzigWolfeDecomposition")
            .def(py::init<Annotation<Ctr, unsigned int>>())
            .def("with_master_optimizer", (DantzigWolfeDecomposition& (DantzigWolfeDecomposition::*)(const OptimizerFactory&)) &DantzigWolfeDecomposition::with_master_optimizer)
            .def("with_default_sub_problem_spec", (DantzigWolfeDecomposition& (DantzigWolfeDecomposition::*)(const DantzigWolfe::SubProblem&)) &DantzigWolfeDecomposition::with_default_sub_problem_spec)

            DEF_DEFAULT_PARAMETERS(DantzigWolfeDecomposition)
            ;

    /*
      .with_default_sub_problem_spec(
              DantzigWolfe::SubProblem()
                                .add_optimizer(HiGHS().with_logs(false))
                                .with_column_pool_clean_up(1500, .75)
      )
     */

    // Problems

    auto problems = m.def_submodule("Problems", "Sub-module for problem parsers.");

    auto problems_gap = problems.def_submodule("GAP", "Generalized Assignment Problem.");

    py::class_<Problems::GAP::Instance>(problems_gap, "Instance")
            .def(py::init<unsigned int, unsigned int>())
            .def("get_n_agents", (unsigned int (Problems::GAP::Instance::*)() const) &Problems::GAP::Instance::n_agents)
            .def("get_n_jobs", (unsigned int (Problems::GAP::Instance::*)() const) &Problems::GAP::Instance::n_jobs)
            .def("get_cost", (double (Problems::GAP::Instance::*)(unsigned int, unsigned int) const) &Problems::GAP::Instance::cost)
            .def("get_resource_consumption", (double (Problems::GAP::Instance::*)(unsigned int, unsigned int) const) &Problems::GAP::Instance::resource_consumption)
            .def("get_capacity", (double (Problems::GAP::Instance::*)(unsigned int) const) &Problems::GAP::Instance::capacity)

            DEF_COUT(Problems::GAP::Instance)
            ;

    problems_gap.def("read_instance", (Problems::GAP::Instance (*)(const std::string&)) &Problems::GAP::read_instance);

}

#endif //IDOL_PY_MODELING_CPP
