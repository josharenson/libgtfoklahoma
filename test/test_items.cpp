/*
 * This file is part of the libgtfoklahoma distribution
 * (https://github.com/arenson/libgtfoklahoma) Copyright (c) 2020 Josh Arenson.
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

#include "helpers.hpp"

#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/items.hpp>

using namespace libgtfoklahoma;
using namespace testhelpers;

TEST_CASE("Items", "[unit]") {

  const char *itemJson = R"(
  [
    {
      "id": 0,
      "category": "BIKE",
      "cost": 10,
      "display_name": "",
      "image_url": "",
      "stat_changes": [{"max_mph": 1}]
    }
  ]
  )";

  Game game("", validActionJson, validEndingJson, validEventJson, validIssueJson, itemJson);
  auto &items = game.getItems();

  SECTION("Items::getItem") {
    auto item = items.getItem(0);
    REQUIRE_FALSE(item == Items::kEmptyItemModel);

    item = items.getItem(-1);
    REQUIRE(item == Items::kEmptyItemModel);
  }

}
