//
// Created by henri on 21/03/23.
//

#ifndef IDOL_NODEUPDATOR_H
#define IDOL_NODEUPDATOR_H

class Model;

template<class NodeT>
class NodeUpdator {
public:
    virtual ~NodeUpdator() = default;

    virtual void apply_local_updates(const NodeT& t_node) = 0;

    virtual void clear_local_updates() = 0;
};

#endif //IDOL_NODEUPDATOR_H
