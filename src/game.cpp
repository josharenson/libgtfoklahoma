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
    : IEventObserver(game) {}

    void onHourChanged(int32_t hour) override { m_game.setCurrentHour(hour); }
    void onMileChanged(int32_t mile) override { m_game.setCurrentMile(mile); }
    bool onEvent(EventModel&) override { return false; }
    bool onIssueOccurred(IssueModel&) override { return false; }
    void onStatsChanged(StatModel&) override {}
    bool onStoreEntered(ActionModel&) override { return false; }
};

Game::Game(std::string name)
    : m_actions(std::make_unique<Actions>(*this)), m_currentHour(0),
      m_currentMile(0), m_events(std::make_unique<Events>(*this)),
      m_issues(std::make_unique<Issues>(*this)),
      m_items(std::make_unique<Items>()), m_name(std::move(name)),
      m_stats(std::make_unique<StatModel>(
          rules::kDefaultBedtimeHour,
          rules::kDefaultHealth,
          0,
          1,
          rules::kDefaultMoneyRemaining,
          rules::kDefaultOddsHealthIssuePerHour,
          rules::kDefaultOddsMechanicalIssuePerHour,
          StatModel::Pace::FRED,
          rules::kDefaultWakeupHour)) {
  m_observers.emplace_back(std::make_unique<InternalObserver>(*this));
}

Game::Game(std::string name,
           const char *actionJson,
           const char *eventJson,
           const char *issueJson,
           const char *itemJson)
: m_actions(std::make_unique<Actions>(*this, actionJson))
, m_currentHour(0)
, m_currentMile(0)
, m_events(std::make_unique<Events>(*this, eventJson))
, m_issues(std::make_unique<Issues>(*this, issueJson))
, m_items(std::make_unique<Items>(itemJson))
, m_name(std::move(name))
, m_stats(std::make_unique<StatModel>(
        rules::kDefaultBedtimeHour,
        rules::kDefaultHealth,
        0,
        1,
        rules::kDefaultMoneyRemaining,
        rules::kDefaultOddsHealthIssuePerHour,
        rules::kDefaultOddsMechanicalIssuePerHour,
        StatModel::Pace::FRED,
        rules::kDefaultWakeupHour)) {
  m_observers.emplace_back(std::make_unique<InternalObserver>(*this));
}

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

/** Inventory management */
void Game::addItemToInventory(int32_t id, int32_t quantity) {
  auto item = getItems()->getItem(id);
  for (int i = 0; i < quantity; i ++) {
    updateStats(item.stat_delta);
  }

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

/** Observer management */
std::vector<std::reference_wrapper<std::unique_ptr<IEventObserver>>>
Game::getObservers() {
  std::vector<std::reference_wrapper<std::unique_ptr<IEventObserver>>> result;
  for (auto &observer : m_observers) {
    result.emplace_back(observer);
  }
  return result;
}

void Game::registerEventObserver(std::unique_ptr<IEventObserver> observer) {
  m_observers.emplace_back(std::move(observer));
}

/** Stats managememt */
bool Game::isAwake() const {
  return m_currentHour >= m_stats->wakeup_hour &&
         m_currentHour < m_stats->bedtime_hour;
}

void Game::updateStats(const StatModel &delta) { *m_stats = *m_stats + delta;
  for (const auto &observer : m_observers) {
    observer->onStatsChanged(*m_stats);
  }
}

/** Time management */
int32_t Game::getCurrentHour() const { return m_currentHour; }
void Game::setCurrentHour(int32_t hour) { m_currentHour = hour; }

