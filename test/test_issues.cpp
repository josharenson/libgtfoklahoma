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
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/issues.hpp>

using namespace libgtfoklahoma;
using namespace testhelpers;

TEST_CASE("Issues", "[unit]") {

  const char *issuesJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "HEALTH ISSUE",
      "display_name": "",
      "image_url": "",
      "type": "HEALTH"
    },
    {
      "id": 10,
      "actions": [0],
      "description": "MECHANICAL ISSUE",
      "display_name": "",
      "dependent_inventory": [0],
      "image_url": "",
      "stat_changes": [{"max_mph": 1}],
      "type": "MECHANICAL"
    }
  ]
  )";

  Game game("", validActionJson, validEndingJson, validEventJson, issuesJson, validItemJson);
  auto &issues = game.getIssues();

  class Observer : public IEventObserver {
  public:
    explicit Observer(Game &game) : IEventObserver(game) {}
    void onGameOver(EndingModel &ending) override {}
    void onHourChanged(int32_t hour) override {}
    void onMileChanged(int32_t mile) override {}
    bool onEvent(EventModel &event) override { return false; }
    bool onIssueOccurred(IssueModel &issue) override {
      issue.chooseAction(0);
      return true;
    }
    void onStatsChanged(StatModel &stats) override {}
    bool onStoreEntered(ActionModel &action) override { return false; }
  };
  //game.registerEventObserver();

  SECTION("Issues::popRandomIssue") {
    // This isn't random because there is only one of each type
    auto id = issues->popRandomIssueId(IssueModel::Type::HEALTH);
    REQUIRE(issues->getIssue(id).description == "HEALTH ISSUE");
    issues->handleIssue(id, std::make_unique<Observer>(game));

    // Issue can't be served again
    id = issues->popRandomIssueId(IssueModel::Type::HEALTH);
    REQUIRE(issues->getIssue(id) == Issues::kEmptyIssueModel);

    // Dependent actions and inventory work
    id = issues->popRandomIssueId(IssueModel::Type::MECHANICAL);
    REQUIRE(issues->getIssue(id) == Issues::kEmptyIssueModel);
    game.getActions()->handleAction(0, std::make_unique<Observer>(game));

    // Should still be empty as inventory requirements aren't met
    id = issues->popRandomIssueId(IssueModel::Type::MECHANICAL);
    REQUIRE(issues->getIssue(id) == Issues::kEmptyIssueModel);

    // NOW we should get the issue as all requirments have been met
    game.addItemToInventory(0);
    id = issues->popRandomIssueId(IssueModel::Type::MECHANICAL);
    REQUIRE_FALSE(issues->getIssue(id) == Issues::kEmptyIssueModel);
  }

  SECTION("Issues::getIssuesThatHaveAlreadyHappened") {
    auto haveHappened = issues->getIssuesThatHaveAlreadyHappened();
    REQUIRE(haveHappened.empty());

    auto id = issues->popRandomIssueId(IssueModel::Type::HEALTH);
    issues->handleIssue(id, std::make_unique<Observer>(game));
    haveHappened = issues->getIssuesThatHaveAlreadyHappened();
    REQUIRE(haveHappened.count(0));
  }
}

namespace TestIssueEndingHints {
std::mutex mutex;
bool running = true;
std::condition_variable simulation;
const std::chrono::milliseconds timeout(1000);
}
TEST_CASE("Issues - Ending Hints") {
  const char *eventJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "",
      "display_name": "",
      "mile": 100
    }
  ]
  )";
  const char *issuesJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "ending_id_hints": [0],
      "description": "HEALTHL ISSUE",
      "display_name": "",
      "image_url": "",
      "stat_changes": [
        {"health": -10000}
      ],
      "type": "HEALTH"
    }
  ]
  )";
  StatModel autoDeath;
  autoDeath.odds_health_issue = 1;

  Game game("", validActionJson, validEndingJson, eventJson, issuesJson, validItemJson);
  game.updateStats(autoDeath);

  class SuicideObserver : public IEventObserver {
  public:
    explicit SuicideObserver(Game &game) : IEventObserver(game) {}
    void onGameOver(EndingModel &ending) override {
      REQUIRE(ending.id == 0);
      TestIssueEndingHints::running = false;
      TestIssueEndingHints::simulation.notify_one();
    }
    void onHourChanged(int32_t hour) override {}
    void onMileChanged(int32_t mile) override {}
    bool onEvent(EventModel &event) override {
      return false; }
    bool onIssueOccurred(IssueModel &issue) override {
      issue.chooseAction(0);
      return true;
    }
    void onStatsChanged(StatModel &stats) override {}
    bool onStoreEntered(ActionModel &action) override { return false; }
  };

  auto observer = std::make_unique<SuicideObserver>(game);
  game.registerEventObserver(std::move(observer));

  Engine engine(game);
  engine.start();

  std::unique_lock<std::mutex> lock(TestIssueEndingHints::mutex);
  if (!TestIssueEndingHints::simulation.wait_until(
      lock,
      std::chrono::system_clock::now() + TestIssueEndingHints::timeout,
      []() { return !TestIssueEndingHints::running; })) {
    FAIL("Timed out while waiting for the test to simulate death. Profound.");
  }
}
