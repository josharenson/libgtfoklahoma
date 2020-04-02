/*
 * This file is part of the libgtfoklahoma distribution
 * (https://github.com/arenson/gtfoklahoma) Copyright (c) 2020 Josh Arenson.
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

#include <libgtfoklahoma/event_observer.hpp>
#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/issues.hpp>

using namespace testhelpers;

TEST_CASE("Issues", "[unit]") {
  using namespace libgtfoklahoma;

  SECTION("Test issues public api") {
    const char *bare_minimum_issue = R"([{
        "id": 1,
        "actions": [0],
        "description": "description",
        "display_name": "display_name",
        "image_url": "image_url",
        "type": "HEALTH"
      }])";
    Game game("", validActionJson, validEventJson, bare_minimum_issue, validItemJson);

    // No issues have happened yet
    auto &issues = game.getIssues();
    REQUIRE(issues->getIssuesThatHaveAlreadyHappened().empty());

    // Mechanical issues are empty
    REQUIRE(issues->popRandomIssue(IssueModel::Type::MECHANICAL) == Issues::kEmptyIssueModel);

    // Health issue is the expected
    auto &health_issue = issues->popRandomIssue(IssueModel::Type::HEALTH);
    REQUIRE_FALSE(health_issue == Issues::kEmptyIssueModel);
    REQUIRE(health_issue.id == 1);
    REQUIRE(health_issue.description == "description");
    REQUIRE(health_issue.display_name == "display_name");
    REQUIRE(health_issue.image_url == "image_url");
    REQUIRE(health_issue.type == IssueModel::Type::HEALTH);

    // Issue was prevented from happening again
    health_issue = issues->popRandomIssue(IssueModel::Type::HEALTH);
    REQUIRE(health_issue == Issues::kEmptyIssueModel);
  }

  SECTION("Dependent actions work") {
    const char *action = R"(
    [
      {
        "id": 0,
        "display_name": "",
        "type": ["NONE"]
      }
    ]
    )";

    const char *issue = R"(
    [
      {
        "id": 0,
        "actions": [0],
        "dependent_actions": [0],
        "description": "",
        "display_name": "",
        "image_url": "",
        "type": "MECHANICAL"
      }
    ]
    )";
    Game game("", action, validEventJson, issue, validItemJson);
    auto &issues = game.getIssues();

    // There should initially be no mechanical issues available
    REQUIRE(issues->popRandomIssue(IssueModel::Type::MECHANICAL) == Issues::kEmptyIssueModel);

    // Perform the dependent action
    game.getActions()->handleAction(0, nullptr);

    // Issue should be available now
    REQUIRE_FALSE(issues->popRandomIssue(IssueModel::Type::MECHANICAL) == Issues::kEmptyIssueModel);
  }

  SECTION("Dependent items work") {

    const char *issue = R"(
    [
      {
        "id": 0,
        "actions": [0],
        "dependent_inventory": [0],
        "description": "",
        "display_name": "",
        "image_url": "",
        "type": "MECHANICAL"
      }
    ]
    )";

    const char *item = R"(
    [
      {
        "id": 0,
        "category": "MISC",
        "cost": 0,
        "display_name": "",
        "image_url": "",
        "type": ["NONE"]
      }
    ]
    )";

    Game game("", validActionJson, validEventJson, issue, item);

    auto &issues = game.getIssues();

    // There should initially be no mechanical issues available
    REQUIRE(issues->popRandomIssue(IssueModel::Type::MECHANICAL) == Issues::kEmptyIssueModel);

    // Acquire the item
    game.addItemToInventory(0);

    // Issue should be available now
    REQUIRE_FALSE(issues->popRandomIssue(IssueModel::Type::MECHANICAL) == Issues::kEmptyIssueModel);
  }

  SECTION("Issue stat changes work") {
    const char *issueJson = R"(
    [
      {
        "id": 0,
        "actions": [0],
        "description": "",
        "display_name": "",
        "image_url": "",
        "stat_changes": [
          {"max_mph": -1}
        ],
        "type": "MECHANICAL"
      }
    ]
    )";

    Game game("", validActionJson, validEventJson, issueJson, validItemJson);
    auto initial_max_mph = game.getStats()->max_mph;

    // Popping the only issue should apply the stat delta
    auto issue = game.getIssues()->popRandomIssue(IssueModel::Type::MECHANICAL);
    REQUIRE_FALSE(issue == Issues::kEmptyIssueModel);
    REQUIRE(game.getStats()->max_mph == initial_max_mph - 1);
  }
}