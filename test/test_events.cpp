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

#include <memory>

#include "helpers.hpp"

#include <libgtfoklahoma/engine.hpp>
#include <libgtfoklahoma/event_observer.hpp>
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
      auto &event = events.getEvent(0);
      REQUIRE_FALSE(event == Events::kEmptyEventModel);
    }

    {
      auto &event = events.getEvent(-1);
      REQUIRE(event == Events::kEmptyEventModel);
    }
  }

  SECTION("Events::eventsAtMile") {
    auto eventsAt0 = events.eventsAtMile(0);

    REQUIRE(eventsAt0.size() == 2);
    REQUIRE(std::find(eventsAt0.begin(), eventsAt0.end(), 0) != eventsAt0.end());
    REQUIRE(std::find(eventsAt0.begin(), eventsAt0.end(), 10) != eventsAt0.end());
  }

  SECTION("Events::hasMoreEvents") {
    REQUIRE_FALSE(events.hasMoreEvents(1));
  }

  SECTION("EventModel::chooseAction") {
    auto &event0 = events.getEvent(events.eventsAtMile(0)[0]);
    event0.chooseAction(0);
    REQUIRE(event0.chosenAction().get() == 0);
  }
}


TEST_CASE("Events - Ending Hints") {

  // The game should end happily as we reach the final destination safely
  Game game("", validActionJson, validEndingJson, validEventJson, validIssueJson, validItemJson);
  EngineStopper stopper;

  class SuicideObserver : public TestObserver {
  public:
    explicit SuicideObserver(EngineStopper &stopper, Game &game) : stopper(stopper), TestObserver(game){}
    void onGameOver(const EndingModel &ending) override {
      REQUIRE(ending.id == 0);
      stopper.stopEngine();
    }
    bool onEvent(EventModel &event) override {
      event.chooseAction(0);
      return true;
    }
  private:
    EngineStopper &stopper;
  };

  auto observer = std::make_unique<SuicideObserver>(stopper, game);
  game.registerEventObserver(std::move(observer));

  Engine engine(game);
  engine.start();
  stopper.waitForEngineToStopOrFail();
}

TEST_CASE("Events - Chosen failable actions apply stats correctly") {

  class Observer : public TestObserver {
  public:
    explicit Observer(bool expect_failure, EngineStopper &stopper, Game &game)
    : expect_failure(expect_failure)
    , stopper(stopper)
    , TestObserver(game) {}

    void onGameOver(const EndingModel &) override { stopper.stopEngine(); }
    bool onEvent(EventModel &event) override {
      if (expect_failure) {
        REQUIRE(m_game.getActions().getAction(0).message_failure ==
                "message_failure");
      } else {
        REQUIRE(m_game.getActions().getAction(0).message_success ==
                "message_success");
      }
      event.chooseAction(0);
      return true;
    }
  private:
    bool expect_failure;
    EngineStopper &stopper;
  };

  SECTION("Failed action is correct") {
    const char *actionJson = R"(
    [
      {
        "id": 0,
        "display_name": "",
        "message_failure": "message_failure",
        "success_chance":0.0,
        "stat_changes_on_failure": [{"health": -1000}],
        "type": ["STAT_CHANGE"]

      }
    ]
    )";

    EngineStopper stopper;
    Game game("", actionJson, validEndingJson, validEventJson, validIssueJson, validItemJson);
    game.registerEventObserver(std::make_shared<Observer>(true, stopper, game));
    Engine engine(game);
    auto initial_health = game.getStats().getPlayerStatsModel().health;
    auto expected_health = initial_health - 1000;
    engine.start();
    stopper.waitForEngineToStopOrFail();
    REQUIRE(game.getStats().getPlayerStatsModel().health == expected_health);
  }

  SECTION("Success action is correct") {
    const char *actionJson = R"(
    [
      {
        "id": 0,
        "display_name": "",
        "message_success": "message_success",
        "success_chance": 1.0,
        "stat_changes_on_success": [{"health": -1000}],
        "type": ["STAT_CHANGE"]

      }
    ]
    )";

    EngineStopper stopper;
    Game game("", actionJson, validEndingJson, validEventJson, validIssueJson, validItemJson);
    game.registerEventObserver(std::make_shared<Observer>(false, stopper, game));
    Engine engine(game);
    auto initial_health = game.getStats().getPlayerStatsModel().health;
    auto expected_health = initial_health - 1000;
    engine.start();
    stopper.waitForEngineToStopOrFail();
    REQUIRE(game.getStats().getPlayerStatsModel().health == expected_health);
  }
}
