//
// Created by henri on 23.10.24.
//
#include "idol/optimizers/mixed-integer-optimization/callbacks/watchers/Plots_OptimalityGap.h"
#include "idol/optimizers/Timer.h"
#include "idol/containers/uuid.h"

#ifdef IDOL_USE_ROOT
#include <TCanvas.h>
#include <TGraph.h>
#include <TLegend.h>
#include <TAxis.h>

idol::Plots::OptimalityGap::Strategy::Strategy(idol::Plots::Manager &t_plot_manager)
        : m_plot_manager(t_plot_manager) {

}

void idol::Plots::OptimalityGap::Strategy::initialize() {

    m_canvas = m_plot_manager.create<TCanvas>(generate_uuid_v4().c_str(), "canvas", 800, 600);
    m_graph_bound = m_plot_manager.create<TGraph>();
    m_graph_obj = m_plot_manager.create<TGraph>();

    m_graph_bound->SetTitle("Optimality Gap over Time");

    m_graph_bound->SetLineColor(kBlue);
    m_graph_bound->SetMarkerStyle(20);
    m_graph_bound->SetMarkerSize(1.05);
    m_graph_bound->SetMarkerColor(kBlue);

    m_graph_obj->SetLineColor(kRed);
    m_graph_obj->SetMarkerStyle(20);
    m_graph_obj->SetMarkerSize(1.05);
    m_graph_obj->SetMarkerColor(kRed);

    m_canvas->cd();
}

void idol::Plots::OptimalityGap::Strategy::operator()(CallbackEvent t_event) {

    if (!m_canvas) {
        initialize();
    }

    const double t = time().count();
    const double bound = best_bound();
    const double obj = best_obj();

    m_canvas->cd();

    if (!is_neg_inf(bound)) {

        if (!m_worst_bound) {
            m_worst_bound = bound;
        }

        if (!m_best_bound || m_best_bound.value() < bound) {
            m_graph_bound->SetPoint(m_n_points_bound, t, bound);
            m_best_bound = bound;
            ++m_n_points_bound;
        } else {
            m_graph_bound->SetPoint(m_n_points_bound, t, bound);
        }

    }

    if (!is_pos_inf(obj)) {

        if (!m_worst_obj) {
            m_worst_obj = obj;
        }

        if (!m_best_obj || m_best_obj.value() > obj) {

            if (m_n_points_obj > 0) {
                m_graph_obj->SetPoint(m_n_points_obj, t, m_best_obj.value());
                ++m_n_points_obj;
            }

            m_graph_obj->SetPoint(m_n_points_obj, t, obj);
            m_best_obj = obj;
            ++m_n_points_obj;
        } else {
            m_graph_obj->SetPoint(m_n_points_obj, t, obj);
        }

    }

    m_canvas->Clear();

    if (m_n_points_bound > 0 && m_n_points_obj > 0) {
        m_graph_bound->Draw("ALP");
        m_graph_obj->Draw("LP SAME");
    } else if (m_n_points_bound > 0) {
        m_graph_bound->Draw("ALP");
    } else if (m_n_points_obj > 0) {
        m_graph_obj->Draw("ALP");
    }

    if (m_worst_bound) {
        const double ymin = *m_worst_bound - 0.05 * std::abs(*m_worst_bound);
        m_graph_bound->SetMinimum(ymin);
        m_graph_obj->SetMinimum(ymin);
    }

    if (m_worst_obj) {
        const double ymax = *m_worst_obj + 0.05 * std::abs(*m_worst_obj);
        m_graph_bound->SetMaximum(ymax);
        m_graph_obj->SetMaximum(ymax);
    }

    m_graph_bound->GetXaxis()->SetLimits(0, t);
    m_graph_obj->GetXaxis()->SetLimits(0, t);

    gPad->SetGrid(1, 1);
    gPad->Update();

    m_canvas->Update();
}
#else

idol::Plots::OptimalityGap::Strategy::Strategy(idol::Plots::Manager & t_plot_manager)
    : m_plot_manager(t_plot_manager)
{
    throw idol::Exception("idol was not compiled with ROOT.");
}

void idol::Plots::OptimalityGap::Strategy::initialize() {

}

void idol::Plots::OptimalityGap::Strategy::operator()(idol::CallbackEvent t_event){
    throw idol::Exception("idol was not compiled with ROOT.");
}

#endif
