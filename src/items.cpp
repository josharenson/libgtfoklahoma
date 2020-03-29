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

#include <libgtfoklahoma/items.hpp>

#include <utility>

#include <spdlog/spdlog.h>

using namespace libgtfoklahoma;

Items::Items(const char *itemsJson) {
  if (m_itemsDocument.Parse(itemsJson).HasParseError() ||
      !m_itemsDocument.IsArray()) {
    spdlog::error("Error parsing items JSON");
    abort();
  }

  auto get_category = [](const std::string &category){
    if (category == "BIKE") {
      return ItemModel::Category::BIKE;
    }

    return ItemModel::Category::INVALID;
  };

  auto item_is_valid = [](const rapidjson::Value &item) {
    return item.HasMember("id") && item["id"].IsInt() &&
           item.HasMember("category") && item["category"].IsString() &&
           item.HasMember("cost") && item["cost"].IsInt() &&
           item.HasMember("display_name") && item["display_name"].IsString() &&
           item.HasMember("image_url") && item["image_url"].IsString() &&
           item.HasMember("stat_changes") && item["stat_changes"].IsArray();
  };

  for (const auto &item : m_itemsDocument.GetArray()) {
    ItemModel model;
    if (!item_is_valid(item)) {
      spdlog::warn("Unable to parse an item! Skipping it.");
      continue;
    }

    model.id = item["id"].GetInt();
    model.category = get_category(item["category"].GetString());
    model.cost = item["cost"].GetInt();
    model.display_name = item["display_name"].GetString();
    model.image_url = item["image_url"].GetString();
    model.stat_delta = Stats::FromJson(item["stat_changes"].GetArray());

    m_items[model.id] = model;
  }
}

ItemModel Items::getItem(int32_t id) {
  if (m_items.count(id)) {
    return m_items[id];
  }
  return ItemModel {};
}
