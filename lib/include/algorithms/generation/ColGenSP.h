//
// Created by henri on 13/12/22.
//

#ifndef IDOL_COLGENSP_H
#define IDOL_COLGENSP_H

#include "SP.h"
#include "../../modeling/variables/TempVar.h"

class ColGenSP : public SP<Generation<ColGenSP>> {
    TempVar m_column_template;
public:
    ColGenSP(Generation<ColGenSP>& t_parent, const Var& t_var);

    void initialize() override;

    void update() override;

    void enrich_master_problem() override;
};

#endif //IDOL_COLGENSP_H
