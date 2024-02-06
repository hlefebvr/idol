//
// Created by henri on 05.02.24.
//

#ifndef IDOL_OSIIDOLSOLVERINTERFACE_H
#define IDOL_OSIIDOLSOLVERINTERFACE_H

#ifdef IDOL_USE_OSI

#include <OsiSolverInterface.hpp>
#include "idol/modeling/models/Model.h"

class OsiIdolSolverInterface : public OsiSolverInterface {
    idol::Model m_model;
public:
    OsiIdolSolverInterface(idol::Model& t_model);

    OsiIdolSolverInterface(const OsiIdolSolverInterface& t_src);

    void initialSolve() override;
    void resolve() override;
    void branchAndBound() override;
    bool isAbandoned() const override;
    bool isProvenOptimal() const override;
    bool isProvenPrimalInfeasible() const override;
    bool isProvenDualInfeasible() const override;
    bool isIterationLimitReached() const override;
    CoinWarmStart *getEmptyWarmStart() const override;
    CoinWarmStart *getWarmStart() const override;
    bool setWarmStart(const CoinWarmStart *warmstart) override;
    int getNumCols() const override;
    int getNumRows() const override;
    CoinBigIndex getNumElements() const override;
    const double *getColLower() const override;
    const double *getColUpper() const override;
    const char *getRowSense() const override;
    const double *getRightHandSide() const override;
    const double *getRowRange() const override;
    const double *getRowLower() const override;
    const double *getRowUpper() const override;
    const double *getObjCoefficients() const override;
    double getObjSense() const override;
    bool isContinuous(int colIndex) const override;
    const CoinPackedMatrix *getMatrixByRow() const override;
    const CoinPackedMatrix *getMatrixByCol() const override;
    double getInfinity() const override;
    const double *getColSolution() const override;
    const double *getRowPrice() const override;
    const double *getReducedCost() const override;
    const double *getRowActivity() const override;
    double getObjValue() const override;
    int getIterationCount() const override;
    std::vector<double *> getDualRays(int maxNumRays, bool fullRay) const override;
    std::vector<double *> getPrimalRays(int maxNumRays) const override;
    void setObjCoeff(int elementIndex, double elementValue) override;
    void setObjSense(double s) override;
    void setColLower(int elementIndex, double elementValue) override;
    void setColUpper(int elementIndex, double elementValue) override;
    void setRowLower(int elementIndex, double elementValue) override;
    void setRowUpper(int elementIndex, double elementValue) override;
    void setRowType(int index, char sense, double rightHandSide, double range) override;
    void setColSolution(const double *colsol) override;
    void setRowPrice(const double *rowprice) override;
    void setContinuous(int index) override;
    void setInteger(int index) override;
    void addCol(const CoinPackedVectorBase &vec, const double collb, const double colub, const double obj) override;
    void deleteCols(const int num, const int *colIndices) override;
    void addRow(const CoinPackedVectorBase &vec, const double rowlb, const double rowub) override;
    void addRow(const CoinPackedVectorBase &vec, const char rowsen, const double rowrhs, const double rowrng) override;
    void loadProblem(const CoinPackedMatrix &matrix, const double *collb, const double *colub, const double *obj,
                     const double *rowlb, const double *rowub) override;
    void assignProblem(CoinPackedMatrix *&matrix, double *&collb, double *&colub, double *&obj, double *&rowlb,
                       double *&rowub) override;
    void loadProblem(const CoinPackedMatrix &matrix, const double *collb, const double *colub, const double *obj,
                     const char *rowsen, const double *rowrhs, const double *rowrng) override;
    void assignProblem(CoinPackedMatrix *&matrix, double *&collb, double *&colub, double *&obj, char *&rowsen,
                       double *&rowrhs, double *&rowrng) override;
    void
    loadProblem(const int numcols, const int numrows, const CoinBigIndex *start, const int *index, const double *value,
                const double *collb, const double *colub, const double *obj, const double *rowlb,
                const double *rowub) override;
    void
    loadProblem(const int numcols, const int numrows, const CoinBigIndex *start, const int *index, const double *value,
                const double *collb, const double *colub, const double *obj, const char *rowsen, const double *rowrhs,
                const double *rowrng) override;
    void writeMps(const char *filename, const char *extension, double objSense) const override;
    void writeLp(const char *filename,
                 const char *extension = "lp",
                 double epsilon = 1e-5,
                 int numberAcross = 10,
                 int decimals = 9,
                 double objSense = 0.0,
                 bool useRowNames = true) const override;
    OsiSolverInterface *clone(bool copyData = true) const override;
    void deleteRows(const int num, const int *rowIndices) override;
protected:
    void applyRowCut(const OsiRowCut &rc) override;
    void applyColCut(const OsiColCut &cc) override;
};

#endif

#endif //IDOL_OSIIDOLSOLVERINTERFACE_H
