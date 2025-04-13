//
// Created by henri on 13.04.25.
//
#include "idol/mixed-integer/modeling/constraints/SOSCtr.h"
#include "idol/mixed-integer/modeling/objects/Env.h"

idol::SOSCtr::SOSCtr(Env &t_env, bool t_is_sos1,
                     std::vector<Var> t_vars,
                     std::vector<double> t_weights,
                     std::string t_name)
                     : Object<SOSCtrVersion, SOSCtr>(
                             t_env.create_sosctr(std::move(t_name), t_is_sos1, std::move(t_vars), std::move(t_weights))) {

}

unsigned int idol::SOSCtr::index(const idol::Model &t_index) const {
    return t_index.get_sosctr_index(*this);
}

