//
// Created by henri on 07/02/23.
//

#ifndef IDOL_BLOCKMODEL_H
#define IDOL_BLOCKMODEL_H

#include "Model.h"

class BlockModel : public Model {
public:
    explicit BlockModel(Env& t_env);

    using Model::get;
    using Model::set;
};

#endif //IDOL_BLOCKMODEL_H
