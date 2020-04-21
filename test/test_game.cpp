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

#include <catch2/catch.hpp>

#include "helpers.hpp"

#include <libgtfoklahoma/engine.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace libgtfoklahoma;
using namespace testhelpers;

TEST_CASE("Game", "[unit]") {
  Game game("");

  SECTION("Game::addItemToInventory") {
    REQUIRE_FALSE(game.hasItemInInventory(0));
    game.addItemToInventory(0);
    REQUIRE(game.hasItemInInventory(0));
  }

  SECTION("Game::removeItemFromInventory") {
    game.addItemToInventory(0);
    REQUIRE(game.hasItemInInventory(0));

    game.removeItemFromInventory(0);
    REQUIRE_FALSE(game.hasItemInInventory(0));
  }

  SECTION("Game::getInventory - default quantity") {
    game.addItemToInventory(0);
    auto inventory = game.getInventory();
    REQUIRE(inventory.size() == 1);
    REQUIRE(inventory[0].get().id == 0);
  }

  SECTION("Game::getInventory - explicit quantity") {
    game.addItemToInventory(0, 10);
    auto inventory = game.getInventory();
    REQUIRE(inventory.size() == 10);
    for (const auto &item : inventory) {
      REQUIRE(item.get().id == 0);
    }
  }

  SECTION("Game::getStats") {
    auto stats = game.getStats()->getPlayerStatsModel();
    REQUIRE(stats.bedtime_hour == rules::kDefaultBedtimeHour);
    REQUIRE(stats.health == rules::kDefaultHealth);
    REQUIRE(stats.kit_weight == 0);
    REQUIRE(stats.max_mph == 1);
    REQUIRE(stats.money_remaining == rules::kDefaultMoneyRemaining);
    REQUIRE(stats.odds_health_issue == rules::kDefaultOddsHealthIssuePerHour);
    REQUIRE(stats.odds_mech_issue == rules::kDefaultOddsMechanicalIssuePerHour);
    // TODO Pace
    REQUIRE(stats.wakeup_hour == rules::kDefaultWakeupHour);
  }

  SECTION("Game::updateStats") {
    StatModel deltaModel;
    const auto &initialStats = game.getStats()->getPlayerStatsModel();

    // I don't want to define a copy C'tor just for this...
    StatModel initialModel;
    initialModel.bedtime_hour = initialStats.bedtime_hour;
    initialModel.health = initialStats.health;
    initialModel.kit_weight = initialStats.kit_weight;
    initialModel.max_mph = initialStats.max_mph;
    initialModel.money_remaining = initialStats.money_remaining;
    initialModel.odds_health_issue = initialStats.odds_health_issue;
    initialModel.odds_mech_issue = initialStats.odds_mech_issue;
    initialModel.pace = initialStats.pace;
    initialModel.wakeup_hour = initialStats.wakeup_hour;

    // Whatever the default is, make sure its updated to something different
    auto expected_pace = initialModel.pace == StatModel::Pace::CHILL_AF
                             ? StatModel::Pace::MERCKX
                             : StatModel::Pace::CHILL_AF;

    deltaModel.bedtime_hour = 1;
    deltaModel.health = 1;
    deltaModel.kit_weight = 1;
    deltaModel.max_mph = 1;
    deltaModel.money_remaining = 1;
    deltaModel.odds_health_issue = .1;
    deltaModel.odds_mech_issue = .1;
    deltaModel.pace = expected_pace;
    deltaModel.wakeup_hour = 1;

    game.updateStats(deltaModel);
    auto &updatedModel = game.getStats()->getPlayerStatsModel();

    REQUIRE(updatedModel.bedtime_hour == initialModel.bedtime_hour + 1);
    REQUIRE(updatedModel.health == initialModel.health + 1);
    REQUIRE(updatedModel.kit_weight == initialModel.kit_weight + 1);
    REQUIRE(updatedModel.max_mph == initialModel.max_mph + 1);
    REQUIRE(updatedModel.odds_health_issue == initialModel.odds_health_issue + .1);
    REQUIRE(updatedModel.odds_mech_issue == initialModel.odds_mech_issue + .1);
    REQUIRE(updatedModel.pace == expected_pace);
    REQUIRE(updatedModel.wakeup_hour == initialModel.wakeup_hour + 1);
  }
}

namespace TestGamEndings {
std::mutex mutex;
bool running = true;
std::condition_variable simulation;
const std::chrono::milliseconds timeout(1000);
}
TEST_CASE("Game - EndingStack") {
  SECTION("Events make correct stack") {
    const char *eventJson = R"(
    [
      {
        "id": 0,
        "actions": [0],
        "ending_id_hints": [0],
        "description": "",
        "display_name": "",
        "mile": 0
      }
    ]
  )";

    class SuicideObserver : public IEventObserver {
    public:
      explicit SuicideObserver(Game &game) : IEventObserver(game) {}
      void onGameOver(EndingModel &ending) override {
        REQUIRE(ending.id == 0);
        // There should be none left, and -1 is invalid
        REQUIRE(m_game.popEndingHintId() == -1);
        TestGamEndings::running = false;
        TestGamEndings::simulation.notify_one();
      }
      void onHourChanged(int32_t hour) override {}
      void onMileChanged(int32_t mile) override {}
      bool onEvent(EventModel &event) override {
        event.chooseAction(0);
        return true;
      }
      bool onIssueOccurred(IssueModel &issue) override { return false; }
      void onStatsChanged(StatModel &stats) override {}
      bool onStoreEntered(ActionModel &action) override { return false; }
    };
    Game game("", validActionJson, validEndingJson, eventJson, validIssueJson, validItemJson);

    auto observer = std::make_unique<SuicideObserver>(game);
    game.registerEventObserver(std::move(observer));

    Engine engine(game);
    engine.start();

    std::unique_lock<std::mutex> lock(TestGamEndings::mutex);
    if (!TestGamEndings::simulation.wait_until(
        lock,
        std::chrono::system_clock::now() + TestGamEndings::timeout,
        []() { return !TestGamEndings::running; })) {
      FAIL("Timed out while waiting for the test to simulate death. Profound.");
    }

  }
}

