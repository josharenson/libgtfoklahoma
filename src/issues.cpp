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

#include <libgtfoklahoma/issues.hpp>

#include <algorithm>
#include <iterator>
#include <random>

#include <rapidjson/document.h>
#include <spdlog/spdlog.h>

#include <libgtfoklahoma/event_observer.hpp>
#include <libgtfoklahoma/game.hpp>

using namespace libgtfoklahoma;


Issues::Issues(Game &game, const char *issuesJson)
: m_game(game) {

  rapidjson::Document issuesDocument;
  if (issuesDocument.Parse(issuesJson).HasParseError() ||
      !issuesDocument.IsArray()) {
    spdlog::error("Parsing error when parsing issues json");
    abort();
  }

  auto issueIsValid = [](const rapidjson::Value &value) {
    return value.HasMember("id") && value["id"].IsInt() &&
           value.HasMember("actions") && value["actions"].IsArray() &&
           value.HasMember("description") && value["description"].IsString() &&
           value.HasMember("display_name") && value["display_name"].IsString() &&
           value.HasMember("image_url") && value["image_url"].IsString() &&
           value.HasMember("type") && value["type"].IsString();
  };

  auto getIssueType = [](const rapidjson::Value &issue) {
    auto category_str = issue["type"].GetString();
    if (!strcmp(category_str, "HEALTH")) { return IssueModel::Type::HEALTH; }
    else if (!strcmp(category_str, "MECHANICAL")) { return IssueModel::Type::MECHANICAL; }
    else { return IssueModel::Type::INVALID; }
  };

  auto idsFromArray = [](const rapidjson::GenericArray<true, rapidjson::Value> &arr) {
    std::vector<int32_t> result;
    for (const auto &id : arr) {
      result.emplace_back(id.GetInt());
    }
    return result;
  };

  for (const auto &issue : issuesDocument.GetArray()) {
    if (!issueIsValid(issue)) {
      spdlog::warn("Error parsing issue type");
      continue;
    }

    IssueModel model;
    model.id = issue["id"].GetInt();
    for (const auto &actionId : issue["actions"].GetArray()) {
      model.actions.emplace_back(actionId.GetInt());
    }
    model.description = issue["description"].GetString();
    model.display_name = issue["display_name"].GetString();
    model.image_url = issue["image_url"].GetString();
    model.type = getIssueType(issue);

    if (issue.HasMember("dependent_actions") && issue["dependent_actions"].IsArray()) {
      model.dependent_actions = idsFromArray(issue["dependent_actions"].GetArray());
    }

    if (issue.HasMember("dependent_inventory") && issue["dependent_inventory"].IsArray()) {
      model.dependent_inventory = idsFromArray(issue["dependent_inventory"].GetArray());
    }

    if (issue.HasMember("ending_id_hints") && issue["ending_id_hints"].IsArray()) {
      for (const auto &endingId : issue["ending_id_hints"].GetArray()) {
        model.ending_id_hints.emplace_back(endingId.GetInt());
      }
    }

    if (issue.HasMember("stat_changes") && issue["stat_changes"].IsArray()) {
      model.stat_delta = Stats::FromJson(issue["stat_changes"].GetArray());
    }

    if (!m_issuesByType.count(model.type)) {
      m_issuesByType[model.type] = {model.id};
    } else {
      m_issuesByType[model.type].push_back(model.id);
    }
    auto id = model.id;
    m_issuesById[id] = std::move(model);
  }
}

IssueModel &Issues::getIssue(int32_t id) {
  if (m_issuesById.count(id)) {
    return m_issuesById[id];
  }

  spdlog::warn("Requested issue id {} that does not exist.", id);
  return kEmptyIssueModel;
}

void Issues::handleIssue(int32_t issueId, const std::shared_ptr<IEventObserver> &observer) {
  IssueModel &issue = getIssue(issueId);
  bool shouldHandle = observer && observer->onIssueOccurred(issue);
  if (!shouldHandle) { return; }

  spdlog::debug("Handling issue {}", issueId);

  for (const auto &endingId : issue.ending_id_hints) {
    m_game.pushEndingHintId(endingId);
  }

  auto actionId = issue.chosenAction().get();
  m_game.getActions().handleAction(actionId, observer);

  // A valid issue exists. Mark it as having happened and apply the stat delta
  m_issuesThatHaveAlreadyHappened.insert(issueId);
  m_game.updateStats(issue.stat_delta);
}

int32_t Issues::popRandomIssueId(IssueModel::Type type) {
  std::vector<int32_t> potential_issues;

  // Get a list of all issues of `type` that haven't happened yet
  std::copy_if(m_issuesByType[type].begin(), m_issuesByType[type].end(),
               std::back_inserter(potential_issues),
               [this](const int32_t id) { return canServeIssue(id); });

  // Shuffle the issues
  auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::shuffle(potential_issues.begin(), potential_issues.end(), std::default_random_engine(seed));

  if (potential_issues.empty()) { return -1; }
  else { return potential_issues[0]; }
}

std::unordered_set<int32_t> Issues::getIssuesThatHaveAlreadyHappened() const {
  return m_issuesThatHaveAlreadyHappened;
}

void Issues::setIssuesThatHaveAlreadyHappened(std::unordered_set<int32_t> ids) {
  for (const auto &id : ids) {
    m_issuesThatHaveAlreadyHappened.insert(id);
  }
}

bool Issues::canServeIssue(int32_t id) const {
  auto &issue = m_issuesById.at(id);

  // Issue can't be served if its dependent actions haven't happened
  if (!issue.dependent_actions.empty()) {
    auto &actions = m_game.getActions();
    for (const auto &action_id : issue.dependent_actions) {
      if (!actions.actionHasHappened(action_id)) { return false; }
    }
  }

  // Issue can't be served if its dependent items aren't in the inventory
  if (!issue.dependent_inventory.empty()) {
    for (const auto &item_id : issue.dependent_inventory) {
      if (!m_game.hasItemInInventory(item_id)) { return false; }
    }
  }

  return !m_issuesThatHaveAlreadyHappened.count(id);
}
