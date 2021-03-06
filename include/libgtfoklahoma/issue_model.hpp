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
#include <future>
#include <string>
#include <vector>

#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {
struct IssueModel {
  enum class Type { INVALID, HEALTH, MECHANICAL };
  int32_t id{-1};
  std::vector<int32_t> actions;

  // This event will only happen if all dependent actions have also happened
  std::vector<int32_t> dependent_actions;

  // This event will only happen if all dependent items are in inventory
  std::vector<int32_t> dependent_inventory;
  std::string description;
  std::string display_name;
  std::vector<int32_t> ending_id_hints;
  std::string image_url;
  StatModel stat_delta;
  Type type{Type::INVALID};

  [[nodiscard]] bool actionIdIsValid(int32_t actionId) const;
  bool chooseAction(int32_t actionId);
  std::future<int32_t> chosenAction();

  bool operator==(const IssueModel &rhs) const;

private:
  std::promise<int32_t> m_chosenAction;
};
} // namespace libgtfoklahoma
