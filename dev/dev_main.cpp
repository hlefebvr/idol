#include <iostream>
#include <utility>
#include "modeling.h"
#include "algorithms.h"
#include "problems/GAP/GAP_Instance.h"
#include "reformulations/Reformulations_DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnMaster.h"
#include "algorithms/dantzig-wolfe/BranchingManagers_OnPricing.h"
#include "algorithms/dantzig-wolfe/Attributes_DantzigWolfe.h"

#include <TApplication.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TAxis.h>

class DantzigWolfePlot : public DantzigWolfe::Callback {
    std::string m_filename;

    std::unique_ptr<TCanvas> m_canvas;
    std::unique_ptr<TMultiGraph> m_graph;
    std::unique_ptr<TGraph> m_best_upper_bound;
    std::unique_ptr<TGraph> m_best_lower_bound;
    std::unique_ptr<TGraph> m_last_lower_bound;
    std::unique_ptr<TLegend> m_legend;

    unsigned int m_n_points = 0;

    void initialize(TGraph* t_graph, const std::string& t_title, short int t_color, double t_min, double t_max) {

        t_graph->SetLineColor(t_color);
        t_graph->SetMarkerColor(t_color);

        t_graph->SetMinimum(t_min);
        t_graph->SetMaximum(t_max);

        t_graph->Draw();

        m_legend->AddEntry(t_graph, t_title.c_str());

        m_graph->Add(t_graph);
    }

    void initialize(double t_min, double t_max) {

        m_canvas = std::make_unique<TCanvas>("c1", "Dantzig-Wolfe optimality gap");
        m_graph = std::make_unique<TMultiGraph>();
        m_best_upper_bound = std::make_unique<TGraph>();
        m_last_lower_bound = std::make_unique<TGraph>();
        m_best_lower_bound = std::make_unique<TGraph>();
        m_legend = std::make_unique<TLegend>();

        m_canvas->SetWindowSize(800, 400);

        initialize(m_best_upper_bound.get(), "Best UB", 2, t_min, t_max);
        initialize(m_last_lower_bound.get(), "Last LB", 3, t_min, t_max);
        initialize(m_best_lower_bound.get(), "Best LB", 4, t_min, t_max);

        m_graph->Draw("L");
        m_legend->Draw();
    }
public:
    explicit DantzigWolfePlot(std::string t_filename = "") : m_filename(std::move(t_filename)) {}

    void execute(Context &t_ctx) override {

        const auto event = t_ctx.event();

        if (event == DantzigWolfe::End) {

            if (m_n_points > 0 && !m_filename.empty()) {
                m_canvas->Print(m_filename.c_str());
            }

            m_n_points = 0;

            return;
        }

        const auto& parent = t_ctx.parent();

        if (parent.status() != Optimal) {
            return;
        }

        if (parent.get(Attr::DantzigWolfe::RelativeGap) > 1) {
            return;
        }

        const double t = parent.time().count(Timer::Milliseconds);
        const double best_ub = parent.get(Attr::DantzigWolfe::BestUb);
        const double best_lb = parent.get(Attr::DantzigWolfe::BestLb);
        const double last_lb = parent.get(Attr::DantzigWolfe::LastLb);

        if (m_n_points == 0) {
            initialize(best_lb, best_ub);
        }

        ++m_n_points;

        m_best_upper_bound->AddPoint(t, best_ub);
        m_best_lower_bound->AddPoint(t, best_lb);
        m_last_lower_bound->AddPoint(t, last_lb);

        m_canvas->Update();
    }

};

