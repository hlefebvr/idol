//
// Created by henri on 31/01/23.
//

#ifndef IDOL_OPTIMIZER_H
#define IDOL_OPTIMIZER_H

#include <string>

#include "../modeling/attributes/AttributeManagers_Base.h"
#include "Logger.h"

class Model;
class Var;
class Ctr;
class Constant;

namespace impl {
    class Optimizer;
}

class impl::Optimizer : protected AttributeManagers::Base {
    const ::Model& m_parent;

    LogLevel m_log_level = Mute;
    Color m_log_color = Default;
    double m_time_limit = std::numeric_limits<double>::max();
    unsigned m_max_n_threads = 1;
protected:
    virtual void build() = 0;

    virtual void add(const Var& t_var) = 0;
    virtual void add(const Ctr& t_ctr) = 0;

    virtual void remove(const Var& t_var) = 0;
    virtual void remove(const Ctr& t_ctr) = 0;

    virtual void update() = 0;

    virtual void write(const std::string& t_name) = 0;

    virtual void optimize() = 0;

    static double as_numeric(const Constant& t_constant);
public:
    explicit Optimizer(const ::Model& t_parent) : m_parent(t_parent) {}

    [[nodiscard]] virtual std::string name() const = 0;

    [[nodiscard]] virtual const ::Model& parent() const { return m_parent; }

    [[nodiscard]] LogLevel log_level() const { return m_log_level; }

    [[nodiscard]] Color log_color() const { return m_log_color; }

    void set_log_level(LogLevel t_level) { m_log_level = t_level; }

    void set_log_color(Color t_color) { m_log_color = t_color; }

    template<class T> T& as() {
        auto* result = dynamic_cast<T*>(this);
        if (!result) {
            throw Exception("Bad cast.");
        }
        return *result;
    }

    template<class T> const T& as() const {
        auto* result = dynamic_cast<const T*>(this);
        if (!result) {
            throw Exception("Bad cast.");
        }
        return *result;
    }

    template<class T> [[nodiscard]] bool is() const {
        return dynamic_cast<const T*>(this);
    }
};

class Optimizer : public impl::Optimizer {
    friend class Model;
public:
    explicit Optimizer(const Model& t_parent) : impl::Optimizer(t_parent) {}
};

#endif //IDOL_OPTIMIZER_H
