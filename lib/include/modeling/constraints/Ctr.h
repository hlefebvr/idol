//
// Created by henri on 27/01/23.
//

#ifndef IDOL_CTR_H
#define IDOL_CTR_H

#include "modeling/objects/Object.h"

class CtrVersion;
class Env;
template<class T> class Versions;
class TempCtr;

namespace impl {
    class Env;
}

class Ctr : public Object<CtrVersion> {
    friend class impl::Env;
public:
    Ctr(Env& t_env, TempCtr&& t_temp_ctr, std::string t_name = "");
    Ctr(Env& t_env, const TempCtr& t_temp_ctr, std::string t_name = "");
    Ctr(Env& t_env, int t_type, std::string t_name = "");

    template<unsigned int N = 1, unsigned int I = 0>
    static Vector<Ctr, N - I> Array(Env& t_env, const Dim<N>& t_dim, int t_type, const std::string& t_name = "") {
        return impl::create_many<Ctr, N, I>(t_dim, t_name, [&](const std::string& t_name_i) {
            return Ctr(t_env, t_type, t_name_i);
        });
    }
};

IDOL_MAKE_HASHABLE(Ctr)

#endif //IDOL_CTR_H
