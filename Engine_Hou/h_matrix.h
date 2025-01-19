//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_MATRIX_H
#define ENGINE_HOU_CLION_H_MATRIX_H

#include "h_vector.h"

template <size_t Col, size_t Row> class Matrix {
public:
    static Matrix Zero;
    static Matrix One;

    Matrix() = default;

    Matrix(float value) {
        for (auto &elem : data_) {
            elem = value;
        }
    }

    Matrix(const std::initializer_list<float> &l) {
        for (size_t i = 0; i < l.size(); i++) {
            Set(i % Col, i / Col, *(l.begin() + i));
        }
    }

    Matrix(const std::initializer_list<Vector<Row>> &l) {
        for (size_t i = 0; i < l.size(); i++) {
            auto it = l.begin();
            for (size_t j = 0; j < Row; i++) {
                Set(i, j, *(l.begin() + j));
            }
        }
    }

    float Get(size_t x, size_t y) const { return data_[y + x * Row]; }
    float& Get(size_t x, size_t y) { return data_[y + x * Row]; }

    void Set(size_t x, size_t y, float value) { data_[y + x * Row] = value; }

    Matrix operator*(float value) const {
        Matrix result = *this;
        for (auto &elem : result.data_) {
            elem *= value;
        }
        return result;
    }

    Matrix operator/(float value) const {
        Matrix result = *this;
        for (auto &elem : result.data_) {
            elem /= value;
        }
        return result;
    }

    Matrix operator+(const Matrix &m) const {
        Matrix result = *this;
        for (size_t i = 0; i < Col * Row; i++) {
            result.data_[i] += m.data_[i];
        }
        return result;
    }

    Matrix operator-(const Matrix &m) const {
        Matrix result = *this;
        for (size_t i = 0; i < Col * Row; i++) {
            result.data_[i] -= m.data_[i];
        }
        return result;
    }

    Matrix &operator*=(float value) {
        for (auto &elem : data_) {
            elem *= value;
        }
        return *this;
    }

    Matrix &operator/=(float value) {
        for (auto &elem : data_) {
            elem /= value;
        }
        return *this;
    }

    Matrix &operator+=(const Matrix &m) {
        for (size_t i = 0; i < Col * Row; i++) {
            data_[i] += m.data_[i];
        }
        return *this;
    }

    Matrix &operator-=(const Matrix &m) {
        for (size_t i = 0; i < Col * Row; i++) {
            data_[i] -= m.data_[i];
        }
        return *this;
    }

    Matrix &operator*=(const Matrix &m) {
        static_assert(Col == Row);

        Matrix tmp;
        for (size_t k = 0; k < Col; k++) {
            for (size_t j = 0; j < Col; j++) {
                float sum = 0;
                for (size_t i = 0; i < Col; i++) {
                    sum += Get(i, k) * m.Get(j, i);
                }
                tmp.Set(j, k, sum);
            }
        }

        *this = tmp;
        return *this;
    }

    static Matrix Ones() { return Matrix(1); }

    static Matrix Zeros() { return Matrix(0); }

    static Matrix Eye() {
        static_assert(Col == Row);

        Matrix<Col, Col> result(0);
        for (size_t i = 0, j = 0; i < Col; i++, j++) {
            result.Set(i, j, 1);
        }
        return result;
    }

    void T() {
        for (size_t i = 0; i < Row; i++) {
            for (size_t j = i + 1; j < Col; j++) {
                std::swap(Get(i, j), Get(j, i));
            }
        }
    }

private:
    float data_[Col * Row];
};

template <size_t Col, size_t Row>
std::ostream &operator<<(std::ostream &o, const Matrix<Col, Row> &m) {
    o << "Matrix[" << std::endl;
    for (size_t y = 0; y < Row; y++) {
        for (size_t x = 0; x < Col; x++) {
            if (x == Col - 1) {
                o << m.Get(x, y);
            } else {
                o << m.Get(x, y) << ", ";
            }
        }
        o << std::endl;
    }
    o << "]";
    return o;
}

template <size_t Col, size_t Row, size_t TCol, size_t TRow>
Matrix<TCol, Row> operator*(const Matrix<Col, Row> &m1,
                            const Matrix<TCol, TRow> &m2) {
    static_assert(Col == TRow);

    Matrix<TCol, Row> result;
    for (size_t k = 0; k < Row; k++) {
        for (size_t j = 0; j < TCol; j++) {
            float sum = 0;
            for (size_t i = 0; i < Col; i++) {
                sum += m1.Get(i, k) * m2.Get(j, i);
            }
            result.Set(j, k, sum);
        }
    }
    return result;
}

template <size_t Col, size_t Row>
Matrix<Row, Col> Transpose(const Matrix<Col, Row> &m) {
    Matrix<Row, Col> result;
    for (size_t i = 0; i < Row; i++) {
        for (size_t j = 0; j < Col; j++) {
            result.Set(i, j, m.Get(j, i));
        }
    }
    return result;
}


template <size_t Col, size_t Row>
Matrix<Row, Col> Inverse(const Matrix<Row, Col> &m) {
    Matrix<Row, Col> inverse = Matrix<Row, Col>::Ones();
    Matrix<Row, Col> temp = m;

    for (size_t i = 0; i < Row; ++i) {
        float pivot = temp.Get(i, i);
        //if (fabs(pivot) < 1e-6) throw std::runtime_error("Matrix is singular!");

        for (size_t j = 0; j < Col; ++j) {
            temp.Set(i, j, temp.Get(i, j) / pivot);
            inverse.Set(i, j, inverse.Get(i, j) / pivot);
        }

        for (size_t k = 0; k < Row; ++k) {
            if (k != i) {
                float factor = temp.Get(k, i);
                for (size_t j = 0; j < Col; ++j) {
                    temp.Set(k, j, temp.Get(k, j) - factor * temp.Get(i, j));
                    inverse.Set(k, j, inverse.Get(k, j) - factor * inverse.Get(i, j));
                }
            }
        }
    }
    return inverse;
}

template <size_t Col, size_t Row, size_t Dim>
Vector<Row> operator*(const Matrix<Col, Row> &m, const Vector<Dim> &v) {
    Vector<Row> result;
    for (size_t j = 0; j < Row; j++) {
        float sum = 0;
        for (size_t i = 0; i < Col; i++) {
            sum += v.data[i] * m.Get(i, j);
        }
        result.data[j] = sum;
    }
    return result;
}



using Mat2x2 = Matrix<2, 2>;
using Mat3x3 = Matrix<3, 3>;
using Mat4x4 = Matrix<4, 4>;

#endif //ENGINE_HOU_CLION_H_MATRIX_H
