//
// Created by Henri on 30/04/2026.
//
#include "idol/robust/optimizers/wrappers/yasol/QpIdolExternSolver.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/optimizers/wrappers/Gurobi/Optimizers_Gurobi.h"

idol::VarType idol::impl::QpIdolExternSolver::to_idol(data::QpVar::NumberSystem t_type) {
    switch (t_type) {
    case data::QpVar::binaries: return Binary;
    case data::QpVar::generals: return Integer;
    case data::QpVar::real: return Continuous;
    default: throw Exception("Unexpected variable type.");
    }
}

idol::CtrType idol::impl::QpIdolExternSolver::to_idol(data::QpRhs::RatioSign t_type) {
    switch (t_type) {
        case data::QpRhs::smallerThanOrEqual: return LessOrEqual;
        case data::QpRhs::equal: return Equal;
        case data::QpRhs::greaterThanOrEqual: return GreaterOrEqual;
        default: throw Exception("Unexpected constraint type.");
    }
}

extSol::QpExternSolver::QpExtSolSolutionStatus idol::impl::QpIdolExternSolver::to_yasol(SolutionStatus t_status, SolutionReason t_reason) {

    switch (t_reason) {
    case Proved: [[fallthrough]];
    case NotSpecified: break;
    case TimeLimit: return ABORT_TIME_LIM;
    case IterLimit: return ABORT_IT_LIM;
    case ObjLimit: return ABORT_OBJ_LIM;
    case Numerical: return ERROR;
    case MemoryLimit: [[fallthrough]];
    case Cycling: [[fallthrough]];
    case SolutionLimit: [[fallthrough]];
    default: throw Exception("Unexpected status reason.");
    }

    switch (t_status) {
    case Loaded: return UNSOLVED;
    case Optimal: return OPTIMAL;
    case Infeasible: return INFEASIBLE;
    case InfOrUnbnd: return INForUNB;
    case Unbounded: return UNBOUNDED;
    case Fail: return ERROR;
    case SubOptimal: return OPTIMAL_INFEAS;
    case Feasible: [[fallthrough]];
    default: throw Exception("Unexpected status.");
    }

}

idol::impl::QpIdolExternSolver::QpIdolExternSolver(idol::Env& t_env, const OptimizerFactory& t_optimizer_factory)
    : QpExternSolverWithSnapshot(CPLEX_C, DEFAULT),
      m_model(t_env),
      m_optimizer_factory(t_optimizer_factory.clone())
{

}

extSol::QpExternSolver::QpExtSolSolutionStatus idol::impl::QpIdolExternSolver::getSolutionStatus() {
    return to_yasol(m_model.get_status(), m_model.get_reason());
}

void idol::impl::QpIdolExternSolver::init(const data::Qlp& qlp, data::QpRhs::Responsibility resp) {
    std::vector<std::vector<data::IndexedElement>> matrix;
    qlp.getCoeffMatrixByResp(matrix,resp);
    init(qlp.getObjectiveFunction(), qlp.getQuantifiedVariables(), matrix, qlp.getRhsVecByResp(resp));
}

void idol::impl::QpIdolExternSolver::init(const data::QpObjFunc& t_obj, const std::vector<data::QpVar>& t_vars,
    const data::QpSparseMatrix& t_matrix, const std::vector<data::QpRhs>& t_rhs) {

    assert(m_model.vars().size() == 0);
    assert(m_model.ctrs().size() == 0);

    // Add variables
    for (const auto& var : t_vars) {
        const auto [lb, ub] = var.getBounds();
        const auto type = to_idol(var.getNumberSystem());
        assert(m_model.vars().size() == var.getIndex());
        m_model.add_var(lb.asDouble(), ub.asDouble(), type, 0., var.getName());
    }

    // Set objective function
    if (t_obj.getObjective() != data::QpObjFunc::min) {
        throw Exception("Maximization problems are not implemented.");
    }
    m_model.set_obj_const(t_obj.getOffset().asDouble());
    for (const auto& [var_index, coeff] : t_obj.getObjectiveElementsSparse()) {
        const auto& var = m_model.get_var_by_index(var_index);
        m_model.set_var_obj(var, coeff.asDouble());
    }

    // Add constraints
    for (auto i = 0; i < t_matrix.size(); ++i) {

        const auto& row = t_matrix[i];
        const auto& rhs = t_rhs[i].getValue().asDouble();
        const auto type = to_idol(t_rhs[i].getRatioSign());

        LinExpr expr;
        for (const auto& elem : row) {
            const auto& var = m_model.get_var_by_index(elem.index);
            expr += elem.value.asDouble() * var;
        }

        // Add constraint to model
        m_model.add_ctr(std::move(expr), type, rhs);

    }

}

