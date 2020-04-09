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

#include <window.hpp>

#include <spdlog/spdlog.h>

using namespace gtfoklahoma;

int32_t WMObject::x() const {
  if (anchors.horizontalCenter > 0) {
    return anchors.horizontalCenter - (width / 2);
  }

  if (anchors.left >= 0) {
    return anchors.left + margins.left;
  }

  if (anchors.right >= 0) {
    return anchors.right - width - margins.right;
  }

  spdlog::warn("Not enough information to position x of this Window");
  return 0;
}

int32_t WMObject::y() const {
  if (anchors.verticalCenter > 0) {
    return anchors.verticalCenter - (height / 2);
  }

  if (anchors.top >= 0) {
    return anchors.top + margins.top;
  }

  if (anchors.bottom >= 0) {
    return anchors.bottom - height - margins.bottom;
  }

  spdlog::warn("Not enough information to position y of this Window");
  return 0;
}

int32_t WMObject::bottom() const {
  return y() + height + margins.bottom;
}

int32_t WMObject::left() const {
  return x() - margins.left;
}

int32_t WMObject::top() const {
  return y() - margins.top;
}

int32_t WMObject::right() const {
  return x() + width + margins.right;
}

int32_t WMObject::horizontalCenter() const {
  return x() + (width / 2);
}

int32_t WMObject::verticalCenter() const {
  return y() + (height / 2);
}
