//
// Created by henri on 07/02/23.
//
#include "modeling/models/BlockModel.h"
#include "modeling/objects/Env.h"

template<class AxisT>
BlockModel<AxisT>::BlockModel(Env &t_env, unsigned int t_n_blocks, Annotation<AxisT, unsigned int> t_axis_annotation)
        : m_master(t_env), m_axis_annotation(std::move(t_axis_annotation)) {

    m_blocks.reserve(t_n_blocks);
    for (unsigned int i = 0 ; i < t_n_blocks ; ++i) {
        m_blocks.emplace_back(t_env);
    }

}

template<class AxisT>
Model &BlockModel<AxisT>::model(const Ctr &t_ctr) {
    const unsigned int block_id = t_ctr.get(annotation(t_ctr));
    return block_id == MasterId ? m_master : block(block_id).model();
}

template<class AxisT>
Model &BlockModel<AxisT>::model(const Var &t_var) {
    const unsigned int block_id = t_var.get(annotation(t_var));
    return block_id == MasterId ? m_master : block(block_id).model();
}

template<class AxisT>
BlockModel<AxisT>::BlockModel(Env &t_env, unsigned int t_n_blocks)
        : BlockModel(t_env, t_n_blocks, Annotation<AxisT, unsigned int>(t_env, "_decomposition")) {

}

template<class AxisT>
void BlockModel<AxisT>::build_opposite_axis() {

    if (m_opposite_axis_annotation.has_value()) {
        throw Exception("Opposite axis has already been built.");
    }

    m_opposite_axis_annotation.emplace(
            Annotation<OppositeAxisT, unsigned int>::make_with_default_value(
                    env(),
                    m_axis_annotation->name() + "_opposite",
                    MasterId
            )
    );

    for (unsigned int i = 0, n = n_blocks() ; i < n ; ++i) {

        if constexpr (std::is_same_v<AxisT, Var>) {

            for (const auto &ctr: block(i).model().ctrs()) {
                ctr.set(m_opposite_axis_annotation.value(), i);
            }

        } else {

            for (const auto &var : block(i).model().vars()) {
                var.set(m_opposite_axis_annotation.value(), i);
            }

        }

    }

}

template<class AxisT>
int BlockModel<AxisT>::get(const Req<int, void> &t_attr) const {

    if (t_attr == Attr::Obj::Sense) {
        return m_master.get(t_attr);
    }

    return Delegate::get(t_attr);
}

template<class AxisT>
void BlockModel<AxisT>::set(const Req<double, Var> &t_attr, const Var &t_var, double t_value) {

    if (has_backend()) {
        backend().set(t_attr, t_var, t_value);
    }

    model(t_var).set(t_attr, t_var, t_value);

}
