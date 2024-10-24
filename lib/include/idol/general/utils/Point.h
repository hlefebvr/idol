//
// Created by henri on 24.10.24.
//

#ifndef IDOL_POINT_H
#define IDOL_POINT_H

#include "SparseVector.h"
#include "types.h"
#include "idol/mixed-integer/modeling/variables/Var.h"
#include "idol/general/numericals.h"

namespace idol {
    template<class T> class Point;
    using PrimalPoint = Point<Var>;
    using DualPoint = Point<Ctr>;
}

template<class T>
class idol::Point : public SparseVector<T, double> {
    SolutionStatus m_status = Loaded;
    SolutionReason m_reason = NotSpecified;
    std::optional<double> m_objective_value;
public:
    Point() = default;
    explicit Point(const SparseVector<T, double>& t_vector) : SparseVector<T, double>(t_vector) {}
    explicit Point(SparseVector<T, double>&& t_vector) : SparseVector<T, double>(std::move(t_vector)) {}

    [[nodiscard]] SolutionStatus status() const { return m_status; }
    void set_status(SolutionStatus t_status) { m_status = t_status; }

    [[nodiscard]] SolutionReason reason() const { return m_reason; }

    void set_reason(SolutionReason t_reason) { m_reason = t_reason; }

    [[nodiscard]] double objective_value() const;

    void set_objective_value(double t_objective_value) { m_objective_value = t_objective_value; }

    [[nodiscard]] bool has_objective_value() const { return m_objective_value.has_value(); }

    void reset_objective_value() { m_objective_value.reset(); }

    Point& operator*=(double t_factor) override { return static_cast<Point&>(SparseVector<T, double>::operator*=(t_factor)); }
    Point& operator+=(const Point& t_other) { return static_cast<Point&>(SparseVector<T, double>::operator+=(t_other)); }
    Point& operator+=(Point&& t_other) { return static_cast<Point&>(SparseVector<T, double>::operator+=(std::move(t_other))); }
    Point& operator-=(const Point& t_other) { return static_cast<Point&>(SparseVector<T, double>::operator-=(t_other)); }
    Point& operator-=(Point&& t_other) { return static_cast<Point&>(SparseVector<T, double>::operator-=(std::move(t_other))); }
};

template<class T>
double idol::Point<T>::objective_value() const {

    if (m_objective_value.has_value()) {
        return m_objective_value.value();
    }

    if (m_status == Unbounded) {
        return -Inf;
    }

    return Inf;
}

namespace idol {
    template<class T>
    static std::ostream &operator<<(std::ostream &t_os, const Point<T> &t_point) {
        t_os << "Status: " << t_point.status() << '\n';
        t_os << "Reason: " << t_point.reason() << '\n';
        t_os << "Objective value: " << t_point.objective_value() << '\n';
        t_os << "Values: " << '\n';
        t_os << static_cast<const SparseVector<T, double> &>(t_point);
        return t_os;
    }

    template<class T> static Point<T> operator+(Point<T>&& t_x, Point<T>&& t_y) { return Point(std::move(t_x)) += t_y; }
    template<class T> static Point<T> operator+(Point<T>&& t_x, const Point<T>& t_y) { return Point(std::move(t_x)) += t_y; }
    template<class T> static Point<T> operator+(const Point<T>& t_x, Point<T>&& t_y) { return Point(std::move(t_y)) += t_x; }
    template<class T> static Point<T> operator+(const Point<T>& t_x, const Point<T>& t_y) { return Point(t_x) += t_y; }

    template<class T> static Point<T> operator*(double t_factor, const Point<T>& t_y) { return Point(t_y) *= t_factor; }
}

#endif //IDOL_POINT_H
