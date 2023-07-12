//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODE_H
#define IDOL_NODE_H

#include <memory>

namespace idol {
    class Model;
    template<class NodeInfoT> class Node;
}

template<class NodeInfoT>
class idol::Node {

    static_assert(std::is_default_constructible_v<NodeInfoT>);

    unsigned int m_id;
    unsigned int m_level;
    std::shared_ptr<NodeInfoT> m_info;

    Node() : m_level(0), m_id(0), m_info(std::make_shared<NodeInfoT>()) {}
public:
    Node(NodeInfoT* t_ptr_to_info, unsigned int t_id, unsigned int t_level) : m_id(t_id), m_level(t_level), m_info(t_ptr_to_info) {}

    Node(const Node&) = default;
    Node(Node&&) noexcept = default;

    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) noexcept = default;

    static Node<NodeInfoT> create_root_node() { return Node<NodeInfoT>(); }

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int level() const { return m_level; }

    [[nodiscard]] const NodeInfoT& info() const { return *m_info; }

    NodeInfoT& info() { return *m_info; }
};

#endif //IDOL_NODE_H
