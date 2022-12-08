//
// Created by henri on 23/11/22.
//

#ifndef IDOL_CTRATTRIBUTES_H
#define IDOL_CTRATTRIBUTES_H

#include "TempCtr.h"
#include "modeling/objects/ObjectAttributesWithUserAttr.h"

class CtrAttributes : public TempCtr, public ObjectAttributesWithUserAttr<Ctr> {
public:
    CtrAttributes() = default;
    CtrAttributes(unsigned int t_id, unsigned int t_index, TempCtr&& t_temp_var)
            : TempCtr(std::move(t_temp_var)), ObjectAttributesWithUserAttr<Ctr>(t_id, t_index) {}

    CtrAttributes(const CtrAttributes&) = default;
    CtrAttributes(CtrAttributes&&) noexcept = default;

    CtrAttributes& operator=(const CtrAttributes&) = default;
    CtrAttributes& operator=(CtrAttributes&&) noexcept = default;
};

#endif //IDOL_CTRATTRIBUTES_H
