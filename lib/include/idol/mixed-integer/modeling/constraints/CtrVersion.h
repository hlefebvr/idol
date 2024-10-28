//
// Created by henri on 27/01/23.
//

#ifndef IDOL_CTRVERSION_H
#define IDOL_CTRVERSION_H

#include "idol/mixed-integer/modeling/objects/Version.h"
#include "idol/mixed-integer/modeling/expressions/InternalLinExpr.h"
#include "idol/mixed-integer/modeling/Types.h"

namespace idol {
    class TempCtr;
    class CtrVersion;
}

class idol::CtrVersion : public Version {
    std::unique_ptr<InternalLinExpr<Var>> m_lhs;
    CtrType m_type;
    double m_rhs = 0.;
public:
    CtrVersion(unsigned int t_index, TempCtr&& t_temp_ctr);

    CtrVersion(unsigned int t_index, const TempCtr& t_temp_ctr);

    CtrVersion(unsigned int t_index, InternalLinExpr<Var>&& t_lhs, CtrType t_type, double t_rhs)
        : Version(t_index),
          m_lhs(std::make_unique<InternalLinExpr<Var>>(std::move(t_lhs))),
          m_type(t_type),
          m_rhs(t_rhs)
        {}

    CtrVersion(unsigned int t_index, CtrType t_type, double t_rhs)
            : Version(t_index),
              m_type(t_type),
              m_rhs(t_rhs)
    {}

    CtrVersion(const CtrVersion& t_src)
            : Version(t_src),
              m_lhs(t_src.m_lhs ? std::make_unique<InternalLinExpr<Var>>(*t_src.m_lhs) : nullptr),
              m_type(t_src.m_type),
              m_rhs(t_src.m_rhs) {}

    double rhs() const { return m_rhs; }

    void set_rhs(double t_rhs) { m_rhs = t_rhs; }

    CtrType type() const { return m_type; }

    void set_type(CtrType t_type) { m_type = t_type; }

    LinExpr<Var>& row() { return *m_lhs; }

    const LinExpr<Var>& lhs() const { return *m_lhs; }

    void set_row(InternalLinExpr<Var>&& t_row) { m_lhs = std::make_unique<InternalLinExpr<Var>>(std::move(t_row)); }

    bool has_row() const { return m_lhs != nullptr; }

    void set_row(const InternalLinExpr<Var>& t_row) { m_lhs = std::make_unique<InternalLinExpr<Var>>(t_row); }

    void reset_row() { m_lhs.reset(); }
};

#endif //IDOL_CTRVERSION_H
