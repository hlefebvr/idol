//
// Created by henri on 27/01/23.
//

#ifndef IDOL_MYVAR_H
#define IDOL_MYVAR_H

#include "MyObject.h"
#include "MyVarVersion.h"

template<class T> class MyVersions;

namespace impl {
    class MyEnv;
}

class MyVar : public MyObject<VarVersion> {
    friend class impl::MyEnv;

    MyVar(std::list<MyVersions<VarVersion>>::iterator t_it, unsigned int t_id, std::string t_name)
        : MyObject<VarVersion>(t_it, t_id, std::move(t_name)) {}
};

#endif //IDOL_MYVAR_H
