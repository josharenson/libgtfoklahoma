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
#include <string>

#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {
struct ItemModel {
  enum class Category { INVALID, BIKE, MISC };
  int32_t id{-1};
  Category category{Category::INVALID};
  int32_t cost{-1}; // For display
  std::string display_name;
  std::string image_url;
  StatModel stat_delta;

  bool operator==(const ItemModel &rhs) const;
};
} // namespace libgtfoklahoma