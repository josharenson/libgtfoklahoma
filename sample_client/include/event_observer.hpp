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

#include <libgtfoklahoma/event_observer.hpp>

namespace libgtfoklahoma {
class Game;
struct ActionModel;
struct EndingModel;
struct EventModel;
struct IssueModel;
struct ItemModel;
struct StatModel;
}
namespace gtfoklahoma {

class Ui;
class EventObserver : public libgtfoklahoma::IEventObserver {
public:

  explicit EventObserver(libgtfoklahoma::Game &game, Ui &ui);
  ~EventObserver() override = default;

  void onGameOver(const libgtfoklahoma::EndingModel &ending) override;
  void onHourChanged(int32_t hour) override;
  void onMileChanged(int32_t mile) override;
  bool onEvent(libgtfoklahoma::EventModel &event) override;
  bool onIssueOccurred(libgtfoklahoma::IssueModel &issue) override;
  void onStatsChanged(const libgtfoklahoma::StatModel &stats) override;
  bool onStoreEntered(libgtfoklahoma::ActionModel &action) override;

private:
  Ui &m_ui;
};
} // namespace gtfoklahoma