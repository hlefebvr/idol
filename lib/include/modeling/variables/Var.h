//
// Created by henri on 27/01/23.
//

#ifndef IDOL_VAR_H
#define IDOL_VAR_H

#include "modeling/objects/Object.h"

class Env;
class VarVersion;
class Column;
template<class T> class Versions;

namespace impl {
    class Env;
}

class Var : public Object<VarVersion, Var> {
    friend class impl::Env;
public:
    Var(Env& t_env, double t_lb, double t_ub, int t_type, std::string t_name = "");
    Var(Env& t_env, double t_lb, double t_ub, int t_type, Column&& t_column, std::string t_name = "");
    Var(Env& t_env, double t_lb, double t_ub, int t_type, const Column& t_column, std::string t_name = "");

    template<unsigned int N = 1, unsigned int I = 0>
    static Vector<Var, N - I> array(Env& t_env, const Dim<N>& t_dim, double t_lb, double t_ub, int t_type, const std::string& t_name = "") {
        return impl::create_many<Var, N, I>(t_dim, t_name, [&](const std::string& t_name_i) {
            return Var(t_env, t_lb, t_ub, t_type, t_name_i);
        });
    }
};

IDOL_MAKE_HASHABLE(Var)

#endif //IDOL_VAR_H
