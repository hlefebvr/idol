//
// Created by henri on 06/12/22.
//

#ifndef IDOL_ATTRIBUTEBADREQUEST_H
#define IDOL_ATTRIBUTEBADREQUEST_H

#include "Exception.h"
#include "modeling/models/Attributes.h"

class AttributeBadRequest : public Exception {
public:
    AttributeBadRequest(const Attribute& t_attr)
        : Exception("Bad request for attribute: " + t_attr.name()) {}
};

#endif //IDOL_ATTRIBUTEBADREQUEST_H
