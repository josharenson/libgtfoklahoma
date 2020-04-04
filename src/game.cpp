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

#include <libgtfoklahoma/game.hpp>

#include <spdlog/spdlog.h>

using namespace libgtfoklahoma;

// We need our own observer to keep track of certain changes
class InternalObserver : public IEventObserver {
public:
    explicit InternalObserver(Game &game)
    : m_game(game) {}

    void onHourChanged(int32_t hour) override { m_game.setCurrentHour(hour); }
    void onMileChanged(int32_t mile) override { m_game.setCurrentMile(mile); }
    bool onEvent(EventModel&, std::vector<std::reference_wrapper<ActionModel>>&) override { return false; }
    bool onStoreEntered(ActionModel&, std::vector<ItemModel>&) override { return false; }

  private:
    Game &m_game;
};

Game::Game(std::string name)
: m_actions(std::make_unique<Actions>(*this))
, m_currentHour(0)
, m_currentMile(0)
, m_events(std::make_unique<Events>(*this))
, m_internalObserver(std::make_unique<InternalObserver>(*this))
, m_issues(std::make_unique<Issues>(*this))
, m_items(std::make_unique<Items>())
, m_name(std::move(name))
, m_stats(std::make_unique<StatModel>()) {}

Game::Game(std::string name,
           const char *actionJson,
           const char *eventJson,
           const char *issueJson,
           const char *itemJson)
: m_actions(std::make_unique<Actions>(*this, actionJson))
, m_currentHour(0)
, m_currentMile(0)
, m_events(std::make_unique<Events>(*this, eventJson))
, m_internalObserver(std::make_unique<InternalObserver>(*this))
, m_issues(std::make_unique<Issues>(*this, issueJson))
, m_items(std::make_unique<Items>(itemJson))
, m_name(std::move(name))
, m_stats(std::make_unique<StatModel>()) {}

/** Access game components */
std::unique_ptr<Actions> &Game::getActions() { return m_actions; }
std::unique_ptr<Events> &Game::getEvents() { return m_events; }
std::unique_ptr<Issues> &Game::getIssues() { return m_issues; }
std::unique_ptr<Items> &Game::getItems() { return m_items; }
std::unique_ptr<StatModel> &Game::getStats() { return m_stats; }

/** Distance management */
int32_t Game::getCurrentMile() const { return m_currentMile; }
void Game::setCurrentMile(const int32_t mile) { m_currentMile = mile; }

/** Event management */
std::vector<int32_t> Game::getQueuedEventIds() const {
 return m_events->eventsAtMile(m_currentMile);
}

/** Internal bits */
// Engine needs to notify Game about events, but if Game had a reference to Engine to
// do this itself, there would be a circular dependency. This provides Engine a
// mechanism to register the internal observer itself.
std::unique_ptr<IEventObserver> Game::getInternalObserver() {
  return std::move(m_internalObserver);
}

/** Inventory management */
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

bool Game::hasItemInInventory(int32_t id) const {
  return m_inventory.count(id);
}

std::vector<std::reference_wrapper<ItemModel>> Game::getInventory() const {
  std::vector<std::reference_wrapper<ItemModel>> result;
  for (const auto &item : m_inventory) {
    for (int i = 0; i < item.second; i++) {
      result.emplace_back(m_items->getItem(item.first));
    }
  }
  return result;
}

/** Stats managememt */
void Game::updateStats(const StatModel &delta) { *m_stats = *m_stats + delta; }

/** Time management */
int32_t Game::getCurrentHour() const { return m_currentHour; }
void Game::setCurrentHour(int32_t hour) { m_currentHour = hour; }

