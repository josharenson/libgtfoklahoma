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

#include <map>
#include <string>

#include <rapidjson/document.h>

namespace libgtfoklahoma {
struct ActionModel {
  std::string display_name;
  int32_t id {-1};
};

class Actions {
public:
  explicit Actions(const char *actionJson=kActionJson);
  [[nodiscard]] ActionModel getAction(int32_t id) const;
  void performAction(int32_t id);

private:
  rapidjson::Document m_actionsDocument;
  std::map<int32_t, ActionModel> m_actions;
  inline static const char *kActionJson = R"JSON(
  [
    {
      "display_name": "GTFO",
      "id": 0
    }
  ]
  )JSON";
};
}