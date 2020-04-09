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

struct Anchors {
  Anchors()
  : bottom(-1)
  , left(-1)
  , right(-1)
  , top(-1)
  , horizontalCenter(-1)
  , verticalCenter(-1) {}

  int32_t bottom;
  int32_t left;
  int32_t right;
  int32_t top;

  int32_t horizontalCenter;
  int32_t verticalCenter;
};

struct Margins {
  Margins()
  : bottom(0)
  , left(0)
  , right(0)
  , top(0) {}

  explicit Margins(int32_t margin)
  : bottom(margin)
  , left(margin)
  , right(margin)
  , top(margin) {}

  int32_t bottom;
  int32_t left;
  int32_t right;
  int32_t top;
};

struct WMObject {
  Anchors anchors;
  Margins margins;

  // Doesn't include margins
  int32_t height = -1;

  // Doesn't include margins
  int32_t width = -1;

  uint16_t bg_color = 0x03;
  uint16_t fg_color = 0x08;

  // Includes margins
  [[nodiscard]] int32_t x() const;
  [[nodiscard]] int32_t y() const;

  // Includes margins, intended to be used for anchoring
  [[nodiscard]] int32_t bottom() const;
  [[nodiscard]] int32_t left() const;
  [[nodiscard]] int32_t top() const;
  [[nodiscard]] int32_t right() const;
  [[nodiscard]] int32_t horizontalCenter() const;
  [[nodiscard]] int32_t verticalCenter() const;
};

struct Window : public WMObject {};
}