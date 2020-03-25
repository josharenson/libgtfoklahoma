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

#pragma once

#include <cstdint>

namespace libgtfoklahoma {

class EventModel;
class IEventObserver {
public:
  virtual ~IEventObserver() = default;

  /*
   * @param hour The hour of the day 0 - 23.
   */
  virtual void onHourChanged(int32_t hour) const = 0;

  /*
   * @param mile The current mile of the journey. Wouldn't be very Texan to use
   * kilometers...
   */
  virtual void onMileChanged(int32_t mile) const = 0;

  /*
   * @param model A reference to the model describing the point of interest.
   */
  virtual void onEvent(const EventModel &model) const = 0;
};
}