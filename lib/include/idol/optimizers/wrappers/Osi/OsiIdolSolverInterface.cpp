//
// Created by henri on 05.02.24.
//

#include "OsiIdolSolverInterface.h"
#include "idol/modeling/expressions/operations/operators.h"

OsiIdolSolverInterface::OsiIdolSolverInterface(idol::Model &t_model) : m_model(t_model.copy()) {

}

void OsiIdolSolverInterface::initialSolve() {
    m_model.optimize();
}

void OsiIdolSolverInterface::resolve() {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::branchAndBound() {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

bool OsiIdolSolverInterface::isAbandoned() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

bool OsiIdolSolverInterface::isProvenOptimal() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

bool OsiIdolSolverInterface::isProvenPrimalInfeasible() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

bool OsiIdolSolverInterface::isProvenDualInfeasible() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

bool OsiIdolSolverInterface::isIterationLimitReached() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

CoinWarmStart *OsiIdolSolverInterface::getEmptyWarmStart() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

CoinWarmStart *OsiIdolSolverInterface::getWarmStart() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

bool OsiIdolSolverInterface::setWarmStart(const CoinWarmStart *warmstart) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

int OsiIdolSolverInterface::getNumCols() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

int OsiIdolSolverInterface::getNumRows() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

CoinBigIndex OsiIdolSolverInterface::getNumElements() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getColLower() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getColUpper() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const char *OsiIdolSolverInterface::getRowSense() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getRightHandSide() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getRowRange() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getRowLower() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getRowUpper() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getObjCoefficients() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

double OsiIdolSolverInterface::getObjSense() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

bool OsiIdolSolverInterface::isContinuous(int colIndex) const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const CoinPackedMatrix *OsiIdolSolverInterface::getMatrixByRow() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const CoinPackedMatrix *OsiIdolSolverInterface::getMatrixByCol() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

double OsiIdolSolverInterface::getInfinity() const {
    return idol::Inf;
}

const double *OsiIdolSolverInterface::getColSolution() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getRowPrice() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getReducedCost() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

const double *OsiIdolSolverInterface::getRowActivity() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

double OsiIdolSolverInterface::getObjValue() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

int OsiIdolSolverInterface::getIterationCount() const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

std::vector<double *> OsiIdolSolverInterface::getDualRays(int maxNumRays, bool fullRay) const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

std::vector<double *> OsiIdolSolverInterface::getPrimalRays(int maxNumRays) const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setObjCoeff(int elementIndex, double elementValue) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setObjSense(double s) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setColLower(int elementIndex, double elementValue) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setColUpper(int elementIndex, double elementValue) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setRowLower(int elementIndex, double elementValue) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setRowUpper(int elementIndex, double elementValue) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setRowType(int index, char sense, double rightHandSide, double range) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setColSolution(const double *colsol) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setRowPrice(const double *rowprice) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setContinuous(int index) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::setInteger(int index) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::addCol(const CoinPackedVectorBase &vec, const double collb, const double colub,
                                    const double obj) {

    const auto* indices = vec.getIndices();
    const auto* values = vec.getElements();

    idol::Column column(obj);
    for (unsigned int i = 0, n = vec.getNumElements() ; i < n ; ++i) {
        const int index = indices[i];
        const double value = values[i];
        const auto& ctr = m_model.get_ctr_by_index(index);
        column.linear().set(ctr, value);
    }

    m_model.add_var(collb, colub, idol::Continuous, std::move(column));

    std::cout << "WARNING: adding column relying on potentially wrong indices..." << std::endl;
}

void OsiIdolSolverInterface::deleteCols(const int num, const int *colIndices) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::addRow(const CoinPackedVectorBase &vec, const double rowlb, const double rowub) {

    const auto* indices = vec.getIndices();
    const auto* values = vec.getElements();

    double rhs;
    idol::CtrType type;
    if (idol::equals(rowlb, rowub, idol::Tolerance::Feasibility)) {
        type = idol::Equal;
        rhs = rowlb;
    } else if (idol::is_pos_inf(rowub)) {
        type = idol::GreaterOrEqual;
        rhs = rowlb;
    } else if (idol::is_neg_inf(rowlb)) {
        type = idol::LessOrEqual;
        rhs = rowub;
    } else {
        throw idol::Exception("Range constraints are not implemented.");
    }

    idol::Expr lhs;
    for (unsigned int i = 0, n = vec.getNumElements() ; i < n ; ++i) {
        const int index = indices[i];
        const double value = values[i];
        const auto& var = m_model.get_var_by_index(index);
        lhs.linear().set(var, value);
    }

    m_model.add_ctr(idol::TempCtr(idol::Row(std::move(lhs), rhs), type));

    std::cout << "WARNING: adding lhs relying on potentially wrong indices..." << std::endl;
}

void OsiIdolSolverInterface::addRow(const CoinPackedVectorBase &vec, const char rowsen, const double rowrhs,
                                    const double rowrng) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::loadProblem(const CoinPackedMatrix &matrix, const double *collb, const double *colub,
                                         const double *obj, const double *rowlb, const double *rowub) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::assignProblem(CoinPackedMatrix *&matrix, double *&collb, double *&colub, double *&obj,
                                           double *&rowlb, double *&rowub) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::loadProblem(const CoinPackedMatrix &matrix, const double *collb, const double *colub,
                                         const double *obj, const char *rowsen, const double *rowrhs,
                                         const double *rowrng) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::assignProblem(CoinPackedMatrix *&matrix, double *&collb, double *&colub, double *&obj,
                                           char *&rowsen, double *&rowrhs, double *&rowrng) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void
OsiIdolSolverInterface::loadProblem(const int numcols, const int numrows, const CoinBigIndex *start, const int *index,
                                    const double *value, const double *collb, const double *colub, const double *obj,
                                    const double *rowlb, const double *rowub) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void
OsiIdolSolverInterface::loadProblem(const int numcols, const int numrows, const CoinBigIndex *start, const int *index,
                                    const double *value, const double *collb, const double *colub, const double *obj,
                                    const char *rowsen, const double *rowrhs, const double *rowrng) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::writeMps(const char *filename, const char *extension, double objSense) const {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

OsiSolverInterface *OsiIdolSolverInterface::clone(bool copyData) const {
    if (!copyData) {
        throw idol::Exception("OsiIdolSolverInterface::clone with copyData = false not implemented.");
    }
    return new OsiIdolSolverInterface(*this);
}

void OsiIdolSolverInterface::applyRowCut(const OsiRowCut &rc) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::applyColCut(const OsiColCut &cc) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

void OsiIdolSolverInterface::deleteRows(const int num, const int *rowIndices) {
    throw idol::Exception("OsiIdolSolverInterface: Not implemented.");
}

OsiIdolSolverInterface::OsiIdolSolverInterface(const OsiIdolSolverInterface &t_src)
    : m_model(t_src.m_model.copy()) {

}

void OsiIdolSolverInterface::writeLp(const char *filename, const char *extension, double epsilon, int numberAcross,
                                     int decimals, double objSense, bool useRowNames) const {
    const_cast<idol::Model &>(m_model).write(std::string(filename).append(".").append(extension));
}
