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

#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <libgtfoklahoma/events.hpp>
#include <libgtfoklahoma/game.hpp>

TEST_CASE("Event::nextEvent") {
  using namespace libgtfoklahoma;
  const char *event_json = R"(
  [
    {
      "description": "description_0",
      "display_name": "display_name_0",
      "mile": 0
    },
    {
      "description": "description_1",
      "display_name": "display_name_1",
      "mile": 1
    },
    {
      "description": "description_2",
      "display_name": "display_name_2",
      "mile": 4
    }
  ]
  )";


  SECTION("Initial event is correct from 0") {
   Events events(0, event_json);
   EventModel model = events.nextEvent();
   REQUIRE(model.description == "description_0");
   REQUIRE(model.display_name == "display_name_0");
   REQUIRE(model.mile == 0);
  }

  SECTION("Initial event is correct from > 0 where mile exactly matches") {
    Events events(1, event_json);
    EventModel model = events.nextEvent();
    REQUIRE(model.description == "description_1");
    REQUIRE(model.display_name == "display_name_1");
    REQUIRE(model.mile == 1);
  }

  SECTION("Initial event is correct from > 0 where mile is in between events") {
    Events events(2, event_json);
    EventModel model = events.nextEvent();
    REQUIRE(model.description == "description_2");
    REQUIRE(model.display_name == "display_name_2");
    REQUIRE(model.mile == 4);
  }

  SECTION("Events::nextEvent increments correctly") {
    Events events(0, event_json);
    events.nextEvent();
    EventModel model = events.nextEvent();
    REQUIRE(model.description == "description_1");
    REQUIRE(model.display_name == "display_name_1");
    REQUIRE(model.mile == 1);
  }
}

TEST_CASE("Game::bump*", "[unit]") {
  libgtfoklahoma::Game game("");

  SECTION("bumpHour") {
    REQUIRE(game.hour() == 0);
    auto actual_value = game.bumpHour();
    auto expected_value = 1;
    REQUIRE(actual_value == expected_value);

    game.setHour(23);
    REQUIRE(game.bumpHour() == 0);
  }

  SECTION("bumpMile") {
    REQUIRE(game.mile() == 0);
    auto actual_value = game.bumpMile();
    auto expected_value = 1;
    REQUIRE(actual_value == expected_value);
  }

  SECTION("bumpTick") {
    REQUIRE(game.tick() == 0);
    auto actual_value = game.bumpTick();
    auto expected_value = 1;
    REQUIRE(actual_value == expected_value);
  }
}