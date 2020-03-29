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

#include <string>

#include <rapidjson/document.h>

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/events.hpp>
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/items.hpp>
#include <libgtfoklahoma/rules.hpp>
#include <libgtfoklahoma/stats.hpp>

TEST_CASE("Actions::getAction", "[unit]") {
  using namespace libgtfoklahoma;
  Game game("");

  const char *action_json = R"(
  [
    {
      "display_name": "display_name_0",
      "id": 0,
      "stat_changes": [{}]
    },
    {
      "display_name": "display_name_1",
      "id": 1,
      "stat_changes": [
        {"bedtime_hour": 5},
        {"kit_weight": 5},
        {"max_mph": 5},
        {"odds_health_issue": 0.5},
        {"odds_mech_issue": 0.5},
        {"pace": "MERCKX"},
        {"wakeup_hour": 5}
      ]
    }
  ]
  )";

  SECTION("Get action by id works") {
   Actions actions(game, action_json);
   ActionModel model_0 = actions.getAction(0);
   ActionModel model_1 = actions.getAction(1);
   REQUIRE(model_0.display_name == "display_name_0");
   REQUIRE(model_1.display_name == "display_name_1");
  }

  SECTION("StatModel Can parse stat changes") {
    Actions actions(game, action_json);
    ActionModel model_1 = actions.getAction(1);
    auto stat_model = model_1.stat_delta;
    REQUIRE(stat_model.bedtime_hour == 5);
    REQUIRE(stat_model.kit_weight == 5);
    REQUIRE(stat_model.max_mph == 5);
    REQUIRE(stat_model.odds_health_issue == .5);
    REQUIRE(stat_model.odds_mech_issue == .5);
    REQUIRE(stat_model.pace == StatModel::Pace::MERCKX);
    REQUIRE(stat_model.wakeup_hour == 5);
  }
}

TEST_CASE("Event::nextEvent", "[unit]") {
  using namespace libgtfoklahoma;
  const char *event_json = R"(
  [
    {
      "actions": [0],
      "description": "description_0",
      "display_name": "display_name_0",
      "mile": 0
    },
    {
      "actions": [1],
      "description": "description_1",
      "display_name": "display_name_1",
      "mile": 1
    },
    {
      "actions": [2],
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

  SECTION("Action ids are correct") {
    Events events(0, event_json);
    int i = 0; // index should match action id
    while (events.hasNextEvent()) {
      auto event = events.nextEvent();
      REQUIRE(event.action_ids[0] == i++);
    }
  }
}

TEST_CASE("EventModel", "[unit]") {
  libgtfoklahoma::EventModel model;
  model.action_ids = {1, 2};

  SECTION("EventModel::chooseAction validates") {
    REQUIRE(model.chooseAction(1));
    REQUIRE_FALSE(model.chooseAction(3));
  }
}

TEST_CASE("Game", "[unit]") {
  using namespace libgtfoklahoma;
  Game game("");

  const char *validActionJson = R"(
  [
    {
      "display_name": "display_name_0",
      "id": 0,
      "stat_changes": [{}]
    }
  ]
  )";

  const char *validEventJson = R"(
  [
    {
      "actions": [0],
      "description": "description_0",
      "display_name": "display_name_0",
      "mile": 0
    }
  ]
  )";

  const char *validItemJson = R"(
  [
    {
        "id": 0,
        "category": "BIKE",
        "cost": 42069,
        "display_name": "Taco Item",
        "image_url": "teapot.svg",
        "stat_changes": [{}]
    }
  ]
  )";

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

  SECTION("Inventory") {
    Game my_game("", validActionJson, validEventJson, validItemJson);

    // Is initially empty
    REQUIRE(my_game.getInventory().empty());

    // Adding 0 does nothing
    my_game.addItemToInventory(0,0);
    REQUIRE(my_game.getInventory().empty());

    // Adding 1 works
    my_game.addItemToInventory(0, 1);
    REQUIRE(my_game.getInventory().size() == 1);

    // Adding 1 more of the same bumps quantity
    my_game.addItemToInventory(0, 1);
    REQUIRE(my_game.getInventory().size() == 2);

    // Get inventory returns 2 of the items added
    auto items = my_game.getInventory();
    for (const auto &item : items) {
      REQUIRE(item.id == 0);
    }

    // Remove 1
    my_game.removeItemFromInventory(0);
    REQUIRE(my_game.getInventory().size() == 1);

    // Remove 1 more and assert empty
    my_game.removeItemFromInventory(0);
    REQUIRE(my_game.getInventory().empty());

    // Add 2 and remove 3, make sure all are removed
    my_game.addItemToInventory(0, 2);
    my_game.removeItemFromInventory(0, 3);
    REQUIRE(my_game.getInventory().empty());
  }

  SECTION("updateStats") {
    StatModel stats_delta;
    stats_delta.bedtime_hour = 5;
    stats_delta.kit_weight = 5;
    stats_delta.max_mph = 5;
    stats_delta.odds_health_issue = .5;
    stats_delta.odds_mech_issue = .5;
    stats_delta.pace = StatModel::Pace::MERCKX;
    stats_delta.wakeup_hour = 5;

    auto &initial_stats = game.getStats();
    auto initial_bedtime_hour = initial_stats->bedtime_hour;
    auto initial_kit_weight = initial_stats->kit_weight;
    auto initial_max_mph = initial_stats->max_mph;
    auto initial_odds_health_issue = initial_stats->odds_health_issue;
    auto initial_odds_mech_issue = initial_stats->odds_mech_issue;
    auto initial_wakeup_hour = initial_stats->wakeup_hour;

    game.updateStats(stats_delta);
    auto &updated_stats = game.getStats();

    REQUIRE(updated_stats->bedtime_hour == initial_bedtime_hour + 5);
    REQUIRE(updated_stats->kit_weight == initial_kit_weight + 5);
    REQUIRE(updated_stats->max_mph == initial_max_mph + 5);
    REQUIRE(updated_stats->odds_health_issue == initial_odds_health_issue + .5);
    REQUIRE(updated_stats->odds_mech_issue == initial_odds_mech_issue + .5);
    REQUIRE(updated_stats->pace == StatModel::Pace::MERCKX);
    REQUIRE(updated_stats->wakeup_hour == initial_wakeup_hour + 5);
  }
}

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

