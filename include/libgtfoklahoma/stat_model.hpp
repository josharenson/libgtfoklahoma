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

namespace libgtfoklahoma {
struct StatModel {

  enum class Pace {INVALID, CHILL_AF, FRED, MERCKX};

  // These all need to be 0 so that adding stat deltas don't include these
  int32_t bedtime_hour;
  int32_t health; // When health is 0, you die
  int32_t kit_weight;
  int32_t max_mph;
  int32_t money_remaining;
  double odds_health_issue;
  double odds_mech_issue;
  Pace pace;
  int32_t wakeup_hour;

  // FIXME: Initial speed must be greater than 0 or else calculating ticks/mile causes a FPE_FLTDIV_TRAP
  explicit StatModel(int32_t bedtimeHour=0,
                     int32_t health=0,
                     int32_t kitWeight=0,
                     int32_t maxMph=1,
                     int32_t moneyRemaining=0,
                     double oddsHealthIssue=0,
                     double oddsMechIssue=0,
                     Pace pace=Pace::INVALID,
                     int32_t wakeupHour=0);

  bool operator== (const StatModel &rhs) const;
  StatModel operator+ (const StatModel &rhs);
};
} // namespace libgtfoklahoma