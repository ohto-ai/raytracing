#include "vector.hh"
#include <catch2/catch.hpp>
using ohtoai::math::Vec3;
using ohtoai::math::make_vector;

TEST_CASE("Vector Basic") {
    REQUIRE(std::is_trivial<Vec3>::value);
}

TEST_CASE("Verify Vector operator") {
    Vec3 v1(1, 2, 3);
    Vec3 v2(4, 5, 6);

    REQUIRE(v1 + v2 == make_vector(5, 7, 9));
    REQUIRE(v1 - v2 == make_vector(-3, -3, -3));
    REQUIRE(v1 * 2 == make_vector(2, 4, 6));
    REQUIRE(2 * v1 == make_vector(2, 4, 6));
    REQUIRE(v1 / 2 == make_vector(0.5f, 1, 1.5f));
    REQUIRE(v1 * v2 == make_vector(4, 10, 18));
}

TEST_CASE("Verify Vector operator Vector") {
    Vec3 v1(1, 2, 3);
    Vec3 v2(4, 5, 6);

    v1 += v2;
    REQUIRE(v1 == make_vector(5, 7, 9));
    v1 -= v2;
    REQUIRE(v1 == make_vector(1, 2, 3));
    v1 *= 2;
    REQUIRE(v1 == make_vector(2, 4, 6));
    v1 /= 2;
    REQUIRE(v1 == make_vector(1, 2, 3));
}

TEST_CASE("Verify Vector operator T") {
    using ohtoai::math::Vector;
    Vec3 v1(1, 2, 3);

    v1 *= 2;
    REQUIRE(v1 == make_vector(2, 4, 6));
    v1 /= 2;
    REQUIRE(v1 == make_vector(1, 2, 3));
}

TEST_CASE("Verify Vector dot, cross, length, etc") {
    Vec3 v1(3, 4, 0);
    Vec3 v2(5, 12, 13);
    const auto epsilon = 0.0001f;

    REQUIRE(v1.dot(v2) == 63);
    REQUIRE(v1.cross(v2) == make_vector(52.0f, -39.0f, 16.0f));
    REQUIRE(v1.length() == 5);
    REQUIRE(v1.length2() == 25);
    REQUIRE(v1.normalized().almost_equal(make_vector(0.6f, 0.8f, 0.0f), 0.0001f));
    v1.normalize();
    REQUIRE(v1.length() == 1);
    REQUIRE(v1.length2() == 1);
}
