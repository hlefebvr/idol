//
// Created by henri on 23/09/22.
//

#ifndef OPTIMIZE_NOTIMPLEMENTED_H
#define OPTIMIZE_NOTIMPLEMENTED_H

#include "Exception.h"

class NotImplemented : public Exception {
public:
    NotImplemented(std::string t_functionality,
                   const std::string& t_method_to_override) : Exception(
                           std::move(t_functionality) + " is not implemented. "
                           "If you wish to implement it, please override the "
                           + t_method_to_override + " method.") {

    }
};

#endif //OPTIMIZE_NOTIMPLEMENTED_H
