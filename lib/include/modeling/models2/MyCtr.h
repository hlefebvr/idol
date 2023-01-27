//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYCTR_H
#define IDOL_MYCTR_H

#include "MyObject.h"
#include "MyCtrVersion.h"

template<class T> class MyVersions;

namespace impl {
    class MyEnv;
}

class MyCtr : public MyObject<CtrVersion> {
    friend class impl::MyEnv;

    MyCtr(std::list<MyVersions<CtrVersion>>::iterator t_it, unsigned int t_id, std::string t_name)
            : MyObject<CtrVersion>(t_it, t_id, std::move(t_name)) {}
};

#endif //IDOL_MYCTR_H
