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

#include <catch2/catch.hpp>

#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/stats.hpp>

TEST_CASE("Stats", "[unit]") {
  using namespace libgtfoklahoma;

  SECTION("updateStats") {
    Game game("");
    StatModel stats_delta;
    stats_delta.bedtime_hour = 5;
    stats_delta.kit_weight = 5;
    stats_delta.max_mph = 5;
    stats_delta.odds_health_issue = .5;
    stats_delta.odds_mech_issue = .5;
    stats_delta.pace = StatModel::Pace::MERCKX;
    stats_delta.wakeup_hour = 5;

    auto &initial_stats = game.getStats();
    auto initial_bedtime_hour = initial_stats->bedtime_hour;
    auto initial_kit_weight = initial_stats->kit_weight;
    auto initial_max_mph = initial_stats->max_mph;
    auto initial_odds_health_issue = initial_stats->odds_health_issue;
    auto initial_odds_mech_issue = initial_stats->odds_mech_issue;
    auto initial_wakeup_hour = initial_stats->wakeup_hour;

    game.updateStats(stats_delta);
    auto &updated_stats = game.getStats();

    REQUIRE(updated_stats->bedtime_hour == initial_bedtime_hour + 5);
    REQUIRE(updated_stats->kit_weight == initial_kit_weight + 5);
    REQUIRE(updated_stats->max_mph == initial_max_mph + 5);
    REQUIRE(updated_stats->odds_health_issue == initial_odds_health_issue + .5);
    REQUIRE(updated_stats->odds_mech_issue == initial_odds_mech_issue + .5);
    REQUIRE(updated_stats->pace == StatModel::Pace::MERCKX);
    REQUIRE(updated_stats->wakeup_hour == initial_wakeup_hour + 5);
  }

  SECTION("Test +") {
    StatModel lhs;
    lhs.bedtime_hour = 0;
    lhs.kit_weight = 0;
    lhs.max_mph = 0;
    lhs.odds_health_issue = 0;
    lhs.odds_mech_issue = 0;
    lhs.pace = StatModel::Pace::CHILL_AF;
    lhs.wakeup_hour = 0;

    StatModel rhs;
    rhs.bedtime_hour = 5;
    rhs.kit_weight = 5;
    rhs.max_mph = 5;
    rhs.odds_health_issue = .5;
    rhs.odds_mech_issue = .5;
    rhs.pace = StatModel::Pace::MERCKX;
    rhs.wakeup_hour = 5;

    auto result = lhs + rhs;
    REQUIRE(result.bedtime_hour == 5);
    REQUIRE(result.kit_weight == 5);
    REQUIRE(result.max_mph == 5);
    REQUIRE(result.odds_health_issue == .5);
    REQUIRE(result.odds_mech_issue == .5);
    REQUIRE(result.pace == StatModel::Pace::MERCKX);
    REQUIRE(result.wakeup_hour == 5);
  }

  SECTION("Stats::FromJson") {
    const char *statJson = R"(
      [
        {"bedtime_hour": 5},
        {"kit_weight": 5},
        {"max_mph": 5},
        {"odds_health_issue": 0.5},
        {"odds_mech_issue": 0.5},
        {"pace": "MERCKX"},
        {"wakeup_hour": 5}
      ]
    )";

    // HACK
    const auto get_arr = [](const rapidjson::Document &doc) {
      return doc.GetArray();
    };

    rapidjson::Document json_document;
    json_document.Parse(statJson);
    auto output = Stats::FromJson(get_arr(json_document));

    REQUIRE(output.bedtime_hour == 5);
    REQUIRE(output.kit_weight == 5);
    REQUIRE(output.max_mph == 5);
    REQUIRE(output.odds_health_issue == .5);
    REQUIRE(output.odds_mech_issue == .5);
    REQUIRE(output.pace == StatModel::Pace::MERCKX);
    REQUIRE(output.wakeup_hour == 5);
  }
}