//
// Created by Henri on 26/03/2026.
//

#ifndef IDOL_OSIIDOLCGLSOLVERINTERFACE_H
#define IDOL_OSIIDOLCGLSOLVERINTERFACE_H

#ifdef IDOL_USE_CGL
#include <OsiSolverInterface.hpp>
#include <OsiCuts.hpp>

#define THROW_CALLED { throw Exception("Function " + std::string(__FUNCTION__) + " was called by Cgl."); }

template<class NodeInfoT>
class idol::CglCutCallback<NodeInfoT>::Strategy::OsiIdolCglSolverInterface : public OsiSolverInterface {
    Strategy& m_parent;
    std::unique_ptr<CoinPackedMatrix> m_matrix;
    std::vector<double> m_col_lower;
    std::vector<double> m_col_upper;
    std::vector<char> m_row_sense;
    std::vector<double> m_rhs;
    std::vector<double> m_row_lower;
    std::vector<double> m_row_upper;
    std::vector<double> m_current_solution;
    std::vector<double> m_row_activity;
public:
    OsiIdolCglSolverInterface(Strategy& parent) : m_parent(parent) {

    }

    void update_current_solution() {
        m_row_activity.clear();
        const auto& model = m_parent.original_model();
        const unsigned int n_cols = model.vars().size();
        m_current_solution = std::vector<double>(n_cols);
        const auto& current_solution = m_parent.node().info().primal_solution();
        for (unsigned int index = 0 ; index < n_cols ; ++index) {
            const auto& var = model.get_var_by_index(index);
            m_current_solution[index] = current_solution.get(var);
        }
    }

    void initialSolve() override THROW_CALLED
    void resolve() override THROW_CALLED
    void branchAndBound() override THROW_CALLED
    bool isAbandoned() const override THROW_CALLED
    bool isProvenOptimal() const override THROW_CALLED
    bool isProvenPrimalInfeasible() const override THROW_CALLED
    bool isProvenDualInfeasible() const override THROW_CALLED
    CoinWarmStart* getEmptyWarmStart() const override THROW_CALLED
    CoinWarmStart* getWarmStart() const override THROW_CALLED
    bool setWarmStart(const CoinWarmStart* warmstart) override THROW_CALLED
    int getNumCols() const override { return m_parent.original_model().vars().size(); }
    int getNumRows() const override { return m_parent.original_model().ctrs().size(); }
    CoinBigIndex getNumElements() const override THROW_CALLED

    const double* getColLower() const override {

        if (m_col_lower.empty()) {
            auto& me = const_cast<OsiIdolCglSolverInterface&>(*this);
            const auto& model = m_parent.original_model();
            const auto n_cols = model.vars().size();
            me.m_col_lower = std::vector<double>(n_cols);
            for (unsigned int index = 0 ; index < n_cols ; ++index) {
                const auto& var = model.get_var_by_index(index);
                me.m_col_lower[index] = model.get_var_lb(var);
            }
        }

        assert(m_col_lower.size() == m_parent.original_model().vars().size());
        return m_col_lower.data();
    }

    const double* getColUpper() const override {

        if (m_col_upper.empty()) {
            auto& me = const_cast<OsiIdolCglSolverInterface&>(*this);
            const auto& model = m_parent.original_model();
            const auto n_cols = model.vars().size();
            me.m_col_upper = std::vector<double>(n_cols);
            for (unsigned int index = 0 ; index < n_cols; ++index) {
                const auto& var = model.get_var_by_index(index);
                me.m_col_upper[index] = model.get_var_ub(var);
            }
        }

        assert(m_col_upper.size() == m_parent.original_model().vars().size());
        return m_col_upper.data();
    }

    const char* getRowSense() const override {

        if (m_row_sense.empty()) {
            auto& me = const_cast<OsiIdolCglSolverInterface&>(*this);
            const auto& model = m_parent.original_model();
            const auto n_rows = model.ctrs().size();
            me.m_row_sense = std::vector<char>(n_rows);
            for (unsigned int index = 0 ; index < n_rows ; ++index) {
                const auto ctr = model.get_ctr_by_index(index);
                const auto type = model.get_ctr_type(ctr);
                switch (type) {
                case LessOrEqual:
                    me.m_row_sense[index] = 'L';
                    break;
                case GreaterOrEqual:
                    me.m_row_sense[index] = 'G';
                    break;
                case Equal:
                    me.m_row_sense[index] = 'E';
                    break;
                default:
                    throw Exception("Constraint type out of bounds.");
                }
            }
        }

        assert(m_row_sense.size() == m_parent.original_model().ctrs().size());
        return m_row_sense.data();
    }

