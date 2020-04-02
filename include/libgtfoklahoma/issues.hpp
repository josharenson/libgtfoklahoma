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

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <rapidjson/document.h>

#include <libgtfoklahoma/game.hpp>
#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {

struct IssueModel {
  enum class Type {INVALID, HEALTH, MECHANICAL};
  int32_t id;
  std::vector<int32_t> actions;

  // This event will only happen if all dependent actions have also happened
  std::vector<int32_t> dependent_actions;

  // This event will only happen if all dependent items are in inventory
  std::vector<int32_t> dependent_inventory;
  std::string description;
  std::string display_name;
  std::string image_url;
  StatModel stat_delta;
  Type type;
};

class Issues {
public:
  explicit Issues(Game &game, const char *issuesJson=kIssuesJson);
  std::optional<IssueModel> popRandomIssue(IssueModel::Type type);

  [[nodiscard]] std::unordered_set<int32_t> getIssuesThatHaveAlreadyHappened() const;
  void setIssuesThatHaveAlreadyHappened(std::unordered_set<int32_t> ids);

private:
  [[nodiscard]] bool canServeIssue(int32_t issueId) const;

private:
  Game &m_game;
  rapidjson::Document m_issuesDocument;
  std::unordered_map<IssueModel::Type, std::vector<int32_t>> m_issuesByType;
  std::unordered_map<int32_t, IssueModel> m_issuesById;
  std::unordered_set<int32_t> m_issuesThatHaveAlreadyHappened;
  inline static const char *kIssuesJson = R"JSON(
  [
    {
      "id": 0,
      "actions": [0, 20]
      "category": "HEALTH",
      "description": "A dodgy fella offers to share a homemade cigarette with you.",
      "display_name": "Roast a bone?",
      "dependent_inventory": [1],
      "image_url": ""
    },
    {
      "id": 10,
      "actions": [1]
      "category": "HEALTH",
      "description": "Probably shouldn't have shared a joint with a stranger :-/ You have a fever, trouble breathing, and a dry cough.",
      "display_name": "You have COVID-19",
      "dependent_actions": [20],
      "stat_changes": [
        {"health", -60},
        {"max_speed", -3}
      ]
    },
    {
      "id": 20,
      "actions": [1],
      "category": "MECHANICAL",
      "description": "Someone stole your rear reflector.",
      "display_name": "Stop! Thief!",
      "stat_changes": [
        {"odds_health_issue", 0.1}
      ]
    }
  ]
  )JSON";
};
}