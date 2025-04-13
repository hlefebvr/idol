//
// Created by henri on 13.04.25.
//

#ifndef IDOL_SOSCTRVERSION_H
#define IDOL_SOSCTRVERSION_H

#include <vector>
#include "idol/mixed-integer/modeling/objects/Version.h"
#include "idol/mixed-integer/modeling/Types.h"
#include "idol/mixed-integer/modeling/variables/Var.h"

namespace idol {
    class SOSCtrVersion;
}

class idol::SOSCtrVersion : public Version {
    bool m_is_sos1;
    std::vector<Var> m_vars;
    std::vector<double> m_weights;
public:
    SOSCtrVersion(unsigned int t_index, bool t_is_sos1, const std::vector<Var>& t_vars, const std::vector<double>& t_weights)
            : Version(t_index),
              m_is_sos1(t_is_sos1),
              m_vars(t_vars),
              m_weights(t_weights) {}

    [[nodiscard]] bool is_sos1() const { return m_is_sos1; }

    [[nodiscard]] const std::vector<Var>& vars() const { return m_vars; }

    [[nodiscard]] const std::vector<double>& weights() const { return m_weights; }
};

#endif //IDOL_SOSCTRVERSION_H
