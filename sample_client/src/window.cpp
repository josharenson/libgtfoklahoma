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
    return anchors.horizontalCenter - (rawWidth() / 2);
  }

  return anchors.left + margins.left;
}

int32_t WMObject::y() const {
  if (anchors.verticalCenter > 0) {
    return anchors.verticalCenter - (rawHeight() / 2);
  }

  return anchors.top + margins.top;
}

int32_t WMObject::bottom() const {
  return y() + rawHeight() + margins.bottom;
}

int32_t WMObject::left() const {
  return x() - margins.left;
}

int32_t WMObject::top() const {
  return y() - margins.top;
}

int32_t WMObject::right() const {
  return x() + rawWidth() + margins.right;
}

int32_t WMObject::horizontalCenter() const {
  return x() + (rawWidth() / 2);
}

int32_t WMObject::verticalCenter() const {
  return y() + (rawHeight() / 2);
}

int32_t WMObject::rawHeight() const { return height; }
int32_t WMObject::rawWidth() const { return width; }

int32_t Window::rawHeight() const {
  if (heightPercent < 0 && height < 0) {
    spdlog::warn("Not enough height information to draw window!");
    return 0;
  }

  // Absolute values have priority
  if (height > 0) { return height; }
  else return (m_termHeight * heightPercent);
}

int32_t Window::rawWidth() const {
  if(widthPercent < 0 && width < 0) {
    spdlog::warn("Not enough width information to draw window!");
    return 0;
  }

  // Absolute values have priority
  if (width > 0) { return width; }
  else return (m_termWidth * widthPercent);
}

