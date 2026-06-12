//
// Created by Henri on 30/03/2026.
//

#ifndef IDOL_CUTPOOL_H
#define IDOL_CUTPOOL_H
#include <list>

#include "idol/general/utils/Point.h"
#include "idol/mixed-integer/modeling/constraints/Ctr.h"

namespace idol {
    class CutPool;
}

class idol::CutPool {

    struct CutHistory {
        idol::Ctr cut;
        double norm_squared = 0.;
        unsigned int age = 0;
        unsigned int n_active = 0;
        double score = 1.;
        CutHistory(Ctr t_cut, double t_norm_squared);
    };

    const unsigned int m_min_age = 10;
    const double m_min_activity_threshold = .2;
    const unsigned int m_max_pool_size = 20000;
    const double m_max_cosine = .95;
    std::list<CutHistory> m_cuts_in_relaxation;
    std::list<Ctr> m_all_cuts;

    bool add_existing_cut_to_relaxation(const Ctr& t_cut, Model& t_relaxation, bool t_force = false);
public:
    bool add_cut(const TempCtr& t_cut, Model& t_relaxation);
    unsigned int recycle(const PrimalPoint& t_current_point, Model& t_relaxation, double t_tol_feasibility);
    void clean_up(Model& t_relaxation);

    static double cosine(const Env& t_env, const Ctr& t_cut1, const Ctr& t_cut2);
    static double dot(const Env& t_env, const Ctr& t_cut1, const Ctr& t_cut2);
    static double norm_squared(const Env& t_env, const Ctr& t_cut);
};

#endif //IDOL_CUTPOOL_H