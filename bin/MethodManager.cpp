//
// Created by Henri on 19/02/2026.
//

#include "MethodManager.h"

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

void MethodManager::print_available_methods(bool t_with_details) {

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

    if (t_with_details) {

        std::cout << "\tHelp for applicable methods:" << "\n";
        for (auto & [score, it] : std::ranges::reverse_view(m_available_methods)) {
            std::cout << "\t\t " << it->first << ": " << it->second.second << "\n";
        }

    }

}

const std::string& MethodManager::get_default_method() const {
    const auto it = m_available_methods.rbegin();
    if (it == m_available_methods.rend()) {
        throw idol::Exception("It seems that there are no available method for your problem class.");
    }
    return it->second->first;
}
