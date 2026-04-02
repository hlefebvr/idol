//
// Created by Henri on 02/04/2026.
//

#ifndef IDOL_ABSTRACTPRESOLVER_H
#define IDOL_ABSTRACTPRESOLVER_H
#include "idol/mixed-integer/modeling/expressions/LinExpr.h"

namespace idol {
    namespace Presolvers {
        class AbstractPresolver;
    }
    class Model;
}

class idol::Presolvers::AbstractPresolver {
protected:
    static double get_min_activity(const Model& t_model, const LinExpr<Var>& t_expr);
    static double get_max_activity(const Model& t_model, const LinExpr<Var>& t_expr);
public:
    virtual ~AbstractPresolver() = default;

    virtual bool execute(Model& t_model) = 0;
    virtual void log_after_termination() const {}
    [[nodiscard]] virtual AbstractPresolver* clone() const = 0;
};

#endif //IDOL_ABSTRACTPRESOLVER_H