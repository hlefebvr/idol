//
// Created by henri on 06/12/22.
//

#ifndef IDOL_UNSUPPORTEDREQUEST_H
#define IDOL_UNSUPPORTEDREQUEST_H

#include "Exception.h"
#include "modeling/attributes/Attributes.h"
#include "backends/parameters/Parameters.h"

class UnsupportedRequest : public Exception {
public:
    explicit UnsupportedRequest(const Attribute& t_attr)
        : Exception("The requested attribute is not supported: " + t_attr.name() + ".") {}
};

class UnsupportedParameter : public Exception {
public:
    template<class T> explicit UnsupportedParameter(const Parameter<T>& t_param)
            : Exception("The requested parameter is not supported: " + t_param.name() + ".") {}
};
#endif //IDOL_UNSUPPORTEDREQUEST_H
