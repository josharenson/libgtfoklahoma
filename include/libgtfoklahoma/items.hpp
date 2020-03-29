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
#include <map>
#include <string>

#include <rapidjson/document.h>

#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {

struct ItemModel {
  enum class Category {INVALID, BIKE};
  int32_t id;
  Category category;
  int32_t cost; // For display
  std::string display_name;
  std::string image_url;
  StatModel stat_delta;
};

class Items {
public:
  explicit Items(const char *itemsJson=kItemsJson);
  ItemModel getItem(int32_t id);

private:
  rapidjson::Document m_itemsDocument;
  std::map<int32_t, ItemModel> m_items;
  inline static const char *kItemsJson = R"JSON(
  [
      {
        "id": 0
        "category": "BIKE",
        "cost": 1200,
        "display_name": "Burly - Over The Road Trucker",
        "image_url": "",
        "stat_changes": [
          {"kit_weight", 35},
          {"max_mph", 12},
          {"money_remaining": 1200},
          {"odds_mech_issue", -0.05},
          {"odds_health_issue", -0.02}
        ]
      }
  ]
  )JSON";
};
}