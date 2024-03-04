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
    std::shared_ptr<Node<NodeInfoT>> m_parent;

    Node() : m_level(0), m_id(0), m_info(std::make_shared<NodeInfoT>()) {}
    Node(NodeInfoT* t_ptr_to_info, unsigned int t_id) : m_id(t_id), m_level(0), m_info(t_ptr_to_info) {}
public:
    Node(NodeInfoT* t_ptr_to_info, unsigned int t_id, const Node<NodeInfoT>& t_parent)
            : m_id(t_id),
              m_level(t_parent.level() + 1),
              m_info(t_ptr_to_info),
              m_parent(std::make_shared<Node<NodeInfoT>>(t_parent))
    {}

    Node(const Node&) = default;
    Node(Node&&) noexcept = default;

    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) noexcept = default;

    static Node<NodeInfoT> create_root_node() { return Node<NodeInfoT>(); }

    static Node<NodeInfoT> create_detached_node(NodeInfoT* t_ptr_to_info) { return Node<NodeInfoT>(t_ptr_to_info, -1); }

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int level() const { return m_level; }

    [[nodiscard]] const NodeInfoT& info() const { return *m_info; }

    NodeInfoT& info() { return *m_info; }

    [[nodiscard]] const Node<NodeInfoT>& parent() const { return *m_parent; }
};

#endif //IDOL_NODE_H
