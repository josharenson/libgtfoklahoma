/*
 * This file is part of the libgtfoklahoma distribution
 * (https://github.com/arenson/libgtfoklahoma) Copyright (c) 2020 Josh Arenson.
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

using namespace libgtfoklahoma;
using namespace testhelpers;

TEST_CASE("Issues", "[unit]") {

  const char *issuesJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "HEALTH ISSUE",
      "display_name": "",
      "image_url": "",
      "type": "HEALTH"
    },
    {
      "id": 10,
      "actions": [0],
      "description": "MECHANICAL ISSUE",
      "display_name": "",
      "dependent_inventory": [0],
      "image_url": "",
      "stat_changes": [{"max_mph": 1}],
      "type": "MECHANICAL"
    }
  ]
  )";

  Game game("", validActionJson, validEndingJson, validEventJson, issuesJson, validItemJson);
  auto &issues = game.getIssues();

  SECTION("Issues::popRandomIssue") {
    // This isn't random because there is only one of each type
    auto id = issues->popRandomIssueId(IssueModel::Type::HEALTH);
    REQUIRE(issues->getIssue(id).description == "HEALTH ISSUE");
    issues->handleIssue(id, nullptr);

    // Issue can't be served again
    id = issues->popRandomIssueId(IssueModel::Type::HEALTH);
    REQUIRE(issues->getIssue(id) == Issues::kEmptyIssueModel);

    // Dependent actions and inventory work
    id = issues->popRandomIssueId(IssueModel::Type::MECHANICAL);
    REQUIRE(issues->getIssue(id) == Issues::kEmptyIssueModel);
    game.getActions()->handleAction(0, nullptr);

    // Should still be empty as inventory requirements aren't met
    id = issues->popRandomIssueId(IssueModel::Type::MECHANICAL);
    REQUIRE(issues->getIssue(id) == Issues::kEmptyIssueModel);

    // NOW we should get the issue as all requirments have been met
    game.addItemToInventory(0);
    id = issues->popRandomIssueId(IssueModel::Type::MECHANICAL);
    REQUIRE_FALSE(issues->getIssue(id) == Issues::kEmptyIssueModel);
  }

  SECTION("Issues::getIssuesThatHaveAlreadyHappened") {
    auto haveHappened = issues->getIssuesThatHaveAlreadyHappened();
    REQUIRE(haveHappened.empty());

    auto id = issues->popRandomIssueId(IssueModel::Type::HEALTH);
    issues->handleIssue(id, nullptr);
    haveHappened = issues->getIssuesThatHaveAlreadyHappened();
    REQUIRE(haveHappened.count(0));
  }

}
