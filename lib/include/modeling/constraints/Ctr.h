//
// Created by henri on 27/01/23.
//

#ifndef IDOL_CTR_H
#define IDOL_CTR_H

#include "modeling/objects/Object.h"

class CtrVersion;

template<class T> class Versions;

namespace impl {
    class Env;
}

class Ctr : public Object<CtrVersion> {
    friend class impl::Env;

    Ctr(std::list<Versions<CtrVersion>>::iterator t_it, unsigned int t_id, std::string t_name)
            : Object<CtrVersion>(t_it, t_id, std::move(t_name)) {}
};

IDOL_MAKE_HASHABLE(Ctr)

#endif //IDOL_CTR_H
