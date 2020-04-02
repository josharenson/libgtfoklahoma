/*
 * This file is part of the libgtfoklahoma distribution
 * (https://github.com/arenson/gtfoklahoma) Copyright (c) 2020 Josh Arenson.
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

#include <libgtfoklahoma/items.hpp>

TEST_CASE("Items", "[unit]") {
  using namespace libgtfoklahoma;

  SECTION("Test Parsing") {
    StatModel expected_stat_model;
    expected_stat_model.kit_weight = 5;
    expected_stat_model.max_mph = 5;
    expected_stat_model.odds_health_issue = .5;
    expected_stat_model.odds_mech_issue = .5;
    expected_stat_model.pace = StatModel::Pace::MERCKX;

    const char *itemJson = R"(
    [
      {
        "id": 0,
        "category": "BIKE",
        "cost": 42069,
        "display_name": "Taco Item",
        "image_url": "teapot.svg",
        "stat_changes": [
          {"kit_weight": 5},
          {"max_mph": 5},
          {"odds_health_issue": 0.5},
          {"odds_mech_issue": 0.5},
          {"pace": "MERCKX"}
        ]
      }
    ]
    )";

    Items items(itemJson);
    auto actual_item = items.getItem(0);
    REQUIRE(actual_item.id == 0);
    REQUIRE(actual_item.category == ItemModel::Category::BIKE);
    REQUIRE(actual_item.cost == 42069);
    REQUIRE(actual_item.display_name == "Taco Item");
    REQUIRE(actual_item.image_url == "teapot.svg");
    REQUIRE(actual_item.stat_delta == expected_stat_model);
  }
}