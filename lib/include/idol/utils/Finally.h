//
// Created by henri on 14.12.23.
//

#ifndef IDOL_FINALLY_H
#define IDOL_FINALLY_H

#include <functional>

namespace idol {
    class Finally;
}

class idol::Finally {
    std::function<void()> m_function;
public:
    explicit Finally(std::function<void()> t_function) : m_function(std::move(t_function)) {}

    ~Finally() { m_function(); }
};


#endif //IDOL_FINALLY_H
