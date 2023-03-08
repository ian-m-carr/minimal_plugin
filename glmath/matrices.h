//
// Created by icarr on 30/10/2022.
//

#pragma once

#include <array>
#include <glmath/vectors.h>

namespace glmath {
    class mat4x4 {
    public:
        using column_type = vec4f;
        using row_type = vec4f;

        explicit mat4x4(float val = 0) : _value() {
            _value = {column_type({val, 0.0f, 0.0f, 0.0f}),
                      column_type({0.0f, val, 0.0f, 0.0f}),
                      column_type({0.0f, 0.0f, val, 0.0f}),
                      column_type({0.0f, 0.0f, 0.0f, val})};
        }

        mat4x4 &scale(const vec3f &v) {
            _value[0] = _value[0] * v.x;
            _value[1] = _value[1] * v.y;
            _value[2] = _value[2] * v.z;
            //_value[3] = _value[3];
            return *this;
        }

        mat4x4 &scale(const float x, const float y, const float z) {
            return scale(vec3f(x, y, z));
        }

        mat4x4 &translate(const vec3f &v) {
            _value[3] = _value[0] * v.x + _value[1] * v.y + _value[2] * v.z + _value[3];
            return *this;
        }

        mat4x4 &translate(const float x, const float y, const float z) {
            return translate(vec3f(x, y, z));
        }

        mat4x4 &rotate(const float angle, const vec3f &v) {
            float const a = angle;
            float const c = std::cos(a);
            float const s = std::sin(a);

            vec3f axis(normalize(v));
            vec3f temp(axis * (float(1) - c));

            mat4x4 rotate;
            rotate[0].x = c + temp.x * axis.x;
            rotate[0].y = temp.x * axis.y + s * axis.z;
            rotate[0].z = temp.x * axis.z - s * axis.y;

            rotate[1].x = temp.y * axis.x - s * axis.z;
            rotate[1].y = c + temp.y * axis.y;
            rotate[1].z = temp.y * axis.z + s * axis.x;

            rotate[2].x = temp.z * axis.x + s * axis.y;
            rotate[2].y = temp.z * axis.y - s * axis.x;
            rotate[2].z = c + temp.z * axis.z;

            mat4x4 result;
            result[0] = _value[0] * rotate[0].x + _value[1] * rotate[0].y + _value[2] * rotate[0].z;
            result[1] = _value[0] * rotate[1].x + _value[1] * rotate[1].y + _value[2] * rotate[1].z;
            result[2] = _value[0] * rotate[2].x + _value[1] * rotate[2].y + _value[2] * rotate[2].z;
            result[3] = _value[3];

            _value = result._value;

            return *this;
        }

        mat4x4 &rotate(const float angle, const float x, const float y, const float z) {
            return rotate(angle, vec3f(x, y, z));
        }

    public:
        const column_type &operator[](int column) const {
            return _value[column];
        }

        column_type &operator[](int column) {
            return _value[column];
        }

        const float *as_pointer_to_float() const { return &_value[0].x; }

    public:
        static const mat4x4 identity;
    private:
        std::array<column_type, 4> _value;
    };

    constexpr typename mat4x4::row_type operator*(const mat4x4 &matrix, const typename mat4x4::row_type &vector) {
        /*
         Ax = |a11 a12 ... a14||x1| = |a11*x1 + a12*x2 + a13*x3 + a14*x4|
              |a21 ... ... a24||x2|   |a21*x1 + a22*x2 + a23*x3 + a24*x4|
              |... ... ... ...||..|   |a31*x1 + a32*x2 + a33*x3 + a34*x4|
              |a41 ... ... a44||x4|   |a41*x1 + a42*x2 + a43*x3 + a44*x4|
        */

        // convert vector into columns
        const typename mat4x4::column_type vcol0(vector.x); // |x1,x1,x1,x1|
        // multiply each vector column by the columns of the matrix
        const typename mat4x4::column_type mult_res0 = matrix[0] * vcol0; // |a11*x1 a21*x1 a31*x1 a14*x1|

        const typename mat4x4::column_type vcol1(vector.y); // |x2,x2,x2,x2|
        const typename mat4x4::column_type mult_res1 = matrix[1] * vcol1; // same for next column
        const typename mat4x4::column_type sum0 = mult_res0 + mult_res1;  // column 0 + 1

        const typename mat4x4::column_type vcol2(vector.z); // repeat for x3
        const typename mat4x4::column_type mult_res2 = matrix[2] * vcol2;

        const typename mat4x4::column_type vcol3(vector.w); // and x4
        const typename mat4x4::column_type mult_res3 = matrix[3] * vcol3;
        const typename mat4x4::column_type sum1 = mult_res2 + mult_res3; // column 2 + 3

        return sum0 + sum1; // sum of all columns
    }

    inline const mat4x4 mat4x4::identity(1.0f);
}