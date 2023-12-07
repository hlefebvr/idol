//
// Created by henri on 07.12.23.
//
#include <idol/modeling/models/read.h>
#include <idol/modeling/expressions/operations/operators.h>

#ifdef IDOL_USE_GLPK
#include <glpk.h>

class finally {
    std::function<void()> m_function;
public:
    explicit finally(std::function<void()> t_function) : m_function(std::move(t_function)) {}

    ~finally() { m_function(); }
};

idol::Model read_from_glpk(idol::Env& t_env, glp_prob* t_model) {

    using namespace idol;

    const int n_variables = glp_get_num_cols(t_model);
    const int n_constraints = glp_get_num_rows(t_model);

    Model result(t_env);

    finally on_terminate([&]() { glp_delete_prob(t_model); });

    for (int j = 1 ; j <= n_variables ; ++j) {

        const double lb = glp_get_col_lb(t_model, j);
        const double ub = glp_get_col_ub(t_model, j);
        const double obj = glp_get_obj_coef(t_model, j);
        VarType type;
        switch (glp_get_col_kind(t_model, j)) {
            case GLP_CV: type = Continuous; break;
            case GLP_IV: type = Integer; break;
            case GLP_BV: type = Binary; break;
            default: throw Exception("Unexpected variable type while parsing.");
        }

        const std::string name = glp_get_col_name(t_model, j);
        result.add_var(lb, ub, type, Column(obj),name);
    }

    for (int i = 1 ; i <= n_constraints ; ++i) {

        CtrType type;
        double rhs;
        switch (glp_get_row_type(t_model, i)) {
            case GLP_UP:
                type = LessOrEqual;
                rhs = glp_get_row_ub(t_model, i);
                break;
            case GLP_LO:
                type = GreaterOrEqual;
                rhs = glp_get_row_lb(t_model, i);
                break;
            case GLP_FX:
                type = Equal;
                rhs = glp_get_row_ub(t_model, i);
                break;
            default: throw Exception("Unexpected variable type while parsing.");
        }

        const std::string name = glp_get_row_name(t_model, i);
        const int nz = glp_get_mat_row(t_model, i, NULL, NULL);

        auto* indices = new int[nz+1];
        auto* coefficients = new double[nz+1];
        glp_get_mat_row(t_model, i, indices, coefficients);

        Expr<Var> lhs;
        for (int k = 1 ; k <= nz ; ++k) {
            const unsigned int var_index = indices[k] - 1;
            const double coefficient = coefficients[k];
            lhs += coefficient * result.get_var_by_index(var_index);
        }

        delete[] indices;
        delete[] coefficients;

        result.add_ctr(TempCtr(Row(std::move(lhs), rhs), type), name);
    }

    if (glp_get_obj_dir(t_model) == GLP_MAX) {
        result.set_obj_sense(Maximize);
    }

    return std::move(result);
}

#endif

idol::Model idol::read_mps_file(Env& t_env, const std::string& t_filename, bool t_use_fixed_format) {
#ifdef IDOL_USE_GLPK
    glp_prob *model = glp_create_prob();
    glp_read_mps(model, t_use_fixed_format ? GLP_MPS_DECK : GLP_MPS_FILE, NULL, t_filename.c_str());
    return read_from_glpk(t_env, model);
#else
    throw Exception("You need to link idol with GLPK to parse MPS files.");
#endif
}

idol::Model idol::read_lp_file(Env& t_env, const std::string& t_filename) {
#ifdef IDOL_USE_GLPK
    glp_prob* model = glp_create_prob();
    glp_read_lp(model,  NULL, t_filename.c_str());
    return read_from_glpk(t_env, model);
#else
    throw Exception("You need to link idol with GLPK to parse LP files.");
#endif
}
