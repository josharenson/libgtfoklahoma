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
#include <string>

namespace libgtfoklahoma {
class Game {
public:
  explicit Game(std::string name);

  [[nodiscard]] int32_t hour() const;
  int32_t bumpHour();
  void setHour(int32_t hour);

  [[nodiscard]] int32_t mile() const;
  int32_t bumpMile();
  void setMile(int32_t mile);

  [[nodiscard]] int64_t tick() const;
  int64_t bumpTick();
  void setTick(int64_t tick);

  // TODO: Make dynamic based on stats
  int32_t ticksUntiNextMile() const;

private:
  int32_t m_currentHour;
  int32_t m_currentMile;
  int64_t m_currentTick;
  std::string m_name;
};
}