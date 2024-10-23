//
// Created by henri on 22.10.24.
//

#ifndef IDOL_MIBSCALLBACKI_H
#define IDOL_MIBSCALLBACKI_H

#include <BlisHeuristic.h>
#include <CglCutGenerator.hpp>
#include "idol/optimizers/mixed-integer-optimization/callbacks/Callback.h"
#include "impl_MibSFromAPI.h"

namespace idol {
    class MibSCallbackI;
}

class idol::MibSCallbackI : public CallbackI {
public:
    class Heuristic;
    class CutGenerator;
private:
    const impl::MibSFromAPI& m_parent;
    Heuristic* m_heuristic;
    CutGenerator* m_cut_generator;
    bool m_is_heuristic_call = false;

    void call(CallbackEvent t_event) {

        if (m_is_heuristic_call) {
            for (const auto &callback: m_parent.m_callbacks) {
                execute(*callback, t_event);
            }
        } else {
            std::cout << "Check if cuts were generated" << std::endl;
        }

    }
public:
    class Heuristic : public ::BlisHeuristic {
        MibSCallbackI& m_parent;
        std::list<Solution::Primal> m_solutions;
    public:
        explicit Heuristic(MibSCallbackI& t_parent) : m_parent(t_parent) {}

        void submit_heuristic_solution(Solution::Primal t_solution) {
            m_solutions.emplace_back(std::move(t_solution));
        }

        bool searchSolution(double &t_objective_value, double *t_new_solution) override {

            m_parent.m_is_heuristic_call = true;
            m_parent.call(InvalidSolution);

            auto solution = m_solutions.end();
            double current_best = m_parent.best_obj();
            for (auto it = m_solutions.begin(), end = m_solutions.end(); it != end ; ++it) {
                if (it->objective_value() < current_best) {
                    solution = it;
                    current_best = it->objective_value();
                }
            }

            if (solution == m_solutions.end()) {
                m_solutions.clear();
                return false;
            }

            const auto& osi_solver = *m_parent.m_parent.m_osi_solver;
            const unsigned int n_vars = osi_solver.getNumCols();
            for (unsigned int i = 0; i < n_vars; ++i) {
                t_new_solution[i] = solution->get(m_parent.m_parent.m_model.get_var_by_index(i));
            }
            t_objective_value = solution->objective_value();

            m_solutions.clear();

            return true;
        }
    };

    class CutGenerator : public ::CglCutGenerator {
        MibSCallbackI& m_parent;
        std::list<TempCtr> m_cuts;
    public:
        explicit CutGenerator(MibSCallbackI& t_parent) : m_parent(t_parent) {}

        void add_user_cut(TempCtr t_cut) {
            m_cuts.emplace_back(std::move(t_cut));
        }

        void generateCuts(const OsiSolverInterface &si, OsiCuts &cs, const CglTreeInfo info) override {
            m_parent.m_is_heuristic_call = false;

            const double infinity = si.getInfinity();
            const auto& model = m_parent.original_model();

            for (const auto& cut : m_cuts) {

                const auto& row = cut.row();
                const double rhs = row.rhs().as_numerical();

                CoinPackedVector lhs;
                for (const auto& [var, constant] : row.linear()) {
                    const unsigned int index = model.get_var_index(var);
                    lhs.insert(index, constant.as_numerical());
                }

                OsiRowCut osi_cut;
                osi_cut.setRow(lhs);

                switch (cut.type()) {
                    case CtrType::LessOrEqual:
                        osi_cut.setLb(-infinity);
                        osi_cut.setUb(rhs);
                        break;
                    case CtrType::Equal:
                        osi_cut.setLb(rhs);
                        osi_cut.setUb(rhs);
                        break;
                    case CtrType::GreaterOrEqual:
                        osi_cut.setLb(rhs);
                        osi_cut.setUb(infinity);
                        break;
                }

                cs.insert(osi_cut);
            }

            m_cuts.clear();

        }

        [[nodiscard]] CglCutGenerator *clone() const override {
            return new CutGenerator(*this);
        }
    };

    explicit MibSCallbackI(const impl::MibSFromAPI& t_parent) : m_parent(t_parent), m_heuristic(new Heuristic(*this)), m_cut_generator(new CutGenerator(*this)) {
        m_heuristic->setHeurCallFrequency(1);
        m_heuristic->setStrategy(BlisHeurStrategyPeriodic);
    }

    BlisHeuristic& heuristic() { return *m_heuristic; }

    [[nodiscard]] const BlisHeuristic& heuristic() const { return *m_heuristic; }

    CglCutGenerator& cut_generator() { return *m_cut_generator; }

    [[nodiscard]] const CglCutGenerator& cut_generator() const { return *m_cut_generator; }
protected:
    [[nodiscard]] const Model &original_model() const override {
        return m_parent.m_model;
    }

    void add_user_cut(const TempCtr &t_cut) override {
        m_cut_generator->add_user_cut(t_cut);
    }

    void add_lazy_cut(const TempCtr &t_cut) override {
        throw Exception("Not implemented.");
    }

    void submit_heuristic_solution(Solution::Primal t_solution) override {
        m_heuristic->submit_heuristic_solution(std::move(t_solution));
    }

    [[nodiscard]] Solution::Primal primal_solution() const override {

        auto& osi_solver = *m_parent.m_osi_solver;
        const auto* solution = osi_solver.getColSolution();
        const unsigned int n_vars = osi_solver.getNumCols();

        Solution::Primal result;
        result.set_reason(NotSpecified);

        if (osi_solver.isProvenOptimal()) {

            result.set_status(Optimal);

        } else {

            if (osi_solver.isProvenPrimalInfeasible()) {
                result.set_status(Infeasible);
                return result;
            }

            if (osi_solver.isProvenDualInfeasible()) {
                result.set_status(Unbounded);
                return result;
            }

            result.set_status(Fail);
            return result;

        }

        for (unsigned int i = 0; i < n_vars; ++i) {
            const auto& var = m_parent.m_model.get_var_by_index(i);
            result.set(var, solution[i]);
        }

        result.set_objective_value(m_parent.m_model.get_obj_expr().constant().as_numerical() + osi_solver.getObjValue());

        return result;
    }

    [[nodiscard]] const Timer &time() const override {
        return m_parent.m_model.optimizer().time();
    }

    [[nodiscard]] double best_obj() const override {
        return m_parent.get_best_obj();
    }

    [[nodiscard]] double best_bound() const override {
        return m_parent.get_best_bound();
    }

    void terminate() override {
        throw Exception("Not available.");
    }
};

#endif //IDOL_MIBSCALLBACKI_H
