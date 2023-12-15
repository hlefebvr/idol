//
// Created by henri on 15.12.23.
//

#ifndef IDOL_PY_MODELING_CPP
#define IDOL_PY_MODELING_CPP

#include <pybind11/pybind11.h>
#include <pybind11/operators.h>

#include <idol/modeling.h>

#define COUT(obj) \
std::stringstream ss; ss << obj; return ss.str();

namespace py = pybind11;

using namespace idol;

PYBIND11_MODULE(pydol, m) {

    m.doc() = "pydol is a python wrapper for the C++ library idol.";

    py::enum_<VarType>(m, "VarType")
            .value("Integer", VarType::Integer)
            .value("Continuous", VarType::Continuous)
            .value("Binary", VarType::Binary)
            .export_values();

    py::class_<Env>(m, "Env")
            .def(py::init<>())
            .def("__getitem__", [](const Env& t_env, const Var& t_var){ return t_env[t_var]; })
            ;

    py::class_<Param>(m, "Param")
            .def(py::init<const Var&>())
            .def(py::init<const Ctr&>())
            ;

    py::class_<Var>(m, "Var")
            .def(py::init<Env&, double, double, VarType, std::string>())

            .def("__repr__", [](const Var& t_var) { COUT(t_var) })
            .def("__str__", [](const Var& t_var) { COUT(t_var) })
            .def("getName", &Var::name)

            // Parametrization

            // Products
            .def("__mul__", [](const Var& t_a, double t_b){ return t_a * t_b; })
            .def("__rmul__", [](const Var& t_a, double t_b){ return t_a * t_b; })
            .def("__mul__", [](const Var& t_a, const Constant& t_b){ return t_a * t_b; })
            .def("__rmul__", [](const Var& t_a, const Constant& t_b){ return t_a * t_b; })

            // Additions
            .def("__add__", [](const Var& t_a, const Var& t_b){ return t_a + t_b; })
            .def("__add__", [](const Var& t_a, const LinExpr<Var>& t_b) { return t_a + t_b; })
            .def("__radd__", [](const Var& t_a, const LinExpr<Var>& t_b) { return t_a + t_b; })
        ;

    py::class_<Constant>(m, "Constant")
            .def(py::init<>())
            .def(py::init<double>())
            ;

    py::class_<LinExpr<Var>>(m, "LinExpr")
            .def(py::init<>())
            .def("__repr__", [](const LinExpr<Var>& t_expr) { COUT(t_expr) })
            .def("__str__", [](const LinExpr<Var>& t_expr) { COUT(t_expr) })

            // Products
            .def("__mul__", [](const LinExpr<Var>& t_a, double t_b){ return t_a * t_b; })
            .def("__rmul__", [](const LinExpr<Var>& t_a, double t_b){ return t_a * t_b; })

            // Additions
            .def("__add__", [](const LinExpr<Var>& t_a, const LinExpr<Var>& t_b){ return t_a + t_b; })
            .def("__add__", [](const LinExpr<Var>& t_a, const Var& t_b){ return t_a + t_b; })
            .def("__add__", [](const Var& t_a, const LinExpr<Var>& t_b){ return t_a + t_b; })
            ;

    py::class_<Model>(m, "Model")
            .def(py::init<Env&>())
            .def(py::init<Env&, ObjectiveSense>())

            .def("__repr__", [](const Model& t_self) { COUT(t_self) })
            .def("__str__", [](const Model& t_self) { COUT(t_self) })

            .def("addVar",
                 (Var (Model::*)(double, double, VarType, std::string)) &Model::add_var,
                 "Adds a new variable to the model",
                 py::arg("t_lb"),
                 py::arg("t_ub"),
                 py::arg("t_type"),
                 py::arg("t_name") = ""
            )
        ;
}

#endif //IDOL_PY_MODELING_CPP
