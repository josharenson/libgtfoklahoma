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

#include <libgtfoklahoma/actions.hpp>

#include <utility>

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include <libgtfoklahoma/event_observer.hpp>
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/items.hpp>

using namespace libgtfoklahoma;

Actions::Actions(Game &game, const char *actionJson)
: kEmptyActionModel(game)
, m_game(game) {

  rapidjson::Document actionsDocument;
  if (actionsDocument.Parse(actionJson).HasParseError() ||
      !actionsDocument.IsArray()) {
    spdlog::error("Parsing error when parsing actions json");
    abort();
  }

  auto actionIsValid = [] (const rapidjson::Value &action) {
    return action.HasMember("display_name") && action["display_name"].IsString() &&
           action.HasMember("id") && action["id"].IsInt() &&
           action.HasMember("type") && action["type"].IsArray();
  };

  auto getActionType = [](const rapidjson::GenericArray<true, rapidjson::Value> &arr) {
    uint32_t type = 0;
    for (const auto &elem : arr) {
      if (!elem.IsString()) {
        spdlog::warn("Error parsing action type");
        continue;
      }

      auto str_value = elem.GetString();
      if (!strcmp(str_value, "NONE")) {
        type |= ActionModel::ActionType::NONE;
      }

      else if (!strcmp(str_value, "STAT_CHANGE")) {
        type |= ActionModel::ActionType::STAT_CHANGE;
      }

      else if (!strcmp(str_value, "STORE")) {
        type |= ActionModel::ActionType::STORE;
      }
    }
    return type;
  };

  for (const auto &action : actionsDocument.GetArray()) {
    if (actionIsValid(action)) {
      auto &model = m_actions.emplace(action["id"].GetInt(), ActionModel(m_game)).first->second;

      model.id = action["id"].GetInt();
      model.display_name = action["display_name"].GetString();
      model.type = getActionType(action["type"].GetArray());

      if (action.HasMember("ending_id_hints") && action["ending_id_hints"].IsArray()) {
        for (const auto &endingId : action["ending_id_hints"].GetArray()) {
          model.ending_id_hints.emplace_back(endingId.GetInt());
        }
      }

      if (model.isStatChangeType() && action.HasMember("stat_changes")){
        model.stat_delta = Stats::FromJson(action["stat_changes"].GetArray());
      }

      if (model.isStoreType() && action.HasMember("items")) {
        for (const auto &id : action["items"].GetArray()) {
          model.item_ids.push_back(id.GetInt());
        }
      }

    } else {
      spdlog::error("Parsing error when adding an action");
    }
  }
}

ActionModel &Actions::getAction(int32_t id) {
  if (m_actions.count(id)) {
    return m_actions.at(id);
  }

  spdlog::warn("Requested action id {} that does not exist.", id);
  return kEmptyActionModel;
}

void Actions::handleAction(int32_t id, const std::unique_ptr<IEventObserver> &observer) {
  ActionModel &action = getAction(id);
  spdlog::debug("Performing action {}", id);

  // If this action causes the game to end, hint to the engine how it should go down
  for (const auto &endingId : action.ending_id_hints) {
    m_game.pushEndingHintId(endingId);
  }

  if (action.isStatChangeType()) {
    m_game.updateStats(action.stat_delta);
  }

  if (action.isStoreType()) {
    observer->onStoreEntered(action);
    action.purchaseComplete().get();
  }

  m_actionsThatHaveAlreadyHappened.insert(id);
}

bool Actions::actionHasHappened(int32_t actionId) {
  return m_actionsThatHaveAlreadyHappened.count(actionId);
}

void Actions::setActionsThatHaveAlreadyHappened(std::unordered_set<int32_t> actionIds) {
  m_actionsThatHaveAlreadyHappened = std::move(actionIds);
}
