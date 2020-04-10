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

#include <libgtfoklahoma/stats.hpp>

#include <spdlog/spdlog.h>

#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/stat_model.hpp>

using namespace libgtfoklahoma;

Stats::Stats(Game &game, StatModel playerStats)
: m_game(game)
, m_playerStats(playerStats) {}

StatModel &Stats::getPlayerStatsModel() { return m_playerStats; }

StatModel Stats::FromJson(const rapidjson::GenericArray<true, rapidjson::Value> &statChangesArray) {
  std::vector<StatNameDeltaPair> result;

  for (const auto &statChange : statChangesArray) {

    // This is dumb, but the only way to get a key name
    std::string stat_name;
    std::variant<int32_t, double, std::string> stat_value;
    for (auto it = statChange.MemberBegin(); it != statChange.MemberEnd(); it++) {
      stat_name = it->name.GetString();

      if (it->value.IsInt()) {
        stat_value = it->value.GetInt();
      } else if (it->value.IsDouble()) {
        stat_value = it->value.GetDouble();
      } else if (it->value.IsString()) {
        stat_value = it->value.GetString();
      } else {
        spdlog::warn("Could not parse stat value!");
      }
      result.emplace_back(stat_name, stat_value);
    }
  }
  return FromString(result);
}

StatModel Stats::FromString(const std::vector<StatNameDeltaPair> &statFields) {
  StatModel result;
  for (const auto &field : statFields) {
    if (field.first == "bedtime_hour") {
      result.bedtime_hour = std::get<int32_t>(field.second);
    }

    else if (field.first == "kit_weight") {
      result.kit_weight = std::get<int32_t>(field.second);
    }

    else if (field.first == "max_mph") {
      result.max_mph = std::get<int32_t>(field.second);
    }

    else if (field.first == "money_remaining") {
      result.money_remaining = std::get<int32_t>(field.second);
    }

    else if (field.first == "odds_health_issue") {
      result.odds_health_issue = std::get<double>(field.second);
    }

    else if (field.first == "odds_mech_issue") {
      result.odds_mech_issue = std::get<double>(field.second);
    }

    else if (field.first == "pace") {
      const auto &pace_str = std::get<std::string>(field.second);
      StatModel::Pace pace = StatModel::Pace::CHILL_AF;
      if (pace_str == "CHILL_AF") { pace = StatModel::Pace::CHILL_AF; }
      else if (pace_str == "FRED") { pace = StatModel::Pace::FRED; }
      else if (pace_str == "MERCKX") { pace = StatModel::Pace::MERCKX; }
      result.pace = pace;
    }

    else if (field.first == "wakeup_hour") {
      result.wakeup_hour = std::get<int32_t>(field.second);
    }
  }
  return result;
}

