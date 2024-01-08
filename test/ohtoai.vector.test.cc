#include "vector.hh"
#include <catch2/catch.hpp>

TEST_CASE("Verify Vector operator") {
    using ohtoai::math::Vector;
    Vector<int, 3> v1(1, 2, 3);
    Vector<int, 3> v2(4, 5, 6);

    REQUIRE(v1 + v2 == Vector<int, 3>(5, 7, 9));
    REQUIRE(v1 - v2 == Vector<int, 3>(-3, -3, -3));
    REQUIRE(v1 * 2 == Vector<int, 3>(2, 4, 6));
    REQUIRE(v1 / 2 == Vector<int, 3>(0, 1, 1));
}

TEST_CASE("Verify Vector operator Vector") {
    using ohtoai::math::Vector;
    Vector<int, 3> v1(1, 2, 3);
    Vector<int, 3> v2(4, 5, 6);

    v1 += v2;
    REQUIRE(v1 == Vector<int, 3>(5, 7, 9));
    v1 -= v2;
    REQUIRE(v1 == Vector<int, 3>(1, 2, 3));
    v1 *= 2;
    REQUIRE(v1 == Vector<int, 3>(2, 4, 6));
    v1 /= 2;
    REQUIRE(v1 == Vector<int, 3>(1, 2, 3));
}

TEST_CASE("Verify Vector operator T") {
    using ohtoai::math::Vector;
    Vector<int, 3> v1(1, 2, 3);

    v1 *= 2;
    REQUIRE(v1 == Vector<int, 3>(2, 4, 6));
    v1 /= 2;
    REQUIRE(v1 == Vector<int, 3>(1, 2, 3));
}

TEST_CASE("Verify Vector dot, cross, length, etc") {
    using ohtoai::math::Vec3f;
    Vec3f v1(3, 4, 0);
    Vec3f v2(5, 12, 13);

    REQUIRE(v1.dot(v2) == 63);
    REQUIRE(v1.cross(v2) == Vec3f(52.0f, -39.0f, 16.0f));
    REQUIRE(v1.length() == 5);
    REQUIRE(v1.length2() == 25);
    REQUIRE(v1.normalize() == Vec3f(0.6f, 0.8f, 0.0f));
    REQUIRE(v1.length() == 1);
    REQUIRE(v1.length2() == 1);
}
