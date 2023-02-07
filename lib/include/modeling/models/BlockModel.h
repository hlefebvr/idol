//
// Created by henri on 07/02/23.
//

#ifndef IDOL_BLOCKMODEL_H
#define IDOL_BLOCKMODEL_H

#include "Model.h"

static const unsigned int MasterId = std::numeric_limits<unsigned int>::max();

template<class AxisT>
class BlockModel : public AbstractModel {
public:
    static_assert(std::is_same_v<AxisT, Var> || std::is_same_v<AxisT, Ctr>);

    using OppositeAxisT = std::conditional_t<std::is_same_v<AxisT, Var>, Ctr, Var>;
    using GenerationPatternT = std::conditional_t<std::is_same_v<AxisT, Ctr>, Column, Row>;

    class Block;
private:
    Model m_master;
    std::vector<Block> m_blocks;

    std::optional<Annotation<AxisT, unsigned int>> m_axis_annotation;
    std::optional<Annotation<OppositeAxisT, unsigned int>> m_opposite_axis_annotation;
protected:
    Annotation<AxisT, unsigned int> annotation(const AxisT&) const { return m_axis_annotation.value(); }
    Annotation<OppositeAxisT, unsigned int> annotation(const OppositeAxisT&) const { return m_opposite_axis_annotation.value(); }

    Model& model(const Var& t_var);
    [[nodiscard]] const Model& model(const Var& t_var) const { return const_cast<BlockModel<AxisT>*>(this)->model(t_var); }

    Model& model(const Ctr& t_ctr);
    [[nodiscard]] const Model& model(const Ctr& t_ctr) const { return const_cast<BlockModel<AxisT>*>(this)->model(t_ctr); }

    // Attribute delegate
    AttributeManager &attribute_delegate(const Attribute &t_attribute) override { return m_master; }
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override { return model(t_object); }
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override { return model(t_object); }
    // Parameter delegate
    AttributeManager &parameter_delegate(const Parameter<double> &t_param) override { return m_master; }
    AttributeManager &parameter_delegate(const Parameter<int> &t_param) override { return m_master; }
    AttributeManager &parameter_delegate(const Parameter<bool> &t_param) override { return m_master; }
public:
    BlockModel(Env& t_env, unsigned int t_n_blocks);

    BlockModel(Env& t_env, unsigned int t_n_blocks, Annotation<AxisT, unsigned int> t_axis_annotation);

    void add(const Var &t_var) override { m_master.add(t_var); }

    [[nodiscard]] bool has(const Var &t_var) const override { return model(t_var).has(t_var); }

    void remove(const Var &t_var) override { model(t_var).remove(t_var); }

    void add(const Ctr &t_ctr) override { m_master.add(t_ctr); }

    [[nodiscard]] bool has(const Ctr &t_ctr) const override { return model(t_ctr).has(t_ctr); }

    void remove(const Ctr &t_ctr) override { model(t_ctr).remove(t_ctr); }

    BlockModel<AxisT> *clone() const override { throw Exception("Not implemented."); }

    [[nodiscard]] unsigned int id() const override { return m_master.id(); }

    [[nodiscard]] ConstIteratorForward<std::vector<Var>> vars() const override { return m_master.vars(); }

    [[nodiscard]] ConstIteratorForward<std::vector<Ctr>> ctrs() const override { return m_master.ctrs(); }

    [[nodiscard]] Env &env() const override { return m_master.env(); }

    void build_opposite_axis();

    [[nodiscard]] unsigned int n_blocks() const { return m_blocks.size(); }

    auto blocks() { return IteratorForward(m_blocks); }

    auto blocks() const { return ConstIteratorForward(m_blocks); }

    auto& master() { return m_master; }

    const auto& master() const { return m_master; }

    auto& block(unsigned int t_index) { return m_blocks[t_index]; }

    const auto& block(unsigned int t_index) const { return m_blocks[t_index]; }

    const auto& axis() const { return m_axis_annotation.value(); }

    const auto& opposite_axis() const { return m_opposite_axis_annotation.value(); }

    using AbstractModel::get;
    using AbstractModel::set;
};

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
BlockModel<AxisT>::BlockModel(Env &t_env, unsigned int t_n_blocks, Annotation<AxisT, unsigned int> t_axis_annotation)
    : m_master(t_env), m_axis_annotation(std::move(t_axis_annotation)) {

    m_blocks.reserve(t_n_blocks);
    for (unsigned int i = 0 ; i < t_n_blocks ; ++i) {
        m_blocks.emplace_back(t_env);
    }

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
class BlockModel<AxisT>::Block {
    Model m_model;
    GenerationPatternT m_generation_pattern;
    std::optional<AxisT> m_aggregator;
public:
    explicit Block(Env& t_env) : m_model(t_env) {}

    const AxisT& aggregator() const { return m_aggregator.value(); }

    [[nodiscard]] bool has_aggregator() const { return m_aggregator.has_value(); }

    void set_aggregator(const AxisT& t_aggregator) { m_aggregator = t_aggregator; }

    void reset_aggregator() { m_aggregator.reset(); }

    GenerationPatternT& generation_pattern() { return m_generation_pattern; }

    const GenerationPatternT& generation_pattern() const { return m_generation_pattern; }

    [[nodiscard]] const auto& model() const { return m_model; }

    auto& model() { return m_model; }
};

template<class AxisT>
std::ostream& operator<<(std::ostream& t_os, const BlockModel<AxisT>& t_model) {
    t_os << "Master:\n" << t_model.master() << '\n';
    for (unsigned int i = 0, n = t_model.n_blocks() ; i < n ; ++i) {
        const auto& block = t_model.block(i);
        t_os << "Block " << i << ":\n" << block.model() << "\nGeneration pattern " << i <<":\n" << block.generation_pattern() << '\n';
    }
    return t_os;
}

#endif //IDOL_BLOCKMODEL_H
