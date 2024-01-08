#include "color.hh"
#include <catch2/catch.hpp>

TEST_CASE("Verify Color endian") {
    using ohtoai::Color;
    Color c(0x11223344);

    REQUIRE(c.red() == 0x44);
    REQUIRE(c.green() == 0x33);
    REQUIRE(c.blue() == 0x22);
    REQUIRE(c.alpha() == 0x11);
}

TEST_CASE("Verify Color operator") {
    using ohtoai::Color;
    Color c(0x11223344);

    c.red() = 0x55;
    c.green() = 0x66;
    c.blue() = 0x77;
    c.alpha() = 0x88;

    REQUIRE(c.red() == 0x55);
    REQUIRE(c.green() == 0x66);
    REQUIRE(c.blue() == 0x77);
    REQUIRE(c.alpha() == 0x88);
    REQUIRE(static_cast<uint32_t>(c) == 0x88776655);
}

TEST_CASE("Verify Color::rgb, Color::rgba") {
    using ohtoai::Color;
    Color c;

    c = Color::rgb(0x11, 0x22, 0x33);
    REQUIRE(c.red() == 0x11);
    REQUIRE(c.green() == 0x22);
    REQUIRE(c.blue() == 0x33);
    REQUIRE(c.alpha() == 0x00);
    REQUIRE(static_cast<uint32_t>(c) == 0x00332211);

    c = Color::rgba(0x11, 0x22, 0x33, 0x44);
    REQUIRE(c.red() == 0x11);
    REQUIRE(c.green() == 0x22);
    REQUIRE(c.blue() == 0x33);
    REQUIRE(c.alpha() == 0x44);
    REQUIRE(static_cast<uint32_t>(c) == 0x44332211);
}

TEST_CASE("Verify Color operator uint32_t") {
    using ohtoai::Color;
    Color c(0x11223344);

    uint32_t& rgba = c;
    rgba = 0x55667788;

    REQUIRE(c.red() == 0x88);
    REQUIRE(c.green() == 0x77);
    REQUIRE(c.blue() == 0x66);
    REQUIRE(c.alpha() == 0x55);
}

TEST_CASE("Verify Color operator Vector3") {
    using ohtoai::Color;
    Color c(ohtoai::math::createVector(1.0f, 2.0f, 3.0f));

    REQUIRE(c.red() == 1);
    REQUIRE(c.green() == 2);
    REQUIRE(c.blue() == 3);
    REQUIRE(c.alpha() == 0);
    REQUIRE(static_cast<uint32_t>(c) == 0x00030201);

    c = ohtoai::math::createVector(5, 6, 7);
    REQUIRE(c.red() == 5);
    REQUIRE(c.green() == 6);
    REQUIRE(c.blue() == 7);
    REQUIRE(c.alpha() == 0);
}

TEST_CASE("Verify Color operator Vector") {
    using ohtoai::Color;
    Color c;

    c = ohtoai::math::createVector(1.0f, 2.0f, 3.0f);
    REQUIRE(c.red() == 1);
    REQUIRE(c.green() == 2);
    REQUIRE(c.blue() == 3);
    REQUIRE(c.alpha() == 0);
    REQUIRE(static_cast<uint32_t>(c) == 0x00030201);

    c = ohtoai::math::createVector(1.0f, 2.0f, 3.0f, 4.0f);
    REQUIRE(c.red() == 1);
    REQUIRE(c.green() == 2);
    REQUIRE(c.blue() == 3);
    REQUIRE(c.alpha() == 4);
    REQUIRE(static_cast<uint32_t>(c) == 0x04030201);

    c = ohtoai::math::createVector(5, 6, 7);
    REQUIRE(c.red() == 5);
    REQUIRE(c.green() == 6);
    REQUIRE(c.blue() == 7);
    REQUIRE(c.alpha() == 0);
    REQUIRE(static_cast<uint32_t>(c) == 0x00070605);

    c = ohtoai::math::createVector(11, 12, 13, 14);
    REQUIRE(c.red() == 11);
    REQUIRE(c.green() == 12);
    REQUIRE(c.blue() == 13);
    REQUIRE(c.alpha() == 14);
    REQUIRE(static_cast<uint32_t>(c) == 0x0E0D0C0B);
}

