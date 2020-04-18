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

#pragma once

#include <cstdint>
#include <unordered_map>

#include <libgtfoklahoma/ending_model.hpp>

namespace libgtfoklahoma {

class Endings {
public:
  explicit Endings(const char *endingsJson = kEndingsJson);
  EndingModel &getEnding(int32_t id);

  void handleEnding(int32_t id);

  inline static EndingModel kEmptyEndingModel;
private:
  std::unordered_map<int32_t, EndingModel> m_endings;

  inline static const char *kEndingsJson = R"(
  [
    {
      "id": 0,
      "description": "Your poor health choices caught up to you. Sometimes you eat the bar, and sometimes the bar eats you.",
      "display_name": "Rest In Power.",
      "image_tag": "health_death"
    }
  ]
  )";
};
} // namespace libgtfoklahoma