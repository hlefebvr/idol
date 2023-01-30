//
// Created by henri on 27/01/23.
//

#ifndef IDOL_VAR_H
#define IDOL_VAR_H

#include "modeling/objects/Object.h"

class VarVersion;

template<class T> class Versions;

namespace impl {
    class Env;
}

class Var : public Object<VarVersion> {
    friend class impl::Env;

    Var(std::list<Versions<VarVersion>>::iterator t_it, unsigned int t_id, std::string t_name)
        : Object<VarVersion>(t_it, t_id, std::move(t_name)) {}
};

IDOL_MAKE_HASHABLE(Var)

#endif //IDOL_VAR_H
