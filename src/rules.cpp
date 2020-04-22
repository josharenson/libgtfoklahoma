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

#include <libgtfoklahoma/rules.hpp>

#include <cmath>

#include <libgtfoklahoma/action_model.hpp>
#include <libgtfoklahoma/stat_model.hpp>

using namespace libgtfoklahoma;
using namespace libgtfoklahoma::rules;

bool libgtfoklahoma::rules::ActionIsSuccessful(const libgtfoklahoma::ActionModel &action) {
  return randomGenerator(gen) <= action.success_chance;
}

bool libgtfoklahoma::rules::HealthIssueThisHour(const libgtfoklahoma::StatModel &stats) {
  return randomGenerator(gen) <= stats.odds_health_issue;
}

bool libgtfoklahoma::rules::MechanicalIssueThisHour(const libgtfoklahoma::StatModel &stats) {
  return randomGenerator(gen) <= stats.odds_mech_issue;
}

int32_t libgtfoklahoma::rules::RealSpeed(const StatModel &stats) {
  return round(stats.max_mph * exp(-kWeightLambda * stats.kit_weight));
}

int32_t libgtfoklahoma::rules::TicksUntilNextMile(const StatModel &stats) {
  return kTicksPerGameHour / RealSpeed(stats);
}