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
: m_game(game) {

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
      ActionModel model;
      model.display_name = action["display_name"].GetString();
      model.id = action["id"].GetInt();
      model.type = getActionType(action["type"].GetArray());

      if (model.isStatChangeType() && action.HasMember("stat_changes")){
        model.stat_delta = Stats::FromJson(action["stat_changes"].GetArray());
      }

      if (model.isStoreType() && action.HasMember("items")) {
        for (const auto &id : action["items"].GetArray()) {
          model.item_ids.push_back(id.GetInt());
        }
      }

      m_actions[model.id] = std::move(model);
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

  if (action.isStatChangeType()) {
    m_game.updateStats(action.stat_delta);
  }

  if (action.isStoreType()) {
    std::vector<ItemModel> items;
    for (const auto &item_id : action.item_ids) {
      items.emplace_back(m_game.getItems()->getItem(item_id));
    }
    observer->onStoreEntered(action, items);
    auto purchasedItems = action.purchasedItems().get();

    // Add purchased items to inventory.. Client should validate item prices
    // but only an idiot would't do backend validation too
    for (const auto &itemId : purchasedItems) {
      auto &item = m_game.getItems()->getItem(itemId);
      if (item.cost <= m_game.getStats()->money_remaining) {
        // Adding the item subtracts funds automatically
        m_game.addItemToInventory(itemId);
      } else {
        spdlog::warn("Player doesn't have enough money for item {}. "
                     "Not adding it to the inventory!", itemId);
      }
    }
  }

  m_actionsThatHaveAlreadyHappened.insert(id);
}

bool Actions::actionHasHappened(int32_t actionId) {
  return m_actionsThatHaveAlreadyHappened.count(actionId);
}

void Actions::setActionsThatHaveAlreadyHappened(std::unordered_set<int32_t> actionIds) {
  m_actionsThatHaveAlreadyHappened = std::move(actionIds);
}

