//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODEUPDATOR_H
#define IDOL_NODEUPDATOR_H

namespace idol {
    class Model;

    template<class NodeT>
    class NodeUpdator;

    template<class NodeT>
    class Node;
}

template<class NodeT>
class idol::NodeUpdator {
public:
    virtual ~NodeUpdator() = default;

    virtual void prepare(const Node<NodeT>& t_node) = 0;

    virtual void clear() = 0;
};

#endif //IDOL_NODEUPDATOR_H
