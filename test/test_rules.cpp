/*
 * This file is part of the libgtfoklahoma distribution (https://github.com/arenson/libgtfoklahoma)
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

// Framework includes
#include <catch2/catch.hpp>

// Helper includes
#include <libgtfoklahoma/stats.hpp>

// Includes under test
#include <libgtfoklahoma/rules.hpp>

#include <iostream>
using namespace libgtfoklahoma;
using namespace libgtfoklahoma::rules;

TEST_CASE("Rules", "[unit]") {
  StatModel stats;

  SECTION("rules::RealSpeed") {

    // Just sanity check my back of the napkin shit
    stats.kit_weight = 50;
    stats.max_mph = 9;
    auto actual_result = RealSpeed(stats);
    REQUIRE(actual_result == 7);

    stats.kit_weight = 100;
    stats.max_mph = 9;
    actual_result = RealSpeed(stats);
    REQUIRE(actual_result == 6);

    stats.kit_weight = 10;
    stats.max_mph = 9;
    actual_result = RealSpeed(stats);
    REQUIRE(actual_result == 9);
  }
}