    const double* getRightHandSide() const override {

        if (m_rhs.empty()) {
            auto& me = const_cast<OsiIdolCglSolverInterface&>(*this);
            const auto& model = m_parent.original_model();
            const auto n_rows = model.ctrs().size();
            me.m_rhs = std::vector<double>(n_rows);
            for (unsigned int index = 0 ; index < n_rows ; ++index) {
                const auto ctr = model.get_ctr_by_index(index);
                me.m_rhs[index] = model.get_ctr_rhs(ctr);
            }
        }

        assert(m_rhs.size() == m_parent.original_model().ctrs().size());
        return m_rhs.data();
    }

    const double* getRowRange() const override THROW_CALLED

    const double* getRowLower() const override {

        if (m_row_lower.empty()) {
            auto& me = const_cast<OsiIdolCglSolverInterface&>(*this);
            const auto& model = m_parent.original_model();
            const unsigned int n_rows = model.ctrs().size();
            me.m_row_lower = std::vector<double>(n_rows);
            for (int i = 0 ; i < n_rows ; ++i) {
                const auto ctr = model.get_ctr_by_index(i);
                const auto rhs = model.get_ctr_rhs(ctr);
                const auto type = model.get_ctr_type(ctr);
                switch (type) {
                case LessOrEqual:
                    me.m_row_lower[i] = -std::numeric_limits<double>::infinity();
                    break;
                case GreaterOrEqual:
                    me.m_row_lower[i] = rhs;
                    break;
                case Equal:
                    me.m_row_lower[i] = rhs;
                    break;
                default:
                    throw Exception("Constraint type out of bounds.");
                }
            }
        }

        assert(m_row_lower.size() == m_parent.original_model().ctrs().size());
        return m_row_lower.data();
    }

    const double* getRowUpper() const override {

        if (m_row_upper.empty()) {
            auto& me = const_cast<OsiIdolCglSolverInterface&>(*this);
            const auto& model = m_parent.original_model();
            const unsigned int n_rows = model.ctrs().size();
            me.m_row_upper = std::vector<double>(n_rows);
            for (int i = 0 ; i < n_rows ; ++i) {
                const auto ctr = model.get_ctr_by_index(i);
                const auto rhs = model.get_ctr_rhs(ctr);
                const auto type = model.get_ctr_type(ctr);
                switch (type) {
                case idol::LessOrEqual:
                    me.m_row_upper[i] = rhs;
                    break;
                case idol::GreaterOrEqual:
                    me.m_row_upper[i] = std::numeric_limits<double>::infinity();
                    break;
                case idol::Equal:
                    me.m_row_upper[i] = rhs;
                    break;
                default:
                    throw Exception("Constraint type out of bounds.");
                }
            }
        }

        assert(m_row_upper.size() == m_parent.original_model().ctrs().size());
        return m_row_upper.data();
    }

    const double* getObjCoefficients() const override THROW_CALLED
    double getObjSense() const override THROW_CALLED

    bool isContinuous(int colIndex) const override {
        const auto& model = m_parent.original_model();
        assert(colIndex < model.vars().size());
        const auto& var = model.get_var_by_index(colIndex);
        return model.get_var_type(var) == Continuous;
    }

    const CoinPackedMatrix* getMatrixByRow() const override {

        if (!m_matrix) {

            const auto& model = m_parent.original_model();
            const auto n_rows = model.ctrs().size();
            const auto n_cols = model.vars().size();

            auto* me = const_cast<OsiIdolCglSolverInterface*>(this);
            me->m_matrix.reset(new CoinPackedMatrix(false, n_rows, n_cols));

            for (int i = 0 ; i < n_rows ; ++i) {
                const auto ctr = model.get_ctr_by_index(i);
                const auto &row = model.get_ctr_row(ctr);

                CoinPackedVector row_vector;
                for (const auto &[var, constant]: row) {
                    const int index = (int) model.get_var_index(var);
                    row_vector.insert(index, constant);
                }

                me->m_matrix->appendRow(row_vector);
            }

        }

        assert(m_matrix->getNumCols() == m_parent.original_model().vars().size());
        assert(m_matrix->getNumRows() == m_parent.original_model().ctrs().size());

        return m_matrix.get();
    }

