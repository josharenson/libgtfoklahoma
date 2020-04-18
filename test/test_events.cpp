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

#include <libgtfoklahoma/events.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace libgtfoklahoma;
using namespace testhelpers;

TEST_CASE("Events", "[unit]") {

  const char *eventsJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "A RAIN EVENT",
      "display_name": "",
      "mile": 0
    },
    {
      "id": 10,
      "actions": [0],
      "description": "ANOTHER RAIN EVENT",
      "display_name": "",
      "mile": 0
    }
  ]
  )";
  Game game("", validActionJson, validEndingJson, eventsJson, validIssueJson, validItemJson);
  auto &events = game.getEvents();

  SECTION("Events::getEvent") {
    {
      auto &event = events->getEvent(0);
      REQUIRE_FALSE(event == Events::kEmptyEventModel);
    }

    {
      auto &event = events->getEvent(-1);
      REQUIRE(event == Events::kEmptyEventModel);
    }
  }

  SECTION("Events::eventsAtMile") {
    auto eventsAt0 = events->eventsAtMile(0);

    REQUIRE(eventsAt0.size() == 2);
    REQUIRE(std::find(eventsAt0.begin(), eventsAt0.end(), 0) != eventsAt0.end());
    REQUIRE(std::find(eventsAt0.begin(), eventsAt0.end(), 10) != eventsAt0.end());
  }

  SECTION("Events::hasMoreEvents") {
    REQUIRE_FALSE(events->hasMoreEvents(1));
  }

  SECTION("EventModel::chooseAction") {
    auto &event0 = events->getEvent(events->eventsAtMile(0)[0]);
    event0.chooseAction(0);
    REQUIRE(event0.chosenAction().get() == 0);
  }
}
