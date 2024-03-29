#include "color.hh"
#include <catch2/catch.hpp>

using ohtoai::math::Color;
using ohtoai::math::make_vector;

TEST_CASE("Color Basic") {
    REQUIRE(std::is_trivial<Color>::value);
}

TEST_CASE("Verify Color endian") {
    Color c(0x11223344);

    REQUIRE(c.red() == 0x11);
    REQUIRE(c.green() == 0x22);
    REQUIRE(c.blue() == 0x33);
    REQUIRE(c.alpha() == 0x44);
}

TEST_CASE("Verify Color operator") {
    Color c(0x11223344);

    c.red() = 0x55;
    c.green() = 0x66;
    c.blue() = 0x77;
    c.alpha() = 0x88;

    REQUIRE(c.red() == 0x55);
    REQUIRE(c.green() == 0x66);
    REQUIRE(c.blue() == 0x77);
    REQUIRE(c.alpha() == 0x88);
    REQUIRE(c.to_easyx_color() == 0x00776655);
    REQUIRE(c.to_rgba() == 0x55667788);
}

TEST_CASE("Verify Color::rgb, Color::rgba") {
    Color c;

    c = Color::rgb(0x11, 0x22, 0x33);
    REQUIRE(c.red() == 0x11);
    REQUIRE(c.green() == 0x22);
    REQUIRE(c.blue() == 0x33);
    REQUIRE(c.alpha() == 0x00);
    REQUIRE(c.to_easyx_color() == 0x332211);

    c = Color::rgba(0x11, 0x22, 0x33, 0x44);
    REQUIRE(c.red() == 0x11);
    REQUIRE(c.green() == 0x22);
    REQUIRE(c.blue() == 0x33);
    REQUIRE(c.alpha() == 0x44);
    REQUIRE(c.to_easyx_color() == 0x332211);
}

TEST_CASE("Verify Color operator Vector3") {
    Color c(make_vector(1.0f, 2.0f, 3.0f));

    REQUIRE(c.red() == 1);
    REQUIRE(c.green() == 2);
    REQUIRE(c.blue() == 3);
    REQUIRE(c.alpha() == 0);
    REQUIRE(c.to_easyx_color() == 0x030201);

    c = make_vector(5, 6, 7);
    REQUIRE(c.red() == 5);
    REQUIRE(c.green() == 6);
    REQUIRE(c.blue() == 7);
    REQUIRE(c.alpha() == 0);
}

TEST_CASE("Verify Color operator Vector") {
    Color c;

    c = make_vector(1.0f, 2.0f, 3.0f);
    REQUIRE(c.red() == 1);
    REQUIRE(c.green() == 2);
    REQUIRE(c.blue() == 3);
    REQUIRE(c.alpha() == 0);
    REQUIRE(c.to_easyx_color() == 0x030201);

    c = make_vector(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(c.red() == 1);
    REQUIRE(c.green() == 2);
    REQUIRE(c.blue() == 3);
    REQUIRE(c.alpha() == 4);
    REQUIRE(c.to_easyx_color() == 0x030201);

    c = make_vector(5, 6, 7);
    REQUIRE(c.red() == 5);
    REQUIRE(c.green() == 6);
    REQUIRE(c.blue() == 7);
    REQUIRE(c.alpha() == 0);
    REQUIRE(c.to_easyx_color() == 0x070605);

    c = make_vector(11, 12, 13, 14);
    REQUIRE(c.red() == 11);
    REQUIRE(c.green() == 12);
    REQUIRE(c.blue() == 13);
    REQUIRE(c.alpha() == 14);
    REQUIRE(c.to_easyx_color() == 0x0D0C0B);
}

