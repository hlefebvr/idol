//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_ATTRIBUTES_H
#define OPTIMIZE_ATTRIBUTES_H

class AbstractAttributes {
public:
    virtual ~AbstractAttributes() = default;
};

template<class ...ParamT>
class Attributes : public AbstractAttributes, public ParamT... {};

#endif //OPTIMIZE_ATTRIBUTES_H
