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
#include <libgtfoklahoma/event_observer.hpp>
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

  // Access game components
  std::unique_ptr<Actions> &getActions();
  std::unique_ptr<Events> &getEvents();
  std::unique_ptr<Issues> &getIssues();
  std::unique_ptr<Items> &getItems();
  std::unique_ptr<StatModel> &getStats();

  // Distance management
  [[nodiscard]] int32_t getCurrentMile() const;
  void setCurrentMile(int32_t mile);

  // Event management
  [[nodiscard]] std::vector<int32_t> getQueuedEventIds() const;

  // Internal bits
  std::unique_ptr<IEventObserver> getInternalObserver();

  // Inventory management
  void addItemToInventory(int32_t id, int32_t quantity=1);
  void removeItemFromInventory(int32_t id, int32_t quantity=1);
  [[nodiscard]] bool hasItemInInventory(int32_t id) const;
  [[nodiscard]] std::vector<std::reference_wrapper<ItemModel>> getInventory() const;

  // Stat managmeent
  void updateStats(const StatModel &delta);

  // Time management
  [[nodiscard]] int32_t getCurrentHour() const;
  void setCurrentHour(int32_t hour);

private:
  std::unique_ptr<Actions> m_actions;
  int32_t m_currentHour;
  int32_t m_currentMile;
  std::unique_ptr<Events> m_events;
  std::unique_ptr<IEventObserver> m_internalObserver;
  std::map<int32_t, int32_t> m_inventory;
  std::unique_ptr<Items> m_items;
  std::unique_ptr<Issues> m_issues;
  std::string m_name;
  std::unique_ptr<StatModel> m_stats;
};
}
