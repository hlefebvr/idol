//
// Created by henri on 20.11.23.
//

#ifndef IDOL_KNAPSACKCOVER_H
#define IDOL_KNAPSACKCOVER_H

#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallbackFactory.h"
#include "idol/optimizers/branch-and-bound/callbacks/BranchAndBoundCallback.h"
#include "idol/optimizers/branch-and-bound/nodes/DefaultNodeInfo.h"

namespace idol::Cuts {
    template<class NodeInfoT> class KnapsackCover;
}

template<class NodeInfoT = idol::DefaultNodeInfo>
class idol::Cuts::KnapsackCover: public BranchAndBoundCallbackFactory<NodeInfoT> {
public:
    class Strategy;
    BranchAndBoundCallback<NodeInfoT> *operator()() override;
    BranchAndBoundCallbackFactory<NodeInfoT> *clone() const override;
};

template<class NodeInfoT>
class idol::Cuts::KnapsackCover<NodeInfoT>::Strategy : public BranchAndBoundCallback<NodeInfoT> {

    struct KnapsackStructure {

        struct Item {
            Var variable;
            double weight;
            double current_value = -1.;
            Item(const Var& t_var, double t_weight);

            Item(const Item&) = default;
            Item(Item&&) = default;

            Item& operator=(const Item&) = default;
            Item& operator=(Item&&) = default;
        };

        std::vector<Item> items;
        double capacity;
        KnapsackStructure(std::vector<Item> t_items, double t_capacity);
    };

    using ItemIterator = typename std::vector<typename KnapsackStructure::Item>::iterator;

    class SetOfItems {
        ItemIterator m_begin;
        ItemIterator m_end;
    public:
        SetOfItems(ItemIterator t_begin, ItemIterator t_end) : m_begin(t_begin), m_end(t_end) {}

        ItemIterator begin() const { return m_begin; }
        ItemIterator end() const { return m_end; }
        unsigned int size() const { return std::distance(m_begin, m_end); }

        static SetOfItems merge_consecutive(const SetOfItems& t_a, const SetOfItems& t_b);
    };

    double m_scaling_factor = 1e6;
    std::list<KnapsackStructure> m_knapsack_structures;
protected:
    // Detect structure
    void detect_knapsack_structure(const Ctr& t_ctr);
    void add_knapsack_structure(const Row& t_row, CtrType t_type);
    bool has_only_binary_variables(const Row& t_row);
    bool has_only_single_signed_coefficients(const Row& t_row);

    // Separation
    void separate_cut(const KnapsackStructure& t_knapsack_structure);
    std::tuple<SetOfItems, SetOfItems, SetOfItems> fix_variables_equal_to_one_or_zero(const SetOfItems& t_set_of_items);
    void set_current_values(const SetOfItems& t_set_of_items);
    double sum_of_weights(const SetOfItems& t_set_of_items);
    std::pair<SetOfItems, SetOfItems> compute_initial_cover(const SetOfItems& t_N_1, const SetOfItems& t_N_free, const SetOfItems& t_N_0, double t_capacity);
    std::pair<SetOfItems, SetOfItems> solve_knapsack_approximately(const SetOfItems& t_set_of_items, double t_capacity);
    std::pair<SetOfItems, SetOfItems> swap_items(const SetOfItems& t_a, const SetOfItems& t_b);
    std::pair<SetOfItems, SetOfItems> compute_minimal_cover(const SetOfItems& t_initial_cover, double t_capacity);
    std::pair<SetOfItems, SetOfItems> partition_minimal_cover(const SetOfItems& t_minimal_cover);
    std::pair<SetOfItems, SetOfItems> partition_remaining_items(const SetOfItems& t_remaining_items);
    void sort_by_non_decreasing_weights(const SetOfItems& t_set_of_items);
    void sort_by_non_increasing_wights(const SetOfItems& t_set_of_items);

    void debug(const std::string& t_name, const SetOfItems& t_set_of_items, bool t_with_values = false);

    void initialize() override;

