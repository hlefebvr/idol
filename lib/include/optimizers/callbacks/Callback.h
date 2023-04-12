//
// Created by henri on 12/04/23.
//

#ifndef IDOL_CALLBACK_H
#define IDOL_CALLBACK_H

#include "modeling/solutions/Solution.h"

class TempCtr;

enum BranchAndBoundEvent {
    NodeLoaded, /* Called when a node is about to be solved */
    IncumbentSolution, /* Called when an incumbent solution has been found */
    InvalidSolution, /* Called when a solution of the relaxation is not valid (e.g., not integer) */
};

static std::ostream &operator<<(std::ostream& t_os, BranchAndBoundEvent t_event) {
    switch (t_event) {
        case IncumbentSolution: return t_os << "IncumbentSolution";
        case InvalidSolution: return t_os << "InvalidSolution";
        case NodeLoaded: return t_os << "NodeLoaded";
        default:;
    }
    throw Exception("Enum out of bounds.");
}

class Callback {
public:
    class Interface;

    virtual ~Callback() = default;
protected:
    virtual void add_user_cut(const TempCtr& t_cut);

    virtual void add_lazy_cut(const TempCtr& t_cut);

    [[nodiscard]] virtual Solution::Primal primal_solution() const;

    virtual void operator()(BranchAndBoundEvent t_event) = 0;
private:
    Interface* m_interface = nullptr;

    void throw_if_no_interface() const;

    friend class Interface;
};

class Callback::Interface {
    friend class ::Callback;
protected:
    virtual void add_user_cut(const TempCtr& t_cut) = 0;

    virtual void add_lazy_cut(const TempCtr& t_cut) = 0;

    [[nodiscard]] virtual Solution::Primal primal_solution() const = 0;

    void execute(Callback& t_cb, BranchAndBoundEvent t_event);
public:
    virtual ~Interface() = default;
};

#endif //IDOL_CALLBACK_H
