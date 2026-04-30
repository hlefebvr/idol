//
// Created by Henri on 30/04/2026.
//
#ifndef IDOL_QPEXTERNSOLVERWITHSNAPSHOT_H
#define IDOL_QPEXTERNSOLVERWITHSNAPSHOT_H

#include "idol/general/utils/exceptions/Exception.h"

namespace idol::impl {
    class QpExternSolverWithSnapshot;
}


#ifdef IDOL_USE_YASOL
#include <ExternSolvers/QpExternSolver.hpp>

class idol::impl::QpExternSolverWithSnapshot : public extSol::QpExternSolver {
protected:
    std::vector<data::QpRhs> RHSs;
    std::vector< std::vector<data::IndexedElement> > LHSs;
    std::vector<data::QpRhs> RHSsSaved;
    std::vector< std::vector<data::IndexedElement> > LHSsSaved;
    std::vector< std::vector<data::IndexedElement> > COLs;
    std::vector<data::IndexedElement> obj_lhs;
    std::vector<data::IndexedElement> obj_lhs_dense;
    std::vector<data::IndexedElement> obj_lhs_saved;
    std::vector<data::IndexedElement> obj_lhs_dense_saved;
    data::QpRhs obj_rhs;
    std::vector<int> lazyRows;
    std::vector<bool> lazyRowIndicator;
    std::vector<int> indicators;
public:
    QpExternSolverWithSnapshot(QpExtSolSolverType t, QpExtSolSolverMode m);

    int computeStatus(const std::vector<data::IndexedElement>& lhs, const data::QpRhs& rhs) override;
    void setStatus(int i, int j) override;
    int getStatus(int i) override;
    void setLazyStatus(int i, bool s) override;
    bool getLazyStatus(int i) override;
    bool getLazyRows(std::vector<int>& lazyRows, std::vector<data::QpNum>& solution, double eps) override;

    // Snapshot functions
    void clearLP_snapshot() override;
    void clearLP_snapshot(int) override;
    void clearLP_snapshot(int, int) override;
    void saveSnapshot() override;
    void retrieveSnapshot() override;
    void addLProw_snapshot(const std::vector<data::IndexedElement>& lhs, const data::QpRhs& rhs) override;
    void addLPobj_snapshot(const std::vector<data::IndexedElement>& o_lhs, const data::QpRhs& o_rhs) override;
    int getLProws_snapshot() override  { return RHSs.size(); }
    void initInternalLP_snapshot(const data::Qlp& qlp) override;
    void reinitLPcols_snapshot() override;
    std::vector<data::IndexedElement>* getCol_snapshot(int i) override { return &(COLs[i]); }
    data::IndexedElement getObj_snapshot(int i) override  { if (obj_lhs_dense.size()>0) return obj_lhs_dense[i]; else return 0.0; }
    std::vector<data::IndexedElement>* getRowLhs_snapshot(unsigned ri) override;
    std::vector<data::QpRhs>* getRowRhs_snapshot() override  { return &RHSs; }
};
#endif

#endif //IDOL_QPEXTERNSOLVERWITHSNAPSHOT_H