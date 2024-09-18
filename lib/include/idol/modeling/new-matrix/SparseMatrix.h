//
// Created by henri on 06.09.24.
//

#ifndef IDOL_SPARSEMATRIX_H
#define IDOL_SPARSEMATRIX_H

#include <vector>
#include <ostream>

namespace idol {
    class SparseMatrix;
    class SparseConstant;

    std::ostream& operator<<(std::ostream& t_out, const SparseMatrix& t_matrix);
}

class idol::SparseConstant {
    double m_value;
public:
    [[nodiscard]] double numerical() const { return m_value; }
};

class idol::SparseMatrix {
    unsigned int m_minor_dimension = 0;
    std::vector<double> m_coefficients;
    std::vector<unsigned int> m_indices;
    std::vector<unsigned int> m_start_indices;
public:
    SparseMatrix() = default;

    SparseMatrix(unsigned int t_minor_dimension) : m_minor_dimension(t_minor_dimension) {}

    [[nodiscard]] unsigned int minor_dimension() const { return m_minor_dimension; }

    [[nodiscard]] unsigned int major_dimension() const { return m_start_indices.size(); }

    void add_major(const std::vector<unsigned int>& t_indices, const std::vector<double>& t_values) {
        m_start_indices.emplace_back(m_indices.size());
        m_indices.insert(m_indices.end(), t_indices.begin(), t_indices.end());
        m_coefficients.insert(m_coefficients.end(), t_values.begin(), t_values.end());
    }

    const std::vector<double> coefficients() const { return m_coefficients; }

    const std::vector<unsigned int> indices() const { return m_indices; }

    const std::vector<unsigned int> start_indices() const { return m_start_indices; }

    class MajorView {
        const SparseMatrix *m_matrix;
        unsigned int m_major = 0;
    public:
        MajorView(const SparseMatrix *t_matrix, unsigned int t_major) : m_matrix(t_matrix), m_major(t_major) {}

        class iterator {
            const SparseMatrix* m_matrix;
            unsigned int m_index = 0;
        public:
            iterator(const SparseMatrix* t_matrix, unsigned int t_index) : m_matrix(t_matrix), m_index(t_index) {}

            iterator(const iterator&) = default;
            iterator(iterator&&) noexcept = default;

            iterator& operator=(const iterator&) = default;
            iterator& operator=(iterator&&) noexcept = default;

            bool operator!=(const iterator& t_other) const {
                return m_index != t_other.m_index;
            }

            bool operator==(const iterator& t_other) const {
                return m_index == t_other.m_index;
            }

            std::pair<unsigned int, double> operator*() const {
                return {m_matrix->m_indices[m_index], m_matrix->m_coefficients[m_index] };
            }

            iterator& operator++() {
                ++m_index;
                return *this;
            }

            iterator& operator--() {
                --m_index;
                return *this;
            }

            iterator& operator+=(unsigned int t_offset) {
                m_index += t_offset;
                return *this;
            }

            iterator& operator-=(unsigned int t_offset) {
                m_index -= t_offset;
                return *this;
            }

            iterator operator+(unsigned int t_offset) const {
                return {
                    m_matrix,
                    m_index + t_offset
                };
            }

            iterator operator-(unsigned int t_offset) const {
                return {
                    m_matrix,
                    m_index - t_offset
                };
            }

            iterator operator-(const iterator& t_other) const {
                return {
                    m_matrix,
                    m_index - t_other.m_index
                };
            }

        };

        iterator begin() const {
            return {
                m_matrix,
                m_matrix->m_start_indices[m_major]
            };
        }

        iterator end() const {

            if (m_major + 1 == m_matrix->major_dimension()) {
                return {
                    m_matrix,
                    (unsigned int) m_matrix->m_coefficients.size()
                };
            }

            return {
                m_matrix,
                m_matrix->m_start_indices[m_major + 1]
            };
        }

    };

    auto get_major(unsigned int t_major) const {
        return MajorView(this, t_major);
    }

    double get_coefficient(unsigned int t_major, unsigned int t_minor) const;
};

template<>
struct std::iterator_traits<idol::SparseMatrix::MajorView::iterator> {
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::pair<unsigned int, double>;
    using difference_type = unsigned int;
    using pointer = value_type*;
    using reference = value_type&;
};

double idol::SparseMatrix::get_coefficient(unsigned int t_major, unsigned int t_minor) const {

    const auto& major = get_major(t_major);
    const auto it = std::lower_bound(
            major.begin(),
            major.end(),
            std::pair<unsigned int, double>(t_minor, .0)
    );

    if (it == major.end() || (*it).first != t_minor) {
        return 0.;
    }

    return (*it).second;
}

std::ostream& idol::operator<<(std::ostream& t_out, const SparseMatrix& t_matrix) {

    const auto& indices = t_matrix.indices();
    const auto& start_indices = t_matrix.start_indices();
    const auto& coefficients = t_matrix.coefficients();

    for (unsigned int major = 0, major_dim = t_matrix.major_dimension() ; major < major_dim ; ++major ) {
        for (const auto& [index, coefficient] : t_matrix.get_major(major)) {
            t_out << coefficient << " " << "x_" << index << " + ";
        }
        t_out << '\n';
    }

    return t_out;
}

#endif //IDOL_SPARSEMATRIX_H
