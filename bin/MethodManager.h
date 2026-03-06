//
// Created by Henri on 19/02/2026.
//

#ifndef IDOL_METHODMANAGER_H
#define IDOL_METHODMANAGER_H

#include <unordered_map>
#include <map>
#include <ranges>

#include "idol/general/utils/exceptions/Exception.h"

class Arguments;

class MethodManager {
public:
    using storage = std::unordered_map<std::string, std::pair<unsigned int, std::string>>;

    MethodManager(storage&& t_all_methods);

    void add(const std::string& t_tag);

    void print_available_methods(const Arguments& t_args);

    [[nodiscard]] const std::string& get_default_method() const;

    [[nodiscard]] const std::string& get_method(const Arguments& t_args) const;

protected:
    const storage m_all_methods;
    std::map<unsigned int, storage::const_iterator> m_available_methods;
};

#endif //IDOL_METHODMANAGER_H