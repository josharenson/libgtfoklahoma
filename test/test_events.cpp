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

#include "helpers.hpp"

#include <libgtfoklahoma/events.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace testhelpers;

TEST_CASE("Events", "[unit]") {
using namespace libgtfoklahoma;

const char *eventJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "description_0",
      "display_name": "display_name_0",
      "mile": 0
    },
    {
      "id": 1,
      "actions": [1],
      "description": "description_1",
      "display_name": "display_name_1",
      "mile": 1
    },
    {
      "id": 1,
      "actions": [2],
      "description": "description_2",
      "display_name": "display_name_2",
      "mile": 4
    }
  ]
  )";

Game game("", validActionJson, eventJson, validIssueJson, validItemJson);
auto &events = game.getEvents();

// Add em'
}

TEST_CASE("EventModel", "[unit]") {
  libgtfoklahoma::EventModel model;
  model.action_ids = {1, 2};

  SECTION("EventModel::chooseAction validates") {
    REQUIRE(model.chooseAction(1));
    REQUIRE_FALSE(model.chooseAction(3));
  }
}
