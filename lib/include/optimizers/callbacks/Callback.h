//
// Created by henri on 12/04/23.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include "modeling/solutions/Solution.h"

class TempCtr;

/**
 * CallbackEvent is an enumerated type used to indicate events during a branch-and-bound tree execution.
 *
 * It is typically used inside of a Callback to influence or modify the behaviour of the algorithm at execution time.
 */
enum CallbackEvent {
    NodeLoaded, /*!< Occurs when a node is about to be solved */
    IncumbentSolution, /*!< Occurs when an incumbent solution has been found */
    InvalidSolution, /*!< Occurs when a solution of the relaxation is not valid (e.g., not integer) */
};

static std::ostream &operator<<(std::ostream& t_os, CallbackEvent t_event) {
    switch (t_event) {
        case IncumbentSolution: return t_os << "IncumbentSolution";
        case InvalidSolution: return t_os << "InvalidSolution";
        case NodeLoaded: return t_os << "NodeLoaded";
        default:;
    }
    throw Exception("Enum out of bounds.");
}

class Callback;

class CallbackI {
    friend class ::Callback;
protected:
    virtual void add_user_cut(const TempCtr& t_cut) = 0;

    virtual void add_lazy_cut(const TempCtr& t_cut) = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const = 0;

    void execute(Callback& t_cb, CallbackEvent t_event);
public:
    virtual ~CallbackI() = default;
};

class Callback {
    friend class ::CallbackI;
public:
    virtual ~Callback() = default;
protected:
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
    [[nodiscard]] virtual Solution::Primal primal_solution() const;

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
