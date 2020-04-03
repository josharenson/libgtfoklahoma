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

#include <libgtfoklahoma/event_model.hpp>

#include <spdlog/spdlog.h>

using namespace libgtfoklahoma;

bool EventModel::chooseAction(int32_t actionId) {
  if (actionIdIsValid(actionId)) {
    m_chosenAction.set_value(actionId);
    return true;
  } else {
    spdlog::error("{} is an invalid action id for this event!", actionId);
    return false;
  }
}

std::future<int32_t> EventModel::chosenAction() {
  return m_chosenAction.get_future();
}

bool EventModel::actionIdIsValid(int32_t actionId) const {
  auto it = std::find(action_ids.cbegin(), action_ids.cend(), actionId);
  return it != action_ids.cend();
}

bool EventModel::operator==(const EventModel &rhs) const {
  return this->id == rhs.id &&
         this->action_ids == rhs.action_ids &&
         this->description == rhs.description &&
         this->display_name == rhs.display_name &&
         this->mile == rhs.mile;
}
