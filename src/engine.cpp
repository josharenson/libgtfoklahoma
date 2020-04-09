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

#include <libgtfoklahoma/engine.hpp>

// System includes
#include <chrono>

// 3P Includes
#include <spdlog/spdlog.h>

// Local includes
#include <libgtfoklahoma/issues.hpp>
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/rules.hpp>

using namespace libgtfoklahoma;

Engine::Engine(Game &game)
: m_game(game)
, m_running(false) {}

Engine::~Engine() { stop(); }

void Engine::start() {
  spdlog::debug("Starting Engine's event loop");
  m_running = true;
  m_eventLoopThread = std::thread(&Engine::mainLoop, this);
}

void Engine::stop() {
  spdlog::debug("Stopping Engine's event loop");
  m_running = false;
  if (m_eventLoopThread.joinable()) { m_eventLoopThread.join(); }
}

void Engine::mainLoop() {
  // Events are checked once per mile
  bool shouldCheckForEvents = true;

  // Issues are checked once per hour
  bool shouldCheckForHealthIssues = false;
  bool shouldCheckForMechanicalIssues = false;

  uint32_t tick = 0;
  int32_t ticksUntilNextMile = rules::TicksUntilNextMile(*m_game.getStats());

  while (m_running) {
    std::this_thread::sleep_for(rules::kTickDelayMs);

    // Handle queued events ensuring that this is only called once per mile
    if (shouldCheckForEvents) {
      for (const auto &observer : m_game.getObservers()) {
        for (const auto &id : m_game.getQueuedEventIds()) {
          m_game.getEvents()->handleEvent(id, observer);
        }
      }
      shouldCheckForEvents = false;
    }

    // Handle any mech issues
    if (shouldCheckForMechanicalIssues &&
        m_game.isAwake() &&
        rules::MechanicalIssueThisHour(*m_game.getStats())) {
      auto id = m_game.getIssues()->popRandomIssueId(IssueModel::Type::MECHANICAL);

      if (id == -1) {
        spdlog::debug("No more mechanical issues available!");
      } else {
        for (const auto &observer : m_game.getObservers()) {
          m_game.getIssues()->handleIssue(id, observer);
        }
      }
    }
    shouldCheckForMechanicalIssues = false;

    // Handle any health issues
    if (shouldCheckForHealthIssues && rules::HealthIssueThisHour(*m_game.getStats())) {
      auto id = m_game.getIssues()->popRandomIssueId(IssueModel::Type::HEALTH);

      if (id == -1) {
        spdlog::debug("No more health issues available!");
      } else {
        for (const auto &observer : m_game.getObservers()) {
          m_game.getIssues()->handleIssue(id, observer);
        }
      }
    }
    shouldCheckForHealthIssues = false;

    // Update the time
    if (!(tick % rules::kTicksPerGameHour)) {
      auto new_hour = getNextHour();
      for (const auto &observer : m_game.getObservers()) {
        observer.get()->onHourChanged(new_hour);
      }
      shouldCheckForHealthIssues = true;
      shouldCheckForMechanicalIssues = true;
    }

    if (!ticksUntilNextMile) {
      ticksUntilNextMile = rules::TicksUntilNextMile(*m_game.getStats());
      auto new_mile = m_game.getCurrentMile() + 1;
      for (const auto &observer : m_game.getObservers()) {
        observer.get()->onMileChanged(new_mile);
      }
      shouldCheckForEvents = true;
    }

    tick++;
    ticksUntilNextMile--;
  }
}

int32_t Engine::getNextHour() const {
  return (m_game.getCurrentHour() + 1) % 24;
}
