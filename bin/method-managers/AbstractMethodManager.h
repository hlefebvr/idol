//
// Created by Henri on 19/05/2026.
//

#ifndef IDOL_NEWMETHODMANAGER_H
#define IDOL_NEWMETHODMANAGER_H

#include <iostream>
#include <string>
#include <vector>

#include "../Arguments.h"
#include "idol/mixed-integer/modeling/models/Model.h"

#define CHECK(CONDITION, TEXT) \
{ \
if (!m_problem_has_been_set) { \
std::cout << " \033[90m• "; \
} \
else { \
const bool __condition_value = (CONDITION); \
std::cout << ((__condition_value) ? " \033[32m✓ " : " \033[31m✗ "); \
if (!__condition_value) { condition_is_met = false; } \
} \
std::cout << TEXT << std::endl; \
std::cout << "\033[0m"; \
}

class Arguments;
class AbstractMethod;

template<class MethodT>
class AbstractMethodManager {
protected:
    const Arguments& m_arguments;

    template<class T> void add();
    virtual void print_methods(const std::vector<MethodT*>& methods) const = 0;
public:
    AbstractMethodManager(const Arguments& t_args);
    virtual ~AbstractMethodManager() = default;

    std::vector<MethodT*> applicable_methods() const;
    std::vector<MethodT*> other_methods() const;

    const MethodT& get_default_method() const;
    const MethodT& get_method(const std::string& t_name) const;

    void print_details() const;
private:
    std::vector<std::unique_ptr<MethodT>> m_all_methods;
};

template <class MethodT>
template <class T>
void AbstractMethodManager<MethodT>::add() {
    m_all_methods.emplace_back(std::make_unique<T>());
}

template <class MethodT>
AbstractMethodManager<MethodT>::AbstractMethodManager(const Arguments& t_args) : m_arguments(t_args) {

}

template <class MethodT>
std::vector<MethodT*> AbstractMethodManager<MethodT>::applicable_methods() const {
    std::vector<MethodT*> result;
    for (const auto& method : m_all_methods) {
        if (method->is_applicable()) {
            result.push_back(method.get());
        }
    }
    return result;
}

template <class MethodT>
std::vector<MethodT*> AbstractMethodManager<MethodT>::other_methods() const {
    std::vector<MethodT*> result;
    for (const auto& method : m_all_methods) {
        if (!method->is_applicable()) {
            result.push_back(method.get());
        }
    }
    return result;
}

template <class MethodT>
const MethodT& AbstractMethodManager<MethodT>::get_default_method() const {
    auto applicable_methods = this->applicable_methods();
    std::sort(applicable_methods.begin(), applicable_methods.end(), [](const MethodT* a, const MethodT* b) { return a->score() > b->score(); });
    if (applicable_methods.empty()) {
        std::cerr << "There are no applicable methods." << std::endl;
        exit(1);
    }
    return *applicable_methods.front();
}

template <class MethodT>
const MethodT& AbstractMethodManager<MethodT>::get_method(const std::string& t_name) const {
    if (t_name.empty()) {
        return get_default_method();
    }
    for (const auto& method : m_all_methods) {
        if (method->name() == t_name) {
            if (!method->is_applicable()) {
                std::cerr << "The requested method exists but is not applicable to this problem.\nHere is the description of the method.\n\n" << std::endl;
                print_methods(std::vector<MethodT*> { method.get() }),
                exit(1);
            }
            return *method;
        }
    }
    std::cout << "The requested method does not exists or none is applicable." << std::endl;
    exit(1);
}

template <class MethodT>
void AbstractMethodManager<MethodT>::print_details() const {

    const auto applicable_methods = this->applicable_methods();
    const auto other_methods = this->other_methods();

    if (!applicable_methods.empty()) {
        std::cout << "\n\033[4mApplicable Methods\033[0m" << std::endl;
        print_methods(applicable_methods);
    }

    if (!other_methods.empty()) {
        std::cout << "\033[4mOther Methods\033[0m" << std::endl;
        print_methods(other_methods);
    }

}

class AbstractMethod {
    bool m_is_applicable = false;
public:
    virtual ~AbstractMethod() = default;

    void set_applicable(bool t_value) { m_is_applicable = t_value; }
    [[nodiscard]] bool is_applicable() const { return m_is_applicable; }
    [[nodiscard]] virtual unsigned int score() const = 0;
    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;
};


#endif //IDOL_NEWMETHODMANAGER_H