unsigned idol::impl::QpIdolExternSolver::getVariableCount() const {
    return m_model.vars().size();
}

unsigned idol::impl::QpIdolExternSolver::getRowCount() const {
    return m_model.ctrs().size();
}

void idol::impl::QpIdolExternSolver::getBase(extSol::QpExternSolver::QpExtSolBase& base) const {

    std::cout << "QpIdolExternSolver::getBase is a gurobi-specific routine for now" << std::endl;

    auto& gurobi = const_cast<Optimizers::Gurobi&>(m_model.optimizer().as<Optimizers::Gurobi>());
    auto& lib = Optimizers::Gurobi::get_dynamic_lib();
    auto* model = gurobi.model();

    const unsigned int vars = getVariableCount();
    const unsigned int cons = getRowCount();

    std::vector<int> vs(vars), cs(cons);
    int error;
    error = lib.GRBgetintattrarray(model, "VBasis", 0, vars, vs.data());
    if (error) {
        throw utils::ExternSolverException(
                "Gurobi Exception caught getting variable basis status. Error Code: "
                        + utils::ToolBox::convertToString(error));
    }

    error = lib.GRBgetintattrarray(model, "CBasis", 0, cons, cs.data());
    if (error) {
        throw utils::ExternSolverException(
                "Gurobi Exception caught getting constraints basis status. Error Code: "
                        + utils::ToolBox::convertToString(error));
    }

    base.variables.resize(vars, -1);
    base.constraints.resize(cons, -1);

    for (unsigned int i = 0; i < vars; i++) {
        switch (vs[i]) {
        case 0: // GRB_BASIC:
            base.variables[i] = extSol::QpExternSolver::Basic;
            break;
        case -1: // GRB_NONBASIC_LOWER:
            base.variables[i] = extSol::QpExternSolver::AtLower;
            break;
        case -2: // GRB_NONBASIC_UPPER:
            base.variables[i] = extSol::QpExternSolver::AtUpper;
            break;
        case -3: // GRB_SUPERBASIC:
            base.variables[i] = extSol::QpExternSolver::FreeOrSuperbasic;
            break;
        default:
            throw utils::ExternSolverException(
                    "Exception caught getting base. Unknown Status: "
                            + utils::ToolBox::convertToString(vs[i]));
        }

    }

    for (unsigned int i = 0; i < cons; i++) {
        switch (cs[i]) {
        case 0: // GRB_BASIC:
            base.constraints[i] = extSol::QpExternSolver::Basic;
            break;
        case -1: // GRB_NONBASIC_LOWER:
            base.constraints[i] = extSol::QpExternSolver::AtLower;
            break;
        case -2: // GRB_NONBASIC_UPPER:
            base.constraints[i] = extSol::QpExternSolver::AtUpper;
            break;
        case -3: // GRB_SUPERBASIC:
            base.constraints[i] = extSol::QpExternSolver::FreeOrSuperbasic;
            break;
        default:
            throw utils::ExternSolverException(
                    "Exception caught getting base. Unknown Status: "
                            + utils::ToolBox::convertToString(cs[i]));
        }
    }

}

