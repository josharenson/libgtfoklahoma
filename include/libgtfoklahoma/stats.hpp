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
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <rapidjson/document.h>

#include <libgtfoklahoma/rules.hpp>
#include <libgtfoklahoma/stat_model.hpp>

namespace libgtfoklahoma {

class Game;
class Stats {
public:
  /**
   * Stats are weird as only one stat object exists per player unlike the rest of the game items
   */
   explicit Stats(Game &game, StatModel playeSstats);
   StatModel &getPlayerStatsModel();

  /**
   *
   * @param statChangesArray an array of {stat_name: stat_value} objects
   * @return A StatModel reflecting the array of key/value pairs passed in
   */
  static StatModel FromJson(const rapidjson::GenericArray<true, rapidjson::Value>& statChangesArray);

public:
  inline static StatModel kDefaultStatModel;

private:
  Game &m_game;

  StatModel m_playerStats;
  using StatNameDeltaPair = std::pair<std::string, std::variant<int32_t, double, std::string>>;
  static StatModel FromString(const std::vector<StatNameDeltaPair>& statFields);

};
} // namespace libgtfoklahoma
