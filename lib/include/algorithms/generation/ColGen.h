//
// Created by henri on 13/12/22.
//

#ifndef IDOL_COLGEN_H
#define IDOL_COLGEN_H

#include "Generation.h"
#include "ColGenSP.h"

class ColGen : public Generation<ColGenSP> {
protected:
    void analyze_master_problem_solution() override;
    void solve_subproblems() override;
    void analyze_subproblems_solution() override;
public:
    explicit ColGen(Algorithm& t_rmp_solution_strategy);
};

#endif //IDOL_COLGEN_H
