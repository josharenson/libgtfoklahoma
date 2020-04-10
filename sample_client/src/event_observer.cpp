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

#include <event_observer.hpp>

// Library includes
#include <libgtfoklahoma/action_model.hpp>
#include <libgtfoklahoma/event_model.hpp>
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/item_model.hpp>

// System includes
#include <utility>

// 3P includes
#include <spdlog/spdlog.h>

// Local includes
#include <ui.hpp>
#include <ui_utils.hpp>

using namespace gtfoklahoma;
using namespace libgtfoklahoma;

EventObserver::EventObserver(Game &game, Ui &ui)
: m_ui(ui)
, IEventObserver(game) {}

void EventObserver::onHourChanged(int32_t hour) {
  spdlog::debug("Current hour is {}", hour);
  m_ui.renderStats(m_game.getStats()->getPlayerStatsModel(), m_game.getCurrentMile(), hour);
}

void EventObserver::onMileChanged(int32_t mile) {
  spdlog::debug("Current mile is {}", mile);
  m_ui.renderStats(m_game.getStats()->getPlayerStatsModel(), mile, m_game.getCurrentHour());
}

bool EventObserver::onEvent(EventModel &event) {
  spdlog::debug("POI Encountered-> " + event.display_name);

  std::vector<std::reference_wrapper<ActionModel>> actions;
  for (const auto &actionId : event.action_ids) {
    actions.emplace_back(m_game.getActions()->getAction(actionId));
  }

  m_ui.renderEvent(event, actions);

  // Convert the choice into an action id and ensure its a valid action id
  const auto validator = [actions, &event]  (int32_t i) -> bool {
    if (i >= event.action_ids.size()) { return false; }
    auto id = actions.at(i).get().id;
    return event.actionIdIsValid(id);
  };

  auto result = UIUtils::getInputInt(m_ui.inputBar(),"Please enter a number-> ", validator);
  event.chooseAction(actions.at(result).get().id);
  return true;
}

bool EventObserver::onIssueOccurred(libgtfoklahoma::IssueModel &issue) {
  spdlog::debug("Issue {} occurred", issue.id);

  std::vector<std::reference_wrapper<ActionModel>> actions;
  for (const auto &actionId : issue.actions) {
    actions.emplace_back(m_game.getActions()->getAction(actionId));
  }

  m_ui.renderIssue(issue, actions);

  // Convert the choice into an action id and ensure its a valid action id
  const auto validator = [actions, &issue]  (int32_t i) -> bool {
    if (i >= issue.actions.size()) { return false; }
    auto id = actions.at(i).get().id;
    return issue.actionIdIsValid(id);
  };

  auto result = UIUtils::getInputInt(m_ui.inputBar(),"Please enter a number-> ", validator);
  issue.chooseAction(actions.at(result).get().id);
  return true;
}

void EventObserver::onStatsChanged(libgtfoklahoma::StatModel &stats) {
  m_ui.renderStats(stats, m_game.getCurrentMile(), m_game.getCurrentHour());
}

bool EventObserver::onStoreEntered(ActionModel &action) {
  spdlog::debug("Entered a store!");

  std::vector<std::reference_wrapper<ItemModel>> items;
  for (const auto &itemId : action.item_ids) {
    items.emplace_back(m_game.getItems()->getItem(itemId));
  }

  m_ui.renderStore(action, items);

  // Convert the choice into an item id and ensure its a valid item id
  // If the response is valid, attempt to purchase the item.
  const auto validator = [&action, &items]  (int32_t i) -> bool {
    if (i > items.size()) { return false; }
    if (i == items.size()) { return true; } // Is "Leave Store" item.
    auto id = items.at(i).get().id;
    return (action.itemIsInStock(id) && action.purchaseItem(items.at(i).get().id));
  };

  int32_t result = -1;
  // "Leave Store" is added to the items list so its the last item and indicates
  // that the player wants to.... leave the store.
  while (result != items.size()) {
    result = UIUtils::getInputInt(m_ui.inputBar(), "Please enter a number-> ",
                                  validator);

  }
  spdlog::debug("Purchase complete");
  spdlog::default_logger()->flush();
  action.completePurchase();
  return true;
}

