//
// Created by henri on 13.04.25.
//

#ifndef IDOL_SOSCTR_H
#define IDOL_SOSCTR_H

#include <utility>

#include "idol/mixed-integer/modeling/objects/Object.h"
#include "idol/mixed-integer/modeling/Types.h"
#include "idol/mixed-integer/modeling/variables/Var.h"

namespace idol {
    class SOSCtrVersion;
    class Env;

    template<class T>
    class Versions;

    namespace impl {
        class Env;
    }

    class SOSCtr;
}

class idol::SOSCtr : public Object<SOSCtrVersion, SOSCtr> {
    friend class impl::Env;
public:
    SOSCtr(Env& t_env, bool t_is_sos1, std::vector<Var> t_vars, std::vector<double> t_weights, std::string t_name = "");

    static SOSCtr SOS1(Env& t_env, std::vector<Var> t_vars, std::vector<double> t_weights, std::string t_name = "") {
        return SOSCtr(t_env, true, std::move(t_vars), std::move(t_weights), std::move(t_name));
    }

    static SOSCtr SOS2(Env& t_env, std::vector<Var> t_vars, std::vector<double> t_weights, std::string t_name = "") {
        return SOSCtr(t_env, false, std::move(t_vars), std::move(t_weights), std::move(t_name));
    }

    [[nodiscard]] unsigned int index(const Model& t_index) const;
};

#endif //IDOL_SOSCTR_H
