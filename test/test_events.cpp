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

namespace TestEventEndingHints {
std::mutex mutex;
bool running = true;
std::condition_variable simulation;
const std::chrono::milliseconds timeout(1000);
}
TEST_CASE("Events - Ending Hints") {

  // The game should end happily as we reach the final destination safely
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

  Game game("", validActionJson, validEndingJson, eventJson, validIssueJson, validItemJson);

  class SuicideObserver : public IEventObserver {
  public:
    explicit SuicideObserver(Game &game) : IEventObserver(game) {}
    void onGameOver(EndingModel &ending) override {
      REQUIRE(ending.id == 0);
      TestEventEndingHints::running = false;
      TestEventEndingHints::simulation.notify_one();
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

  auto observer = std::make_unique<SuicideObserver>(game);
  game.registerEventObserver(std::move(observer));

  Engine engine(game);
  engine.start();

  std::unique_lock<std::mutex> lock(TestEventEndingHints::mutex);
  if (!TestEventEndingHints::simulation.wait_until(
      lock,
      std::chrono::system_clock::now() + TestEventEndingHints::timeout,
      []() { return !TestEventEndingHints::running; })) {
    FAIL("Timed out while waiting for the test to simulate death. Profound.");
  }
}
