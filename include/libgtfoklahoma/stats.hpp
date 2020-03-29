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

#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include <libgtfoklahoma/rules.hpp>

namespace libgtfoklahoma {

using StatNameDeltaPair = std::pair<std::string, std::variant<int32_t, double, std::string>>;
struct StatModel {
  enum class Pace {CHILL_AF, FRED, MERCKX};

  int32_t bedtime_hour {rules::kDefaultBedtimeHour};
  int32_t kit_weight{0};
  int32_t max_mph {0};
  double odds_health_issue {rules::kDefaultOddsHealthIssue};
  double odds_mech_issue {rules::kDefaultOddsMechanicalIssue};
  Pace pace {Pace::FRED};
  int32_t wakeup_hour {rules::kDefaultWakeupHour};

  static StatModel FromString(const std::vector<StatNameDeltaPair>& statFields) {
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

      else if (field.first == "odds_health_issue") {
        result.odds_health_issue = std::get<double>(field.second);
      }

      else if (field.first == "odds_mech_issue") {
        result.odds_mech_issue = std::get<double>(field.second);
      }

      else if (field.first == "pace") {
        const auto &pace_str = std::get<std::string>(field.second);
        Pace pace = Pace::CHILL_AF;
        if (pace_str == "CHILL_AF") { pace = Pace::CHILL_AF; }
        else if (pace_str == "FRED") { pace = Pace::FRED; }
        else if (pace_str == "MERCKX") { pace = Pace::MERCKX; }
        result.pace = pace;
      }

      else if (field.first == "wakeup_hour") {
        result.wakeup_hour = std::get<int32_t>(field.second);
      }
    }
    return result;
  }

  StatModel operator+(const StatModel &rhs) {
    this->bedtime_hour += rhs.bedtime_hour;
    this->kit_weight += rhs.kit_weight;
    this->max_mph += rhs.max_mph;
    this->odds_health_issue += rhs.odds_health_issue;
    this->odds_mech_issue += rhs.odds_mech_issue;
    this->pace = rhs.pace;
    this->wakeup_hour += rhs.wakeup_hour;
    return *this;
  }
};


class Stats {
public:
  Stats() = default;

private:
  // These are core stats, they are a function of themselves + dependent
  // secondary stats
  int32_t m_bedtimeHour;
  int32_t m_wakeupHour;

  int32_t m_milesPerHour;

  double m_oddsHealthIssue;
  double m_oddsMechanicalIssue;

  int32_t m_moneyRemaining;

  // These are secondary stats
  double m_kitWeight;
  int32_t m_kitMaxMilesPerHour;

};
} // namespace libgtfoklahoma