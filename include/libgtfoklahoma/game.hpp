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

#pragma once

#include <cstdint>
#include <map>
#include <string>

#include <libgtfoklahoma/items.hpp>

namespace libgtfoklahoma {

class Actions;
struct StatModel;
class Game {
public:
  explicit Game(std::string name);
  explicit Game(std::string name,
                const char *actionJson,
                const char *eventJson,
                const char *itemJson);

  [[nodiscard]] int32_t hour() const;
  int32_t bumpHour();
  void setHour(int32_t hour);

  [[nodiscard]] int32_t mile() const;
  int32_t bumpMile();
  void setMile(int32_t mile);

  [[nodiscard]] int64_t tick() const;
  int64_t bumpTick();
  void setTick(int64_t tick);

  // TODO: Make dynamic based on stats
  [[nodiscard]] int32_t ticksUntiNextMile() const;

  std::unique_ptr<Actions> &getActions() { return m_actions; }
  std::unique_ptr<Items> &getItems() { return m_items; }

  void addItemToInventory(int32_t id, int32_t quantity);
  void removeItemFromInventory(int32_t id, int32_t quantity=1);
  [[nodiscard]] std::vector<ItemModel> getInventory() const;

  std::unique_ptr<StatModel> &getStats() { return m_stats; }
  void updateStats(const StatModel &delta);

private:
  std::unique_ptr<Actions> m_actions;
  int32_t m_currentHour;
  int32_t m_currentMile;
  int64_t m_currentTick;
  std::map<int32_t, int32_t> m_inventory;
  std::unique_ptr<Items> m_items;
  std::string m_name;
  std::unique_ptr<StatModel> m_stats;
};
}