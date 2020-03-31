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

// Library includes
#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/events.hpp>
#include <libgtfoklahoma/items.hpp>

// System includes
#include <iostream>

// 3P includes
#include <spdlog/spdlog.h>

// Local includes
#include <event_observer.hpp>
#include <utility>

using namespace gtfoklahoma;
using namespace libgtfoklahoma;

void EventObserver::onHourChanged(int32_t hour) {
  spdlog::debug("Current hour is " + std::to_string(hour));
}

void EventObserver::onMileChanged(int32_t mile) {

}

void EventObserver::onEvent(EventModel &model, std::vector<std::reference_wrapper<ActionModel>> &actions) {
  spdlog::debug("POI Encountered-> " + model.display_name);
  int32_t result;

  int choice = 0;
  for (const auto &action :actions) {
    std::cout << "\t" + std::to_string(choice) + ". " + action.get().display_name
    << std::endl;
    choice++;
  }

  while (true) {
    std::cout << "Enter a number-> ";
    std::cin >> result;
    if (model.chooseAction(result)) {
      break;
    } else {
      std::cout << "\nInvalid choice!" << std::endl;
    }
  }
}

void EventObserver::onStoreEntered(ActionModel &action, std::vector<libgtfoklahoma::ItemModel> &items) {
  spdlog::debug("Entered a store!");
  std::cout << "Items available: \n";

  int i = 0;
  for (const auto &item : items) {
    std::cout << "\t" + std::to_string(i) + ". " +
    item.display_name << std::endl;
    i++;
  }
  std::cout << "\t" + std::to_string(i) + ". Leave store" << std::endl;

  int32_t result;
  while (true) {
    std::cout << "Enter the number of an item to purchase it-> ";
    std::cin >> result;

    if (result == i) {
      break;
    } else if (result < i) {
      action.purchaseItem(items[result].id);
    } else {
      std::cout << "\nInvalid choice!" << std::endl;
    }
  }
  action.completePurchase();
}