void idol::impl::QpIdolExternSolver::setBase(extSol::QpExternSolver::QpExtSolBase& base) {

    std::cout << "QpIdolExternSolver::setBase is a gurobi-specific routine for now" << std::endl;

	updateModel();

    auto& gurobi = const_cast<Optimizers::Gurobi&>(m_model.optimizer().as<Optimizers::Gurobi>());
    auto& lib = Optimizers::Gurobi::get_dynamic_lib();
    auto* model = gurobi.model();

	const unsigned int vars = getVariableCount();
	const unsigned int cons = getRowCount();

	if ((base.constraints.size() != cons) || (base.variables.size() != vars)) {
		throw utils::ExternSolverException(
				"QpExtSolCplexC::setBase(const extSol::QpExternSolver::QpExtSolBase& base) -> (base.constraints.size() != this->getRowCount()) || (base.variables.size()!=this->getVariableCount())");
	}

	std::vector<int> vs(vars), cs(cons);

	for (unsigned int i = 0; i < base.variables.size(); i++) {
		switch (base.variables[i]) {
		case extSol::QpExternSolver::Basic:
			vs[i] = 0; // GRB_BASIC;
			break;
		case extSol::QpExternSolver::AtLower:
			vs[i] = -1; // GRB_NONBASIC_LOWER;
			break;
		case extSol::QpExternSolver::AtUpper:
			vs[i] = -2; // GRB_NONBASIC_UPPER;
			break;
		case extSol::QpExternSolver::FreeOrSuperbasic:
			vs[i] = -3; // GRB_SUPERBASIC;
			break;
		default:
			throw utils::ExternSolverException("Exception caught setting base. Unknown Status: " + utils::ToolBox::convertToString(base.variables[i]));
		}
	}

	for (unsigned int i = 0; i < base.constraints.size(); i++) {
		switch (base.constraints[i]) {
		case extSol::QpExternSolver::Basic:
			cs[i] = 0; // GRB_BASIC;
			break;
		case extSol::QpExternSolver::AtLower:
			cs[i] = -1; // GRB_NONBASIC_LOWER;
			break;
		case extSol::QpExternSolver::AtUpper:
			cs[i] = -2; // GRB_NONBASIC_UPPER;
			break;
		case extSol::QpExternSolver::FreeOrSuperbasic:
			cs[i] = -3; // GRB_SUPERBASIC;
			break;
		default:
			throw utils::ExternSolverException("Exception caught setting base. Unknown Status: " + utils::ToolBox::convertToString(base.constraints[i]));
		}
	}

    int error;
	error = lib.GRBsetintattrarray(model, "VBasis", 0, vars, vs.data());
	if (error) {
		throw utils::ExternSolverException("Gurobi Exception caught setting variable basis status. Error Code: " + utils::ToolBox::convertToString(error));
	}

	error = lib.GRBsetintattrarray(model, "CBasis", 0, cons, cs.data());
	if (error) {
		std::cout << cons << " " << this->getRowCount() << " " << cs.size() << std::endl;
		throw utils::ExternSolverException("Gurobi Exception caught setting constraints basis status. Error Code: " + utils::ToolBox::convertToString(error));
	}

}

void idol::impl::QpIdolExternSolver::adaptToSolverMode(QpExtSolSolverMode mode) {
    std::cout << "Mode requested: " << mode << std::endl;
}

void idol::impl::QpIdolExternSolver::setParameters(const ExtSolverParameters&) {
    std::cout << "Set parameters is skipped" << std::endl;
}

bool idol::impl::QpIdolExternSolver::changeObjFuncCoeff(unsigned index, const data::QpNum& coeff) {
    const auto& var = m_model.get_var_by_index(index);
    m_model.set_var_obj(var, coeff.asDouble());
    return true;
}

void idol::impl::QpIdolExternSolver::setVarLB(unsigned i, const data::QpNum& lb) {
    const auto& var = m_model.get_var_by_index(i);
    m_model.set_var_lb(var, lb.asDouble());
}

void idol::impl::QpIdolExternSolver::setVarUB(unsigned i, const data::QpNum& ub) {
    const auto& var = m_model.get_var_by_index(i);
    m_model.set_var_ub(var, ub.asDouble());
}

void idol::impl::QpIdolExternSolver::changeRhsElement(unsigned t_index, const data::QpNum& t_rhs) {
    const auto& ctr = m_model.get_ctr_by_index(t_index);
    m_model.set_ctr_rhs(ctr, t_rhs.asDouble());
}

extSol::QpExternSolver::QpExtSolSolutionStatus idol::impl::QpIdolExternSolver::solve(unsigned itLimit, unsigned timeLimit) {

    if (!m_model.has_optimizer()) {
        m_model.use(*m_optimizer_factory);
    }

    m_model.optimizer().set_param_time_limit(timeLimit);
    m_model.optimize();

    return getSolutionStatus();
}

data::QpNum idol::impl::QpIdolExternSolver::getObjValue() {
    return m_model.get_best_obj();
}

void idol::impl::QpIdolExternSolver::getValues(std::vector<data::QpNum>& t_result) {
    t_result.resize(m_model.vars().size());
    for (const auto& var : m_model.vars()) {
        const auto index = m_model.get_var_index(var);
        const auto value = m_model.get_var_primal(var);
        t_result[index] = value;
    }
}

