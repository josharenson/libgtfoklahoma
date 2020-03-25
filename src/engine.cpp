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

#include <libgtfoklahoma/constants.hpp>
#include <libgtfoklahoma/engine.hpp>

#include <thread>


using namespace libgtfoklahoma;

Engine::Engine(Game game)
: m_events(Events(game.mile()))
, m_nextEvent(m_events.nextEvent())
, m_game(std::move(game)) {}

void Engine::registerEventObserver(std::unique_ptr<IEventObserver> observer) {
  m_eventObservers.emplace_back(std::move(observer));
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void Engine::run() {
  int32_t ticksUntilNextMile = m_game.ticksUntiNextMile();

  for (;;) {
    std::this_thread::sleep_for(kTickDelayMs);
    m_game.bumpTick();
    ticksUntilNextMile--;

    if (!(m_game.tick() % kTicksPerGameHour)) {
      auto new_hour = m_game.bumpHour();
      for (const auto &observer : m_eventObservers) {
        observer->onHourChanged(new_hour);
      }
    }

    if (m_nextEvent.mile == m_game.mile()) {
      for (const auto &observer : m_eventObservers) {
        observer->onEvent(m_nextEvent);
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
  }
}
#pragma clang diagnostic pop
