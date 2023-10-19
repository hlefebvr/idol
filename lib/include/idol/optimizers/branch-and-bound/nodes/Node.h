//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODE_H
#define IDOL_NODE_H

#include <memory>

namespace idol {
    class Model;
    template<class NodeVarInfoT> class Node;
}

template<class NodeVarInfoT>
class idol::Node {

    static_assert(std::is_default_constructible_v<NodeVarInfoT>);

    unsigned int m_id;
    unsigned int m_level;
    std::shared_ptr<NodeVarInfoT> m_info;
    std::shared_ptr<Node<NodeVarInfoT>> m_parent;

    Node() : m_level(0), m_id(0), m_info(std::make_shared<NodeVarInfoT>()) {}
    Node(NodeVarInfoT* t_ptr_to_info, unsigned int t_id) : m_id(t_id), m_level(0), m_info(t_ptr_to_info) {}
public:
    Node(NodeVarInfoT* t_ptr_to_info, unsigned int t_id, const Node<NodeVarInfoT>& t_parent)
            : m_id(t_id),
              m_level(t_parent.level() + 1),
              m_info(t_ptr_to_info),
              m_parent(std::make_shared<Node<NodeVarInfoT>>(t_parent))
    {}

    Node(const Node&) = default;
    Node(Node&&) noexcept = default;

    Node& operator=(const Node&) = default;
    Node& operator=(Node&&) noexcept = default;

    static Node<NodeVarInfoT> create_root_node() { return Node<NodeVarInfoT>(); }

    static Node<NodeVarInfoT> create_detached_node(NodeVarInfoT* t_ptr_to_info) { return Node<NodeVarInfoT>(t_ptr_to_info, -1); }

    [[nodiscard]] unsigned int id() const { return m_id; }

    [[nodiscard]] unsigned int level() const { return m_level; }

    [[nodiscard]] const NodeVarInfoT& info() const { return *m_info; }

    NodeVarInfoT& info() { return *m_info; }

    [[nodiscard]] const Node<NodeVarInfoT>& parent() const { return *m_parent; }
};

#endif //IDOL_NODE_H
