#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "reformulations/Reformulations_Benders.h"
#include "algorithms/benders/Benders.h"
#include "problems/FLP/FLP_Instance.h"
#include "modeling/models/Env.h"
#include "algorithms/benders/Nodes_BendersIIS.h"

class MyModel;

class VarVersion {
    unsigned int m_index;
    double m_lower_bound;
    double m_upper_bound;
public:
    VarVersion(unsigned int t_index, double t_lb, double t_ub ) : m_index(t_index), m_lower_bound(t_lb), m_upper_bound(t_ub) {}

    [[nodiscard]] double lb() const { return m_lower_bound; }

    [[nodiscard]] double ub() const { return m_upper_bound; }

    [[nodiscard]] unsigned int index() const { return m_index; }

    void set_index(unsigned int t_index) { m_index = t_index; }
};

template<class T>
class ObjectVersions {
    static const unsigned int m_buffer_size = 10;
    std::vector<std::optional<T>> m_versions;
public:
    template<class ...ArgsT> void create(const MyModel& t_model, unsigned int t_index, ArgsT&& ...t_args);

    void remove(const MyModel& t_model);

    bool has_version(const MyModel& t_model) const;

    [[nodiscard]] const T& get(const MyModel& t_model) const;

    [[nodiscard]] T& get(const MyModel& t_model);
};

template<class T>
class MyObjectId {
    const std::list<ObjectVersions<VarVersion>>::iterator m_it;
    unsigned int m_id;
    const std::string m_name;
public:
    MyObjectId(std::list<ObjectVersions<VarVersion>>::iterator t_it, unsigned int t_id, std::string t_name)
        : m_it(t_it), m_id(t_id), m_name(std::move(t_name)) {}

    auto& versions() { return *m_it; }
    [[nodiscard]] const auto& versions() const { return *m_it; }

    [[nodiscard]] const auto& name() const { return m_name; }
};

template<class T>
class MyObject {
protected:
    std::shared_ptr<MyObjectId<T>> m_object_id;
public:
    MyObject(std::list<ObjectVersions<VarVersion>>::iterator t_it, unsigned int t_id, std::string t_name)
        : m_object_id(std::make_shared<MyObjectId<T>>(t_it, t_id, std::move(t_name))) {}

    [[nodiscard]] auto& versions() { return m_object_id->versions(); }

    [[nodiscard]] const auto& versions() const { return m_object_id->versions(); }

    [[nodiscard]] const std::string& name() const { return m_object_id->name(); }