int main(int t_argc, char** t_argv) {

    TApplication app("app", &t_argc, t_argv);

    using namespace Problems::GAP;

    //auto instance = read_instance("/home/henri/CLionProjects/optimize/examples/ex2_branch_and_price_gap/demo.txt");
    //auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/tiny/instance_n2_30__2.txt");
    auto instance = read_instance("/home/henri/CLionProjects/idol_benchmark/GAP/data/small/instance_n3_40__0.txt");

    const unsigned int n_knapsacks = instance.n_agents();
    const unsigned int n_items = instance.n_jobs();

    Model model;
    auto complicating_constraint = model.add_user_attr<unsigned int>(0, "complicating_constraint");

    // Variables
    auto x = model.add_vars(Dim<2>(n_knapsacks, n_items), 0., 1., Binary, 0., "x");

    // Objective function
    Expr objective = idol_Sum(
                i, Range(n_knapsacks),
                idol_Sum(j, Range(n_items), instance.cost(i, j) * x[i][j])
            );
    model.set(Attr::Obj::Expr, objective);

    // Knapsack constraints
    for (unsigned int i = 0 ; i < n_knapsacks ; ++i) {
        auto ctr = model.add_ctr( idol_Sum(j, Range(n_items), instance.resource_consumption(i, j) * x[i][j]) <= instance.capacity(i) );
        model.set<unsigned int>(complicating_constraint, ctr, i+1);
    }

    // Assignment constraints
    for (unsigned int j = 0 ; j < n_items ; ++j) {
        model.add_ctr(idol_Sum(i, Range(n_knapsacks), x[i][j]) == 1);
    }

    Solvers::Gurobi gurobi(model);
    gurobi.solve();

    std::cout << gurobi.get(Attr::Solution::ObjVal) << std::endl;

    // DW reformulation
    Reformulations::DantzigWolfe result(model, complicating_constraint);

    Logs::set_level<BranchAndBound>(Trace);
    Logs::set_color<BranchAndBound>(Blue);

    Logs::set_level<DantzigWolfe>(Info);
    Logs::set_color<DantzigWolfe>(Yellow);

    BranchAndBound solver;

    //solver.set(Param::BranchAndBound::NodeSelection, NodeSelections::DepthFirst);
    //solver.set(Param::Algorithm::MaxIterations, 10);

    auto& node_strategy = solver.set_node_strategy<NodeStrategies::Basic<Nodes::Basic>>();
    node_strategy.set_active_node_manager_strategy<ActiveNodesManagers::Basic>();
    node_strategy.set_branching_strategy<BranchingStrategies::MostInfeasible>(flatten<Var, 2>(x));
    node_strategy.set_node_updator_strategy<NodeUpdators::ByBoundVar>();

    auto& dantzig_wolfe = solver.set_solution_strategy<DantzigWolfe>(model, complicating_constraint);

    dantzig_wolfe.set_callback<DantzigWolfePlot>("graph.pdf");

    dantzig_wolfe.set(Param::DantzigWolfe::CleanUpThreshold, 1500);
    dantzig_wolfe.set(Param::DantzigWolfe::SmoothingFactor, 0);
    dantzig_wolfe.set(Param::DantzigWolfe::FarkasPricing, false);
    dantzig_wolfe.set(Param::DantzigWolfe::LogFrequency, 1);

    auto& master = dantzig_wolfe.set_master_solution_strategy<Solvers::Gurobi>();
    master.set(Param::Algorithm::InfeasibleOrUnboundedInfo, true);

    for (unsigned int i = 1 ; i <= n_knapsacks ; ++i) {
        dantzig_wolfe.subproblem(i).set_exact_solution_strategy<Solvers::Gurobi>();
        dantzig_wolfe.subproblem(i).set_branching_manager<BranchingManagers::OnPricing>();
    }

    solver.set(Param::Algorithm::TimeLimit, 600);
    solver.set(Param::Algorithm::MaxIterations, 1);

    try {
        solver.solve();
    } catch (const GRBException& err) {
        std::cout << err.getErrorCode() << ": " << err.getMessage() << std::endl;
    }

    std::cout << solver.get(Attr::Solution::ObjVal) << std::endl;
    std::cout << solver.time().count() << std::endl;

    app.Run();

    return 0;
}
