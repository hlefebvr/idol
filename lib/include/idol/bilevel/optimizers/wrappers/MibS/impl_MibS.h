//
// Created by henri on 21.10.24.
//

#ifndef IDOL_IMPL_MIBS_H
#define IDOL_IMPL_MIBS_H

namespace idol::impl {
    class MibS;
}

class idol::impl::MibS {
public:
    virtual ~MibS() = default;

    [[nodiscard]] virtual SolutionStatus get_status() const = 0;

    [[nodiscard]] virtual SolutionReason get_reason() const = 0;

    [[nodiscard]] virtual double get_best_obj() const = 0;

    [[nodiscard]] virtual double get_best_bound() const = 0;

    [[nodiscard]] virtual double get_var_primal(const Var& t_var) const = 0;

    virtual void solve() = 0;
};

#endif //IDOL_IMPL_MIBS_H
