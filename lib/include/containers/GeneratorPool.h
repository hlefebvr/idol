//
// Created by henri on 13/12/22.
//

#ifndef IDOL_GENERATORPOOL_H
#define IDOL_GENERATORPOOL_H

#include "../modeling/solutions/Solution.h"
#include <list>

template<class KeyT, class ValueT = Solution::Primal>
class GeneratorPool {
    std::list<std::pair<KeyT, ValueT>> m_values;
public:
    void add(KeyT t_key, ValueT t_value) {
        m_values.template emplace_back(std::move(t_key), std::move(t_value));
    }

    typename std::list<std::pair<KeyT, ValueT>>::iterator erase(const typename std::list<std::pair<KeyT, ValueT>>::iterator & t_it) {
        return m_values.erase(t_it);
    }

    [[nodiscard]] const ValueT& last_inserted() const { return m_values.back().second; }

    void clear() { m_values.clear(); }

    [[nodiscard]] unsigned int size() const { return m_values.size(); }

    using Values = IteratorForward<std::list<std::pair<KeyT, ValueT>>>;
    using ConstValues = ConstIteratorForward<std::list<std::pair<KeyT, ValueT>>>;

    Values values() { return Values(m_values); }
    [[nodiscard]] ConstValues values() const { return ConstValues(m_values); }
};

#endif //IDOL_GENERATORPOOL_H
