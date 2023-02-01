//
// Created by henri on 06/12/22.
//

#ifndef IDOL_UNSUPPORTEDREQUEST_H
#define IDOL_UNSUPPORTEDREQUEST_H

#include "Exception.h"
#include "modeling/attributes/Attributes.h"

class UnsupportedRequest : public Exception {
public:
    explicit UnsupportedRequest(const Attribute& t_attr)
        : Exception("The requested attribute is not supported: " + t_attr.name() + ".") {}
};

#endif //IDOL_UNSUPPORTEDREQUEST_H
