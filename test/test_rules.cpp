/*
 * This file is part of the libgtfoklahoma distribution (https://github.com/arenson/gtfoklahoma)
 * Copyright (c) 2020 Josh Arenson.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <catch2/catch.hpp>

#include <libgtfoklahoma/rules.hpp>

TEST_CASE("Rules", "[unit]") {
  using namespace libgtfoklahoma::rules;
  SECTION("RealSpeed") {

    // Just sanity check my back of the napkin shit
    auto actual_result = RealSpeed(9, 50);
    REQUIRE(actual_result == 7);

    actual_result = RealSpeed(9, 100);
    REQUIRE(actual_result == 6);

    actual_result = RealSpeed(9, 10);
    REQUIRE(actual_result == 9);
  }
}