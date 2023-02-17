//
// Created by henri on 20/12/22.
//

#ifndef IDOL_CALLBACKS_PLOTOPTIMALITYGAP_H
#define IDOL_CALLBACKS_PLOTOPTIMALITYGAP_H

#ifdef IDOL_USE_ROOT

#include "algorithms/dantzig-wolfe/DantzigWolfe.h"
#include "algorithms/callbacks/Algorithm_Events.h"
#include "UserCallback.h"

#include <TApplication.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TLegend.h>
#include <TCanvas.h>
#include <TAxis.h>
#include <TSystem.h>

namespace Callbacks {
    class PlotOptimalityGap;
}

class Callbacks::PlotOptimalityGap : public UserCallback<Algorithm> {

    static unsigned int s_id;

    std::string m_filename;
    unsigned int m_id = ++s_id;

    std::unique_ptr<TCanvas> m_canvas;
    std::unique_ptr<TMultiGraph> m_graph;
    std::unique_ptr<TGraph> m_best_upper_bound;
    std::unique_ptr<TGraph> m_best_lower_bound;
    std::unique_ptr<TGraph> m_iter_lower_bound;
    std::unique_ptr<TGraph> m_iter_upper_bound;
    std::unique_ptr<TLegend> m_legend;

    unsigned int m_number_of_points = 0;
    unsigned int m_number_of_rendered_plots = 0;
    unsigned int m_max_number_of_rendered_plots = 1;

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

        if (m_canvas) {

            m_best_lower_bound = std::unique_ptr<TGraph>();
            m_best_upper_bound = std::unique_ptr<TGraph>();
            m_iter_lower_bound = std::unique_ptr<TGraph>();
            m_iter_upper_bound = std::unique_ptr<TGraph>();

            m_canvas->Close();
        }

        std::string canvas_name = "c" + std::to_string(m_id);
        m_canvas = std::make_unique<TCanvas>(canvas_name.c_str(), "Dantzig-Wolfe optimality gap");
        m_graph = std::make_unique<TMultiGraph>();

        m_canvas->SetWindowSize(800, 400);

        m_iter_upper_bound = std::make_unique<TGraph>();
        m_best_upper_bound = std::make_unique<TGraph>();
        m_iter_lower_bound = std::make_unique<TGraph>();
        m_best_lower_bound = std::make_unique<TGraph>();
        m_legend = std::make_unique<TLegend>();

        initialize(m_iter_upper_bound.get(), "Iter UB", 1, t_min, t_max);
        initialize(m_best_upper_bound.get(), "Best UB", 2, t_min, t_max);
        initialize(m_iter_lower_bound.get(), "Iter LB", 3, t_min, t_max);
        initialize(m_best_lower_bound.get(), "Best LB", 4, t_min, t_max);

        m_graph->Draw("L");
        m_legend->Draw();
    }
public:
    explicit PlotOptimalityGap(std::string t_filename = "") : m_filename(std::move(t_filename)) {}

    void execute(const EventType& t_event) override {

        if (m_number_of_rendered_plots >= m_max_number_of_rendered_plots) {
            return;
        }

        if (t_event == Event_::Algorithm::Begin) {
            m_number_of_points = 0;
            return;
        }

        if (t_event == Event_::Algorithm::End) {

            if (m_number_of_points > 0 && !m_filename.empty()) {
                m_canvas->Print(m_filename.c_str());
            }

            m_number_of_points = 0;
            ++m_number_of_rendered_plots;

            return;
        }

        if (t_event != Event_::Algorithm::NewBestLb
            && t_event != Event_::Algorithm::NewBestUb
            && t_event != Event_::Algorithm::NewIterLb
            && t_event != Event_::Algorithm::NewIterUb) {
            return;
        }

        const auto& parent = this->parent();

        if (parent.get(Attr::Algorithm::RelativeGap) > 1) {
            return;
        }

        const double t = parent.time().count(Timer::Milliseconds);
        const double best_ub = parent.get(Attr::Algorithm::BestUb);
        const double best_lb = parent.get(Attr::Algorithm::BestLb);
        const double iter_ub = parent.get(Attr::Algorithm::IterUb);
        const double iter_lb = parent.get(Attr::Algorithm::IterLb);

        if (is_pos_inf(best_ub) || is_neg_inf(best_lb)) {
            return;
        }

        if (m_number_of_points == 0) {
            initialize(best_lb, best_ub);
        }

        ++m_number_of_points;

        m_best_upper_bound->AddPoint(t, best_ub);
        m_best_lower_bound->AddPoint(t, best_lb);
        m_iter_lower_bound->AddPoint(t, iter_lb);
        m_iter_upper_bound->AddPoint(t, iter_ub);

        m_canvas->Update();
    }

};

#endif

#endif //IDOL_CALLBACKS_PLOTOPTIMALITYGAP_H
