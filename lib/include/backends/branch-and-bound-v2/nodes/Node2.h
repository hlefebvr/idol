//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODE2_H
#define IDOL_NODE2_H

#include <memory>

template<class NodeInfoT>
class Node2 {

    static_assert(std::is_default_constructible_v<NodeInfoT>);

    const unsigned int m_id;
    const unsigned int m_level;
    std::unique_ptr<NodeInfoT> m_info;

    Node2() : m_level(0), m_id(0), m_info(std::make_unique<NodeInfoT>()) {}
public:
    Node2(NodeInfoT* t_ptr_to_info, unsigned int t_id, unsigned int t_level) : m_id(t_id), m_level(t_level), m_info(t_ptr_to_info) {}

    static Node2<NodeInfoT>* create_root_node() { return new Node2<NodeInfoT>(); }

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int level() const { return m_level; }

    [[nodiscard]] double objective_value() const { return m_info->objective_value(); }

    [[nodiscard]] int status() const { return m_info->status(); }

    [[nodiscard]] int reason() const { return m_info->reason(); }

    [[nodiscard]] const NodeInfoT& info() const { return *m_info; }

    NodeInfoT& info() { return *m_info; }

    void save(const AbstractModel& t_original_formulation, const AbstractModel& t_model) { m_info->save(t_original_formulation, t_model); }
};

#endif //IDOL_NODE2_H
