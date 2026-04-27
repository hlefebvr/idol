//
// Created by henri on 05.02.24.
//

#ifdef IDOL_USE_OSI

#include <CoinPackedVector.hpp>
#include <OsiRowCut.hpp>
#include <OsiColCut.hpp>
#include <CoinWarmStartBasis.hpp>
#include "idol/mixed-integer/optimizers/wrappers/Osi/OsiIdolSolverInterface.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

#define OSI_IDOL_DEBUG std::cout << __FUNCTION__ << std::endl;

static inline void
IdolConvertSenseToBound(const char rowsen, const double rowrng,
                       const double rowrhs, double &rlb, double &rub) {
    switch (rowsen) {
        case 'E':
            rlb  = rowrhs;
            rub  = rowrhs;
            break;

        case 'L':
            rlb  = idol::Inf;
            rub  = rowrhs;
            break;

        case 'G':
            rlb  = rowrhs;
            rub  = idol::Inf;
            break;

        case 'R':
            if( rowrng >= 0 ) {
                rlb = rowrhs - rowrng;
                rub = rowrhs;
            } else {
                rlb = rowrhs;
                rub = rowrhs + rowrng;
            }

            break;
        case 'N':
            rlb  = -idol::Inf;
            rub  = idol::Inf;
            break;

        default:
            throw idol::Exception("OsiIdolSolverInterface: Unknown sense.");
    }
}

OsiIdolSolverInterface::OsiIdolSolverInterface(idol::Model &t_model) : m_model(&t_model), m_model_has_to_be_destroy(false) {

}

OsiIdolSolverInterface::OsiIdolSolverInterface(idol::Env& t_env, const idol::OptimizerFactory& t_optimizer) :
    m_model(new idol::Model(t_env)), m_model_has_to_be_destroy(true), m_optimizer(t_optimizer.clone()) {


}

OsiIdolSolverInterface::~OsiIdolSolverInterface() {

    delete[] m_row_sense; m_row_sense = nullptr;
    delete[] m_row_range; m_row_range = nullptr;
    delete[] m_row_activity; m_row_activity = nullptr;
    delete[] m_row_upper; m_row_upper = nullptr;
    delete[] m_row_lower; m_row_lower = nullptr;
    delete[] m_row_price; m_row_price = nullptr;
    delete[] m_matrix_by_col; m_matrix_by_col = nullptr;
    delete[] m_matrix_by_row; m_matrix_by_row = nullptr;
    delete[] m_col_lower; m_col_lower = nullptr;
    delete[] m_col_upper; m_col_upper = nullptr;
    delete[] m_col_obj; m_col_obj = nullptr;
    delete[] m_col_solution; m_col_solution = nullptr;

    if (m_model_has_to_be_destroy) {
        delete m_model; m_model = nullptr;
    }
}

void OsiIdolSolverInterface::set_optimizer_if_not_exists() {
    if (m_model->has_optimizer()) {
        return;
    }
    m_model->use(*m_optimizer);
}

void OsiIdolSolverInterface::initialSolve() {
    OSI_IDOL_DEBUG
    set_optimizer_if_not_exists();
    resolve();
}

void OsiIdolSolverInterface::resolve() {
    OSI_IDOL_DEBUG
    set_optimizer_if_not_exists();
    relax();
    m_model->optimize();

    delete[] m_col_solution; m_col_solution = nullptr;
}

void OsiIdolSolverInterface::branchAndBound() {
    unrelax();

    set_optimizer_if_not_exists();

    m_model->optimize();

    delete[] m_col_solution; m_col_solution = nullptr;
}

bool OsiIdolSolverInterface::isAbandoned() const {
    return m_model->get_status() == idol::Fail;
}

bool OsiIdolSolverInterface::isProvenOptimal() const {
    return m_model->get_status() == idol::Optimal;
}

bool OsiIdolSolverInterface::isProvenPrimalInfeasible() const {
    return m_model->get_status() == idol::Infeasible;
}

