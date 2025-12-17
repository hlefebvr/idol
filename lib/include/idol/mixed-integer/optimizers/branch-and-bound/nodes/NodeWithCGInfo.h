//
// Created by henri on 17.12.25.
//

#ifndef IDOL_NODEWITHCGINFO_H
#define IDOL_NODEWITHCGINFO_H

#include "DefaultNodeInfo.h"

namespace idol {
    class NodeWithCGInfo;
}

class idol::NodeWithCGInfo : public idol::DefaultNodeInfo {
    std::vector<std::vector<std::pair<double, PrimalPoint>>> m_active_columns;
public:
    NodeWithCGInfo() = default;

    void save(const Model& t_original_formulation, const Model& t_model) override;

    [[nodiscard]] NodeWithCGInfo* create_child() const override;

    using ActiveColumns = std::vector<std::vector<std::pair<double, PrimalPoint>>>;

    static const ActiveColumns& get_active_columns(const Model& t_model);

    static DefaultNodeUpdator<NodeWithCGInfo>* create_updator(const Model& t_src_model, Model& t_relaxation);
};

#endif //IDOL_NODEWITHCGINFO_H