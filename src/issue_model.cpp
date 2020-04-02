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

#include <libgtfoklahoma/issue_model.hpp>

using namespace libgtfoklahoma;

bool IssueModel::operator==(const IssueModel &rhs) const {
  return this->id == rhs.id &&
         this->actions == rhs.actions &&
         this->dependent_actions == rhs.dependent_actions &&
         this->dependent_inventory == rhs.dependent_inventory &&
         this->description == rhs.description &&
         this->display_name == rhs.display_name &&
         this->image_url == rhs.image_url &&
         this->stat_delta == rhs.stat_delta &&
         this->type == rhs.type;
}
