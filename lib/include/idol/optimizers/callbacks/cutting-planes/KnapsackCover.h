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
    std::optional<bool> m_lifting;
public:
    class Strategy;
    BranchAndBoundCallback<NodeInfoT> *operator()() override;
    BranchAndBoundCallbackFactory<NodeInfoT> *clone() const override;

    KnapsackCover& with_lifting(bool t_value);
};

template<class NodeInfoT>
idol::Cuts::KnapsackCover<NodeInfoT> &idol::Cuts::KnapsackCover<NodeInfoT>::with_lifting(bool t_value) {

    if (m_lifting.has_value()) {
        throw Exception("Lifting has already been configured.");
    }

    m_lifting = t_value;

    return *this;
}

template<class NodeInfoT>
class idol::Cuts::KnapsackCover<NodeInfoT>::Strategy : public BranchAndBoundCallback<NodeInfoT> {

    struct KnapsackStructure {

        struct Item {
            Var variable;
            int weight;
            double current_value = -1.;
            std::optional<int> cut_coefficient;
            Item(const Var& t_var, int t_weight);

            Item(const Item&) = default;
            Item(Item&&) = default;

            Item& operator=(const Item&) = default;
            Item& operator=(Item&&) = default;
        };

        std::vector<Item> items;
        int capacity;
        KnapsackStructure(std::vector<Item> t_items, int t_capacity);
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

    std::list<KnapsackStructure> m_knapsack_structures;
    const bool m_lifting;
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
    int sum_of_weights(const SetOfItems& t_set_of_items);
    std::pair<SetOfItems, SetOfItems> compute_initial_cover(const SetOfItems& t_N_1, const SetOfItems& t_N_free, const SetOfItems& t_N_0, int t_capacity);
    std::pair<SetOfItems, SetOfItems> solve_knapsack_approximately(const SetOfItems& t_set_of_items, int t_capacity);
    std::pair<SetOfItems, SetOfItems> swap_items(const SetOfItems& t_a, const SetOfItems& t_b);
    std::pair<SetOfItems, SetOfItems> compute_minimal_cover(const SetOfItems& t_initial_cover, int t_capacity);
    std::pair<SetOfItems, SetOfItems> partition_minimal_cover(const SetOfItems& t_minimal_cover);
    std::pair<SetOfItems, SetOfItems> partition_remaining_items(const SetOfItems& t_remaining_items);
    void sort_by_non_decreasing_weights(const SetOfItems& t_set_of_items);
    void sort_by_non_increasing_weights(const SetOfItems& t_set_of_items);
    int sequential_up_and_down_lifting(const SetOfItems& t_C1, const SetOfItems& t_C2, const SetOfItems &t_F, const SetOfItems& t_R, int t_capacity);
    double compute_cut_activity(const SetOfItems& t_set_of_items, int t_right_hand_side);
    TempCtr create_cut(const SetOfItems& t_set_of_items, int t_right_hand_side);

    void debug(const std::string& t_name, const SetOfItems& t_set_of_items, bool t_with_values = false, bool t_with_cut = false);

    void initialize() override;

