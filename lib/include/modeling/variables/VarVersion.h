//
// Created by henri on 27/01/23.
//

#ifndef IDOL_VARVERSION_H
#define IDOL_VARVERSION_H

#include "modeling/matrix/Column.h"
#include "modeling/objects/Version.h"
#include "TempVar.h"

class VarVersion : public Version, public TempVar {
public:
    VarVersion(unsigned int t_index, TempVar&& t_temp_var) : Version(t_index), TempVar(std::move(t_temp_var)) {}
};

#endif //IDOL_VARVERSION_H
