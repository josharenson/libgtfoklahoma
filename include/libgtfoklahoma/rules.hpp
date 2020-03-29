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

#include <chrono>
#include <cmath>
#include <cstdint>

namespace libgtfoklahoma::rules {

#pragma mark - Engine Ticks
const int32_t kTicksPerGameHour = 2;
const int32_t kTicksPerRealSecond = 2;
constexpr auto kTickDelayMs =
    std::chrono::milliseconds(1000 / kTicksPerRealSecond);

#pragma mark - Stat Defaults
const int32_t kDefaultBedtimeHour = 19;
const int32_t kDefaultWakeupHour = 6;
const int32_t kDefaultMoneyRemaining = 2000;

#pragma mark - Safety Constants
const double kDefaultOddsHealthIssue = 0.4;
const double kDefaultOddsMechanicalIssue = 0.4;

#pragma mark - Speed Constants
/* Weight causes an "exponential decay" in speed
 * Real Speed = Max Speed * e^(-lambda * Kit Weight)
 */
const double kWeightLambda = 0.004;
static int32_t RealSpeed(int32_t maxMph, int32_t kitWeight) {
  return round(maxMph * exp(-kWeightLambda * kitWeight));
}
} // namespace libgtfoklahoma