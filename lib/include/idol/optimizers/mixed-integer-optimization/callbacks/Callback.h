//
// Created by henri on 12/04/23.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include "idol/modeling/solutions/Point.h"

namespace idol {
    class TempCtr;

    class Timer;

/**
 * CallbackEvent is an enumerated type used to indicate events during a branch-and-bound tree execution.
 *
 * It is typically used inside of a Callback to influence or modify the behaviour of the algorithm at execution time.
 */
    enum CallbackEvent {
        NodeLoaded, /*!< Occurs when a node is about to be solved */
        IncumbentSolution, /*!< Occurs when an incumbent solution has been found */
        InvalidSolution, /*!< Occurs when a solution of the relaxation is not valid (e.g., not integer) */
        PrunedSolution
    };

    static std::ostream &operator<<(std::ostream& t_os, idol::CallbackEvent t_event) {
        using namespace idol;
        switch (t_event) {
            case IncumbentSolution: return t_os << "IncumbentSolution";
            case InvalidSolution: return t_os << "InvalidSolution";
            case NodeLoaded: return t_os << "NodeLoaded";
            case PrunedSolution: return t_os << "PrunedSolution";
            default:;
        }
        throw Exception("Enum out of bounds.");
    }

    class Callback;
    class CallbackI;
}

class idol::CallbackI {
    friend class ::idol::Callback;
protected:
    [[nodiscard]] virtual const Model& original_model() const = 0;

    virtual void add_user_cut(const TempCtr& t_cut) = 0;

    virtual void add_lazy_cut(const TempCtr& t_cut) = 0;

    virtual void submit_heuristic_solution(PrimalPoint t_solution) = 0;

    [[nodiscard]] virtual PrimalPoint primal_solution() const = 0;

    [[nodiscard]] virtual const Timer& time() const = 0;

    [[nodiscard]] virtual double best_obj() const = 0;

    [[nodiscard]] virtual double best_bound() const = 0;

    virtual void terminate() = 0;

    void execute(Callback& t_cb, CallbackEvent t_event);
public:
    virtual ~CallbackI() = default;
};

class idol::Callback {
    friend class ::idol::CallbackI;
public:
    virtual ~Callback() = default;
protected:
    /**
     * Accesses the original model given to the solver
     */
     [[nodiscard]] virtual const Model& original_model() const;

     /**
      * Submits a new heuristic solution
      */
      virtual void submit_heuristic_solution(const PrimalPoint& t_solution);

    /**
     * Adds a user cut to the relaxation
     * @param t_cut the cut to be added
     */
    virtual void add_user_cut(const TempCtr& t_cut);

    /**
     * Adds a lazy cut to the relaxation
     * @param t_cut the cut to be added
     */
    virtual void add_lazy_cut(const TempCtr& t_cut);

    /**
     * Returns the solution of the node which is currently being explored (when available)
     * @return the solution of the current node
     */
    [[nodiscard]] virtual PrimalPoint primal_solution() const;

    /**
     * Returns the current time
     * @return the optimizer's time
     */
    [[nodiscard]] const Timer& time() const;

    /**
     * Returns the best objective value found so far
     * @return the best objective value
     */
    [[nodiscard]] double best_obj() const;

    /**
     * Returns the best bound found so far
     * @return the best bound
     */
    [[nodiscard]] double best_bound() const;

    /**
     * Asks the optimizer to terminate the optimization process as soon as possible
     */
    void terminate();

    /**
     * This method is left for the user to write and consists in the main execution block of the callback.
     *
     * @param t_event the event which triggered the call
     */
    virtual void operator()(CallbackEvent t_event) = 0;
private:
    CallbackI* m_interface = nullptr;

    void throw_if_no_interface() const;
};

#endif //IDOL_CALLBACK_H
