//
// Created by henri on 27/01/23.
//

#ifndef IDOL_VARVERSION_H
#define IDOL_VARVERSION_H

#include "idol/mixed-integer/modeling/matrix/Column.h"
#include "idol/mixed-integer/modeling/objects/Version.h"
#include "TempVar.h"

namespace idol {
    class VarVersion;
}

class idol::VarVersion : public Version, public TempVar {
public:
    VarVersion(unsigned int t_index, double t_lb, double t_ub, VarType t_type, Column&& t_column) : Version(t_index), TempVar(t_lb, t_ub, t_type, std::move(t_column)) {}
    VarVersion(unsigned int t_index, TempVar&& t_temp_var) : Version(t_index), TempVar(std::move(t_temp_var)) {}
    VarVersion(unsigned int t_index, const TempVar& t_temp_var) : Version(t_index), TempVar(t_temp_var) {}
};

#endif //IDOL_VARVERSION_H
