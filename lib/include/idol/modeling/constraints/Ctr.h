//
// Created by henri on 27/01/23.
//

#ifndef IDOL_CTR_H
#define IDOL_CTR_H

#include "idol/modeling/objects/Object.h"
#include "idol/modeling/Types.h"

namespace idol {
    class CtrVersion;
    class Env;

    template<class T>
    class Versions;

    class TempCtr;
    class Constant;

    namespace impl {
        class Env;
    }

    class Ctr;
}

class idol::Ctr : public Object<CtrVersion, Ctr> {
    friend class impl::Env;
public:
    Ctr(Env& t_env, TempCtr&& t_temp_ctr, std::string t_name = "");
    Ctr(Env& t_env, const TempCtr& t_temp_ctr, std::string t_name = "");
    Ctr(Env& t_env, CtrType t_type, Constant&& t_constant, std::string t_name = "");
    Ctr(Env& t_env, CtrType t_type, const Constant& t_constant, std::string t_name = "");

    template<unsigned int N = 1, unsigned int I = 0>
    static Vector<Ctr, N - I> make_vector(Env& t_env, const Dim<N>& t_dim, CtrType t_type, const Constant& t_constant, const std::string& t_name = "") {
        return idol::impl::create_many<Ctr, N, I>(t_dim, t_name, [&](const std::string& t_name_i) {
            return Ctr(t_env, t_type, t_constant, t_name_i);
        });
    }
};

IDOL_MAKE_HASHABLE(Ctr)

#endif //IDOL_CTR_H