    void operator()(CallbackEvent t_event) override;
};

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::debug(const std::string &t_name,
                                                           const SetOfItems &t_set_of_items,
                                                           bool t_with_values) {

    std::cout << t_name << ": ";
    for (const auto& item : t_set_of_items) {
        std::cout << item.variable;
        if (t_with_values) {
            std::cout << "(" << item.current_value << ")";
        }
        std::cout << ", ";
    }
    std::cout << std::endl;

}

template<class NodeInfoT>
typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems::merge_consecutive(const SetOfItems &t_a,
                                                                             const SetOfItems &t_b) {

    if (t_a.end() != t_b.begin()) {
        throw Exception("Trying to merge non-consecutive sets of items while calling merge_consecutive.");
    }

    return {
        t_a.begin(),
        t_b.end()
    };

}

template<class NodeInfoT>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::KnapsackStructure::KnapsackStructure(std::vector<Item> t_items, double t_capacity)
        : items(std::move(t_items)), capacity(t_capacity) {

}

template<class NodeInfoT>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::KnapsackStructure::Item::Item(const idol::Var &t_var, double t_weight)
        : variable(t_var), weight(t_weight) {

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::initialize() {
    BranchAndBoundCallback<NodeInfoT>::initialize();

    m_knapsack_structures.clear();

    const auto& model = this->original_model();

    for (const auto& ctr : this->original_model().ctrs()) {
        detect_knapsack_structure(ctr);
    }

    std::cout << "Lifted Minimal KnapsackStructure Cuts 1, " << m_knapsack_structures.size() << " candidates found" << std::endl;
}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::detect_knapsack_structure(const idol::Ctr &t_ctr) {

    const auto& model = this->original_model();
    const auto& row = model.get_ctr_row(t_ctr);

    if (!row.quadratic().empty()) {
        return;
    }

    if (!has_only_binary_variables(row)) {
        return;
    }

    if (!has_only_single_signed_coefficients(row)) {
        return;
    }

    const auto type = model.get_ctr_type(t_ctr);

    if (type == Equal) {
        return;
    }

    add_knapsack_structure(row, type);

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::add_knapsack_structure(const idol::Row &t_row, idol::CtrType t_type) {

    const unsigned int n_items = t_row.linear().size();
    const double factor = (t_type == LessOrEqual ? 1. : -1.) * m_scaling_factor;

    std::vector<typename KnapsackStructure::Item> items;
    items.reserve(n_items);

    for (const auto& [var, coefficient] : t_row.linear()) {
        items.emplace_back(var, factor * coefficient.as_numerical());
    }

    m_knapsack_structures.emplace_back(std::move(items), factor * t_row.rhs().as_numerical());

}

template<class NodeInfoT>
bool idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::has_only_binary_variables(const idol::Row &t_row) {

    const auto& model = this->original_model();

    for (const auto& [var, coefficient] : t_row.linear()) {

        if (model.get_var_type(var) != Binary) {
            return false;
        }

    }

    return true;
}

template<class NodeInfoT>
bool idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::has_only_single_signed_coefficients(const idol::Row &t_row) {

    std::optional<bool> are_all_non_negative;

    for (const auto& [var, coefficient] : t_row.linear()) {

        bool is_non_negative = coefficient.as_numerical() >= 0;

        if (!are_all_non_negative.has_value()) [[unlikely]] {
            are_all_non_negative = is_non_negative;
            continue;
        }

        if (is_non_negative != are_all_non_negative) {
            return false;
        }

    }

    return true;
}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::operator()(idol::CallbackEvent t_event) {

    if (t_event != InvalidSolution) {
        return;
    }

    for (const auto& knapsack_structure : m_knapsack_structures) {
        separate_cut(knapsack_structure);
    }

    if (this->node().level() == 0) {
        throw Exception("Manual stop");
    }

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::separate_cut(const idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::KnapsackStructure &t_knapsack_structure) {

    auto knapsack_structure = t_knapsack_structure;
    SetOfItems N(knapsack_structure.items.begin(), knapsack_structure.items.end());

    set_current_values(N);

    const auto [N_1, N_free, N_0] = fix_variables_equal_to_one_or_zero(N);
    const double sum_weights_of_variables_not_fixed_to_zero = sum_of_weights(N_1) + sum_of_weights(N_free);

    if (sum_weights_of_variables_not_fixed_to_zero < knapsack_structure.capacity + 1) {
        return;
    }

    // Initial cover
    auto [initial_cover, not_initial_cover] = compute_initial_cover(
                                                                N_1,
                                                                N_free,
                                                                N_0,
                                                                sum_weights_of_variables_not_fixed_to_zero - (knapsack_structure.capacity + 1)
                                                                );

    // Minimal cover and partition
    auto [C, others] = compute_minimal_cover(initial_cover, t_knapsack_structure.capacity);
    auto not_in_C = SetOfItems::merge_consecutive(others, not_initial_cover);
    auto [C1, C2] = partition_minimal_cover(C);

    // Lifting sequence and computing the lifting coefficients
    auto [F, R] = partition_remaining_items(not_in_C);
    sort_by_non_decreasing_weights(C1);
    sort_by_non_increasing_wights(C2);
    sort_by_non_increasing_wights(F);
    sort_by_non_increasing_wights(R);

    debug("not_in_C", not_in_C, true);
    debug("F", F, true);
    debug("R", R, true);

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::sort_by_non_increasing_wights(const SetOfItems &t_set_of_items) {

    std::sort(t_set_of_items.begin(), t_set_of_items.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.weight <= t_b.weight;
    });

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::sort_by_non_decreasing_weights(const SetOfItems &t_set_of_items) {

    std::sort(t_set_of_items.begin(), t_set_of_items.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.weight >= t_b.weight;
    });

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::set_current_values(const SetOfItems &t_set_of_items) {

    const auto& primal_solution = this->node().info().primal_solution();

    for (auto& item : t_set_of_items) {
        item.current_value = primal_solution.get(item.variable);
    }

}

template<class NodeInfoT>
std::tuple<
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
        >
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::fix_variables_equal_to_one_or_zero(const SetOfItems &t_set_of_items) {

    auto end_of_fixed_to_one = t_set_of_items.begin();
    auto begin_of_fixed_to_zero = t_set_of_items.end();

    for (auto it = end_of_fixed_to_one ; it != begin_of_fixed_to_zero && end_of_fixed_to_one != begin_of_fixed_to_zero ; ) {

        if (equals(it->current_value, 0., Tolerance::Integer)) {
            --begin_of_fixed_to_zero;
            std::iter_swap(it, begin_of_fixed_to_zero);
            continue;
        }

        if (equals(it->current_value, 1., Tolerance::Integer)) {
            std::iter_swap(it, end_of_fixed_to_one);
            ++end_of_fixed_to_one;
            ++it;
            continue;
        }

        ++it;

    }

    return {
            { t_set_of_items.begin(), end_of_fixed_to_one }, // fixed to 1
            { end_of_fixed_to_one, begin_of_fixed_to_zero }, // free
            { begin_of_fixed_to_zero, t_set_of_items.end() }, // fixed to 0
    };
}

template<class NodeInfoT>
double idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::sum_of_weights(const SetOfItems &t_set_of_items) {

    double result = 0.;

    for (auto it = t_set_of_items.begin(), end = t_set_of_items.end() ; it != end ; ++it) {
        result += it->weight;
    }

    return result;
}

template<class NodeInfoT>
std::pair<
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::compute_initial_cover(const SetOfItems &t_N_1,
                                                                     const SetOfItems &t_N_free,
                                                                     const SetOfItems &t_N_0,
                                                                     double t_capacity) {

    auto [ones, zeroes] = solve_knapsack_approximately(t_N_free, t_capacity);

    assert(ones.end() == zeroes.begin());

    auto [flipped_zeroes, flipped_ones] = swap_items(ones, zeroes);

    auto initial_cover = SetOfItems::merge_consecutive(t_N_1, flipped_zeroes);
    auto not_initial_cover = SetOfItems::merge_consecutive(flipped_ones, t_N_0);

    return { initial_cover, not_initial_cover };

}

template<class NodeInfoT>
std::pair<
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::partition_minimal_cover(const SetOfItems &t_minimal_cover) {

    auto end_of_C1 = t_minimal_cover.end();

    for (auto it = t_minimal_cover.begin() ; it != end_of_C1 ; ) {

        if (!equals(it->current_value, 1., Tolerance::Integer)) {
            ++it;
            continue;
        }

        --end_of_C1;
        std::iter_swap(it, end_of_C1);

    }

    return {
            { t_minimal_cover.begin(), end_of_C1 },
            { end_of_C1, t_minimal_cover.end() }
    };
}

template<class NodeInfoT>
std::pair<
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::partition_remaining_items(const SetOfItems &t_remaining_items) {

    auto end_of_F = t_remaining_items.end();

    for (auto it = t_remaining_items.begin() ; it != end_of_F ; ) {

        if (!equals(it->current_value, 0, Tolerance::Integer)) {
            ++it;
            continue;
        }

        --end_of_F;
        std::iter_swap(it, end_of_F);

    }

    return {
            { t_remaining_items.begin(), end_of_F },
            { end_of_F, t_remaining_items.end() },
    };
}

template<class NodeInfoT>
std::pair<
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
    >
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::solve_knapsack_approximately(const SetOfItems &t_set_of_items,
                                                                            double t_capacity) {

    std::sort(t_set_of_items.begin(),
              t_set_of_items.end(),
              [&](const auto& t_a, const auto& t_b) {

        const double score_a = (1. - t_a.current_value) / t_a.weight;
        const double score_b = (1. - t_b.current_value) / t_b.weight;

        return score_a >= score_b;

    });

    double weight_of_packed_items = 0.;
    auto it = t_set_of_items.begin();
    auto end = t_set_of_items.end();

    auto begin_zero = it;

    for ( ; it != end ; ++it) {

        if (weight_of_packed_items + it->weight > t_capacity) {
            // all remaining-s go to zero
            break;
        }

        // set to 1
        std::iter_swap(it, begin_zero);
        ++begin_zero;

        weight_of_packed_items += it->weight;

    }

    return {
            { t_set_of_items.begin(), begin_zero }, // ones
            { begin_zero, t_set_of_items.end() }, // zeroes
    };

}

template<class NodeInfoT>
std::pair<
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
        typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::swap_items(const SetOfItems &t_a,
                                                          const SetOfItems &t_b) {

    const auto min_size = std::min(t_a.size(), t_b.size());
    const auto max_size = std::max(t_a.size(), t_b.size());

    std::swap_ranges(t_a.begin(), t_a.begin() + min_size, t_a.begin() + max_size);

    ItemIterator split = t_a.begin() + t_b.size();

    return {
            { t_a.begin(), split },
            { split, t_b.end() }
    };
}

template<class NodeInfoT>
std::pair<
    typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems,
    typename idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::SetOfItems
>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::compute_minimal_cover(const SetOfItems &t_initial_cover,
                                                                     double t_capacity) {

    std::sort(t_initial_cover.begin(), t_initial_cover.end(), [](const auto& t_a, const auto& t_b) {

        const double score_a = (1. - t_a.current_value) / t_a.weight;
        const double score_b = (1. - t_b.current_value) / t_b.weight;

        if (equals(score_a, score_b, Tolerance::Sparsity)) {
            return t_a.weight <= t_b.weight;
        }

        return score_a >= score_b;
    });

    double sum_of_weights_in_C = sum_of_weights(t_initial_cover);
    auto end = t_initial_cover.end();

    for (auto it = t_initial_cover.begin() ; it != end ; ) {

        if (sum_of_weights_in_C - it->weight <= t_capacity) {
            ++it;
            continue;
        }

        sum_of_weights_in_C -= it->weight;
        --end;
        std::iter_swap(it, end);

    }

    return {
        { t_initial_cover.begin(), end },
        { end, t_initial_cover.end() },
    };

}

template<class NodeInfoT>
idol::BranchAndBoundCallbackFactory<NodeInfoT> *idol::Cuts::KnapsackCover<NodeInfoT>::clone() const {
    return new KnapsackCover<NodeInfoT>(*this);
}

template<class NodeInfoT>
idol::BranchAndBoundCallback<NodeInfoT> *idol::Cuts::KnapsackCover<NodeInfoT>::operator()() {
    return new Strategy();
}

#endif //IDOL_KNAPSACKCOVER_H
