#pragma once
// point.hh

#ifndef _OHTOAI_POINT_H_
#define _OHTOAI_POINT_H_

#include "vector.hh"

namespace ohtoai {
    namespace math {
        template <typename T>
        class Point3 : public Vector<T, 3> {
        public:
            Point3() : Vector<T, 3>() {}
            Point3(T x, T y, T z) : Vector<T, 3>(x, y, z) {}
            Point3(const Vector<T, 3>& v) : Vector<T, 3>(v) {}
            Point3(const Point3<T>& p) : Vector<T, 3>(p) {}
            T& x() { return e[0]; }
            T& y() { return e[1]; }
            T& z() { return e[2]; }
            const T& x() const { return e[0]; }
            const T& y() const { return e[1]; }
            const T& z() const { return e[2]; }
        };

        template <typename T>
        class Point2 : public Vector<T, 2> {
        public:
            Point2() : Vector<T, 2>() {}
            Point2(T x, T y) : Vector<T, 2>(x, y) {}
            Point2(const Vector<T, 2>& v) : Vector<T, 2>(v) {}
            Point2(const Point2<T>& p) : Vector<T, 2>(p) {}
            T& x() { return e[0]; }
            T& y() { return e[1]; }
            const T& x() const { return e[0]; }
            const T& y() const { return e[1]; }
        };

        using Point3f = Point3<float>;
        using Point3d = Point3<double>;
        using Point3i = Point3<int>;
        using Point2f = Point2<float>;
        using Point2d = Point2<double>;
        using Point2i = Point2<int>;
    }
}

#endif // !_OHTOAI_POINT_H_
