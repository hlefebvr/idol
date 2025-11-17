//
// Created by henri on 04.03.24.
//
#include "idol/bilevel/modeling/read_from_file.h"
#include "idol/mixed-integer/modeling/models/Model.h"
#include "idol/general/utils/Set.h"
#include "idol/mixed-integer/modeling/objects/Versions.h"
#include "idol/mixed-integer/modeling/expressions/operations/operators.h"
#include <fstream>
#include <filesystem>
#include <algorithm>

using namespace idol;

class AuxParser {
    Env& m_env;

    Map<std::string, double> m_lower_level_variables;
    Set<std::string> m_lower_level_constraints;

    unsigned int m_n_variables = 0;
    unsigned int m_n_constraints = 0;
    std::unique_ptr<Model> m_high_point_relaxation;
    Bilevel::Description& m_lower_level_description;

    void read_aux_file(const std::string& t_path_to_aux, const std::function<Model(Env&, const std::string&)>& t_read_model_from_file);
    bool read_tag(std::ifstream& t_file, const std::string& t_tag, bool t_mandatory = true);
    void read_endl(std::ifstream& t_file);
    void read_n_variables(std::ifstream& t_file);
    void read_n_constraints(std::ifstream& t_file);
    void read_follower_variables(std::ifstream& t_file);
    void read_follower_constraints(std::ifstream& t_file);
    std::string read_path(std::ifstream& t_file, const std::string& t_path_to_aux);
    void read_file(std::ifstream& t_file, const std::string& t_path_to_aux, const std::function<Model(Env&, const std::string&)>& t_read_model_from_file);
    void read_name(std::ifstream& t_file);

    void set_var_annotations();
    void set_ctr_annotations();
    void create_lower_level_objective();
public:
    explicit AuxParser(Env& t_env,
                       Bilevel::Description& t_lower_level_description,
                       const std::string& t_path_to_aux,
                       const std::function<Model(Env&, const std::string&)>& t_create_model_from_mps);

    [[nodiscard]] Model model() { auto result = std::move(*m_high_point_relaxation); m_high_point_relaxation.reset(); return result; }
};

AuxParser::AuxParser(Env &t_env,
                     Bilevel::Description &t_lower_level_description,
                     const std::string& t_path_to_aux,
                     const std::function<Model(Env&, const std::string&)>& t_create_model_from_mps
                     )
    : m_env(t_env),
      m_lower_level_description(t_lower_level_description) {

    read_aux_file(t_path_to_aux, t_create_model_from_mps);
    set_var_annotations();
    set_ctr_annotations();
    create_lower_level_objective();

}

void AuxParser::read_aux_file(const std::string &t_path_to_aux, const std::function<Model(Env&, const std::string&)>& t_read_model_from_file) {

    std::ifstream file(t_path_to_aux);

    if (!file.is_open()) {
        throw Exception("Could not open aux file " + t_path_to_aux + ".");
    }

    read_tag(file, "@NUMVARS");
    read_n_variables(file);

    read_tag(file, "@NUMCONSTRS");
    read_n_constraints(file);

    read_tag(file, "@VARSBEGIN");
    read_follower_variables(file);
    read_tag(file, "@VARSEND");

    read_tag(file, "@CONSTRSBEGIN");
    read_follower_constraints(file);
    read_tag(file, "@CONSTRSEND");

    if (read_tag(file, "@NAME", false)) {
        read_name(file);
    }

    if (m_high_point_relaxation) {
        file.close();
        std::cout << "Skipping parsing HPR from MPS or LP file, using argument instead." << std::endl;
        return;
    }

    if (read_tag(file, "@MPS", false) || read_tag(file, "@LP", false)) {
        read_file(file, t_path_to_aux, t_read_model_from_file);
    } else {
        throw Exception("Parsing error: could not parse_variables LP or MPS tags.");
    }

    file.close();
}

void AuxParser::read_endl(std::ifstream &t_file) {

    std::string current_line;
    std::getline(t_file, current_line);

    if (!std::all_of(current_line.begin(), current_line.end(), [](char t_c){ return std::isspace(t_c); })) {
        throw Exception("Parsing error, expected end of line.");
    }

}

