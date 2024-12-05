//
// Created by henri on 27/01/23.
//

#ifndef IDOL_VARVERSION_H
#define IDOL_VARVERSION_H

#include "idol/mixed-integer/modeling/objects/Version.h"
#include "idol/mixed-integer/modeling/expressions/LinExpr.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"

namespace idol {
    class TempVar;
    class VarVersion;
}

class idol::VarVersion : public Version {
    double m_lb;
    double m_ub;
    VarType m_type;
    double m_obj;
    std::unique_ptr<LinExpr<Ctr>> m_column;
public:
    VarVersion(unsigned int t_index, double t_lb, double t_ub, VarType t_type, double t_obj, LinExpr<Ctr>&& t_column);
    VarVersion(unsigned int t_index, TempVar&& t_temp_var);
    VarVersion(unsigned int t_index, const TempVar& t_temp_var);

    VarVersion(const VarVersion& t_other);
    VarVersion(VarVersion&& t_other) noexcept = default;

    double lb() const { return m_lb; }

    void set_lb(double t_lb) { m_lb = t_lb; }

    double ub() const { return m_ub; }

    void set_ub(double t_ub) { m_ub = t_ub; }

    VarType type() const { return m_type; }

    void set_type(VarType t_type) { m_type = t_type; }

    double obj() const { return m_obj; }

    void set_obj(double t_obj) { m_obj = t_obj; }

    LinExpr<Ctr>& column() { return *m_column; }

    const LinExpr<Ctr>& column() const { return *m_column; }

    void set_column(LinExpr<Ctr>&& t_column) { m_column = std::make_unique<LinExpr<Ctr>>(std::move(t_column)); }

    bool has_column() const { return m_column.operator bool(); }

    void set_column(const LinExpr<Ctr>& t_column) { m_column = std::make_unique<LinExpr<Ctr>>(t_column); }

    void reset_column() { m_column.reset(); }

};

#endif //IDOL_VARVERSION_H
