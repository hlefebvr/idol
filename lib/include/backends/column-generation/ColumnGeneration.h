//
// Created by henri on 07/02/23.
//

#ifndef IDOL_COLUMNGENERATION_H
#define IDOL_COLUMNGENERATION_H

#include "../Algorithm.h"
#include "modeling/models/BlockModel.h"

class ColumnGeneration : public Algorithm {
protected:
    void initialize() override;

    void add(const Var &t_var) override;

    void add(const Ctr &t_ctr) override;

    void remove(const Var &t_var) override;

    void remove(const Ctr &t_ctr) override;

    void update() override;

    void write(const std::string &t_name) override;

    void hook_optimize() override;
public:
    explicit ColumnGeneration(const BlockModel<Ctr>& t_model);
};

#endif //IDOL_COLUMNGENERATION_H
