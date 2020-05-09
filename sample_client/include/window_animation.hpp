/*
 * This file is part of the libgtfoklahoma distribution
 * (https://github.com/arenson/gtfoklahoma) Copyright (c) 2020 Josh Arenson.
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
#include <cstdint>
#include <string>
#include <thread>
#include <vector>

namespace gtfoklahoma {
class Window;
class WindowAnimation {
public:
  WindowAnimation(const Window &window, std::vector<std::string> buffer, int32_t frameDelayMs);
  ~WindowAnimation();

  void start();
  void stop();

private:
  void loop();

private:
  const Window &m_window;
  int32_t m_curX;
  std::vector<std::string> m_buffer;
  std::thread m_animation;
  std::atomic<bool> m_running;
  const std::chrono::milliseconds m_frameDelay;
};
} // namespace gtfoklahoma