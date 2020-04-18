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

#include <libgtfoklahoma/endings.hpp>

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include <libgtfoklahoma/ending_model.hpp>

using namespace libgtfoklahoma;

Endings::Endings(const char *endingsJson) {
  rapidjson::Document endingsDocument;
  if (endingsDocument.Parse(endingsJson).HasParseError() ||
      !endingsDocument.IsArray()) {
    spdlog::error("Parsing error when parsing endings json");
    abort();
  }

  auto endingIsValid = [](const rapidjson::Value &ending){
    return ending.HasMember("id") && ending["id"].IsInt() &&
           ending.HasMember("description") && ending["description"].IsString() &&
           ending.HasMember("display_name") && ending["display_name"].IsString() &&
           ending.HasMember("image_tag") && ending["image_tag"].IsString();
  };

  for (const auto &ending : endingsDocument.GetArray()) {
    if (!endingIsValid(ending)) {
      spdlog::warn("Parsing error when adding an ending.");
      continue;
    }

    EndingModel model;
    model.id = ending["id"].GetInt();
    model.description = ending["description"].GetString();
    model.display_name = ending["display_name"].GetString();
    model.image_tag = ending["image_tag"].GetString();

    m_endings[model.id] = model;
  }
}

EndingModel &Endings::getEnding(int32_t id) {
  if (m_endings.count(id)) {
    return m_endings[id];
  }

  spdlog::warn("Requested ending id {} that does not exist.", id);
  return kEmptyEndingModel;
}

void Endings::handleEnding(int32_t id) {

}