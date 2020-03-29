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

Actions::Actions(Game &game, const char *actionJson) : m_game(game) {
  if (m_actionsDocument.Parse(actionJson).HasParseError() ||
      !m_actionsDocument.IsArray()) {
    spdlog::error("Parsing error when parsing actions json");
    abort();
  }

  auto actionIsValid = [this] (const rapidjson::Value &action) {
    return action["display_name"].IsString() && action["id"].IsInt() &&
           !m_actions.count(action["id"].GetInt()) &&
           action["stat_changes"].IsArray();
  };

  for (const auto &action : m_actionsDocument.GetArray()) {
    if (actionIsValid(action)) {
      ActionModel model;
      model.display_name = action["display_name"].GetString();
      model.id = action["id"].GetInt();
      model.stat_delta = StatModel::FromString(
          parseStatModifiers(action["stat_changes"].GetArray()));

      m_actions[model.id] = std::move(model);
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
  m_game.updateStats(model.stat_delta);
}

std::vector<StatNameDeltaPair> Actions::parseStatModifiers(const rapidjson::GenericArray<true, rapidjson::Value>& statChangesArray) {
  std::vector<StatNameDeltaPair> result;



  for (int i = 0; i < statChangesArray.Size(); i++) {

    // This is dumb, but the only way to get a key name
    std::string stat_name;
    std::variant<int32_t, double, std::string> stat_value;
    for (auto m = statChangesArray[i].MemberBegin(); m != statChangesArray[i].MemberEnd(); m++) {
      stat_name = m->name.GetString();

      if (m->value.IsInt()) {
        stat_value = m->value.GetInt();
      } else if (m->value.IsDouble()) {
        stat_value = m->value.GetDouble();
      } else if (m->value.IsString()) {
        stat_value = m->value.GetString();
      }
      result.emplace_back(stat_name, stat_value);
    }
  }
  return result;
}
