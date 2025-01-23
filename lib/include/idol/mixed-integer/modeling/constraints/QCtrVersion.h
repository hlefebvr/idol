//
// Created by henri on 19.11.24.
//

#ifndef IDOL_QCTRVERSION_H
#define IDOL_QCTRVERSION_H

#include "idol/mixed-integer/modeling/objects/Version.h"
#include "idol/mixed-integer/modeling/expressions/QuadExpr.h"

namespace idol {
    class TempQCtr;
    class QCtrVersion;
}

class idol::QCtrVersion : public Version {
    QuadExpr<Var> m_expr;
    CtrType m_type;
public:
    QCtrVersion(unsigned int t_index, TempQCtr&& t_temp_ctr);

    QCtrVersion(unsigned int t_index, const TempQCtr& t_temp_ctr);

    QCtrVersion(unsigned int t_index, QuadExpr<Var>&& t_expr, CtrType t_type)
        : Version(t_index),
          m_expr(std::move(t_expr)),
          m_type(t_type)
        {}

    QCtrVersion(unsigned int t_index, CtrType t_type)
        : Version(t_index),
          m_type(t_type)
        {}

    QCtrVersion(const QCtrVersion& t_src) = default;

    [[nodiscard]] CtrType type() const { return m_type; }

    void set_type(CtrType t_type) { m_type = t_type; }

    QuadExpr<Var>& expr() { return m_expr; }

    [[nodiscard]] const QuadExpr<Var>& expr() const { return m_expr; }

    void set_expr(QuadExpr<Var>&& t_expr) { m_expr = std::move(t_expr); }
};

#endif //IDOL_QCTRVERSION_H