    template<class ...ArgsT>
    void create_version(const MyModel& t_model, unsigned int t_index, ArgsT&& ...t_args) const {
        m_object_id->versions().create(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    void remove_version(const MyModel& t_model) const {
        m_object_id->versions().remove(t_model);
    }
};

class MyVar : public MyObject<VarVersion> {
    friend class MyEnv;
public:
    MyVar(std::list<ObjectVersions<VarVersion>>::iterator t_it, unsigned int t_id, std::string t_name)
        : MyObject<VarVersion>(t_it, t_id, std::move(t_name)) {}
};

class MyEnv {
    unsigned int m_max_object_id = 0;

    unsigned int m_max_model_id = 0;
    std::list<unsigned int> m_free_model_ids;

    std::list<ObjectVersions<VarVersion>> m_variables;
public:
    unsigned int create_model_id() {

        if (m_free_model_ids.empty()) {
            return m_max_model_id++;
        }

        auto result = m_free_model_ids.back();
        m_free_model_ids.pop_back();
        return result;
    }

    void free_model_id(const MyModel& t_model);

    template<class ...ArgsT>
    void add_variable(const MyModel& t_model, const MyVar& t_var, unsigned int t_index, ArgsT&& ...t_args) {
        t_var.create_version(t_model, t_index, std::forward<ArgsT>(t_args)...);
    }

    template<class ...ArgsT>
    MyVar create_variable(const MyModel& t_model, std::string t_name, unsigned int t_index, ArgsT&& ...t_args) {
        m_variables.emplace_front();
        const unsigned int id = m_max_object_id++;
        std::string name = t_name.empty() ? "Var_" + std::to_string(id) : std::move(t_name);
        MyVar result(m_variables.begin(), m_max_object_id, std::move(name));
        add_variable(t_model, result, t_index, std::forward<ArgsT>(t_args)...);
        return result;
    }

    void remove(const MyModel& t_model, const MyVar& t_var) {
        t_var.remove_version(t_model);
    }

    [[nodiscard]] const VarVersion& get(const MyModel& t_model, const MyVar& t_var) const {
        return t_var.versions().get(t_model);
    }
};

class MyModel {
    MyEnv& m_env;
    const unsigned int m_id;

    std::vector<MyVar> m_variables;
public:
    explicit MyModel(MyEnv& t_env) : m_env(t_env), m_id(t_env.create_model_id()) {}

    ~MyModel() {
        for (const auto& var : m_variables) {
            m_env.remove(*this, var);
        }
        m_env.free_model_id(*this);
    }

    MyVar create_var(double t_lb, double t_ub, std::string t_name = "") {
        const unsigned int index = m_variables.size();
        auto result = m_env.create_variable(*this, std::move(t_name), index, t_lb, t_ub);
        m_variables.emplace_back(result);
        return result;
    }

    void add_var(const MyVar& t_var, double t_lb, double t_ub) {
        const unsigned int index = m_variables.size();
        m_env.add_variable(*this, t_var, index, t_lb, t_ub);
        m_variables.emplace_back(t_var);
    }

    void remove(const MyVar& t_var) {
        const auto index = t_var.versions().get(*this).index();
        m_variables.back().versions().get(*this).set_index(index);
        m_variables[index] = m_variables.back();
        m_variables.pop_back();
        m_env.remove(*this, t_var);
    }

    [[nodiscard]] const VarVersion& get(const MyVar& t_var) const {
        return m_env.get(*this, t_var);
    }

    [[nodiscard]] unsigned int id() const { return m_id; }
};

void MyEnv::free_model_id(const MyModel &t_model)  {
    m_free_model_ids.emplace_back(t_model.id());
}

template<class T>
template<class ...ArgsT>
void ObjectVersions<T>::create(const MyModel &t_model, unsigned int t_index, ArgsT&& ...t_args) {
    const unsigned int index = t_model.id();

    if (m_versions.size() <= index) {
        m_versions.resize(index + m_buffer_size);
    } else if (m_versions[index].has_value()) {
        throw Exception("Trying to add twice a given object to model.");
    }

    m_versions[index] = T(t_index, std::forward<ArgsT>(t_args)...);
}

template<class T>
const T &ObjectVersions<T>::get(const MyModel &t_model) const {
    return m_versions[t_model.id()].value();
}

template<class T>
T &ObjectVersions<T>::get(const MyModel &t_model) {
    return m_versions[t_model.id()].value();
}

template<class T>
void ObjectVersions<T>::remove(const MyModel &t_model) {
    if (!has_version(t_model)) { return; }
    m_versions[t_model.id()].reset();
}

template<class T>
bool ObjectVersions<T>::has_version(const MyModel &t_model) const {
    const unsigned int index = t_model.id();
    return index < m_versions.size() && m_versions[index].has_value();
}

int main(int t_argc, char** t_argv) {

    MyEnv env;
    MyModel model(env);

    auto y = model.create_var(10, 9, "y");
    auto x = model.create_var(-1., 1., "x");

    model.remove(y);

    MyModel model2(env);
    model2.add_var(x, 0., 1.);

    std::cout << model.get(x).index() << std::endl;
    std::cout << model2.get(x).index() << std::endl;

    std::cout << x.name() << std::endl;

    return 0;

    /*
    Logs::set_level<BranchAndBound>(Trace);
    Logs::set_color<BranchAndBound>(Blue);
    Logs::set_level<Benders>(Trace);
    Logs::set_color<Benders>(Yellow);

    auto instance = Problems::FLP::read_instance_1991_Cornuejols_et_al("/home/henri/CLionProjects/optimize/dev/flp_instance.txt");

    const unsigned int n_customers = instance.n_customers();
    const unsigned int n_facilities = instance.n_facilities();

    {
        Model direct;
        auto y = direct.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "y");
        auto x = direct.add_vars(Dim<2>(n_facilities, n_customers), 0., 1., Continuous, 0., "x");

        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            direct.add_ctr(idol_Sum(i, Range(n_facilities), x[i][j]) == 1);
        }

        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            for (unsigned int i = 0 ; i < n_facilities ; ++i) {
                direct.add_ctr(x[i][j] <= y[i]);
            }
        }

        for (unsigned int i = 0 ; i < n_facilities ; ++i) {
            direct.add_ctr(idol_Sum(j, Range(n_customers), instance.demand(j) * x[i][j]) <= instance.capacity(i) * y[i]);
        }

        direct.set(Attr::Obj::Expr, idol_Sum(
                    i, Range(n_facilities),
                    instance.fixed_cost(i) * y[i]
                    + idol_Sum(j, Range(n_customers),
                      instance.demand(j) * instance.per_unit_transportation_cost(i, j) * x[i][j]
                     )
                ));

        Solvers::Gurobi gurobi(direct);
        gurobi.solve();

        std::cout << gurobi.primal_solution() << std::endl;
    }

    Model subproblem(Env::global(), Maximize);
    auto u = subproblem.add_vars(Dim<1>(n_customers), 0., Inf, Continuous, 0., "u");
    auto v = subproblem.add_vars(Dim<2>(n_facilities, n_customers), 0., Inf, Continuous, 0., "v");
    auto w = subproblem.add_vars(Dim<1>(n_facilities), 0., Inf, Continuous, 0., "w");

    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        for (unsigned int j = 0 ; j < n_customers ; ++j) {
            subproblem.add_ctr(u[j] - v[i][j] - instance.demand(j) * w[i] <= instance.demand(j) * instance.per_unit_transportation_cost(i, j));
        }
    }

    Model rmp;
    auto z = rmp.add_var(0., Inf, Continuous, 1, "z");
    auto y = rmp.add_vars(Dim<1>(n_facilities), 0., 1., Binary, 0., "y");
    Expr expr;
    for (unsigned int i = 0 ; i < n_facilities ; ++i) {
        expr -= instance.capacity(i) * !w[i] * y[i];
        for (unsigned int j = 0; j < n_customers; ++j) {
            expr -= !v[i][j] * y[i];
        }
    }
    for (unsigned int j = 0; j < n_customers; ++j) {
        expr += !u[j];
    }
    auto cut = rmp.add_ctr(z >= expr);
    rmp.set(Attr::Obj::Expr, idol_Sum(i, Range(n_facilities), instance.fixed_cost(i) * y[i]) + z);

    Benders solver(rmp, true);

    //solver.set(Param::Algorithm::MaxIterations, 0);

    auto& master_solver = solver.set_master_solution_strategy<Solvers::Gurobi>();
    master_solver.set(Param::Algorithm::ResetBeforeSolving, true);

    auto& benders_subproblem = solver.add_subproblem(subproblem, cut, z);
    benders_subproblem.set_exact_solution_strategy<Solvers::Gurobi>();
    benders_subproblem.exact_solution_strategy().set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);

    solver.solve();

    std::cout << "--------------" << std::endl;
    std::cout << solver.primal_solution() << std::endl;

    return 0;
     */
}