bool OsiIdolSolverInterface::isProvenDualInfeasible() const {
    return m_model->get_status() == idol::Unbounded;
}

bool OsiIdolSolverInterface::isIterationLimitReached() const {
    return m_model->get_reason() == idol::IterLimit;
}

CoinWarmStart *OsiIdolSolverInterface::getEmptyWarmStart() const {
    OSI_IDOL_DEBUG
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

CoinWarmStart *OsiIdolSolverInterface::getWarmStart() const {
    OSI_IDOL_DEBUG
    return new CoinWarmStartBasis();
}

bool OsiIdolSolverInterface::setWarmStart(const CoinWarmStart *warmstart) {
    OSI_IDOL_DEBUG
    std::cout << "Skipping Warm Start." << std::endl;
    return true;
}

int OsiIdolSolverInterface::getNumCols() const {
    return (int) m_model->vars().size();
}

int OsiIdolSolverInterface::getNumRows() const {
    return (int) m_model->ctrs().size();
}

CoinBigIndex OsiIdolSolverInterface::getNumElements() const {
    if (m_matrix_by_row) {
        return m_matrix_by_row->getNumElements();
    }
    return getMatrixByCol()->getNumElements();
}

const double *OsiIdolSolverInterface::getColLower() const {
    if (!m_col_lower) {
        const unsigned int n_cols = getNumCols();
        m_col_lower = new double[n_cols];
        for (int i = 0 ; i < n_cols ; ++i) {
            const auto var = m_model->get_var_by_index(i);
            m_col_lower[i] = m_model->get_var_lb(var);
        }
    }
    return m_col_lower;
}

const double *OsiIdolSolverInterface::getColUpper() const {
    if (!m_col_upper) {
        const unsigned int n_cols = getNumCols();
        m_col_upper = new double[n_cols];
        for (int i = 0 ; i < n_cols ; ++i) {
            const auto var = m_model->get_var_by_index(i);
            m_col_upper[i] = m_model->get_var_ub(var);
        }
    }
    return m_col_upper;
}

const char *OsiIdolSolverInterface::getRowSense() const {
    if (!m_row_sense) {
        getRightHandSide();
    }
    return m_row_sense;
}

const double *OsiIdolSolverInterface::getRightHandSide() const {

    if (!m_row_rhs || !m_row_range || !m_row_sense) {

        const unsigned int n_rows = getNumRows();

        delete[] m_row_sense;
        delete[] m_row_rhs;
        delete[] m_row_range;

        m_row_sense = new char[n_rows];
        m_row_rhs = new double[n_rows];
        m_row_range = new double[n_rows];

        auto* row_lower = getRowLower();
        auto* row_upper = getRowUpper();

        for (int i = 0 ; i < n_rows ; ++i) {
            convertBoundToSense(row_lower[i], row_upper[i], m_row_sense[i], m_row_rhs[i], m_row_range[i]);
        }

    }

    return m_row_rhs;
}

const double *OsiIdolSolverInterface::getRowRange() const {
    if (!m_row_range) {
        getRightHandSide();
    }
    return m_row_range;
}

const double *OsiIdolSolverInterface::getRowLower() const {

    if (!m_row_lower) {
        const unsigned int n_rows = getNumRows();
        m_row_lower = new double[n_rows];
        for (int i = 0 ; i < n_rows ; ++i) {
            const auto ctr = m_model->get_ctr_by_index(i);
            const auto rhs = m_model->get_ctr_rhs(ctr);
            const auto type = m_model->get_ctr_type(ctr);
            switch (type) {
                case idol::LessOrEqual:
                    m_row_lower[i] = -getInfinity();
                    break;
                case idol::GreaterOrEqual:
                    m_row_lower[i] = rhs;
                    break;
                case idol::Equal:
                    m_row_lower[i] = rhs;
                    break;
            }
        }
    }
    return m_row_lower;
}

const double *OsiIdolSolverInterface::getRowUpper() const {

    if (!m_row_upper) {
        const unsigned int n_rows = getNumRows();
        m_row_upper = new double[n_rows];
        for (int i = 0 ; i < n_rows ; ++i) {
            const auto ctr = m_model->get_ctr_by_index(i);
            const auto rhs = m_model->get_ctr_rhs(ctr);
            const auto type = m_model->get_ctr_type(ctr);
            switch (type) {
                case idol::LessOrEqual:
                    m_row_upper[i] = rhs;
                    break;
                case idol::GreaterOrEqual:
                    m_row_upper[i] = getInfinity();
                    break;
                case idol::Equal:
                    m_row_upper[i] = rhs;
                    break;
            }
        }
    }
    return m_row_upper;
}

const double *OsiIdolSolverInterface::getObjCoefficients() const {
    OSI_IDOL_DEBUG
    if (!m_col_obj) {
        const unsigned int n_cols = getNumCols();
        m_col_obj = new double[n_cols];
        for (int i = 0 ; i < n_cols ; ++i) {
            const auto var = m_model->get_var_by_index(i);
            m_col_obj[i] = m_model->get_var_obj(var);
        }
    }
    return m_col_obj;
}

double OsiIdolSolverInterface::getObjSense() const {
    OSI_IDOL_DEBUG
    return m_model->get_obj_sense() == idol::Minimize ? 1. : -1.;
}

bool OsiIdolSolverInterface::isContinuous(int colIndex) const {
    const auto var = m_model->get_var_by_index(colIndex);
    return m_model->get_var_type(var) == idol::Continuous;
}

const CoinPackedMatrix *OsiIdolSolverInterface::getMatrixByRow() const {
    OSI_IDOL_DEBUG
    if (!m_matrix_by_row) {

        const unsigned int n_rows = getNumRows();
        const unsigned int n_cols = getNumCols();

        m_matrix_by_row = new CoinPackedMatrix(false, n_rows, n_cols);

        for (int i = 0 ; i < n_rows ; ++i) {
            const auto ctr = m_model->get_ctr_by_index(i);
            const auto &row = m_model->get_ctr_row(ctr);

            CoinPackedVector row_vector;
            for (const auto &[var, constant]: row) {
                const int index = (int) m_model->get_var_index(var);
                row_vector.insert(index, constant);
            }

            m_matrix_by_row->appendRow(row_vector);
        }

    }
    return m_matrix_by_row;
}

const CoinPackedMatrix *OsiIdolSolverInterface::getMatrixByCol() const {
    OSI_IDOL_DEBUG

    if (!m_matrix_by_col) {

        const unsigned int n_rows = getNumRows();
        const unsigned int n_cols = getNumCols();

        m_matrix_by_col = new CoinPackedMatrix(true, n_cols, n_rows);

        for (int i = 0 ; i < n_cols ; ++i) {
            const auto var = m_model->get_var_by_index(i);
            const auto &column = m_model->get_var_column(var);

            CoinPackedVector col_vector;
            for (const auto &[ctr, constant]: column) {
                const int index = (int) m_model->get_ctr_index(ctr);
                col_vector.insert(index, constant);
            }

            m_matrix_by_col->appendCol(col_vector);
        }

    }

    return m_matrix_by_col;
}

double OsiIdolSolverInterface::getInfinity() const {
    return idol::Inf;
}

const double *OsiIdolSolverInterface::getColSolution() const {


    const unsigned int n_cols = getNumCols();

    if (n_cols > 0 && !m_col_solution) {

        m_col_solution = new double[n_cols];
        for (int i = 0 ; i < n_cols ; ++i) {
            const auto var = m_model->get_var_by_index(i);
            m_col_solution[i] = m_model->get_var_primal(var);
        }

    }

    return m_col_solution;
}

const double *OsiIdolSolverInterface::getRowPrice() const {
    OSI_IDOL_DEBUG

    if (const unsigned int n_rows = getNumRows() ; n_rows > 0 && !m_row_price) {

        m_row_price = new double[n_rows];
        for (int i = 0 ; i < n_rows ; ++i) {
            const auto ctr = m_model->get_ctr_by_index(i);
            m_row_price[i] = m_model->get_ctr_dual(ctr);
        }

    }

    return m_row_price;

}

const double *OsiIdolSolverInterface::getReducedCost() const {
    OSI_IDOL_DEBUG

    if (const unsigned int n_cols = getNumCols() ; n_cols > 0 && !m_reduced_cost) {

        m_reduced_cost = new double[n_cols];
        for (int i = 0 ; i < n_cols ; ++i) {
            const auto var = m_model->get_var_by_index(i);
            m_reduced_cost[i] = m_model->get_var_reduced_cost(var);
        }

    }

    return m_reduced_cost;

}

const double *OsiIdolSolverInterface::getRowActivity() const {
    OSI_IDOL_DEBUG

    throw idol::Exception("OsiSolverInterface::getRowActivity not implemented.");

    if (const unsigned int n_rows = getNumRows() ; n_rows > 0 && !m_row_activity) {
        m_row_activity = new double[n_rows];
        const auto& solution = getColSolution();
        const auto* matrix = getMatrixByRow();
        matrix->times(solution, m_row_activity);
    }

    return m_row_activity;
}

double OsiIdolSolverInterface::getObjValue() const {
    assert(idol::equals(m_model->get_obj_expr().affine().constant(), 0., 1e-4));
    return m_model->get_best_obj();
}

int OsiIdolSolverInterface::getIterationCount() const {
    OSI_IDOL_DEBUG
    std::cout << "Returning fake number of iterations." << std::endl;
    return 1;
}

std::vector<double *> OsiIdolSolverInterface::getDualRays(int maxNumRays, bool fullRay) const {
    OSI_IDOL_DEBUG

    if (fullRay) {
        throw idol::Exception("OsiIdolSolverInterface:getDualRays: Full ray not implemented.");
    }

    const unsigned int n_rows = getNumRows();
    auto* ray = new double[n_rows];

    for (int i = 0 ; i < n_rows ; ++i) {
        const auto ctr = m_model->get_ctr_by_index(i);
        ray[i] = m_model->get_ctr_farkas(ctr);
    }

    return std::vector<double*>(1, ray);
}

std::vector<double *> OsiIdolSolverInterface::getPrimalRays(int maxNumRays) const {
    OSI_IDOL_DEBUG

    const unsigned int n_cols = getNumCols();
    auto* ray = new double[n_cols];

    for (int i = 0 ; i < n_cols ; ++i) {
        const auto var = m_model->get_var_by_index(i);
        ray[i] = m_model->get_var_ray(var);
    }

    return std::vector<double*>(1, ray);
}

void OsiIdolSolverInterface::setObjCoeff(int elementIndex, double elementValue) {
    OSI_IDOL_DEBUG

    if (m_col_obj) {
        m_col_obj[elementIndex] = elementValue;
        delete[] m_reduced_cost; m_reduced_cost = nullptr;
    }

    const auto var = m_model->get_var_by_index(elementIndex);
    m_model->set_var_obj(var, elementValue);
}

void OsiIdolSolverInterface::setObjSense(double s) {
    m_model->set_obj_sense(s > 0 ? idol::Minimize : idol::Maximize);
}

void OsiIdolSolverInterface::setColLower(int elementIndex, double elementValue) {

    const auto var = m_model->get_var_by_index(elementIndex);
    m_model->set_var_lb(var, elementValue);

    if (m_col_lower) {
        m_col_lower[elementIndex] = elementValue;
    }

}

void OsiIdolSolverInterface::setColUpper(int elementIndex, double elementValue) {

    const auto var = m_model->get_var_by_index(elementIndex);
    m_model->set_var_ub(var, elementValue);

    if (m_col_upper) {
        m_col_upper[elementIndex] = elementValue;
    }

}

void OsiIdolSolverInterface::setRowLower(int elementIndex, double elementValue) {

    double rhs = getRightHandSide()[elementIndex];
    double range = getRowRange()[elementIndex];
    char sense = getRowSense()[elementIndex];
    double lower = 0, upper = 0;

    convertSenseToBound(sense, rhs, range, lower, upper);
    if (!idol::equals(lower, elementValue, m_model->env().get_tol_feasibility())) {
        convertBoundToSense(elementValue, upper, sense, rhs, range);
        setRowType(elementIndex, sense, rhs, range);
    }
}

void OsiIdolSolverInterface::setRowUpper(int i, double elementValue) {

    double rhs = getRightHandSide()[i];
    double range = getRowRange()[i];
    char sense = getRowSense()[i];
    double lower = 0, upper = 0;

    convertSenseToBound(sense, rhs, range, lower, upper);
    if (!idol::equals(upper, elementValue, m_model->env().get_tol_feasibility())) {
        convertBoundToSense(lower, elementValue, sense, rhs, range);
        setRowType(i, sense, rhs, range);
    }

}

void OsiIdolSolverInterface::setRowType(int index, char sense, double rightHandSide, double range) {

    idol::CtrType type;
    if (sense == 'E') {
        type = idol::Equal;
    } else if (sense == 'L') {
        type = idol::LessOrEqual;
    } else if (sense == 'G') {
        type = idol::GreaterOrEqual;
    } else if (sense == 'R') {
        // range constraint: lb = rhs - range, ub = rhs
        throw idol::Exception("Range constraints not implemented yet.");
    } else {
        throw idol::Exception("Unknown sense.");
    }

    const auto& ctr = m_model->get_ctr_by_index(index);
    m_model->set_ctr_type(ctr, type);
    m_model->set_ctr_rhs(ctr, rightHandSide);

    delete[] m_row_sense; m_row_sense = nullptr;
}

void OsiIdolSolverInterface::setColSolution(const double *colsol) {
    OSI_IDOL_DEBUG
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setRowPrice(const double *rowprice) {
    OSI_IDOL_DEBUG
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setContinuous(int index) {
    const auto var = m_model->get_var_by_index(index);
    m_model->set_var_type(var, idol::Continuous);
}

void OsiIdolSolverInterface::setInteger(int index) {
    const auto var = m_model->get_var_by_index(index);
    m_model->set_var_type(var, idol::Integer);
}

void OsiIdolSolverInterface::addCol(const CoinPackedVectorBase &vec, const double collb, const double colub,
                                    const double obj) {
    OSI_IDOL_DEBUG

    const auto* indices = vec.getIndices();
    const auto* values = vec.getElements();

    idol::LinExpr<idol::Ctr> column;
    for (unsigned int i = 0, n = vec.getNumElements() ; i < n ; ++i) {
        const int index = indices[i];
        const double value = values[i];
        const auto& ctr = m_model->get_ctr_by_index(index);
        column.set(ctr, value);
    }

    m_model->add_var(collb, colub, idol::Continuous, obj, std::move(column));

    std::cout << "WARNING: adding column relying on potentially wrong indices..." << std::endl;
}

void OsiIdolSolverInterface::deleteCols(const int num, const int *colIndices) {
    OSI_IDOL_DEBUG
    throw idol::Exception("OsiIdolSolverInterface: Delete columns is not implemented.");
    for (int i = 0 ; i < num ; ++i) {
        const int index = colIndices[i];
        const auto var = m_model->get_var_by_index(index);
        m_model->remove(var);
    }

}

void OsiIdolSolverInterface::addRow(const CoinPackedVectorBase &vec, const double rowlb, const double rowub) {
    OSI_IDOL_DEBUG

    const double tol_feasibility = m_model->optimizer().get_tol_feasibility();

    const auto* indices = vec.getIndices();
    const auto* values = vec.getElements();

    double rhs;
    idol::CtrType type;
    if (idol::equals(rowlb, rowub, tol_feasibility)) {
        type = idol::Equal;
        rhs = rowlb;
    } else if (idol::is_pos_inf(rowub)) {
        type = idol::GreaterOrEqual;
        rhs = rowlb;
    } else if (idol::is_neg_inf(rowlb)) {
        type = idol::LessOrEqual;
        rhs = rowub;
    } else {
        std::cout << rowlb << " " << rowub << std::endl;
        throw idol::Exception("Range constraints are not implemented.");
    }

    idol::LinExpr<idol::Var> lhs;
    for (unsigned int i = 0, n = vec.getNumElements() ; i < n ; ++i) {
        const int index = indices[i];
        const double value = values[i];
        const auto& var = m_model->get_var_by_index(index);
        lhs += value * var;
    }

    m_model->add_ctr(idol::TempCtr(std::move(lhs), type, rhs));

    // TODO: this can be handled more efficiently for the matrix (i.e., just add the row)
    delete[] m_row_sense; m_row_sense = nullptr;
    delete[] m_row_rhs;   m_row_rhs   = nullptr;
    delete[] m_row_range; m_row_range = nullptr;
    delete[] m_row_lower; m_row_lower = nullptr;
    delete[] m_row_upper; m_row_upper = nullptr;
    delete[] m_row_activity; m_row_activity = nullptr;
    delete[] m_row_price; m_row_price = nullptr;
    delete m_matrix_by_col; m_matrix_by_col = nullptr;
    delete m_matrix_by_row; m_matrix_by_row = nullptr;

    std::cout << "WARNING: adding row relying on potentially wrong indices..." << std::endl;
}

void OsiIdolSolverInterface::addRow(const CoinPackedVectorBase &vec, const char rowsen, const double rowrhs,
                                    const double rowrng) {
    OSI_IDOL_DEBUG

    double lb = -idol::Inf, ub = idol::Inf;
    IdolConvertSenseToBound( rowsen, rowrng, rowrhs, lb, ub );
    addRow(vec, lb, ub);
}

void OsiIdolSolverInterface::loadProblem(const CoinPackedMatrix &matrix, const double *collb, const double *colub,
                                         const double *obj, const double *rowlb, const double *rowub) {

    int n_rows = matrix.getNumRows();

    char   * row_sense;
    double * row_rhs;
    double * row_range;

    if( n_rows ) {
        row_sense = new char  [n_rows];
        row_rhs   = new double[n_rows];
        row_range = new double[n_rows];
    }
    else {
        row_sense = nullptr;
        row_rhs   = nullptr;
        row_range = nullptr;
    }

    int i;
    if( rowlb == nullptr && rowub == nullptr)
        for ( i = n_rows - 1; i >= 0; --i )
            convertBoundToSense(-idol::Inf, idol::Inf, row_sense[i], row_rhs[i], row_range[i] );
    else if( rowlb == nullptr)
        for ( i = n_rows - 1; i >= 0; --i )
            convertBoundToSense(-idol::Inf, rowub[i], row_sense[i], row_rhs[i], row_range[i] );
    else if( rowub == nullptr)
        for ( i = n_rows - 1; i >= 0; --i )
            convertBoundToSense(rowlb[i], idol::Inf, row_sense[i], row_rhs[i], row_range[i] );
    else
        for ( i = n_rows - 1; i >= 0; --i )
            convertBoundToSense(rowlb[i], rowub[i], row_sense[i], row_rhs[i], row_range[i] );

    loadProblem(matrix, collb, colub, obj, row_sense, row_rhs, row_range );

    if( n_rows ) {
        delete [] row_sense;
        delete [] row_rhs;
        delete [] row_range;
    }

}

void OsiIdolSolverInterface::assignProblem(CoinPackedMatrix *&matrix, double *&collb, double *&colub, double *&obj,
                                           double *&rowlb, double *&rowub) {

    loadProblem( *matrix, collb, colub, obj, rowlb, rowub );

    delete matrix;   matrix = nullptr;
    delete[] collb;  collb  = nullptr;
    delete[] colub;  colub  = nullptr;
    delete[] obj;    obj    = nullptr;
    delete[] rowlb;  rowlb  = nullptr;
    delete[] rowub;  rowub  = nullptr;

}

void OsiIdolSolverInterface::loadProblem(const CoinPackedMatrix &matrix,
                                         const double *collb,
                                         const double *colub,
                                         const double *obj,
                                         const char *rowsen,
                                         const double *rowrhs,
                                         const double *rowrng) {

    assert(m_model->vars().size() == 0 && m_model->ctrs().size() == 0);

    const int n_cols = matrix.getNumCols();
    const int n_rows = matrix.getNumRows();

    if (matrix.isColOrdered()) {
        // Add constraints
        for (int i = 0 ; i < n_rows ; ++i) {

            idol::CtrType type;
            if (rowsen[i] == 'E') {
                type = idol::Equal;
            } else if (rowsen[i] == 'L') {
                type = idol::LessOrEqual;
            } else if (rowsen[i] == 'G') {
                type = idol::GreaterOrEqual;
            } else {
                throw idol::Exception("OsiIdolSolverInterface: Unknown sense.");
            }
            assert(idol::equals(rowrng[i], 0., 1e-4));
            m_model->add_ctr(idol::LinExpr<idol::Var>(), type, rowrhs[i]);
        }

        // Add variables
        for (int i = 0 ; i < n_cols ; ++i) {
            idol::LinExpr<idol::Ctr> column;
            const CoinShallowPackedVector& col = matrix.getVector(i);
            for (int j = 0 ; j < col.getNumElements() ; ++j) {
                const int row = col.getIndices()[j];
                const double value = col.getElements()[j];
                const auto& ctr = m_model->get_ctr_by_index(row);
                column += value * ctr;
            }
            m_model->add_var(collb[i], colub[i], idol::Continuous, obj[i], std::move(column));
        }
    } else {

        // Add variables
        for (int i = 0 ; i < n_cols ; ++i) {
            m_model->add_var(collb[i], colub[i], idol::Continuous, obj[i]);
        }

        // Add constraints
        for (int i = 0 ; i < n_rows ; ++i) {

            idol::CtrType type;
            if (rowsen[i] == 'E') {
                type = idol::Equal;
            } else if (rowsen[i] == 'L') {
                type = idol::LessOrEqual;
            } else if (rowsen[i] == 'G') {
                type = idol::GreaterOrEqual;
            } else {
                throw idol::Exception("OsiIdolSolverInterface: Unknown sense.");
            }
            assert(idol::equals(rowrng[i], 0., 1e-4));

            idol::LinExpr expr;
            const CoinShallowPackedVector& row = matrix.getVector(i);

            for (int j = 0 ; j < row.getNumElements() ; ++j) {
                const int col = row.getIndices()[j];
                const double value = row.getElements()[j];
                const auto& var = m_model->get_var_by_index(col);

                expr += value * var;
            }

            m_model->add_ctr(std::move(expr), type, rowrhs[i]);

        }
    }

}

void OsiIdolSolverInterface::assignProblem(CoinPackedMatrix *&matrix, double *&collb, double *&colub, double *&obj,
                                           char *&rowsen, double *&rowrhs, double *&rowrng) {
    OSI_IDOL_DEBUG

    loadProblem( *matrix, collb, colub, obj, rowsen, rowrhs, rowrng );

    delete matrix;   matrix = nullptr;
    delete[] collb;  collb  = nullptr;
    delete[] colub;  colub  = nullptr;
    delete[] obj;    obj    = nullptr;
    delete[] rowsen; rowsen = nullptr;
    delete[] rowrhs; rowrhs = nullptr;
    delete[] rowrng; rowrng = nullptr;
}

void
OsiIdolSolverInterface::loadProblem(const int numcols, const int numrows, const CoinBigIndex *start, const int *index,
                                    const double *value, const double *collb, const double *colub, const double *obj,
                                    const double *rowlb, const double *rowub) {
    OSI_IDOL_DEBUG

    char * rowSense = new char  [numrows];
    auto * rowRhs   = new double[numrows];
    auto * rowRange = new double[numrows];

    for ( int i = numrows - 1; i >= 0; --i )
    {
        const double lower = rowlb ? rowlb[i] : -idol::Inf;
        const double upper = rowub ? rowub[i] : idol::Inf;
        convertBoundToSense( lower, upper, rowSense[i], rowRhs[i], rowRange[i] );
    }

    loadProblem(numcols, numrows, start, index, value, collb, colub, obj,
                rowSense, rowRhs, rowRange);

    delete [] rowSense;
    delete [] rowRhs;
    delete [] rowRange;

}

void
OsiIdolSolverInterface::loadProblem(const int numcols, const int numrows, const CoinBigIndex *start, const int *index,
                                    const double *value, const double *collb, const double *colub, const double *obj,
                                    const char *rowsen, const double *rowrhs, const double *rowrng) {
    OSI_IDOL_DEBUG
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::writeMps(const char *filename, const char *extension, double objSense) const {
    OSI_IDOL_DEBUG

    std::string f(filename);
    std::string e(extension);
    std::string fullname = f + "." + e;

    writeMpsNative(fullname.c_str(),NULL, NULL, 0, 2, objSense);

}

OsiSolverInterface *OsiIdolSolverInterface::clone(bool copyData) const {
    OSI_IDOL_DEBUG

    throw idol::Exception("OsiIdolSolverInterface::clone is not implemented.");
}

void OsiIdolSolverInterface::applyRowCut(const OsiRowCut &rc) {
    OSI_IDOL_DEBUG

    const auto& row = rc.row();
    addRow(row, rc.lb(), rc.ub());
}

void OsiIdolSolverInterface::applyColCut(const OsiColCut &cc) {
    OSI_IDOL_DEBUG

    const unsigned int n_cols = getNumCols();
    auto * MskColLB = new double[n_cols];
    auto * MskColUB = new double[n_cols];
    const CoinPackedVector & lbs = cc.lbs();
    const CoinPackedVector & ubs = cc.ubs();
    int i;

    for( i = 0; i < getNumCols(); ++i )
    {
        MskColLB[i] = getColLower()[i];
        MskColUB[i] = getColUpper()[i];
    }

    for( i = 0; i < lbs.getNumElements(); ++i )
        if ( lbs.getElements()[i] > MskColLB[lbs.getIndices()[i]] )
            setColLower( lbs.getIndices()[i], lbs.getElements()[i] );
    for( i = 0; i < ubs.getNumElements(); ++i )
        if ( ubs.getElements()[i] < MskColUB[ubs.getIndices()[i]] )
            setColUpper( ubs.getIndices()[i], ubs.getElements()[i] );

    delete[] MskColLB;
    delete[] MskColUB;

}

void OsiIdolSolverInterface::deleteRows(const int num, const int *rowIndices) {
    OSI_IDOL_DEBUG
    throw idol::Exception("OsiIdolSolverInterface: Delete rows is not implemented.");
}

void OsiIdolSolverInterface::writeLp(const char *filename, const char *extension, double epsilon, int numberAcross,
                                     int decimals, double objSense, bool useRowNames) const {
    m_model->write(std::string(filename).append(".").append(extension));
}

void OsiIdolSolverInterface::relax() {
    for (const auto& var : m_model->vars()) {
        const auto type = m_model->get_var_type(var);
        if (type == idol::Integer || type == idol::Binary) {
            m_model->set_var_type(var, idol::Continuous);
            m_relaxed_variables.emplace_back(var, type);
        }
    }
}

void OsiIdolSolverInterface::unrelax() {
    for (const auto& [var, prev_type] : m_relaxed_variables) {
        assert(prev_type == idol::Integer || prev_type == idol::Binary);
        m_model->set_var_type(var, prev_type);
    }
    m_relaxed_variables.clear();
}

#endif
