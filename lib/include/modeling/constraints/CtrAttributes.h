//
// Created by henri on 23/11/22.
//

#ifndef IDOL_CTRATTRIBUTES_H
#define IDOL_CTRATTRIBUTES_H

#include "TempCtr.h"
#include "../user_attributes/UserAttr.h"
#include <any>

class CtrAttributes : public TempCtr, public ObjectAttributes<Ctr> {
    std::vector<std::optional<int>> m_annotations;
public:
    CtrAttributes() = default;
    CtrAttributes(unsigned int t_id, unsigned int t_index, TempCtr&& t_temp_var)
            : TempCtr(std::move(t_temp_var)), ObjectAttributes<Ctr>(t_id, t_index) {}

    CtrAttributes(const CtrAttributes&) = default;
    CtrAttributes(CtrAttributes&&) noexcept = default;

    CtrAttributes& operator=(const CtrAttributes&) = default;
    CtrAttributes& operator=(CtrAttributes&&) noexcept = default;

    void set_user_attribute(const UserAttr& t_annotation, int t_value) {
        if (m_annotations.size() >= t_annotation.index()) {
            m_annotations.resize(t_annotation.index()+1);
        }
        m_annotations.at(t_annotation.index()) = t_value;
    }

    int get_user_attribute(const UserAttr& t_annotation) const {
        if (t_annotation.index() >= m_annotations.size()) {
            return t_annotation.default_value();
        }
        return m_annotations.at(t_annotation.index()).value_or(t_annotation.default_value());
    }
};

#endif //IDOL_CTRATTRIBUTES_H
