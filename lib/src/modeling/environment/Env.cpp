//
// Created by henri on 07/09/22.
//
#include "../../../include/modeling/environment/Env.h"

Env* Env::s_unique_env = nullptr;

Env &Env::get() {
    if (!s_unique_env) {
        s_unique_env = new Env();
    }
    return *s_unique_env;
}
