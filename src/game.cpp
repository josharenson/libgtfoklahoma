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

#include <spdlog/spdlog.h>

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/stats.hpp>

using namespace libgtfoklahoma;

Game::Game(std::string name)
: m_actions(std::make_unique<Actions>(*this))
, m_currentHour(0)
, m_currentMile(0)
, m_currentTick(0)
, m_items(std::make_unique<Items>())
, m_name(std::move(name))
, m_stats(std::make_unique<StatModel>()) {}

Game::Game(std::string name,
           const char *actionJson,
           const char *eventJson,
           const char *itemJson)
: m_actions(std::make_unique<Actions>(*this, actionJson))
, m_currentHour(0)
, m_currentMile(0)
, m_currentTick(0)
, m_items(std::make_unique<Items>(itemJson))
, m_name(std::move(name))
, m_stats(std::make_unique<StatModel>()) {}

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

void Game::addItemToInventory(int32_t id, int32_t quantity) {
  if (m_inventory.count(id)) {
    m_inventory[id] += quantity;
  } else {
    m_inventory[id] = quantity;
  }
}

void Game::removeItemFromInventory(int32_t id, int32_t quantity) {
  if (!m_inventory.count(id)) {
    spdlog::warn("Trying to remove an item that you don't have!");
    return;
  }

  if (m_inventory[id] <= quantity) {
    m_inventory.erase(id);
  } else {
    m_inventory[id] -= quantity;
  }
}

std::vector<ItemModel> Game::getInventory() const {
  std::vector<ItemModel> result;
  for (const auto &item : m_inventory) {
    for (int i = 0; i < item.second; i++) {
      result.push_back(m_items->getItem(item.first));
    }
  }
  return result;
}

void Game::updateStats(StatModel &delta) {
  *m_stats = *m_stats + delta;
}

