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

#include <chrono>
#include <condition_variable>
#include <mutex>

#include "helpers.hpp"

#include <libgtfoklahoma/actions.hpp>
#include <libgtfoklahoma/engine.hpp>
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
      "stat_changes_regardless": [{}],
      "type": ["STAT_CHANGE"]
    },
    {
      "display_name": "display_name_1",
      "id": 10,
      "stat_changes_regardless": [
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
      "stat_changes_regardless": [{"kit_weight": 5}],
      "type": ["STAT_CHANGE", "STORE"]
    }
  ]
  )";

  Game game("", actionJson, validEndingJson, validEventJson, validIssueJson, validItemJson);
  auto &actions = game.getActions();

  SECTION("Action::getAction") {
    {
      auto &action = actions.getAction(0);
      REQUIRE_FALSE(action == actions.kEmptyActionModel);
    }

    {
      auto &action = actions.getAction(-1);
      REQUIRE(action == actions.kEmptyActionModel);
    }
  }

  SECTION("Action::actionHasHappened") {
      REQUIRE_FALSE(actions.actionHasHappened(0));
      actions.handleAction(0, nullptr);
      REQUIRE(actions.actionHasHappened(0));
  }

  SECTION("Action::setActionsThatHaveAlreadyHappened") {
      REQUIRE_FALSE(actions.actionHasHappened(0));
      actions.setActionsThatHaveAlreadyHappened({0});
      REQUIRE(actions.actionHasHappened(0));
  }

  SECTION("ActionModel::purchaseItem") {
      REQUIRE(actions.getAction(0).purchaseItem(0));
      actions.getAction(0).completePurchase();
      actions.getAction(0).purchaseComplete().get();
      auto purchasedItems = game.getInventory();

      REQUIRE(purchasedItems.size());
      REQUIRE(purchasedItems[0].get().id == 0);
  }

  SECTION("ActionModel::is*type") {
    ActionModel &model = actions.getAction(2);

    REQUIRE_FALSE(model.isNoneType());
    REQUIRE(model.isStatChangeType());
    REQUIRE(model.isStoreType());

    REQUIRE(model.item_ids[0] == 0);
    REQUIRE(model.item_ids[1] == 1);

    REQUIRE(model.stat_delta_regardless.kit_weight == 5);
  }
}

namespace TestActionEndingHints {
std::mutex mutex;
bool running = true;
std::condition_variable simulation;
const std::chrono::milliseconds timeout(1000);
} // namespace TestActionEndingHints
TEST_CASE("Actions - Ending hints") {

  const char *eventJson = R"(
  [
    {
      "id": 0,
      "actions": [0],
      "description": "",
      "display_name": "",
      "mile": 0
    }
  ]
  )";

  const char *actionsJson = R"(
  [
    {
      "display_name": "",
      "ending_id_hints": [0],
      "id": 0,
      "items": [0],
      "stat_changes_regardless": [
        {"health": -10000}
      ],
      "type": ["STAT_CHANGE"]
    }
  ]
  )";

  Game game("", actionsJson, validEndingJson, eventJson, validIssueJson, validItemJson);

  // We can't mock this because ownership is transferred and then blah blah
  // deleting an object that was already deleted something custom deleter didn't work
  // without crazy templates everywhere so screw it.
  class NonsenseObserver : public TestObserver {
  public:
    explicit NonsenseObserver(Game &game) : TestObserver(game) {}
    void onGameOver(const EndingModel &ending) override {
      REQUIRE(ending.id == 0);
      TestActionEndingHints::running = false;
      TestActionEndingHints::simulation.notify_one();
    }
    bool onEvent(EventModel &event) override {
      event.chooseAction(0);
      return true;
    }
  };

  auto observer = std::make_unique<NonsenseObserver>(game);
  game.registerEventObserver(std::move(observer));

  Engine engine(game);
  engine.start();

  std::unique_lock<std::mutex> lock(TestActionEndingHints::mutex);
  if (!TestActionEndingHints::simulation.wait_until(
          lock,
          std::chrono::system_clock::now() + TestActionEndingHints::timeout,
          []() { return !TestActionEndingHints::running; })) {
    FAIL("Timed out while waiting for the test to simulate death. Profound.");
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
  auto mockObserverPtr = std::shared_ptr<IEventObserver>(&mockObserver.get());

  Game game("", actionsJson, validEndingJson, validEventJson, validIssueJson, itemsJson);
  //Engine engine(game);
  //engine.registerEventObserver(std::move(mockObserverPtr));

  SECTION("Purchased item is added to inventory when funds are available") {
    StatModel stats;
    stats.money_remaining = 1; // Make sure there is enough
    game.updateStats(stats);
    REQUIRE(game.getStats().getPlayerStatsModel().money_remaining >= 1);
    auto initial_money = game.getStats().getPlayerStatsModel().money_remaining;

    // Tell the mock to purchase the item
    When(Method(mockObserver, onStoreEntered)).AlwaysDo([](ActionModel &action){
      auto UNUSED = action.purchaseItem(0);
      action.completePurchase();
      return true;
    });

    game.getActions().handleAction(0, mockObserverPtr);
    auto inventory = game.getInventory();
    REQUIRE(inventory.size() == 1);
    REQUIRE(inventory[0].get().id == 0);
    auto current_money = game.getStats().getPlayerStatsModel().money_remaining;
    REQUIRE(current_money == initial_money - 1);
  }
}