void AuxParser::read_n_variables(std::ifstream &t_file) {
    t_file >> m_n_variables;
    read_endl(t_file);
}

void AuxParser::read_n_constraints(std::ifstream &t_file) {
    t_file >> m_n_constraints;
    read_endl(t_file);
}

void AuxParser::read_follower_variables(std::ifstream &t_file) {

    for (int j = 0; j < m_n_variables ; ++j) {
        std::string name;
        double obj;
        t_file >> name >> obj;

        m_lower_level_variables.emplace(name, obj);
    }

    read_endl(t_file);
}

void AuxParser::read_follower_constraints(std::ifstream &t_file) {

    for (int i = 0; i < m_n_constraints ; ++i) {
        std::string name;
        t_file >> name;

        m_lower_level_constraints.emplace(name);
    }

    read_endl(t_file);
}

std::string AuxParser::read_path(std::ifstream &t_file, const std::string &t_path_to_aux) {
    std::filesystem::path path;
    t_file >> path;
    if (path.is_relative()) {
        path = std::filesystem::path(t_path_to_aux).replace_filename(path.filename());
    }
    return path.string();
}

void AuxParser::read_file(std::ifstream &t_file, const std::string &t_path_to_aux, const std::function<Model(Env&, const std::string&)>& t_read_model_from_file) {

    const auto path = read_path(t_file, t_path_to_aux);

    if (std::filesystem::exists(path)) {
        m_high_point_relaxation = std::make_unique<Model>(t_read_model_from_file(m_env, path));
        return;
    }

    if (std::filesystem::exists(path + ".gz")) {
        m_high_point_relaxation = std::make_unique<Model>(t_read_model_from_file(m_env, path + ".gz"));
        return;
    }

    throw Exception("Could not find base instance file " + t_path_to_aux);

}

bool AuxParser::read_tag(std::ifstream &t_file, const std::string &t_tag, bool t_mandatory) {

    std::streampos previous_position = t_file.tellg();

    std::string current_line;
    const bool success = std::getline(t_file, current_line) &&  current_line.find(t_tag) != std::string::npos;

    if (success) {
        return success;
    }

    if (!t_mandatory) {
        t_file.seekg(previous_position);
        return false;
    }

    throw Exception("Parsing error, could not parse_variables tag: " + t_tag);

}

void AuxParser::read_name(std::ifstream &t_file) {

    std::string current_line;
    std::getline(t_file, current_line);

}

void AuxParser::set_var_annotations() {

    for (const auto& var : m_high_point_relaxation->vars()) {

        if (m_lower_level_variables.find(var.name()) == m_lower_level_variables.end()) {
            m_lower_level_description.make_upper_level(var);
            continue;
        }

        m_lower_level_description.make_lower_level(var);

    }

}

void AuxParser::set_ctr_annotations() {

    for (const auto& ctr : m_high_point_relaxation->ctrs()) {

        if (m_lower_level_constraints.find(ctr.name()) == m_lower_level_constraints.end()) {
            m_lower_level_description.make_upper_level(ctr);
            continue;
        }

        m_lower_level_description.make_lower_level(ctr);

    }

}

void AuxParser::create_lower_level_objective() {

    AffExpr obj;

    for (const auto& var : m_high_point_relaxation->vars()) {

        if (auto it = m_lower_level_variables.find(var.name()) ; it != m_lower_level_variables.end()) {
            obj += it->second * var;
        }

    }

    m_lower_level_description.set_lower_level_obj(std::move(obj));
}

idol::Model
idol::Bilevel::impl::read_from_file(Env& t_env,
                              const std::string& t_path_to_aux,
                              Bilevel::Description& t_lower_level_description,
                              const std::function<Model(Env&, const std::string&)>& t_mps_reader) {

    AuxParser parser(t_env, t_lower_level_description, t_path_to_aux, t_mps_reader);

    return parser.model();

}