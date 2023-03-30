//
// Created by henri on 30/03/23.
//

#ifndef IDOL_CALLBACKFACTORY_H
#define IDOL_CALLBACKFACTORY_H

template<class NodeInfoT> class Callback;

template<class NodeInfoT>
class CallbackFactory {
public:
    virtual ~CallbackFactory() = default;

    virtual Callback<NodeInfoT>* operator()() = 0;

    virtual CallbackFactory<NodeInfoT>* clone() const = 0;
};

#endif //IDOL_CALLBACKFACTORY_H
