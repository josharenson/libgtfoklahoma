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

#include <chrono>
#include <cstdint>
#include <random>

namespace libgtfoklahoma {
class StatModel;
}
namespace libgtfoklahoma::rules {

#pragma mark - Engine Ticks
const int32_t kTicksPerGameHour = 120;
const int32_t kTicksPerRealSecond = 60;
constexpr auto kTickDelayMs =
    std::chrono::milliseconds(1000 / kTicksPerRealSecond);

#pragma mark - Stat Defaults
const int32_t kDefaultBedtimeHour = 19;
const int32_t kDefaultWakeupHour = 6;
const int32_t kDefaultMoneyRemaining = 2000;

#pragma mark - Safety Constants
const int32_t kDefaultHealth = 100;
const double kDefaultOddsHealthIssuePerHour = 0.4;
// Mech issues can only happen when awake
const double kDefaultOddsMechanicalIssuePerHour = 0.4;
static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_real_distribution<double> randomGenerator(0,1);
bool HealthIssueThisHour(const libgtfoklahoma::StatModel &stats);
bool MechanicalIssueThisHour(const libgtfoklahoma::StatModel &stats);

#pragma mark - Speed Constants
/* Weight causes an "exponential decay" in speed
 * Real Speed = Max Speed * e^(-lambda * Kit Weight)
 */
const double kWeightLambda = 0.004;
int32_t RealSpeed(const libgtfoklahoma::StatModel &stats);
int32_t TicksUntilNextMile(const libgtfoklahoma::StatModel &stats);
} // namespace libgtfoklahoma::rules
