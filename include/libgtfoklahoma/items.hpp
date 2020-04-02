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
#include <unordered_map>
#include <string>

#include <libgtfoklahoma/item_model.hpp>

namespace libgtfoklahoma {

class Items {
public:
  explicit Items(const char *itemsJson=kItemsJson);
  ItemModel &getItem(int32_t id);

public:
  inline static ItemModel kEmptyItemModel;

private:
  std::unordered_map<int32_t, ItemModel> m_items;

  inline static const char *kItemsJson = R"JSON(
  [
      {
        "id": 0,
        "category": "BIKE",
        "cost": 1200,
        "display_name": "Burly - Over The Road Trucker",
        "image_url": "",
        "stat_changes": [
          {"kit_weight": 35},
          {"max_mph": 12},
          {"money_remaining": 1200},
          {"odds_mech_issue": -0.05},
          {"odds_health_issue": -0.02}
        ]
      },
      {
        "id": 1,
        "category": "MISC",
        "cost": 2,
        "display_name": "Bick Lighter",
        "image_url": "",
        "stat_changes": [{}]
      }
  ]
  )JSON";
};
} // namespace libgtfoklahoma