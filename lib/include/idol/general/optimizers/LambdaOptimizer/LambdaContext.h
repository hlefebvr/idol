//
// Created by charlotte on 05.06.2026.
//

#ifndef IDOL_LAMBDA_CONTEXT_H
#define IDOL_LAMBDA_CONTEXT_H

#include "idol/general/utils/types.h"
#include "idol/mixed-integer/modeling/models/Model.h"

namespace idol {
    class LambdaContext {
        idol::SolutionStatus m_status;
        std::vector<std::pair<std::string, double>> m_vars_primal;
    public:
        LambdaContext() = default;

        void set_status(idol::SolutionStatus t_status){
            std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
            m_status = t_status;
        }
        idol::SolutionStatus get_status() const{
            std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
            return m_status;
        }
        void set_var_primal(const idol::Var& t_var, double t_value){
            std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 

            const auto& name = t_var.name();

            std::cout << "name address = " << &name << std::endl;
            std::cout << "name = " << name << std::endl;
                
            std::string s = t_var.name();

            std::cout << "copied name = " << s << std::endl;

            m_vars_primal.emplace_back(s, t_value);
        }
        std::vector<std::pair<std::string, double>> get_vars_primal() const{
            std::cout << __FILE__ << " " << "BEGIN " << __FUNCTION__ << " (" << __LINE__ << ")" << std::endl; 
            return m_vars_primal;
        }
    };
}

#endif // IDOL_LAMBDA_CONTEXT_H