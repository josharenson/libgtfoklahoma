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
#include <future>
#include <string>
#include <vector>

namespace libgtfoklahoma {
struct EventModel {
  int32_t id;

  std::vector<int32_t> action_ids;
  std::string description;
  std::string display_name;
  int32_t mile{-1};

  bool chooseAction(int32_t actionId);
  std::future<int32_t> chosenAction();

  bool operator==(const EventModel &rhs) const;

private:
  [[nodiscard]] bool actionIdIsValid(int32_t actionId) const;

private:
  std::promise<int32_t> m_chosenAction;
};
} // namespace libgtfoklahoma