//
// Created by henri on 27/01/23.
//

#ifndef IDOL_VAR_H
#define IDOL_VAR_H

#include "idol/modeling/objects/Object.h"
#include "idol/modeling/Types.h"

namespace idol {

    class Env;
    class VarVersion;
    class Column;

    template<class T>
    class Versions;

    namespace impl {
        class Env;
    }

    class Var;
}


/**
 * Variable modeling object.
 *
 * The class is used to represent a variable in an optimization model.
 *
 * Variables can be created by calling a constructor of `Var`, passing the optimization environment which will store
 * the variable's versions as argument. The variable can then be added to a model using the `Model::add` method.
 *
 * Another approach is to directly use the `Model::add_var` method.
 *
 * ```cpp
 * Var x = model.add_var(0, 1, Continuous, "x");
 * ```
 */
class idol::Var : public Object<VarVersion, Var> {
    friend class impl::Env;
public:
    /**
     * Constructor.
     *
     * Creates a new variable in the optimization environment.
     *
     * If no name is given, a unique name in the environment is given.
     * @param t_env The optimization environment.
     * @param t_lb The lower bound (possibly `-Inf`) for the default version of the variable.
     * @param t_ub The upper bound (possibly `+Inf`) for the default version of the variable.
     * @param t_type The type for the default version of the variable.
     * @param t_name The given name of the variable.
     */
    Var(Env& t_env, double t_lb, double t_ub, VarType t_type, std::string t_name = "");

    /**
     * Constructor.
     *
     * Creates a new variable in the optimization environment.
     *
     * If no name is given, a unique name in the environment is given.
     * @param t_env The optimization environment.
     * @param t_lb The lower bound (possibly `-Inf`) for the default version of the variable.
     * @param t_ub The upper bound (possibly `+Inf`) for the default version of the variable.
     * @param t_type The type for the default version of the variable.
     * @param t_column The column for the default version of the variable.
     * @param t_name The given name of the variable.
     */
    Var(Env& t_env, double t_lb, double t_ub, VarType t_type, Column&& t_column, std::string t_name = "");

    /**
     * Constructor.
     *
     * Creates a new variable in the optimization environment.
     *
     * If no name is given, a unique name in the environment is given.
     * @param t_env The optimization environment.
     * @param t_lb The lower bound (possibly `-Inf`) for the default version of the variable.
     * @param t_ub The upper bound (possibly `+Inf`) for the default version of the variable.
     * @param t_type The type for the default version of the variable.
     * @param t_column The column for the default version of the variable.
     * @param t_name The given name of the variable.
     */
    Var(Env& t_env, double t_lb, double t_ub, VarType t_type, const Column& t_column, std::string t_name = "");

    /**
     * Creates a (nested) vector of variables.
     *
     *
     * *Example*:
     *
     * ```cpp
     * Env env;
     *
     * auto x = Var::make_vector(env, Dim<2>(m, n), 0., 1., Continuous, "x");
     *
     * std::cout << x[1][4].name() << std::endl; // "x_1_4"
     * ```
     * @tparam N The dimension of the (nested) vector.
     * @tparam I Used for recursion.
     * @param t_env The environment to store the variables.
     * @param t_dim The number of elements in each dimension.
     * @param t_type The type of the variables.
     * @param t_name The given name of the variables.
     * @return A (nested) vector of variables.
     */
    template<unsigned int N = 1, unsigned int I = 0>
    static Vector<Var, N - I> make_vector(Env& t_env, const Dim<N>& t_dim, double t_lb, double t_ub, VarType t_type, const std::string& t_name = "") {
        return impl::create_many<Var, N, I>(t_dim, t_name.empty() ? "Var" : t_name, [&](const std::string& t_name_i) {
            return Var(t_env, t_lb, t_ub, t_type, t_name_i);
        });
    }
};

IDOL_MAKE_HASHABLE(Var)

#endif //IDOL_VAR_H
