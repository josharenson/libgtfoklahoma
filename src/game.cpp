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

#include <libgtfoklahoma/game.hpp>

using namespace libgtfoklahoma;

Game::Game(std::string name)
: m_currentHour(0)
, m_currentMile(0)
, m_currentTick(0)
, m_name(std::move(name)) {}

int32_t Game::hour() const { return m_currentHour % 24; }
int32_t Game::bumpHour() {
  m_currentHour++;
  return hour();
}
void Game::setHour(const int32_t hour) { m_currentHour = hour; }

int32_t Game::mile() const { return m_currentMile; }
int32_t Game::bumpMile() {
  m_currentMile++;
  return mile();
}
void Game::setMile(const int32_t mile) { m_currentMile = mile; }

int64_t Game::tick() const { return m_currentTick; }
int64_t Game::bumpTick() {
  m_currentTick++;
  return tick();
}
void Game::setTick(const int64_t tick) { m_currentTick = tick; }

int32_t Game::ticksUntiNextMile() const {
  return 5;
}