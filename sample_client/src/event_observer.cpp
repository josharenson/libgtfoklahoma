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
#include <libgtfoklahoma/items.hpp>

// System includes
#include <iostream>
#include <utility>

// 3P includes
#include <spdlog/spdlog.h>

using namespace gtfoklahoma;
using namespace libgtfoklahoma;

void EventObserver::onHourChanged(int32_t hour) {
  spdlog::debug("Current hour is {}", hour);
}

void EventObserver::onMileChanged(int32_t mile) {
  spdlog::debug("Current mile is {}", mile);
}

bool EventObserver::onEvent(EventModel &model, std::vector<std::reference_wrapper<ActionModel>> &actions) {
  spdlog::debug("POI Encountered-> " + model.display_name);
  int32_t result;

  std::cout << "Welcome to " + model.display_name << std::endl;

  int choice = 0;
  for (const auto &action :actions) {
    std::cout << "\t" + std::to_string(choice) + ". " + action.get().display_name
    << std::endl;
    choice++;
  }

  while (true) {
    std::cout << "Enter a number-> ";
    std::cin >> result;
    if (actions.size() >= result && model.chooseAction(actions.at(result).get().id)) {
      break;
    } else {
      std::cout << "\nInvalid choice!" << std::endl;
    }
  }

  return true;
}

bool EventObserver::onStoreEntered(ActionModel &action, std::vector<libgtfoklahoma::ItemModel> &items) {
  spdlog::debug("Entered a store!");
  std::cout << "Items available: \n";

  int choice = 0;
  for (const auto &item : items) {
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
      action.purchaseItem(items[result].id);
    } else {
      std::cout << "\nInvalid choice!" << std::endl;
    }
  }
  action.completePurchase();

  return true;
}

