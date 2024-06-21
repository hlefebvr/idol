//
// Created by henri on 20.06.24.
//

#include "idol/modeling/bilevel-optimization/write_to_file.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"

using namespace idol;

class AuxWriter {
    const Model& m_model;
    const Bilevel::LowerLevelDescription& m_description;
    const std::string& m_filename;
protected:
    std::list<Var> make_lower_level_vars_list();
    std::list<Ctr> make_lower_level_ctrs_list();
public:
    AuxWriter(const Model& t_model, const Bilevel::LowerLevelDescription& t_description, const std::string& t_filename)
            : m_model(t_model), m_description(t_description), m_filename(t_filename) {}
    void write();
};

void AuxWriter::write() {
    // AUX file
    std::ofstream aux_file(m_filename + ".aux");

    if (!aux_file.is_open()) {
        throw Exception("Could not create .aux file.");
    }

    std::list<Var> lower_level_vars = make_lower_level_vars_list();
    std::list<Ctr> lower_level_ctrs = make_lower_level_ctrs_list();
    const auto& lower_level_obj = m_description.follower_obj();

    aux_file << "@NUMVARS\n" << lower_level_vars.size() << "\n";
    aux_file << "@NUMCONSTRS\n" << lower_level_ctrs.size() << "\n";
    aux_file << "@VARSBEGIN\n";
    for (const auto& var : lower_level_vars) {
        aux_file << var.name() << '\t' << lower_level_obj.linear().get(var).as_numerical() << "\n";
    }
    aux_file << "@VARSEND\n";
    aux_file << "@CONSTRSBEGIN\n";
    for (const auto& ctr : lower_level_ctrs) {
        aux_file << ctr.name() << "\n";
    }
    aux_file << "@CONSTRSEND\n";
    aux_file << "@NAME\n" << m_filename << "\n";
    aux_file << "@MPS\n" << m_filename << ".mps\n";

    aux_file.close();
}

std::list<Var> AuxWriter::make_lower_level_vars_list() {

    std::list<Var> result;

    const auto& follower_vars = m_description.follower_vars();

    for (const auto& var : m_model.vars()) {
        if (var.get(follower_vars) != MasterId) {
            result.emplace_back(var);
        }
    }

    return result;
}

std::list<Ctr> AuxWriter::make_lower_level_ctrs_list() {

    std::list<Ctr> result;

    const auto& follower_ctrs = m_description.follower_ctrs();

    for (const auto& ctr : m_model.ctrs()) {
        if (ctr.get(follower_ctrs) != MasterId) {
            result.emplace_back(ctr);
        }
    }

    return result;
}

class MpsWriter {
    const Model &m_model;
    const Bilevel::LowerLevelDescription &m_description;
    const std::string &m_filename;
protected:
    std::list<Var> make_vars_list(const std::function<bool(const Var&)>& t_filter);
public:
    MpsWriter(const Model &t_model, const Bilevel::LowerLevelDescription &t_description, const std::string &t_filename)
            : m_model(t_model), m_description(t_description), m_filename(t_filename) {}
    void write();
};

void MpsWriter::write() {

    const auto continuous_vars = make_vars_list([this](const Var& t_var) {
        return m_model.get_var_type(t_var) == Continuous;
    });

    const auto integer_vars = make_vars_list([this](const Var& t_var) {
        return m_model.get_var_type(t_var) != Continuous;
    });

    std::ofstream file(m_filename + ".mps");

    if (!file.is_open()) {
        throw Exception("Could not create .mps file.");
    }

    file << std::left;

    file << "NAME " << m_filename << "\n";

    file << "ROWS\n";

    file << " N  OBJ\n";

    for (const auto& ctr : m_model.ctrs()) {

        const auto type = m_model.get_ctr_type(ctr);
        switch (type) {
            case LessOrEqual:
                file << " L  ";
                break;
            case GreaterOrEqual:
                file << " G  ";
                break;
            case Equal:
                file << " E  ";
                break;
            default:
                throw Exception("Unexpected constraint type.");
        }

        file << ctr.name() << '\n';
    }

    file << "COLUMNS\n";

    const auto add_entry = [&](const std::string& t_var_name, const std::string& t_ctr_name, double t_value) {
        file << "    " << std::setw(10) << t_var_name << ' ' << std::setw(10) << t_ctr_name  << ' ' << t_value << '\n';
    };

    const auto add_row_entries = [&](const Var& t_var) {
        const auto& column = m_model.get_var_column(t_var);
        add_entry(t_var.name(), "OBJ", column.obj().as_numerical());
        for (const auto& [ctr, constant] : column.linear()) {
            add_entry(t_var.name(), ctr.name(), constant.as_numerical());
        }
    };

    for (const auto& var : continuous_vars) {
        add_row_entries(var);
    }

    if (!integer_vars.empty()) {
        file << "    MARKER    'MARKER'                 'INTORG'\n";
        for (const auto& var : integer_vars) {
            add_row_entries(var);
        }
        file << "    MARKER    'MARKER'                 'INTEND'\n";
    }

    file << "RHS\n";

    for (const auto& ctr : m_model.ctrs()) {
        file << "    RHS       " << std::setw(10) << ctr.name() << ' ' << m_model.get_ctr_row(ctr).rhs().as_numerical() << '\n';
    }

    file << "BOUNDS\n";

    for (const auto& var : m_model.vars()) {
        const auto lb = m_model.get_var_lb(var);
        const auto ub = m_model.get_var_ub(var);
        const auto type = m_model.get_var_type(var);
        if (!is_neg_inf(lb)) {
            file << " L" << (type == Continuous ? 'O' : 'I') << " BND       " << std::setw(10) << var.name() << ' ' << lb << '\n';
        }
        if (!is_pos_inf(ub)) {
            file << " U" << (type == Continuous ? 'P' : 'I') << " BND       " << std::setw(10) << var.name() << ' ' << ub << '\n';
        }
    }

    file << "ENDATA\n";

}

std::list<Var> MpsWriter::make_vars_list(const std::function<bool(const Var &)> &t_filter) {
    std::list<Var> result;
    for (const auto& var : m_model.vars()) {
        if (t_filter(var)) {
            result.emplace_back(var);
        }
    }
    return result;
}

void idol::Bilevel::write_to_file(const Model& t_model, const Bilevel::LowerLevelDescription& t_description, const std::string& t_filename) {

    // LP file
    MpsWriter(t_model, t_description, t_filename).write();

    // AUX file
    AuxWriter(t_model, t_description, t_filename).write();
}