TEST_CASE("Actions - Success/Failure") {
 SECTION("Failure case") {
    const char *actionsJson = R"(
    [
      {
        "display_name": "",
        "id": 0,
        "items": [0],
        "success_chance": 0.0,
        "stat_changes_regardless": [{"max_mph": 1}],
        "type": ["STAT_CHANGE"]
      }
    ]
    )";
    Game game("", actionsJson, validEndingJson, validEventJson, validIssueJson, validItemJson);
    auto &action = game.getActions().getAction(0);
    REQUIRE(action.failed());
  }
}

TEST_CASE("Actions - inventory dependent") {
  const char *actionsJson = R"(
  [
    {
      "display_name": "",
      "id": 0,
      "dependent_inventory_ids":[
        {"id": 0, "qty": 1}
      ],
      "type": ["NONE"]
    }
  ]
  )";
  Game game("", actionsJson, validEndingJson, validEventJson, validIssueJson, validItemJson);

  SECTION("ActionModel::isVisible - negative case") {
    REQUIRE_FALSE(game.getActions().getAction(0).isVisible());
  }

  SECTION("ActionModel::isVisible - positive case") {
    game.addItemToInventory(0, 1);
    REQUIRE(game.getActions().getAction(0).isVisible());
  }
}

TEST_CASE("Actions - inventory deltas") {
  const char *actionsJson = R"(
  [
    {
      "display_name": "",
      "id": 0,
      "inventory_delta_regardless": [
        {"id": 0, "qty": -1}
      ],
      "inventory_delta_on_success": [
        {"id": 1, "qty": 1}
      ],
      "success_chance": 1.0,
      "type": ["NONE"]
    },
    {
      "display_name": "",
      "id": 1,
      "inventory_delta_regardless": [
        {"id": 0, "qty": -1}
      ],
      "inventory_delta_on_failure": [
        {"id": 2, "qty": 2}
      ],
      "success_chance": 0.0,
      "type": ["NONE"]
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
    },
    {
      "id": 1,
      "category": "MISC",
      "cost": 1,
      "display_name": "",
      "image_url": "",
      "stat_changes": [{"money_remaining": -1}]
    },
    {
      "id": 2,
      "category": "MISC",
      "cost": 1,
      "display_name": "",
      "image_url": "",
      "stat_changes": [{"money_remaining": -1}]
    }
  ]
  )";

  Game game("", actionsJson, validEndingJson, validEventJson, validIssueJson, itemsJson);

  SECTION("Actions - inventory delta regardless") {
    game.addItemToInventory(0, 1);
    REQUIRE(game.inventoryCount(0) == 1);
    game.getActions().handleAction(0, nullptr);
    REQUIRE(game.inventoryCount(0) == 0);
  }

  SECTION("Actions - inventory delta on success") {
    game.getActions().handleAction(0, nullptr);
    REQUIRE(game.inventoryCount(1) == 1);
  }

  SECTION("Actions - inventory delta on failure") {
    game.getActions().handleAction(1, nullptr);
    REQUIRE(game.inventoryCount(2) == 2);
  }
}