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
#include <libgtfoklahoma/event_observer.hpp>
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/rules.hpp>

using namespace libgtfoklahoma;

Engine::Engine(Game &game)
: m_running(false)
, m_game(game) {}

Engine::~Engine() { stop(); }

void Engine::registerEventObserver(std::unique_ptr<IEventObserver> observer) {
  m_eventObservers.emplace_back(std::move(observer));
}

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
  int32_t ticksUntilNextMile = m_game.ticksUntiNextMile();

  while (m_running) {
    std::this_thread::sleep_for(rules::kTickDelayMs);


    // Update the time
    if (!(m_game.tick() % rules::kTicksPerGameHour)) {
      auto new_hour = m_game.bumpHour();
      for (const auto &observer : m_eventObservers) {
        observer->onHourChanged(new_hour);
      }
    }

    if (!ticksUntilNextMile) {
      ticksUntilNextMile = m_game.ticksUntiNextMile();
      m_game.setCurrentMile(m_game.currentMile() + 1);
      for (const auto &observer : m_eventObservers) {
        observer->onMileChanged(m_game.currentMile());
        handleEventsAtMile(m_game.currentMile(), observer);
      }
    }

    m_game.bumpTick();
    ticksUntilNextMile--;
  }
}

void Engine::handleEventsAtMile(int32_t mile, const std::unique_ptr<IEventObserver> &observer) {
  auto eventIds = m_game.getEvents()->eventsAtMile(mile);
  for (const auto &eventId : eventIds) {
    m_game.getEvents()->handleEvent(eventId, observer);
  }
}
