//
// Created by henri on 19.11.24.
//

#ifndef IDOL_QCTR_H
#define IDOL_QCTR_H

#include "idol/mixed-integer/modeling/objects/Object.h"
#include "idol/mixed-integer/modeling/Types.h"

namespace idol {
    class QCtrVersion;
    class QCtr;
    class TempQCtr;

    namespace impl {
        class Env;
    }

}

class idol::QCtr : public Object<QCtrVersion, QCtr> {
    friend class impl::Env;
public:
    QCtr(Env& t_env, TempQCtr&& t_temp_ctr, std::string t_name = "");

    QCtr(Env& t_env, const TempQCtr& t_temp_ctr, std::string t_name = "");

    QCtr(Env& t_env, CtrType t_type, std::string t_name = "");

    template<unsigned int N = 1, unsigned int I = 0>
    static Vector<Ctr, N - I> make_vector(Env& t_env, const Dim<N>& t_dim, CtrType t_type, const std::string& t_name = "") {
        return idol::impl::create_many<Ctr, N, I>(t_dim, t_name, [&](const std::string& t_name_i) {
            return QCtr(t_env, t_type, t_name_i);
        });
    }

    unsigned int index(const Model& t_model) const;
};

#endif //IDOL_QCTR_H
