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

#include <rapidjson/document.h>

#include <libgtfoklahoma/rules.hpp>

namespace libgtfoklahoma {

struct StatModel {
  enum class Pace {CHILL_AF, FRED, MERCKX};

  int32_t bedtime_hour {rules::kDefaultBedtimeHour};
  int32_t kit_weight{0};
  int32_t max_mph {0};
  int32_t money_remaining{rules::kDefaultMoneyRemaining};
  double odds_health_issue {rules::kDefaultOddsHealthIssue};
  double odds_mech_issue {rules::kDefaultOddsMechanicalIssue};
  Pace pace {Pace::FRED};
  int32_t wakeup_hour {rules::kDefaultWakeupHour};

  bool operator== (const StatModel &rhs) const {
    return this->bedtime_hour == rhs.bedtime_hour &&
           this->kit_weight == rhs.kit_weight &&
           this->max_mph == rhs.max_mph &&
           this->money_remaining == rhs.money_remaining &&
           this->odds_health_issue == rhs.odds_health_issue &&
           this->odds_mech_issue == rhs.odds_mech_issue &&
           this->pace == rhs.pace &&
           this->wakeup_hour == rhs.wakeup_hour;
  }

  StatModel operator+ (const StatModel &rhs) {
    this->bedtime_hour += rhs.bedtime_hour;
    this->kit_weight += rhs.kit_weight;
    this->max_mph += rhs.max_mph;
    this->money_remaining += rhs.money_remaining;
    this->odds_health_issue += rhs.odds_health_issue;
    this->odds_mech_issue += rhs.odds_mech_issue;
    this->pace = rhs.pace;
    this->wakeup_hour += rhs.wakeup_hour;
    return *this;
  }
};


class Stats {
public:
  static StatModel FromJson(const rapidjson::GenericArray<true, rapidjson::Value>& statChangesArray);

private:
  using StatNameDeltaPair = std::pair<std::string, std::variant<int32_t, double, std::string>>;
  static StatModel FromString(const std::vector<StatNameDeltaPair>& statFields);

};
} // namespace libgtfoklahoma