TEST_CASE("Stats", "[unit]") {
  using namespace libgtfoklahoma;

  SECTION("Test +") {
    StatModel lhs;
    lhs.bedtime_hour = 0;
    lhs.kit_weight = 0;
    lhs.max_mph = 0;
    lhs.odds_health_issue = 0;
    lhs.odds_mech_issue = 0;
    lhs.pace = StatModel::Pace::CHILL_AF;
    lhs.wakeup_hour = 0;

    StatModel rhs;
    rhs.bedtime_hour = 5;
    rhs.kit_weight = 5;
    rhs.max_mph = 5;
    rhs.odds_health_issue = .5;
    rhs.odds_mech_issue = .5;
    rhs.pace = StatModel::Pace::MERCKX;
    rhs.wakeup_hour = 5;

    auto result = lhs + rhs;
    REQUIRE(result.bedtime_hour == 5);
    REQUIRE(result.kit_weight == 5);
    REQUIRE(result.max_mph == 5);
    REQUIRE(result.odds_health_issue == .5);
    REQUIRE(result.odds_mech_issue == .5);
    REQUIRE(result.pace == StatModel::Pace::MERCKX);
    REQUIRE(result.wakeup_hour == 5);
  }

  SECTION("Stats::FromJson") {
    const char *statJson = R"(
      [
        {"bedtime_hour": 5},
        {"kit_weight": 5},
        {"max_mph": 5},
        {"odds_health_issue": 0.5},
        {"odds_mech_issue": 0.5},
        {"pace": "MERCKX"},
        {"wakeup_hour": 5}
      ]
    )";

    // HACK
    const auto get_arr = [](const rapidjson::Document &doc) {
      return doc.GetArray();
    };

    rapidjson::Document json_document;
    json_document.Parse(statJson);
    auto output = Stats::FromJson(get_arr(json_document));

    REQUIRE(output.bedtime_hour == 5);
    REQUIRE(output.kit_weight == 5);
    REQUIRE(output.max_mph == 5);
    REQUIRE(output.odds_health_issue == .5);
    REQUIRE(output.odds_mech_issue == .5);
    REQUIRE(output.pace == StatModel::Pace::MERCKX);
    REQUIRE(output.wakeup_hour == 5);
  }
}