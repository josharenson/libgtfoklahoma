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

#include <window_animation.hpp>

#include <ui_utils.hpp>
#include <window.hpp>

using namespace gtfoklahoma;

WindowAnimation::WindowAnimation(const Window &window,
                                 std::vector<std::string> buffer,
                                 int32_t frameDelayMs)
    : m_window(window), m_curX(window.x()), m_buffer(std::move(buffer))
    , m_running(false), m_frameDelay(frameDelayMs)
    { start(); }

WindowAnimation::~WindowAnimation() { stop(); }

void WindowAnimation::start() {
  m_running = true;
  m_animation = std::thread(&WindowAnimation::loop, this);
}

void WindowAnimation::stop() {
  m_running = false;
  if (m_animation.joinable()) {
    m_animation.join();
  }
}

void WindowAnimation::loop() {
  while (m_running) {
    if (m_curX >= m_window.width + m_window.x() + m_buffer[0].length()) {
      m_curX = m_window.x();
    } else {
      m_curX += 5;
    }
    UIUtils::clearWindow(m_window);
    UIUtils::blitRaw(m_buffer, m_curX, m_window.y(), m_window.fg_color,
                     m_window.bg_color);
    std::this_thread::sleep_for(m_frameDelay);
  }
}
