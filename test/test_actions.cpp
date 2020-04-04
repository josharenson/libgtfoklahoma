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

#include <catch2/catch.hpp>
#include <catch/fakeit.hpp>

#include "helpers.hpp"

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/event_observer.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace fakeit;
using namespace libgtfoklahoma;
using namespace testhelpers;

TEST_CASE("Actions", "[unit]") {

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
      "id": 10,
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

  SECTION("Action::getAction") {
    {
      auto &action = actions->getAction(0);
      REQUIRE_FALSE(action == Actions::kEmptyActionModel);
    }

    {
      auto &action = actions->getAction(-1);
      REQUIRE(action == Actions::kEmptyActionModel);
    }
  }

  SECTION("Action::actionHasHappened") {
      REQUIRE_FALSE(actions->actionHasHappened(0));
      actions->handleAction(0, nullptr);
      REQUIRE(actions->actionHasHappened(0));
  }

  SECTION("Action::setActionsThatHaveAlreadyHappened") {
      REQUIRE_FALSE(actions->actionHasHappened(0));
      actions->setActionsThatHaveAlreadyHappened({0});
      REQUIRE(actions->actionHasHappened(0));
  }

  SECTION("ActionModel::purchaseItem") {
      actions->getAction(0).purchaseItem(0);
      actions->getAction(0).completePurchase();
      auto purchasedItems = actions->getAction(0).purchasedItems().get();

      REQUIRE(purchasedItems.size());
      REQUIRE(purchasedItems[0] == 0);
  }

  SECTION("ActionModel::is*type") {
    ActionModel &model = actions->getAction(2);

    REQUIRE_FALSE(model.isNoneType());
    REQUIRE(model.isStatChangeType());
    REQUIRE(model.isStoreType());

    REQUIRE(model.item_ids[0] == 0);
    REQUIRE(model.item_ids[1] == 1);

    REQUIRE(model.stat_delta.kit_weight == 5);
  }
}

TEST_CASE("Actions - Store Type Actions") {
  // Describe a store we can enter
  const char *actionsJson = R"(
  [
    {
      "display_name": "",
      "id": 0,
      "items": [0],
      "type": ["STORE"]
    }
  ]
  )";

  const char *itemsJson = R"(
  [
    {
      "id": 0,
      "category": "MISC",
      "cost": 1,
      "display_name": "",
      "image_url": "",
      "stat_changes": [{"money_remaining": -1}]
    }
  ]
  )";

  Mock<IEventObserver> mockObserver;
  Fake(Dtor(mockObserver));

  auto mockObserverPtr = std::unique_ptr<IEventObserver>(&mockObserver.get());

  Game game("", actionsJson, validEventJson, validIssueJson, itemsJson);
  //Engine engine(game);
  //engine.registerEventObserver(std::move(mockObserverPtr));

  SECTION("Purchased item is added to inventory when funds are available") {
    StatModel stats;
    stats.money_remaining = 1; // Make sure there is enough
    game.updateStats(stats);
    REQUIRE(game.getStats()->money_remaining >= 1);
    auto initial_money = game.getStats()->money_remaining;

    // Tell the mock to purchase the item
    When(Method(mockObserver, onStoreEntered)).AlwaysDo([](ActionModel &action){
      action.purchaseItem(0);
      action.completePurchase();
      return true;
    });

    game.getActions()->handleAction(0, mockObserverPtr);
    auto inventory = game.getInventory();
    REQUIRE(inventory.size() == 1);
    REQUIRE(inventory[0].get().id == 0);
    auto current_money = game.getStats()->money_remaining;
    REQUIRE(current_money == initial_money - 1);
  }
}