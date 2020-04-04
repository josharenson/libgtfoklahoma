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

#include <libgtfoklahoma/events.hpp>

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/event_observer.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace libgtfoklahoma;
using namespace rapidjson;

Events::Events(Game &game, const char *eventJson)
: m_game(game) {
  rapidjson::Document eventDocument;
  if (eventDocument.Parse(eventJson).HasParseError() ||
      !eventDocument.IsArray()) {
    spdlog::error("Parsing error when parsing events json");
    abort();
  }

  auto event_is_valid = [](const rapidjson::Value &event){
    return event.HasMember("id") && event["id"].IsInt() &&
           event.HasMember("actions") && event["actions"].IsArray() &&
           event.HasMember("description") && event["description"].IsString() &&
           event.HasMember("display_name") && event["display_name"].IsString() &&
           event.HasMember("mile") && event["mile"].IsInt();
  };

  auto idsFromArray = [](const rapidjson::GenericArray<true, rapidjson::Value> &arr) {
    std::vector<int32_t> result;
    for (const auto &id : arr) {
      result.emplace_back(id.GetInt());
    }
    return result;
  };

  for (const auto &event : eventDocument.GetArray()) {
    if (!event_is_valid(event)) {
      spdlog::warn("Parsing error while adding an event");
      continue;
    }

    EventModel model;
    model.id = event["id"].GetInt();
    model.action_ids = idsFromArray(event["actions"].GetArray());
    model.description = event["description"].GetString();
    model.display_name = event["display_name"].GetString();
    model.mile = event["mile"].GetInt();

    m_eventsByMile[model.mile].push_back(model.id);
    m_eventsById[model.id] = std::move(model);
  }
}

EventModel &Events::getEvent(int32_t id) {
  if (m_eventsById.count(id)) {
    return m_eventsById.at(id);
  }

  spdlog::warn("Requested event id {} that does not exist.", id);
  return kEmptyEventModel;
}

void Events::handleEvent(int32_t id, const std::unique_ptr<IEventObserver> &observer) {
  auto &event = getEvent(id);
  std::vector<std::reference_wrapper<ActionModel>> actionModels;
  for (const auto &actionId : event.action_ids) {
    actionModels.emplace_back(m_game.getActions()->getAction(actionId));
  }

  bool blockUntilResponse = observer->onEvent(event);
  if (blockUntilResponse) {
    auto actionId = event.chosenAction().get();
    auto &actions = m_game.getActions();
    actions->handleAction(actionId, observer);
  }
}

std::vector<int32_t> Events::eventsAtMile(int32_t mile) {
  if (m_eventsByMile.count(mile))
    return m_eventsByMile.at(mile);
  else
    return {};
}

bool Events::hasMoreEvents(int32_t mile) const {
  return m_eventsByMile.upper_bound(mile) != m_eventsByMile.end();
}
