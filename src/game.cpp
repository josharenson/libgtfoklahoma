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

#include <libgtfoklahoma/stat_model.hpp>

using namespace libgtfoklahoma;

// We need our own observer to keep track of certain changes
class InternalObserver : public IEventObserver {
public:
    explicit InternalObserver(Game &game)
    : IEventObserver(game) {}

    void onGameOver(const EndingModel&) override {}
    void onHourChanged(int32_t hour) override { m_game.setCurrentHour(hour); }
    void onMileChanged(int32_t mile) override { m_game.setCurrentMile(mile); }
    bool onEvent(EventModel&) override { return false; }
    bool onIssueOccurred(IssueModel&) override { return false; }
    void onStatsChanged(const StatModel&) override {}
    bool onStoreEntered(ActionModel&) override { return false; }
};

Game::Game(std::string name)
: m_actions(Actions(*this))
, m_currentHour(0)
, m_currentMile(0)
, m_endings(Endings())
, m_events(Events(*this))
, m_issues(Issues(*this))
, m_items(Items())
, m_name(std::move(name))
, m_stats(Stats(
          *this,
          StatModel(
              rules::kDefaultBedtimeHour,
              rules::kDefaultHealth,
              0,
              1,
              rules::kDefaultMoneyRemaining,
              rules::kDefaultOddsHealthIssuePerHour,
              rules::kDefaultOddsMechanicalIssuePerHour,
              StatModel::Pace::FRED,
              rules::kDefaultWakeupHour))) {
  m_observers.emplace_back(std::make_unique<InternalObserver>(*this));
}

Game::Game(std::string name,
           const char *actionJson,
           const char *endingJson,
           const char *eventJson,
           const char *issueJson,
           const char *itemJson)
: m_actions(Actions(*this, actionJson))
, m_currentHour(0)
, m_currentMile(0)
, m_endings(Endings(endingJson))
, m_events(Events(*this, eventJson))
, m_issues(Issues(*this, issueJson))
, m_items(Items(itemJson))
, m_name(std::move(name))
, m_stats(Stats(
          *this,
          StatModel(
              rules::kDefaultBedtimeHour,
              rules::kDefaultHealth,
    0,
       1,
               rules::kDefaultMoneyRemaining,
               rules::kDefaultOddsHealthIssuePerHour,
               rules::kDefaultOddsMechanicalIssuePerHour,
         StatModel::Pace::FRED,
               rules::kDefaultWakeupHour))) {
  m_observers.emplace_back(std::make_unique<InternalObserver>(*this));
}

/** Access game components */
Actions &Game::getActions() { return m_actions; }
Events &Game::getEvents() { return m_events; }
Issues &Game::getIssues() { return m_issues; }
Items &Game::getItems() { return m_items; }
Stats &Game::getStats() { return m_stats; }

/** Distance management */
int32_t Game::getCurrentMile() const { return m_currentMile; }
void Game::setCurrentMile(const int32_t mile) { m_currentMile = mile; }

/** Ending management */
bool Game::gameOver() {
  return m_stats.getPlayerStatsModel().health <= 0 ||
         !m_events.hasMoreEvents(m_currentMile);
}

Endings &Game::getEndings() { return m_endings; }

int32_t Game::popEndingHintId() {
  if (m_endingHints.empty()) {
    spdlog::error("Trying to end the game but there are no ending hints on the "
                  "ending stack!");
    return -1;
  }

  auto result = m_endingHints.top();
  m_endingHints.pop();
  return result;
}

void Game::pushEndingHintId(int32_t endingId) {
  m_endingHints.push(endingId);
}

/** Event management */
std::vector<int32_t> Game::getQueuedEventIds() const {
 return m_events.eventsAtMile(m_currentMile);
}

/** Inventory management */
void Game::addItemToInventory(int32_t id, int32_t quantity) {
  auto item = getItems().getItem(id);
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

int32_t Game::inventoryCount(int32_t id) const {
  return m_inventory.count(id);
}

std::vector<std::reference_wrapper<ItemModel>> Game::getInventory() {
  std::vector<std::reference_wrapper<ItemModel>> result;
  for (const auto &item : m_inventory) {
    for (int i = 0; i < item.second; i++) {
      result.emplace_back(m_items.getItem(item.first));
    }
  }
  return result;
}

/** Observer management */
std::vector<std::shared_ptr<IEventObserver>>
Game::getObservers() {
  return m_observers;
}

void Game::registerEventObserver(std::shared_ptr<IEventObserver> observer) {
  m_observers.emplace_back(std::move(observer));
}

/** Stats management */
bool Game::playerIsAwake() const {
  return m_currentHour >= m_stats.getPlayerStatsModel().wakeup_hour &&
         m_currentHour < m_stats.getPlayerStatsModel().bedtime_hour;
}

void Game::updateStats(const StatModel &delta) {
  StatModel cpy(m_stats.getPlayerStatsModel());
  m_stats.setPlayerStatsModel(cpy + delta);
  for (const auto &observer : m_observers) {
    observer->onStatsChanged(m_stats.getPlayerStatsModel());
  }
}

/** Time management */
int32_t Game::getCurrentHour() const { return m_currentHour; }
void Game::setCurrentHour(int32_t hour) { m_currentHour = hour; }
