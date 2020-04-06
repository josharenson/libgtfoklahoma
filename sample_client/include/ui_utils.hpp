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

namespace gtfoklahoma {

struct Text;
struct Window;
class UIUtils {
public:
  UIUtils();
  ~UIUtils();

  void clearWindow(const Window &window);
  void drawText(const Text &text);

  [[nodiscard]] int32_t termHeight() const;
  [[nodiscard]] int32_t termWidth() const;

private:
  std::string elide(const std::string &str, int32_t result_length,
                           const std::string &elideText = "...");

  bool writeWord(const std::string &word, int32_t row, int32_t col_begin,
                        int32_t col_end, uint16_t fg_color, uint16_t bg_color);

private:
  int32_t m_height;
  int32_t m_width;

  inline static const uint32_t kEmptyChar = static_cast<uint32_t>(' ');
};
}