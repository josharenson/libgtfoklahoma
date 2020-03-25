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
#include <libgtfoklahoma/events.hpp>

// 3P includes
#include <spdlog/spdlog.h>

// Local includes
#include <event_observer.hpp>

using namespace gtfoklahoma;
using namespace libgtfoklahoma;

void EventObserver::onHourChanged(int32_t hour) const {
  spdlog::debug("Current hour is " + std::to_string(hour));
}

void EventObserver::onMileChanged(int32_t mile) const {

}

void EventObserver::onEvent(const EventModel &model) const {
  spdlog::debug("POI Encountered-> " + model.display_name);
}
