//
// Created by henri on 23/11/22.
//

#ifndef IDOL_VARATTRIBUTES_H
#define IDOL_VARATTRIBUTES_H

#include "TempVar.h"
#include "modeling/objects/ObjectAttributes.h"

class VarAttributes : public TempVar, public ObjectAttributes<Var> {
public:
    VarAttributes() = default;
    VarAttributes(unsigned int t_id, unsigned int t_index, TempVar&& t_temp_var)
        : TempVar(std::move(t_temp_var)), ObjectAttributes<Var>(t_id, t_index) {}

    VarAttributes(const VarAttributes&) = default;
    VarAttributes(VarAttributes&&) noexcept = default;

    VarAttributes& operator=(const VarAttributes&) = default;
    VarAttributes& operator=(VarAttributes&&) noexcept = default;
};

#endif //IDOL_VARATTRIBUTES_H