    void operator()(CallbackEvent t_event) override;
public:
    Strategy(bool t_use_lifting);
};

template<class NodeInfoT>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::Strategy(bool t_use_lifting) : m_lifting(t_use_lifting) {

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::debug(const std::string &t_name,
                                                           const SetOfItems &t_set_of_items,
                                                           bool t_with_values,
                                                           bool t_with_cut) {

    std::cout << t_name << ": ";
    for (const auto& item : t_set_of_items) {
        if (t_with_cut) {
            if (item.cut_coefficient.has_value()) {
                std::cout << item.cut_coefficient.value();
            } else {
                std::cout << "?";
            }
            std::cout << " ";
        }
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
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::KnapsackStructure::KnapsackStructure(std::vector<Item> t_items, int t_capacity)
        : items(std::move(t_items)), capacity(t_capacity) {

}

template<class NodeInfoT>
idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::KnapsackStructure::Item::Item(const idol::Var &t_var, int t_weight)
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
    const double factor = (t_type == LessOrEqual ? 1. : -1.);

    auto copy = t_row;
    copy.scale_to_integers(Tolerance::Digits);

    std::vector<typename KnapsackStructure::Item> items;
    items.reserve(n_items);

    for (const auto& [var, coefficient] : copy.linear()) {
        items.emplace_back(var, factor * coefficient.as_numerical());
    }

    m_knapsack_structures.emplace_back(std::move(items), factor * copy.rhs().as_numerical());

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

    if (this->node().level() > 0) {
        //throw Exception("Manual stop");
    }

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::separate_cut(const idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::KnapsackStructure &t_knapsack_structure) {

    auto knapsack_structure = t_knapsack_structure;
    SetOfItems N(knapsack_structure.items.begin(), knapsack_structure.items.end());

    set_current_values(N);

    const auto [N_1, N_free, N_0] = fix_variables_equal_to_one_or_zero(N);
    const int sum_weights_of_variables_not_fixed_to_zero = sum_of_weights(N_1) + sum_of_weights(N_free);

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

    // Minimal cover
    auto [C, others] = compute_minimal_cover(initial_cover, t_knapsack_structure.capacity);

    int rhs;

    if (m_lifting) {

        // Partition
        auto not_in_C = SetOfItems::merge_consecutive(others, not_initial_cover);
        auto [C1, C2] = partition_minimal_cover(C);

        // Lifting sequence and computing the lifting coefficients
        auto [F, R] = partition_remaining_items(not_in_C);
        sort_by_non_decreasing_weights(C1);
        sort_by_non_increasing_weights(C2);
        sort_by_non_increasing_weights(F);
        sort_by_non_increasing_weights(R);

        rhs = sequential_up_and_down_lifting(C1, C2, F, R, knapsack_structure.capacity);

    } else {

        rhs = C.size() - 1;
        for (auto& item : C) {
            item.cut_coefficient = 1.;
        }

    }

    const double activity = compute_cut_activity(N, rhs);

    if (activity < Tolerance::Feasibility) {
        return;
    }

    auto cut = create_cut(N, rhs);

    this->add_user_cut(cut);

}

template<class NodeInfoT>
idol::TempCtr idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::create_cut(const SetOfItems &t_set_of_items, int t_right_hand_side) {

    Expr result;

    for (const auto& item : t_set_of_items) {

        if (!item.cut_coefficient.has_value()) {
            continue;
        }

        result += item.cut_coefficient.value() * item.variable;

    }

    return result <= t_right_hand_side;
}

template<class NodeInfoT>
double idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::compute_cut_activity(const SetOfItems &t_set_of_items, int t_right_hand_side) {

    double result = 0.;

    for (const auto& item : t_set_of_items) {

        if (!item.cut_coefficient.has_value()) {
            continue;
        }

        result += item.cut_coefficient.value() * item.current_value;

    }

    return result - t_right_hand_side;
}

template<class NodeInfoT>
int idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::sequential_up_and_down_lifting(const SetOfItems &t_C1,
                                                                                    const SetOfItems &t_C2,
                                                                                    const SetOfItems &t_F,
                                                                                    const SetOfItems &t_R,
                                                                                    int t_capacity) {

    // Set coefficients of variables in C1 to one
    for (auto& item : t_C1) {
        item.cut_coefficient = 1;
    }

    const auto initialize_min_weights = [](const SetOfItems& t_C1) {

        const unsigned int n_C1 = t_C1.size();

        std::vector<int> result;
        result.reserve(n_C1 + 1);
        result.emplace_back(0);

        auto it = t_C1.begin();
        for (unsigned int w = 0 ; w < n_C1 ; ++w) {
            result.emplace_back(result[w] + it->weight);
            ++it;
        }

        return result;
    };

    auto min_weights = initialize_min_weights(t_C1);

    int alpha_0 = t_C1.size() - 1;
    int lift_rhs = alpha_0;

    int fixed_ones_weight = sum_of_weights(t_C2);

    assert(fixed_ones_weight >= 0);

    // Lifting items in F
    for (auto& item : t_F) {

        int z;
        if (t_capacity - fixed_ones_weight - item.weight < 0) { // knapsack is infeasible
            z = 0;
        } else if (min_weights[alpha_0] <= t_capacity - fixed_ones_weight - item.weight) { // easy case
            z = lift_rhs;
        } else { // binary search for z

            int lb = 0;
            int ub = lift_rhs + 1;
            while (lb < ub - 1) {
                const int middle = (lb + ub) / 2;
                if ( min_weights[middle] <= t_capacity - fixed_ones_weight - item.weight ) {
                    lb = middle;
                } else {
                    ub = middle;
                }
            }

            assert(lb == ub - 1);
            assert(0 <= lb && lb < min_weights.size());
            assert(min_weights[lb] <= t_capacity - fixed_ones_weight - item.weight);
            assert(lb == min_weights.size() - 1 || min_weights[lb + 1] > t_capacity - fixed_ones_weight - item.weight);

            z = lb;

            assert(z <= lift_rhs);

        }

        const int alpha_j = alpha_0 - z;

        item.cut_coefficient = alpha_j;

        if (alpha_j == 0) {
            continue;
        }

        min_weights.resize(min_weights.size() + alpha_j, std::numeric_limits<int>::infinity());

        for (int w = min_weights.size() - 1 ; w >= 0 ; w--) {

            if (w < alpha_j ) {
                min_weights[w] = std::min(min_weights[w], item.weight);
            } else {
                min_weights[w] = std::min(min_weights[w], min_weights[w - alpha_j] + item.weight);
            }

        }

    }

    // Lifting in C2
    for (auto& item : t_C2) {

        int lb = 0;
        int ub = min_weights.size();
        while (lb < ub - 1) {
            int middle = (lb + ub) / 2;
            if ( min_weights[middle] <= t_capacity - fixed_ones_weight + item.weight ) {
                lb = middle;
            } else {
                ub = middle;
            }
        }
        int z = lb;

        const int alpha_j = z - alpha_0;

        item.cut_coefficient = alpha_j;

        alpha_0 += alpha_j;

        fixed_ones_weight -= item.weight;

        if (alpha_j == 0) {
            continue;
        }

        if (alpha_j > 0) {
            min_weights.resize(min_weights.size() + alpha_j, std::numeric_limits<int>::infinity());
        }

        for (int w = min_weights.size() - 1 ; w >= 0 ; w--) {

            if (w < alpha_j ) {
                min_weights[w] = std::min(min_weights[w], item.weight);
            } else {
                min_weights[w] = std::min(min_weights[w], min_weights[w - alpha_j] + item.weight);
            }

        }


    }

    assert(fixed_ones_weight == 0);

    // Lifting in R
    for (auto& item : t_R) {

        int z;
        if (min_weights[alpha_0] <= t_capacity - item.weight) {
            z = alpha_0;
        } else { // binary search for z

            int lb = 0;
            int ub = alpha_0 + 1;
            while (lb < ub - 1) {
                int middle = (lb + ub) / 2;
                if ( min_weights[middle] <= t_capacity - item.weight ) {
                    lb = middle;
                } else {
                    ub = middle;
                }
            }
            z = lb;

        }

        const int alpha_j = alpha_0 - z;

        item.cut_coefficient = alpha_j;

        if (alpha_j == 0) {
            continue;
        }

        for (int w = min_weights.size() - 1 ; w >= 0 ; w--) {

            if (w < alpha_j ) {
                min_weights[w] = std::min(min_weights[w], item.weight);
            } else {
                min_weights[w] = std::min(min_weights[w], min_weights[w - alpha_j] + item.weight);
            }

        }


    }

    return alpha_0;

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::sort_by_non_increasing_weights(const SetOfItems &t_set_of_items) {

    std::sort(t_set_of_items.begin(), t_set_of_items.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.weight >= t_b.weight;
    });

}

template<class NodeInfoT>
void idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::sort_by_non_decreasing_weights(const SetOfItems &t_set_of_items) {

    std::sort(t_set_of_items.begin(), t_set_of_items.end(), [](const auto& t_a, const auto& t_b) {
        return t_a.weight <= t_b.weight;
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
int idol::Cuts::KnapsackCover<NodeInfoT>::Strategy::sum_of_weights(const SetOfItems &t_set_of_items) {

    int result = 0.;

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
                                                                     int t_capacity) {

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
                                                                            int t_capacity) {

    std::sort(t_set_of_items.begin(),
              t_set_of_items.end(),
              [&](const auto& t_a, const auto& t_b) {

        const double score_a = (1. - t_a.current_value) / t_a.weight;
        const double score_b = (1. - t_b.current_value) / t_b.weight;

        return score_a >= score_b;

    });

    int weight_of_packed_items = 0.;
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
                                                                     int t_capacity) {

    std::sort(t_initial_cover.begin(), t_initial_cover.end(), [](const auto& t_a, const auto& t_b) {

        const double score_a = (1. - t_a.current_value) / t_a.weight;
        const double score_b = (1. - t_b.current_value) / t_b.weight;

        if (equals(score_a, score_b, Tolerance::Sparsity)) {
            return t_a.weight <= t_b.weight;
        }

        return score_a >= score_b;
    });

    int sum_of_weights_in_C = sum_of_weights(t_initial_cover);
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
    return new Strategy(m_lifting.has_value() ? m_lifting.value() : true);
}

#endif //IDOL_KNAPSACKCOVER_H
