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

#include <libgtfoklahoma/events.hpp>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace libgtfoklahoma;
using namespace rapidjson;

Events::Events(const int32_t initialMile, const char *eventJson) {
  if (m_eventDocument.Parse(eventJson).HasParseError() ||
      !m_eventDocument.IsArray()) {
    spdlog::error("Parsing error when parsing events json");
    abort();
  }

  for (int i = 0; i < m_eventDocument.GetArray().Size(); i++) {
    auto array = m_eventDocument.GetArray();
    if (array[i].IsObject() && array[i].GetObject()["mile"].IsInt() &&
        array[i].GetObject()["mile"].GetInt() >= initialMile) {
      m_nextEventIdx = i;
      break;
    }
  }
}

EventModel Events::nextEvent() {
  EventModel model = {};
  auto array = m_eventDocument.GetArray();

  // Out of bounds, return empty model
  if (array.Size() <= m_nextEventIdx || !array[m_nextEventIdx].IsObject()) {
    spdlog::error("Could not find the requested event. Index out of bounds.");
    return model;
  }

  auto event = array[m_nextEventIdx].GetObject();
  if (event["description"].IsString() && event["display_name"].IsString() &&
      event["mile"].IsInt()) {
    model.description = event["description"].GetString();
    model.display_name = event["display_name"].GetString();
    model.mile = event["mile"].GetInt();
    m_nextEventIdx++;
    return model;
  } else {
    spdlog::error("Could not parse event #{}", m_nextEventIdx);
    return model;
  }
}
