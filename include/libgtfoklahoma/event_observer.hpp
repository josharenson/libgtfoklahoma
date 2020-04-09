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

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace libgtfoklahoma {

class Game;
class ActionModel;
struct EventModel;
struct IssueModel;
struct ItemModel;
struct StatModel;
class IEventObserver {
public:

  explicit IEventObserver(Game &game)
  : m_game(game){}

  virtual ~IEventObserver() = default;

  /*
   * @param hour The hour of the day 0 - 23.
   */
  virtual void onHourChanged(int32_t hour) = 0;

  /*
   * @param mile The current mile of the journey. Wouldn't be very Texan to use
   * kilometers...
   */
  virtual void onMileChanged(int32_t mile) = 0;

  /**
   * @param event A reference to the model describing the point of interest.
   * @return true if this observer is responsible for handling this action
   */
  virtual bool onEvent(EventModel &event) = 0;

  /**
   *
   * @param issue - A reference to the IssueModel that occurred
   * @return true if this observer is responsible for handling this action
   */
  virtual bool onIssueOccurred(IssueModel &issue) = 0;

  /**
   *
   * @param stats - A reference to the updated stat model
   */
  virtual void onStatsChanged(StatModel &stats) = 0;

  /**
   *
   * @param action - A referene to the action that triggered entering the store
   * @return true if this observer is responsible for handling this action
   */
  virtual bool onStoreEntered(ActionModel &action) = 0;

protected:
  Game &m_game;
};
}
