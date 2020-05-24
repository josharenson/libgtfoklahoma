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

      if (action.HasMember("dependent_inventory_ids") && action["dependent_inventory_ids"].IsArray()) {
        for (const auto &dependentInventoryId : action["dependent_inventory_ids"].GetArray()) {
          auto pair = dependentInventoryId.GetObject();
          if (pair.HasMember("id") && pair["id"].IsInt()
              && pair.HasMember("qty") && pair["qty"].IsInt()) {
            model.dependent_inventory_ids.emplace_back(pair["id"].GetInt(), pair["qty"].GetInt());
          }
        }
      }

      model.display_name = action["display_name"].GetString();
      model.type = getActionType(action["type"].GetArray());

      if (action.HasMember("ending_id_hints") && action["ending_id_hints"].IsArray()) {
        for (const auto &endingId : action["ending_id_hints"].GetArray()) {
          model.ending_id_hints.emplace_back(endingId.GetInt());
        }
      }

      if (model.isStatChangeType() && action.HasMember("success_chance")) {
        model.can_fail = true;
        model.success_chance = action["success_chance"].GetFloat();
      }

      if (model.isStatChangeType() && action.HasMember("message_failure")) {
        model.message_failure = action["message_failure"].GetString();
      }

      if (model.isStatChangeType() && action.HasMember("message_success")) {
        model.message_success = action["message_success"].GetString();
      }

      if (model.isStatChangeType() && action.HasMember("stat_changes_on_failure")){
        model.stat_delta_on_failure = Stats::FromJson(action["stat_changes_on_failure"].GetArray());
      }

      if (model.isStatChangeType() && action.HasMember("stat_changes_on_success")){
        model.stat_delta_on_success = Stats::FromJson(action["stat_changes_on_success"].GetArray());
      }

      if (model.isStatChangeType() && action.HasMember("stat_changes_regardless")){
        model.stat_delta_regardless = Stats::FromJson(action["stat_changes_regardless"].GetArray());
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

  // Determine all action outcomes now because free-will isn't real.
  for (auto &action_id_pair : m_actions) {
    auto &action = action_id_pair.second;
      action.m_successful = rules::ActionIsSuccessful(action);
  }
}

ActionModel &Actions::getAction(int32_t id) {
  if (m_actions.count(id)) {
    return m_actions.at(id);
  }

  spdlog::warn("Requested action id {} that does not exist.", id);
  return kEmptyActionModel;
}

void Actions::handleAction(int32_t id, const std::shared_ptr<IEventObserver> &observer) {
  ActionModel &action = getAction(id);
  spdlog::debug("Performing action {}", id);

  // If this action causes the game to end, hint to the engine how it should go down
  for (const auto &endingId : action.ending_id_hints) {
    m_game.pushEndingHintId(endingId);
  }

  // Update stats if this action has stat changes associated with it
  if (action.isStatChangeType()) {
    m_game.updateStats(action.stat_delta_regardless);
    if (action.can_fail && action.failed()) {
      m_game.updateStats(action.stat_delta_on_failure);
    } else if (action.can_fail) {
      m_game.updateStats(action.stat_delta_on_success);
    }
  }

  // If the action is a store (weird design but it works), block until
  // the player has left the store
  if (action.isStoreType()) {
    observer->onStoreEntered(action);
    action.purchaseComplete().get();
  }

  // There are entities that are dependent on actions having happened
  // eg: "You can only explode if someone has previously set us up the bomb"
  // so track what has happened
  m_actionsThatHaveAlreadyHappened.insert(id);
}

bool Actions::actionHasHappened(int32_t actionId) {
  return m_actionsThatHaveAlreadyHappened.count(actionId);
}

void Actions::setActionsThatHaveAlreadyHappened(std::unordered_set<int32_t> actionIds) {
  m_actionsThatHaveAlreadyHappened = std::move(actionIds);
}
