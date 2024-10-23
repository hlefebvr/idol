//
// Created by henri on 22.10.24.
//
#include "idol/optimizers/mixed-integer-optimization/callbacks/watchers/PlotManager.h"

#ifdef IDOL_USE_ROOT
#include <TSystem.h>

idol::Plots::Manager::PlotEnv* idol::Plots::Manager::PlotEnv::m_instance = nullptr;

idol::Plots::Manager::PlotEnv::PlotEnv() : m_root("ROOT", nullptr, nullptr), m_n_manager(0) {

}

idol::Plots::Manager::PlotEnv &idol::Plots::Manager::PlotEnv::get() {
    if (!m_instance) {
        m_instance = new PlotEnv();
    }
    return *m_instance;
}

void idol::Plots::Manager::PlotEnv::declare_plot_manager() {
    get().m_n_manager++;
}

void idol::Plots::Manager::PlotEnv::destroy_plot_manager() {
    auto& env = get();
    env.m_n_manager--;
    if (env.m_n_manager == 0) {
        m_instance->m_root.Terminate();
        delete m_instance;
        m_instance = nullptr;
    }
}

idol::Plots::Manager::Manager() {
    PlotEnv::declare_plot_manager();
}

idol::Plots::Manager::~Manager() {
    while (true) {

        bool has_open_canvas = false;

        for (auto &ptr_canvas: m_canvas) {
            if (ptr_canvas->GetCanvasImp()) {
                has_open_canvas = true;
                break;
            }
        }

        if (!has_open_canvas) {
            PlotEnv::destroy_plot_manager();
            break;
        }

        gSystem->ProcessEvents();

    }

}

#else

idol::Plots::Manager::PlotEnv::PlotEnv() {
    throw Exception("idol was not compiled with ROOT.");
}

idol::Plots::Manager::PlotEnv &idol::Plots::Manager::PlotEnv::get() {
    throw Exception("idol was not compiled with ROOT.");
}

void idol::Plots::Manager::PlotEnv::declare_plot_manager() {
    throw Exception("idol was not compiled with ROOT.");
}

void idol::Plots::Manager::PlotEnv::destroy_plot_manager() {
    throw Exception("idol was not compiled with ROOT.");
}

idol::Plots::Manager::Manager() {
    throw Exception("idol was not compiled with ROOT.");
}

#endif
