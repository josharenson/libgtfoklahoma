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

#include <cstdint>
#include <functional>
#include <map>
#include <string>

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/events.hpp>
#include <libgtfoklahoma/issues.hpp>
#include <libgtfoklahoma/items.hpp>
#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {

class Game {
public:
  explicit Game(std::string name);
  explicit Game(std::string name,
                const char *actionJson,
                const char *eventJson,
                const char *issueJson,
                const char *itemJson);

  [[nodiscard]] int32_t hour() const;
  int32_t bumpHour();
  void setHour(int32_t hour);

  [[nodiscard]] int32_t currentMile() const;
  void setCurrentMile(int32_t mile);

  [[nodiscard]] int64_t tick() const;
  int64_t bumpTick();
  void setTick(int64_t tick);

  // TODO: Make dynamic based on stats
  [[nodiscard]] int32_t ticksUntiNextMile() const;

  std::unique_ptr<Actions> &getActions() { return m_actions; }
  std::unique_ptr<Events> &getEvents() { return m_events; }
  std::unique_ptr<Issues> &getIssues() { return m_issues; }
  std::unique_ptr<Items> &getItems() { return m_items; }
  std::unique_ptr<StatModel> &getStats() { return m_stats; }

  void addItemToInventory(int32_t id, int32_t quantity=1);
  void removeItemFromInventory(int32_t id, int32_t quantity=1);
  [[nodiscard]] bool hasItemInInventory(int32_t id) const;
  [[nodiscard]] std::vector<std::reference_wrapper<ItemModel>> getInventory() const;

  void updateStats(const StatModel &delta);

private:
  std::unique_ptr<Actions> m_actions;
  int32_t m_currentHour;
  int32_t m_currentMile;
  int64_t m_currentTick;
  std::unique_ptr<Events> m_events;
  std::map<int32_t, int32_t> m_inventory;
  std::unique_ptr<Items> m_items;
  std::unique_ptr<Issues> m_issues;
  std::string m_name;
  std::unique_ptr<StatModel> m_stats;
};
}
