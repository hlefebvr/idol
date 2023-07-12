//
// Created by henri on 17/10/22.
//

#ifndef IDOL_NODESET_H
#define IDOL_NODESET_H

#include <map>
#include "containers/IteratorForward.h"

namespace idol {
    template<class NodeT>
    class NodeSet;
}

template<class NodeT>
class idol::NodeSet {
    using by_objective_value_t = std::multimap<double, NodeT>;
    using by_level_t = std::multimap<unsigned int, NodeT>;
    by_objective_value_t m_by_objective_value;
    by_level_t m_by_level;
public:
    class const_iterator;

    using ByObjectiveValueNodes = ConstIteratorForward<by_objective_value_t, const_iterator>;
    using ByLevelNodes = ConstIteratorForward<by_level_t, const_iterator>;

    [[nodiscard]] ByObjectiveValueNodes by_objective_value() const { return ByObjectiveValueNodes(m_by_objective_value); }
    [[nodiscard]] ByLevelNodes by_level() const { return ByLevelNodes(m_by_level); }

    const_iterator emplace(NodeT t_node);

    void clear();

    [[nodiscard]] bool empty() const;

    [[nodiscard]] unsigned int size() const { return m_by_objective_value.size(); }

    const_iterator erase(const const_iterator& t_it);

    void merge(NodeSet<NodeT>&& t_node_set);
};

template<class NodeT>
void idol::NodeSet<NodeT>::merge(NodeSet<NodeT> &&t_node_set) {

    for (auto pair : t_node_set.m_by_objective_value) {
        m_by_objective_value.emplace(pair);
    }

    for (auto pair : t_node_set.m_by_level) {
        m_by_level.emplace(pair);
    }

    t_node_set.clear();

}

template<class NodeT>
typename idol::NodeSet<NodeT>::const_iterator idol::NodeSet<NodeT>::emplace(NodeT t_node) {
    auto it = m_by_objective_value.template emplace(t_node.info().objective_value(), t_node);
    m_by_level.template emplace(t_node.level(), t_node);
    return const_iterator(std::move(it));
}

template<class NodeT>
void idol::NodeSet<NodeT>::clear()  {
    m_by_objective_value.clear();
    m_by_level.clear();
}

template<class NodeT>
bool idol::NodeSet<NodeT>::empty() const {
    return m_by_objective_value.empty();
}

template<class NodeT>
typename idol::NodeSet<NodeT>::const_iterator idol::NodeSet<NodeT>::erase(const NodeSet::const_iterator &t_it) {
    const unsigned int id = t_it->id();

    if (t_it.is_by_level()) {
        const double objective_value = t_it->info().objective_value();
        auto it = m_by_objective_value.lower_bound(objective_value);
        for (; it->second.id() != id ; ++it);
        m_by_objective_value.erase(it);
        return const_iterator(m_by_level.erase(t_it.m_by_level_it));
    }

    const unsigned int level = t_it->level();
    auto it = m_by_level.lower_bound(level);
    for (; it->second.id() != id ; ++it);
    m_by_level.erase(it);
    return const_iterator(m_by_objective_value.erase(t_it.m_by_objective_value_it));
}


template<class NodeT>
class idol::NodeSet<NodeT>::const_iterator {
    friend class NodeSet<NodeT>;

    using by_objective_value_it_t = typename by_objective_value_t::const_iterator;
    using by_level_it_t = typename by_level_t::const_iterator;

    bool m_is_by_level = true;
    by_objective_value_it_t m_by_objective_value_it;
    by_level_it_t m_by_level_it;
public:
    const_iterator() = default;
    const_iterator(by_objective_value_it_t t_it) : m_by_objective_value_it(std::move(t_it)), m_is_by_level(false) {} // NOLINT(google-explicit-constructor)
    const_iterator(by_level_it_t t_it) : m_by_level_it(std::move(t_it)), m_is_by_level(true) {} // NOLINT(google-explicit-constructor)
    bool operator!=(const const_iterator& t_rhs) const {
        if (m_is_by_level != t_rhs.m_is_by_level) {
            return true;
        }
        if (m_is_by_level) {
            return m_by_level_it != t_rhs.m_by_level_it;
        }
        return m_by_objective_value_it != t_rhs.m_by_objective_value_it;
    }

    [[nodiscard]] bool is_by_level() const { return m_is_by_level; }

    const_iterator& operator=(const const_iterator& t_rhs) {
        if (this == &t_rhs) { return *this; }
        m_by_level_it = t_rhs.m_by_level_it;
        m_by_objective_value_it = t_rhs.m_by_objective_value_it;
        m_is_by_level = t_rhs.m_is_by_level;
        return *this;
    }
    const_iterator& operator++() {
        if (m_is_by_level) {
            ++m_by_level_it;
        } else {
            ++m_by_objective_value_it;
        }
        return *this;
    }
    const_iterator& operator--() {
        if (m_is_by_level) {
            --m_by_level_it;
        } else {
            --m_by_objective_value_it;
        }
        return *this;
    }
    const NodeT& operator*() const {
        if (m_is_by_level) {
            return m_by_level_it->second;
        }
        return m_by_objective_value_it->second;
    }

    const NodeT* operator->() const {
        if (m_is_by_level) {
            return &m_by_level_it->second;
        }
        return &m_by_objective_value_it->second;
    }

    NodeT* operator->() {
        if (m_is_by_level) {
            return &m_by_level_it->second;
        }
        return &m_by_objective_value_it->second;
    }
};

#endif //IDOL_NODESET_H
