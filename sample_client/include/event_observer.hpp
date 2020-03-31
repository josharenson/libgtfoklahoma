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
struct ActionModel;
struct EventModel;
struct ItemModel;
}
namespace gtfoklahoma {

class EventObserver : public libgtfoklahoma::IEventObserver {
public:

  ~EventObserver() override = default;

  void onHourChanged(int32_t hour) override;
  void onMileChanged(int32_t mile) override;
  void onEvent(libgtfoklahoma::EventModel &model, std::vector<std::reference_wrapper<libgtfoklahoma::ActionModel>> &actions) override;
  void onStoreEntered(libgtfoklahoma::ActionModel &action, std::vector<libgtfoklahoma::ItemModel> &items) override;
};
} // namespace gtfoklahoma