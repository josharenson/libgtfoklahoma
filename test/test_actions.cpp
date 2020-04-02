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

#include "helpers.hpp"

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace testhelpers;

TEST_CASE("Actions", "[unit]") {
  using namespace libgtfoklahoma;

  const char *actionJson = R"(
  [
    {
      "id": 0,
      "display_name": "display_name_0",
      "stat_changes": [{}],
      "type": ["STAT_CHANGE"]
    },
    {
      "display_name": "display_name_1",
      "id": 1,
      "stat_changes": [
        {"bedtime_hour": 5},
        {"kit_weight": 5},
        {"max_mph": 5},
        {"odds_health_issue": 0.5},
        {"odds_mech_issue": 0.5},
        {"pace": "MERCKX"},
        {"wakeup_hour": 5}
      ],
      "type": ["STAT_CHANGE"]
    },
    {
      "display_name": "display_name_1",
      "id": 2,
      "items": [0, 1],
      "stat_changes": [{"kit_weight": 5}],
      "type": ["STAT_CHANGE", "STORE"]
    }
  ]
  )";

  Game game("", actionJson, validEventJson, validIssueJson, validItemJson);
  auto &actions = game.getActions();

  SECTION("Get action by id works") {
    ActionModel &model_0 = actions->getAction(0);
    ActionModel &model_1 = actions->getAction(1);
    REQUIRE(model_0.display_name == "display_name_0");
    REQUIRE(model_1.display_name == "display_name_1");
  }

  SECTION("StatModel Can parse stat changes") {
    ActionModel &model_1 = actions->getAction(1);
    auto stat_model = model_1.stat_delta;
    REQUIRE(stat_model.bedtime_hour == 5);
    REQUIRE(stat_model.kit_weight == 5);
    REQUIRE(stat_model.max_mph == 5);
    REQUIRE(stat_model.odds_health_issue == .5);
    REQUIRE(stat_model.odds_mech_issue == .5);
    REQUIRE(stat_model.pace == StatModel::Pace::MERCKX);
    REQUIRE(stat_model.wakeup_hour == 5);
  }

  SECTION("actions types work") {
    ActionModel &model = actions->getAction(2);

    REQUIRE_FALSE(model.isNoneType());
    REQUIRE(model.isStatChangeType());
    REQUIRE(model.isStoreType());

    REQUIRE(model.item_ids[0] == 0);
    REQUIRE(model.item_ids[1] == 1);

    REQUIRE(model.stat_delta.kit_weight == 5);
  }
}