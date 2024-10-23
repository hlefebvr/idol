//
// Created by henri on 23.10.24.
//

#ifndef IDOL_PLOTS_OPTIMALITYGAP_H
#define IDOL_PLOTS_OPTIMALITYGAP_H

#include "idol/optimizers/mixed-integer-optimization/callbacks/CallbackFactory.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/Callback.h"
#include "idol/optimizers/mixed-integer-optimization/callbacks/watchers/PlotManager.h"

namespace idol::Plots {
    class OptimalityGap;
}

class TCanvas;
class TGraph;
class TLegend;

class idol::Plots::OptimalityGap : public CallbackFactory {
    Manager& m_plot_manager;
public:
    class Strategy : public Callback {
        Manager& m_plot_manager;
        TCanvas* m_canvas;
        TGraph* m_graph_bound;
        TGraph* m_graph_obj;
        TLegend* m_legend;
        int m_n_points_bound = 0;
        int m_n_points_obj = 0;
        std::optional<double> m_worst_bound;
        std::optional<double> m_worst_obj;
        std::optional<double> m_best_obj;
        std::optional<double> m_best_bound;
    protected:
        void operator()(CallbackEvent t_event) override;
    public:
        explicit Strategy(Manager& t_plot_manager);
    };

    explicit OptimalityGap(Manager& t_plot_manager) : m_plot_manager(t_plot_manager) {}

    Callback *operator()() override {
        return new Strategy(m_plot_manager);
    }

    [[nodiscard]] CallbackFactory *clone() const override {
        return new OptimalityGap(*this);
    }
};

#endif //IDOL_PLOTS_OPTIMALITYGAP_H
