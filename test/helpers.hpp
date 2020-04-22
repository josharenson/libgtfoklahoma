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

#pragma once

#include <libgtfoklahoma/event_observer.hpp>

namespace testhelpers {

// Used to instantiate a Game when you don't care about these types
static const char *validActionJson = R"(
  [
    {
      "id": 0,
      "display_name": "display_name_0",
      "type": ["STAT_CHANGE"],
      "stat_changes_regardless": [{}]
    }
  ]
  )";

static const char *validEndingJson = R"(
  [
    {
      "id": 0,
      "description": "description_0",
      "display_name": "display_name_0",
      "image_tag": "image_tag_0"
    }
  ]
  )";

static const char *validEventJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "description_0",
      "display_name": "display_name_0",
      "ending_id_hints": [0],
      "mile": 0
    }
  ]
  )";

static const char *validIssueJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "Fake Health Issue",
      "display_name": "Fake News!",
      "image_url": "",
      "type": "HEALTH"
    }
  ]
  )";

static const char *validItemJson = R"(
  [
    {
        "id": 0,
        "category": "BIKE",
        "cost": 420,
        "display_name": "Taco Item",
        "image_url": "teapot.svg",
        "stat_changes": [{}]
    }
  ]
  )";

class TestObserver : public libgtfoklahoma::IEventObserver {
public:
  explicit TestObserver(libgtfoklahoma::Game &game) : IEventObserver(game) {}
  void onGameOver(const libgtfoklahoma::EndingModel &ending) override {}
  void onHourChanged(int32_t hour) override {}
  void onMileChanged(int32_t mile) override {}
  bool onEvent(libgtfoklahoma::EventModel &event) override { return false; }
  bool onIssueOccurred(libgtfoklahoma::IssueModel &issue) override { return false; }
  void onStatsChanged(const libgtfoklahoma::StatModel &stats) override {}
  bool onStoreEntered(libgtfoklahoma::ActionModel &action) override { return false; }
};

class EngineStopper {
public:
  EngineStopper()
  : m_running(true)
  , m_timeout(1000) {}

  void stopEngine() {
    m_running = false;
    m_simulation.notify_one();
  }

  void waitForEngineToStopOrFail () {
    std::unique_lock<std::mutex> lock(EngineStopper::mutex);
    if (!m_simulation.wait_until(
        lock,
        std::chrono::system_clock::now() + m_timeout,
        [this]() { return !m_running; })) {
      FAIL("Timed out while waiting for the test to simulate death. Profound.");
    }
  };
private:
  std::mutex mutex;
  bool m_running = true;
  std::condition_variable m_simulation;
  const std::chrono::milliseconds m_timeout;
};


}
