#include <catch2/catch_test_macros.hpp>

#include "idol/mixed-integer/optimizers/branch-and-bound/BranchAndBound.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/node-selection-rules/factories/BestEstimate.h"
#include "idol/mixed-integer/optimizers/branch-and-bound/branching-rules/factories/StrongBranching.h"
#include "idol/mixed-integer/optimizers/callbacks/watchers/ExportBranchAndBoundTreeToCSV.h"

template class idol::NodeSelectionRules::BestEstimate<idol::DefaultNodeInfo>;
template class idol::BranchingRules::StrongBranching<idol::DefaultNodeInfo>;
template class idol::Utils::ExportBranchAndBoundTreeToCSV<idol::DefaultNodeInfo>;

TEST_CASE("Default node information supports built-in objective consumers", "[branch-and-bound][default-node-info]") {

    idol::BranchAndBound<> branch_and_bound;
    branch_and_bound.with_node_selection_rule(idol::BestEstimate());
    branch_and_bound.with_branching_rule(idol::StrongBranching());
    branch_and_bound.add_callback(idol::Utils::ExportBranchAndBoundTreeToCSV<>("branch-and-bound-tree.csv"));

    SUCCEED();
}
