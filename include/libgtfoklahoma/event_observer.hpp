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
#include <memory>
#include <utility>
#include <vector>

namespace libgtfoklahoma {

class ActionModel;
class Engine;
struct EventModel;
struct ItemModel;

class IEventObserver {
public:

  virtual ~IEventObserver() = default;

  /*
   * @param hour The hour of the day 0 - 23.
   */
  virtual void onHourChanged(int32_t hour) = 0;

  /*
   * @param mile The current mile of the journey. Wouldn't be very Texan to use
   * kilometers...
   */
  virtual void onMileChanged(int32_t mile) = 0;

  /*
   * @param model A reference to the model describing the point of interest.
   * @returns bool true if the engine should block the event loop until a
   * response has been recorded, false if the signal should be ignored
   */
  virtual bool onEvent(EventModel &model, std::vector<std::reference_wrapper<ActionModel>> &actions) = 0;

  /*
   * @param models A vector of items available in the store
   */
  virtual bool onStoreEntered(ActionModel &action, std::vector<ItemModel> &items) = 0;
};
}
