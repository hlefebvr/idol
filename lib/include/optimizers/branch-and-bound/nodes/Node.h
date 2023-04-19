//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODE_H
#define IDOL_NODE_H

#include <memory>

class Model;

template<class NodeInfoT>
class Node {

    static_assert(std::is_default_constructible_v<NodeInfoT>);

    const unsigned int m_id;
    const unsigned int m_level;
    std::unique_ptr<NodeInfoT> m_info;

    Node() : m_level(0), m_id(0), m_info(std::make_unique<NodeInfoT>()) {}
public:
    Node(NodeInfoT* t_ptr_to_info, unsigned int t_id, unsigned int t_level) : m_id(t_id), m_level(t_level), m_info(t_ptr_to_info) {}

    static Node<NodeInfoT>* create_root_node() { return new Node<NodeInfoT>(); }

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int level() const { return m_level; }

    [[nodiscard]] const NodeInfoT& info() const { return *m_info; }

    NodeInfoT& info() { return *m_info; }
};

#endif //IDOL_NODE_H
