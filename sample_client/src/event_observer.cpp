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
#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/events.hpp>
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/items.hpp>

// System includes
#include <iostream>
#include <utility>

// 3P includes
#include <spdlog/spdlog.h>

using namespace gtfoklahoma;
using namespace libgtfoklahoma;

EventObserver::EventObserver(Game &game)
: IEventObserver(game) {}

void EventObserver::onHourChanged(int32_t hour) {
  spdlog::debug("Current hour is {}", hour);
}

void EventObserver::onMileChanged(int32_t mile) {
  spdlog::debug("Current mile is {}", mile);
}

bool EventObserver::onEvent(EventModel &event) {
  spdlog::debug("POI Encountered-> " + event.display_name);
  int32_t result;

  std::cout << "Welcome to " + event.display_name << std::endl;

  int32_t choice = 0;
  auto actionIds = event.action_ids;
  for (const auto &actionId :actionIds) {
    auto &action = m_game.getActions()->getAction(actionId);

    std::cout << "\t" + std::to_string(choice) + ". " + action.display_name
    << std::endl;
    choice++;
  }

  while (true) {
    std::cout << "Enter a number-> ";
    std::cin >> result;
    if (actionIds.size() >= result && event.chooseAction(actionIds.at(result))) {
      break;
    } else {
      std::cout << "\nInvalid choice!" << std::endl;
    }
  }

  return true;
}

bool EventObserver::onIssueOccurred(libgtfoklahoma::IssueModel &issue) {
  spdlog::debug("Issue {} occurred", issue.id);
  std::cout << issue.display_name << std::endl;

  auto actionIds = issue.actions;
  int32_t choice = 0;
  for (const auto &actionId : actionIds) {
    auto &action = m_game.getActions()->getAction(actionId);
    std::cout << "\t" + std::to_string(choice) + ". " + action.display_name
              << std::endl;
    choice++;
  }

  int32_t result;
  while (true) {
    std::cout << "Enter a number-> ";
    std::cin >> result;
    if (actionIds.size() >= result && issue.chooseAction(actionIds[result])) {
      break;
    } else {
      std::cout << "\nInvalid choice!" << std::endl;
    }
  }
  return true;
}

bool EventObserver::onStoreEntered(ActionModel &action) {
  spdlog::debug("Entered a store!");
  std::cout << "Items available: \n";

  int32_t choice = 0;
  auto itemIds = action.item_ids;
  for (const auto &itemId : itemIds) {
    auto &item = m_game.getItems()->getItem(itemId);
    std::cout << "\t" + std::to_string(choice) + ". " +
    item.display_name << std::endl;
    choice++;
  }
  std::cout << "\t" + std::to_string(choice) + ". Leave store" << std::endl;

  int32_t result;
  while (true) {
    std::cout << "Enter the number of an item to purchase it or " +
    std::to_string(choice) + " to exit -> ";

    std::cin >> result;

    if (result == choice) {
      break;
    } else if (result < choice) {
      action.purchaseItem(itemIds[result]);
    } else {
      std::cout << "\nInvalid choice!" << std::endl;
    }
  }
  action.completePurchase();

  return true;
}

