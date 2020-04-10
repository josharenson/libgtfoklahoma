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

#include <libgtfoklahoma/stat_model.hpp>

using namespace libgtfoklahoma;

StatModel::StatModel(int32_t bedtimeHour,
                     int32_t health,
                     int32_t kitWeight,
                     int32_t maxMph,
                     int32_t moneyRemaining,
                     double oddsHealthIssue,
                     double oddsMechIssue,
                     Pace pace,
                     int32_t wakeupHour)
: bedtime_hour(bedtimeHour)
, health(health)
, kit_weight(kitWeight)
, max_mph(maxMph)
, money_remaining(moneyRemaining)
, odds_health_issue(oddsHealthIssue)
, odds_mech_issue(oddsMechIssue)
, pace(pace)
, wakeup_hour(wakeupHour) {}

bool StatModel::operator==(const StatModel &rhs) const {
  return this->bedtime_hour == rhs.bedtime_hour &&
         this->health == rhs.health &&
         this->kit_weight == rhs.kit_weight &&
         this->max_mph == rhs.max_mph &&
         this->money_remaining == rhs.money_remaining &&
         this->odds_health_issue == rhs.odds_health_issue &&
         this->odds_mech_issue == rhs.odds_mech_issue &&
         this->pace == rhs.pace &&
         this->wakeup_hour == rhs.wakeup_hour;
}

StatModel StatModel::operator+(const StatModel &rhs) {
  this->bedtime_hour += rhs.bedtime_hour;
  this->health += rhs.health;
  this->kit_weight += rhs.kit_weight;
  this->max_mph += rhs.max_mph;
  this->money_remaining += rhs.money_remaining;
  this->odds_health_issue += rhs.odds_health_issue;
  this->odds_mech_issue += rhs.odds_mech_issue;
  if (rhs.pace != Pace::INVALID) { this->pace = rhs.pace; }
  this->wakeup_hour += rhs.wakeup_hour;
  return *this;
}