    const CoinPackedMatrix* getMatrixByCol() const override THROW_CALLED

    double getInfinity() const override { return Inf; }

    const double* getColSolution() const override {
        assert(m_current_solution.size() == m_parent.original_model().vars().size());
        return m_current_solution.data();
    }

    const double* getRowPrice() const override THROW_CALLED
    const double* getReducedCost() const override THROW_CALLED

    const double* getRowActivity() const override {

        if (m_row_activity.empty()) {
            auto& me = const_cast<OsiIdolCglSolverInterface&>(*this);
            const auto& model = m_parent.original_model();
            const unsigned int n_rows = model.ctrs().size();
            me.m_row_activity = std::vector<double>(n_rows);
            const auto& current_solution = m_parent.node().info().primal_solution();
            for (unsigned int index = 0 ; index < n_rows ; ++index) {
                const auto& ctr = model.get_ctr_by_index(index);
                const auto& row = model.get_ctr_row(ctr);
                me.m_row_activity[index] = evaluate(row, current_solution);
            }
        }

        assert(m_row_activity.size() == m_parent.original_model().ctrs().size());
        return m_row_activity.data();
    }

    std::vector<double*> getDualRays(int maxNumRays, bool fullRay) const override THROW_CALLED
    std::vector<double*> getPrimalRays(int maxNumRays) const override THROW_CALLED
    void setObjCoeff(int elementIndex, double elementValue) override THROW_CALLED
    void setColLower(int elementIndex, double elementValue) override THROW_CALLED
    void setColUpper(int elementIndex, double elementValue) override THROW_CALLED
    void setRowLower(int elementIndex, double elementValue) override THROW_CALLED
    void setRowUpper(int elementIndex, double elementValue) override THROW_CALLED
    void setColSolution(const double* colsol) override THROW_CALLED
    void setRowPrice(const double* rowprice) override THROW_CALLED
    void setContinuous(int index) override THROW_CALLED
    void setInteger(int index) override THROW_CALLED
    void addCol(const CoinPackedVectorBase& vec, const double collb, const double colub, const double obj) override THROW_CALLED
    void addRow(const CoinPackedVectorBase& vec, const double rowlb, const double rowub) override THROW_CALLED
    void deleteRows(const int num, const int* rowIndices) override THROW_CALLED
    void loadProblem(const CoinPackedMatrix& matrix, const double* collb, const double* colub, const double* obj, const double* rowlb, const double* rowub) override THROW_CALLED
    void assignProblem(CoinPackedMatrix*& matrix, double*& collb, double*& colub, double*& obj, double*& rowlb, double*& rowub) override THROW_CALLED
    void loadProblem(const CoinPackedMatrix& matrix, const double* collb, const double* colub, const double* obj, const char* rowsen, const double* rowrhs, const double* rowrng) override THROW_CALLED
    void assignProblem(CoinPackedMatrix*& matrix, double*& collb, double*& colub, double*& obj, char*& rowsen, double*& rowrhs, double*& rowrng) override THROW_CALLED
    void loadProblem(const int numcols, const int numrows, const CoinBigIndex* start, const int* index, const double* value, const double* collb, const double* colub, const double* obj, const double* rowlb, const double* rowub) override THROW_CALLED
    void loadProblem(const int numcols, const int numrows, const CoinBigIndex* start, const int* index, const double* value, const double* collb, const double* colub, const double* obj, const char* rowsen, const double* rowrhs, const double* rowrng) override THROW_CALLED
    void writeMps(const char* filename, const char* extension, double objSense) const override THROW_CALLED
    OsiSolverInterface* clone(bool copyData) const override THROW_CALLED
    bool isIterationLimitReached() const override THROW_CALLED
    double getObjValue() const override THROW_CALLED
    int getIterationCount() const override THROW_CALLED
    void setObjSense(double s) override THROW_CALLED
    void setRowType(int index, char sense, double rightHandSide, double range) override THROW_CALLED
    void deleteCols(const int num, const int* colIndices) override THROW_CALLED
    void addRow(const CoinPackedVectorBase& vec, const char rowsen, const double rowrhs, const double rowrng) override THROW_CALLED

protected:
    void applyRowCut(const OsiRowCut& rc) override THROW_CALLED
    void applyColCut(const OsiColCut& cc) override THROW_CALLED
};

#endif
#endif //IDOL_OSIIDOLCGLSOLVERINTERFACE_H
