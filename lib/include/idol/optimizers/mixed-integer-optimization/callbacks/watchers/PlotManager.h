//
// Created by henri on 22.10.24.
//

#ifndef IDOL_PLOTMANAGER_H
#define IDOL_PLOTMANAGER_H

#include <list>
#include "idol/errors/Exception.h"

#ifdef IDOL_USE_ROOT
#include <TApplication.h>
#include <TCanvas.h>
#endif

namespace idol::Plots {
    class Manager;
}

class idol::Plots::Manager {
    class PlotEnv {
        static PlotEnv* m_instance;
#ifdef IDOL_USE_ROOT
        TApplication m_root;
#endif
        unsigned int m_n_manager;

        PlotEnv();

        static PlotEnv& get();
    public:
        static void declare_plot_manager();
        static void destroy_plot_manager();
    };

#ifdef IDOL_USE_ROOT
    std::list<std::unique_ptr<TObject>> m_objects;
    std::list<std::unique_ptr<TCanvas>> m_canvas;
#endif
public:
    Manager();

    template<class T, class ...ArgsT> T* create(ArgsT&& ...args) {
#ifdef IDOL_USE_ROOT
        auto* obj = new T(std::forward<ArgsT>(args)...);

        if constexpr (std::is_same<T, TCanvas>::value) {
            m_canvas.emplace_back(obj);
        } else {
            m_objects.emplace_back(obj);
        }

        return obj;
#else
        throw idol::Exception("idol was not compiled with ROOT.");
#endif
    }

    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;

    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;

    ~Manager();
};

#endif //IDOL_PLOTMANAGER_H
