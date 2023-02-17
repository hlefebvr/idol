//
// Created by henri on 07/02/23.
//

#ifndef IDOL_BLOCKMODEL_H
#define IDOL_BLOCKMODEL_H

#include "Model.h"
#include "backends/Backend.h"

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
    [[nodiscard]] const Annotation<AxisT, unsigned int>& annotation(const AxisT&) const { return m_axis_annotation.value(); }
    [[nodiscard]] const Annotation<OppositeAxisT, unsigned int>& annotation(const OppositeAxisT&) const { return m_opposite_axis_annotation.value(); }

    Model& model(const Var& t_var);
    [[nodiscard]] const Model& model(const Var& t_var) const { return const_cast<BlockModel<AxisT>*>(this)->model(t_var); }

    Model& model(const Ctr& t_ctr);
    [[nodiscard]] const Model& model(const Ctr& t_ctr) const { return const_cast<BlockModel<AxisT>*>(this)->model(t_ctr); }

    // Attribute delegate
    AttributeManager &attribute_delegate(const Attribute &t_attribute) override { return backend(); }
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Var &t_object) override { return backend(); }
    AttributeManager &attribute_delegate(const Attribute &t_attribute, const Ctr &t_object) override { return backend(); }
    // Parameter delegate
    AttributeManager &parameter_delegate(const Parameter<double> &t_param) override { return backend(); }
    AttributeManager &parameter_delegate(const Parameter<int> &t_param) override { return backend(); }
    AttributeManager &parameter_delegate(const Parameter<bool> &t_param) override { return backend(); }
public:
    BlockModel(Env& t_env, unsigned int t_n_blocks);
    BlockModel(Env& t_env, unsigned int t_n_blocks, Annotation<AxisT, unsigned int> t_axis_annotation);

    // Variables
    void add(const Var &t_var) override { m_master.add(t_var); }
    void add(const Var &t_var, TempVar&& t_temp_var) override { m_master.add(t_var, std::move(t_temp_var)); }
    [[nodiscard]] bool has(const Var &t_var) const override { return model(t_var).has(t_var); }
    void remove(const Var &t_var) override { model(t_var).remove(t_var); }
    [[nodiscard]] ConstIteratorForward<std::vector<Var>> vars() const override { return m_master.vars(); }

    // Constraints
    void add(const Ctr &t_ctr) override { m_master.add(t_ctr); }
    void add(const Ctr &t_ctr, TempCtr &&t_row) override { m_master.add(t_ctr, std::move(t_row)); }
    [[nodiscard]] bool has(const Ctr &t_ctr) const override { return model(t_ctr).has(t_ctr); }
    void remove(const Ctr &t_ctr) override { model(t_ctr).remove(t_ctr); }
    [[nodiscard]] ConstIteratorForward<std::vector<Ctr>> ctrs() const override { return m_master.ctrs(); }

    // Model
    [[nodiscard]] BlockModel<AxisT> *clone() const override { throw Exception("Not implemented."); }
    [[nodiscard]] unsigned int id() const override { return m_master.id(); }
    [[nodiscard]] Env &env() const override { return m_master.env(); }

    // Master
    auto& master() { return m_master; }
    [[nodiscard]] const auto& master() const { return m_master; }

    // Blocks
    [[nodiscard]] unsigned int n_blocks() const { return m_blocks.size(); }
    auto blocks() { return IteratorForward(m_blocks); }
    [[nodiscard]] auto blocks() const { return ConstIteratorForward(m_blocks); }
    auto& block(unsigned int t_index) { return m_blocks[t_index]; }
    [[nodiscard]] const auto& block(unsigned int t_index) const { return m_blocks[t_index]; }
    void build_opposite_axis();

    // Axis
    [[nodiscard]] const auto& axis() const { return m_axis_annotation.value(); }
    [[nodiscard]] const auto& opposite_axis() const { return m_opposite_axis_annotation.value(); }

    using AbstractModel::get;
    using AbstractModel::set;

    // Models' attributes
    [[nodiscard]] int get(const Req<int, void> &t_attr) const override;
    //[[nodiscard]] const Constant& get(const Req<Constant, void>& t_attr) const override;
    //[[nodiscard]] const Expr<Var, Var>& get(const Req<Expr<Var, Var>, void>& t_attr) const override;
    //[[nodiscard]] const LinExpr<Ctr>& get(const Req<LinExpr<Ctr>, void>& t_attr) const override;
    //[[nodiscard]] const Constant& get(const Req<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var) const override;
    //void set(const Req<int, void> &t_attr, int t_value) override;
    //void set(const Req<Expr<Var, Var>, void>& t_attr, Expr<Var, Var>&& t_value) override;
    //void set(const Req<LinExpr<Ctr>, void>& t_attr, LinExpr<Ctr>&& t_value) override;
    //void set(const Req<Constant, void> &t_attr, Constant &&t_value) override;
    //void set(const Req<Constant, Ctr, Var>& t_attr, const Ctr& t_ctr, const Var& t_var, Constant&& t_value) override;

    // Constraints' attributes
    //[[nodiscard]] int get(const Req<int, Ctr>& t_attr, const Ctr& t_ctr) const override;
    //[[nodiscard]] const Row& get(const Req<Row, Ctr>& t_attr, const Ctr& t_ctr) const override;
    //[[nodiscard]] const Constant& get(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr) const override;
    //void set(const Req<Constant, Ctr>& t_attr, const Ctr& t_ctr, Constant&& t_value) override;
    //void set(const Req<int, Ctr> &t_attr, const Ctr &t_ctr, int t_value) override;
    //void set(const Req<Row, Ctr> &t_attr, const Ctr &t_ctr, Row &&t_value) override;

    // Variables' attributes
    //[[nodiscard]] int get(const Req<int, Var>& t_attr, const Var& t_ctr) const override;
    //[[nodiscard]] double get(const Req<double, Var>& t_attr, const Var& t_var) const override;
    //[[nodiscard]] const Column& get(const Req<Column, Var>& t_attr, const Var& t_var) const override;
    //[[nodiscard]] const Constant& get(const Req<Constant, Var>& t_attr, const Var& t_var) const override;
    //void set(const Req<int, Var> &t_attr, const Var &t_var, int t_value) override;
    void set(const Req<double, Var>& t_attr, const Var& t_var, double t_value) override;
    //void set(const Req<Constant, Var> &t_attr, const Var &t_var, Constant &&t_value) override;
    //void set(const Req<Column, Var> &t_attr, const Var &t_var, Column &&t_value) override;
};

template class BlockModel<Ctr>;
template class BlockModel<Var>;

template<class AxisT>
class BlockModel<AxisT>::Block {
    Model m_model;
    GenerationPatternT m_generation_pattern;
    std::optional<AxisT> m_aggregator;
public:
    explicit Block(Env& t_env) : m_model(t_env) {}

    // Aggregators
    [[nodiscard]] const AxisT& aggregator() const { return m_aggregator.value(); }
    [[nodiscard]] bool has_aggregator() const { return m_aggregator.has_value(); }
    void set_aggregator(const AxisT& t_aggregator) { m_aggregator = t_aggregator; }
    void reset_aggregator() { m_aggregator.reset(); }

    // Generation patterns
    GenerationPatternT& generation_pattern() { return m_generation_pattern; }
    [[nodiscard]] const GenerationPatternT& generation_pattern() const { return m_generation_pattern; }

    // Model
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
