//
// Created by henri on 27/01/23.
//

#ifndef IDOL_CTR_H
#define IDOL_CTR_H

#include "idol/mixed-integer/modeling/objects/Object.h"
#include "idol/mixed-integer/modeling/Types.h"
#include "idol/mixed-integer/modeling/constraints/TempCtr.h"

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
    /**
     * Constructor.
     *
     * Creates a new constraint in the optimization environment.
     *
     * If no name is given, a unique name in the environment is given.
     * @param t_env The optimization environment.
     * @param t_temp_ctr The temporary constraint used to create the default version of the constraint.
     * @param t_name The given name for the constraint.
     */
    Ctr(Env& t_env, TempCtr&& t_temp_ctr, std::string t_name = "");

    /**
     * Constructor.
     *
     * Creates a new constraint in the optimization environment.
     *
     * If no name is given, a unique name in the environment is given.
     * @param t_env The optimization environment.
     * @param t_temp_ctr The temporary constraint used to create the default version of the constraint.
     * @param t_name The given name for the constraint.
     */
    Ctr(Env& t_env, const TempCtr& t_temp_ctr, std::string t_name = "");

    /**
     * Constructor.
     *
     * Creates a new constraint in the optimization environment.
     *
     * If no name is given, a unique name in the environment is given.
     * @param t_env The optimization environment.
     * @param t_type The type of the constraint in its default version.
     * @param t_constant The right hand-side of the constraint in its default version.
     * @param t_name The given name for the constraint.
     */
    Ctr(Env& t_env, CtrType t_type, double t_constant, std::string t_name = "");

    /**
     * Creates a (nested) vector of constraints.
     *
     *
     * *Example*:
     *
     * ```cpp
     * Env env;
     *
     * auto constraint = Ctr::make_vector(env, Dim<2>(m, n), LessOrEqual, 0., "constraint");
     *
     * std::cout << constraint[1][4].name() << std::endl; // "constraint_1_4"
     * ```
     * @tparam N The dimension of the (nested) vector.
     * @tparam I Used for recursion.
     * @param t_env The environment to store the constraints.
     * @param t_dim The number of elements in each dimension.
     * @param t_type The type of the constraints.
     * @param t_constant The right hand-side of the constraints.
     * @param t_name The given name of the constraints.
     * @return A (nested) vector of constraints.
     */
    template<unsigned int N = 1, unsigned int I = 0>
    static Vector<Ctr, N - I> make_vector(Env& t_env, const Dim<N>& t_dim, CtrType t_type, double t_constant, const std::string& t_name = "") {
        return idol::impl::create_many<Ctr, N, I>(t_dim, t_name, [&](const std::string& t_name_i) {
            return Ctr(t_env, t_type, t_constant, t_name_i);
        });
    }

    unsigned int index(const Model& t_index) const;
};

//IDOL_MAKE_HASHABLE(Ctr)

#endif //IDOL_CTR_H
