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

#include <libgtfoklahoma/actions.hpp>

#include <spdlog/spdlog.h>

using namespace libgtfoklahoma;

Actions::Actions(const char *actionJson) {
  if (m_actionsDocument.Parse(actionJson).HasParseError() ||
      !m_actionsDocument.IsArray()) {
    spdlog::error("Parsing error when parsing actions json");
    abort();
  }

  auto actionIsValid = [this] (const rapidjson::Value &action) {
    return action["display_name"].IsString() && action["id"].IsInt() &&
           !m_actions.count(action["id"].GetInt());
  };

  for (const auto &action : m_actionsDocument.GetArray()) {
    if (actionIsValid(action)) {
      ActionModel model;
      model.display_name = action["display_name"].GetString();
      model.id = action["id"].GetInt();
      m_actions[model.id] = model;
    } else {
      spdlog::error("Parsing error when adding an action");
    }
  }
}

ActionModel Actions::getAction(int32_t id) const {
  if (m_actions.count(id)) {
    return m_actions.at(id);
  }

  spdlog::warn("Requested action id {} that does not exist.", id);
  return {};
}

void Actions::performAction(int32_t id) {
  ActionModel model = getAction(id);
  spdlog::debug("Performing action {}", id);
}