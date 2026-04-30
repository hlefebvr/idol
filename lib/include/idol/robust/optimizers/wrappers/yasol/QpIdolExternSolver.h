//
// Created by Henri on 30/04/2026.
//

#ifndef IDOL_QPIDOLEXTERNSOLVER_H
#define IDOL_QPIDOLEXTERNSOLVER_H

#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/robust/optimizers/wrappers/yasol/QpExternSolverWithSnapshot.h"

namespace idol::impl {
    class QpIdolExternSolver;
}

#ifdef IDOL_USE_YASOL

#define THROW_NOT_IMPLEMENTED { throw idol::Exception("Not implemented " + std::string(__FUNCTION__) + ".\n" + std::string(__FILE__) + ":" + std::to_string(__LINE__)); }

class idol::impl::QpIdolExternSolver : public idol::impl::QpExternSolverWithSnapshot {
    idol::Model m_model;
    std::unique_ptr<idol::OptimizerFactory> m_optimizer_factory;

    static VarType to_idol(data::QpVar::NumberSystem t_type);
    static CtrType to_idol(data::QpRhs::RatioSign t_type);
    static extSol::QpExternSolver::QpExtSolSolutionStatus to_yasol(SolutionStatus t_status, SolutionReason t_reason);
public:
    QpIdolExternSolver(idol::Env& t_env, const OptimizerFactory& t_optimizer_factory);

    void clear() override THROW_NOT_IMPLEMENTED
    QpExtSolSolutionStatus getSolutionStatus() override;
    void* getSolverEnv() override { return nullptr; }
    void* getSolverModel() override { std::cout << "Called getSolverModel" << std::endl; return nullptr; }
    void init(const data::Qlp&) override THROW_NOT_IMPLEMENTED
    void init(const data::Qlp& qlp, data::QpRhs::Responsibility resp) override;
    void init(const data::QpObjFunc&, const std::vector<data::QpVar>&, const data::QpSparseMatrix&,
        const std::vector<data::QpRhs>&) override;
    void init(const std::string&) override THROW_NOT_IMPLEMENTED
    unsigned getVariableCount() const override;
    unsigned getRowCount() const override;
    unsigned getCutCount() const override THROW_NOT_IMPLEMENTED
    unsigned getNonZeros() const override THROW_NOT_IMPLEMENTED
    void readFromFile(const std::string&) override THROW_NOT_IMPLEMENTED
    void writeToFile(const std::string&, const std::string&) override THROW_NOT_IMPLEMENTED
    void getBase(extSol::QpExternSolver::QpExtSolBase&) const override;
    void setBase(extSol::QpExternSolver::QpExtSolBase&) override;
    void setRayGuess(const std::vector<data::QpNum>&) override THROW_NOT_IMPLEMENTED
    void adaptToSolverMode(QpExtSolSolverMode) override;
    void setParameters(const ExtSolverParameters&) override;
    bool changeObjFuncCoeff(unsigned index, const data::QpNum& coeff) override;
    void setVarLB(unsigned i, const data::QpNum& lb) override;
    void setVarUB(unsigned i, const data::QpNum& ub) override;
    void changeRhsElement(unsigned, const data::QpNum&) override;
    void changeRhsElements(const std::vector<unsigned int>&, const std::vector<data::QpNum>&) override THROW_NOT_IMPLEMENTED
    void changeRhsElements(const std::vector<int>&, const std::vector<double>&) override THROW_NOT_IMPLEMENTED
    extSol::QpExternSolver::QpExtSolSolutionStatus solve(unsigned itLimit, unsigned timeLimit) override;
    data::QpNum getObjValue() override;
    void getValues(std::vector<data::QpNum>&) override;
    void getDuals(std::vector<data::QpNum>&) override THROW_NOT_IMPLEMENTED
    void getReducedCosts(std::vector<data::QpNum>&) override;
    void getDualFarkas(std::vector<data::QpNum>&) override;
    void getExtendedDuals(std::vector<data::QpNum>&) override THROW_NOT_IMPLEMENTED
    void getExtendedDualFarkas(std::vector<data::QpNum>&) override THROW_NOT_IMPLEMENTED
    void getExtendedDualFarkas(std::vector<data::QpNum>&, const data::QpSparseMatrix&, const data::QpSparseMatrix&) override;
    std::vector<std::pair<std::vector<std::pair<unsigned int, double>>, double>>* CreateCuts(
        extSol::QpExternSolver& externSolver, int* types, int8_t* assigns, int decLev, unsigned initime, int* solu,
        int* fixs, int* blcks, int orgN, int cuttype, int delCuts, double intLB) override THROW_NOT_IMPLEMENTED
    void addCut(const std::vector<data::IndexedElement>&, data::QpRhs::RatioSign, const data::QpNum&) override;
    void removeCuts() override THROW_NOT_IMPLEMENTED
    void removeCut(unsigned) override THROW_NOT_IMPLEMENTED
    void removeCutsFromCut(unsigned) override;
    void updateModel() override;
    void getQlpFromLpFile(const std::string&, data::Qlp&) override THROW_NOT_IMPLEMENTED
    void getRhs(std::vector<data::QpRhs>&) override THROW_NOT_IMPLEMENTED
    void getLB(std::vector<data::QpNum>&) override;
    void getUB(std::vector<data::QpNum>&) override;
    void prepareMatrixRowForm() override {}
    void getObjective(std::vector<data::IndexedElement>& lhs, bool& doMax, double& offset) override THROW_NOT_IMPLEMENTED
    void getRowLhs(unsigned, std::vector<data::IndexedElement>&) override;
    void getRowLhsOfTableauByColumn(unsigned, std::vector<data::QpNum>&) override THROW_NOT_IMPLEMENTED
    void getBinvArow(unsigned cIndex, std::vector<data::QpNum>& binvArow) override THROW_NOT_IMPLEMENTED
    bool getBendersCut(unsigned stage, std::vector<data::IndexedElement>& lhs, data::QpRhs::RatioSign& sign,
        data::QpNum& rhs, bool org, void* vpt) override THROW_NOT_IMPLEMENTED
    void getBendersCutNew(unsigned stage, std::vector<data::IndexedElement>& lhs, data::QpRhs::RatioSign& sign,
        data::QpNum& rhs, bool check, std::vector<double>& mRows, std::vector<double>& lbs,
        std::vector<double>& ubs) override THROW_NOT_IMPLEMENTED
};

#endif

#endif //IDOL_QPIDOLEXTERNSOLVER_H