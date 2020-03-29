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

#include <libgtfoklahoma/engine.hpp>

// System includes
#include <chrono>

// 3P Includes
#include <spdlog/spdlog.h>

// Local includes
#include <libgtfoklahoma/event_observer.hpp>
#include <libgtfoklahoma/rules.hpp>

using namespace libgtfoklahoma;

Engine::Engine(Game game)
: m_running(false)
, m_events(Events(game.mile()))
, m_nextEvent(m_events.nextEvent())
, m_game(std::move(game)) {}

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

    if (!(m_game.tick() % rules::kTicksPerGameHour)) {
      auto new_hour = m_game.bumpHour();
      for (const auto &observer : m_eventObservers) {
        observer->onHourChanged(new_hour);
      }
    }

    if (m_nextEvent.mile == m_game.mile()) {
      for (const auto &observer : m_eventObservers) {
        observer->onEvent(m_nextEvent);
        auto actionIdToPerform = m_nextEvent.chosenAction().get();
        m_game.getActions()->performAction(actionIdToPerform);
      }
      m_nextEvent = m_events.nextEvent();
    }

    if (!ticksUntilNextMile) {
      ticksUntilNextMile = m_game.ticksUntiNextMile();
      auto new_mile = m_game.bumpMile();
      for (const auto &observer : m_eventObservers) {
        observer->onMileChanged(new_mile);
      }
    }

    m_game.bumpTick();
    ticksUntilNextMile--;
  }

}
