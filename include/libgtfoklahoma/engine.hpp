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

#include <atomic>
#include <memory>
#include <thread>
#include <vector>


namespace libgtfoklahoma {

class Game;
class Engine {
public:
  explicit Engine(Game &game);
  ~Engine();

  void start();
  void stop();

private:
  int32_t getNextHour() const;
  void mainLoop();

private:
  Game &m_game;

  std::thread m_eventLoopThread;
  std::atomic<bool> m_running;
};
}
