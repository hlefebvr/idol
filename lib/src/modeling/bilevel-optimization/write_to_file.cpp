//
// Created by henri on 20.06.24.
//

#include "idol/modeling/bilevel-optimization/write_to_file.h"
#include "idol/optimizers/mixed-integer-optimization/wrappers/Gurobi/Gurobi.h"

using namespace idol;

class AuxWriter {
    const Model& m_model;
    const Bilevel::Description& m_description;
    const std::string& m_filename;
protected:
    std::list<Var> make_lower_level_vars_list();
    std::list<Ctr> make_lower_level_ctrs_list();
public:
    AuxWriter(const Model& t_model, const Bilevel::Description& t_description, const std::string& t_filename)
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

    aux_file << "@NUMVARS\n" << lower_level_vars.size() << "\n";
    aux_file << "@NUMCONSTRS\n" << lower_level_ctrs.size() << "\n";
    aux_file << "@VARSBEGIN\n";
    for (const auto& var : lower_level_vars) {
        aux_file << var.name() << '\t' << m_model.get_var_column(var).obj().as_numerical() << "\n";
    }
    aux_file << "@VARSEND\n";
    aux_file << "@CONSTRSBEGIN\n";
    for (const auto& ctr : lower_level_ctrs) {
        aux_file << ctr.name() << "\n";
    }
    aux_file << "@CONSTRSEND\n";
    aux_file << "@NAME\n" << m_filename << "\n";
    aux_file << "@LP\n" << m_filename << ".mps\n";

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

void idol::Bilevel::write_to_file(const Model& t_model, const Bilevel::Description& t_description, const std::string& t_filename) {

    // LP file
    /*
    std::ofstream lp_file(t_filename + ".lp");

    if (!lp_file.is_open()) {
        throw Exception("Could not create .lp file.");
    }

    lp_file << t_model;

    lp_file.close();
     */
    auto copy = t_model.copy();
    copy.use(Gurobi());
    copy.write(t_filename + ".mps");

    // AUX file
    AuxWriter(t_model, t_description, t_filename).write();
}