void idol::impl::QpIdolExternSolver::getReducedCosts(std::vector<data::QpNum>& t_result) {
    t_result.resize(m_model.vars().size());
    for (const auto& var : m_model.vars()) {
        const auto index = m_model.get_var_index(var);
        const auto value = m_model.get_var_reduced_cost(var);
        t_result[index] = value;
    }
}

void idol::impl::QpIdolExternSolver::getDualFarkas(std::vector<data::QpNum>& t_result) {
    t_result.resize(m_model.ctrs().size());
    for (const auto& ctr : m_model.ctrs()) {
        const auto index = m_model.get_ctr_index(ctr);
        const auto value = m_model.get_ctr_farkas(ctr);
        t_result[index] = value;
    }
}

void idol::impl::QpIdolExternSolver::getExtendedDualFarkas(std::vector<data::QpNum>& extFarkas,
                                                           const data::QpSparseMatrix& constraints,
                                                           const data::QpSparseMatrix& cuts) {
    unsigned int vars = this->getVariableCount();
    unsigned int cons = this->getRowCount();
    std::vector<data::QpNum> boundMultipliers(vars), farkasCertificate;
    this->getDualFarkas(farkasCertificate);
    data::QpNum::removeSmallValuesFromVec(farkasCertificate);

    extFarkas = farkasCertificate;
    extFarkas.resize(cons + 2 * vars, 0);
    for (unsigned int i = 0; i < boundMultipliers.size(); i++) {
        boundMultipliers[i].setZero();
        for (unsigned j = 0; j < constraints[i].size(); j++) {
            boundMultipliers[i] += (constraints[i][j].value
                    * farkasCertificate[constraints[i][j].index]);
        }
        for (unsigned j = 0; j < cuts[i].size(); j++) {
            boundMultipliers[i] += (cuts[i][j].value
                    * farkasCertificate[cuts[i][j].index]);
        }
    }
    data::QpNum::removeSmallValuesFromVec(boundMultipliers);

    for (unsigned int i = 0; i < boundMultipliers.size(); i++) {
        if (boundMultipliers[i].isZero())
            continue;
        unsigned int index = cons + i;
        if (boundMultipliers[i] > 0)
            index += vars;
        extFarkas[index] = (boundMultipliers[i] *= -1.0);
    }
}

void idol::impl::QpIdolExternSolver::addCut(const std::vector<data::IndexedElement>& t_lhs, data::QpRhs::RatioSign t_type, const data::QpNum& t_rhs) {

    LinExpr expr;
    for (const auto& elem : t_lhs) {
        const auto& var = m_model.get_var_by_index(elem.index);
        expr += elem.value.asDouble() * var;
    }

    const auto type = to_idol(t_type);
    m_model.add_ctr(std::move(expr), type, t_rhs.asDouble());

}

void idol::impl::QpIdolExternSolver::removeCutsFromCut(unsigned index) {
    if (this->getRowCount() <= index) {
        throw utils::ExternSolverException("QpIdolExternSolver::removeCut(unsigned int index) --> Index Exception.");
    }
    while (index < m_model.ctrs().size()) { // TODO sure of that ?
        const auto& ctr = m_model.get_ctr_by_index(index);
        m_model.remove(ctr);
    }
}

void idol::impl::QpIdolExternSolver::updateModel() {
    m_model.update();
}

void idol::impl::QpIdolExternSolver::getLB(std::vector<data::QpNum>& t_result) {
    t_result.resize(m_model.vars().size());
    for (const auto& var : m_model.vars()) {
        const auto index = m_model.get_var_index(var);
        const auto value = m_model.get_var_lb(var);
        t_result[index] = value;
    }
}

void idol::impl::QpIdolExternSolver::getUB(std::vector<data::QpNum>& t_result) {
    t_result.resize(m_model.vars().size());
    for (const auto& var : m_model.vars()) {
        const auto index = m_model.get_var_index(var);
        const auto value = m_model.get_var_ub(var);
        t_result[index] = value;
    }
}

void idol::impl::QpIdolExternSolver::getRowLhs(unsigned t_index, std::vector<data::IndexedElement>& t_result) {
    const auto& ctr = m_model.get_ctr_by_index(t_index);
    const auto& row = m_model.get_ctr_row(ctr);
    t_result.reserve(row.size());
    for (const auto& var : m_model.vars()) {
        const auto index = m_model.get_var_index(var);
        const auto value = row.get(var);
        if (!is_zero(value, m_model.optimizer().get_tol_feasibility())) {
            t_result.emplace_back(index, value);
        }
    }
    assert(t_result.size() == row.size());
}
