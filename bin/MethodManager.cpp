//
// Created by Henri on 19/02/2026.
//

#include "MethodManager.h"

#include "Arguments.h"

MethodManager::MethodManager(storage&& t_all_methods) : m_all_methods(std::move(t_all_methods)) {}

void MethodManager::add(const std::string& t_tag) {
    const auto it = m_all_methods.find(t_tag);
    if (it == m_all_methods.end()) {
        throw idol::Exception("Could not find method tag: " + t_tag + '.');
    }
    const auto [result, success] = m_available_methods.emplace(it->second.first, it);
    if (!success) {
        throw idol::Exception("There is a tie in the available methods. Please contact henri.lefebvre@cnrs.fr to resolve this issue.");
    }
}

void MethodManager::print_available_methods(const Arguments& t_args) {

    std::cout << "-- Detected: applicable methods are [";

    bool first = true;
    for (auto& [score, it] : std::ranges::reverse_view(m_available_methods)) {
        if (!first) {
            std::cout << ", ";
        }
        std::cout << it->first;
        first = false;
    }

    std::cout << "]\n";

    if (!t_args.solve) {

        std::cout << "\nDetails (all methods):\n" << std::endl;
        for (auto & [key, pair] : m_all_methods) {
            std::cout << " - " << key << ":\n\t" << pair.second << "\n\n";
        }

        exit(0);

    }

}

const std::string& MethodManager::get_default_method() const {
    const auto it = m_available_methods.rbegin();
    if (it == m_available_methods.rend()) {
        throw idol::Exception("It seems that there are no available method for your problem class.");
    }
    return it->second->first;
}

const std::string& MethodManager::get_method(const Arguments& t_args) const {

    if (t_args.method.empty()) {
        return get_default_method();
    }

    const auto it = m_all_methods.find(t_args.method);
    if (it == m_all_methods.end()) {
        throw idol::Exception("The requested method does not exist for this problem type.");
    }

    const auto method = m_available_methods.find(it->second.first);
    if (method == m_available_methods.end()) {
        throw idol::Exception("The requested method exists, but cannot be used in this context (assumptions).");
    }

    return t_args.method;
}
