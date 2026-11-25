#ifndef IDOL_CPLEX_CALLBACK_I_H
#define IDOL_CPLEX_CALLBACK_I_H

#include <exception>
#include <list>
#include <memory>
#include <optional>
#include <vector>
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"
#include "idol/mixed-integer/optimizers/callbacks/Callback.h"
#include "idol/mixed-integer/optimizers/wrappers/Cplex/headers/header_cplex.h"

namespace idol::Optimizers { class Cplex; }

namespace idol {
class CplexCallbackI final : public CallbackI {
    Optimizers::Cplex& m_parent;
    std::list<std::unique_ptr<Callback>> m_callbacks;
    void* m_cbdata = nullptr;
    int m_wherefrom = 0;
    CallbackEvent m_event = NodeLoaded;
    const double* m_callback_solution = nullptr;
    std::optional<PrimalPoint> m_heuristic_solution;
    double m_best_obj = Inf;
    double m_best_bound = -Inf;
    unsigned int m_node_count = 0;
    volatile int m_terminate = 0;
    bool m_added_cut = false;
    std::exception_ptr m_exception;
    void call(CallbackEvent);
    void set_context(void*, int, CallbackEvent, const double* = nullptr);
    void clear_context();
    void add_cut(const TempCtr&, bool);
public:
    explicit CplexCallbackI(Optimizers::Cplex&);
    void add_callback(Callback*);
    void register_callbacks();
    void rethrow_exception();
    static int IDOL_CPXPUBLIC user_cut(CPXCENVptr, void*, int, void*, int*);
    static int IDOL_CPXPUBLIC lazy_cut(CPXCENVptr, void*, int, void*, int*);
    static int IDOL_CPXPUBLIC heuristic(CPXCENVptr, void*, int, void*, double*, double*, int*, int*);
    static int IDOL_CPXPUBLIC info(CPXCENVptr, void*, int, void*);
protected:
    [[nodiscard]] const Model& original_model() const override;
    void add_user_cut(const TempCtr&) override;
    void add_lazy_cut(const TempCtr&) override;
    void submit_heuristic_solution(PrimalPoint) override;
    [[nodiscard]] PrimalPoint primal_solution() const override;
    [[nodiscard]] const Timer& time() const override;
    [[nodiscard]] double best_obj() const override;
    [[nodiscard]] double best_bound() const override;
    [[nodiscard]] unsigned int node_count() const override;
    void terminate() override;
};
}

#endif
