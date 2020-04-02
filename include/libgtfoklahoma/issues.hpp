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

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <libgtfoklahoma/issue_model.hpp>
#include <libgtfoklahoma/stats.hpp>

namespace libgtfoklahoma {

class Game;
class Issues {
public:
  explicit Issues(Game &game, const char *issuesJson=kIssuesJson);
  IssueModel &popRandomIssue(IssueModel::Type type);

  [[nodiscard]] std::unordered_set<int32_t> getIssuesThatHaveAlreadyHappened() const;
  void setIssuesThatHaveAlreadyHappened(std::unordered_set<int32_t> ids);

public:
  inline static IssueModel kEmptyIssueModel;

private:
  [[nodiscard]] bool canServeIssue(int32_t issueId) const;

private:
  Game &m_game;

  std::unordered_map<IssueModel::Type, std::vector<int32_t>> m_issuesByType;
  std::unordered_map<int32_t, IssueModel> m_issuesById;
  std::unordered_set<int32_t> m_issuesThatHaveAlreadyHappened;

  inline static const char *kIssuesJson = R"JSON(
  [
    {
      "id": 0,
      "actions": [0, 20],
      "description": "A dodgy fella offers to share a homemade cigarette with you.",
      "display_name": "Roast a bone?",
      "dependent_inventory": [1],
      "image_url": "",
      "type": "HEALTH"
    },
    {
      "id": 10,
      "actions": [1],
      "description": "Probably shouldn't have shared a joint with a stranger :-/ You have a fever, trouble breathing, and a dry cough.",
      "display_name": "You have COVID-19",
      "dependent_actions": [20],
      "image_url": "",
      "stat_changes": [
        {"health": -60},
        {"max_speed": -3}
      ],
      "type": "HEALTH"
    },
    {
      "id": 20,
      "actions": [1],
      "description": "Someone stole your rear reflector.",
      "display_name": "Stop! Thief!",
      "image_url": "",
      "stat_changes": [
        {"odds_health_issue": 0.1}
      ],
      "type": "MECHANICAL"
    }
  ]
  )JSON";
};
} // namespace libgtfoklahoma