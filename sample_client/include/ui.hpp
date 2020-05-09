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

#include <memory>
#include <vector>

#include <ui_utils.hpp>
#include <window.hpp>

namespace libgtfoklahoma {
struct ActionModel;
struct EventModel;
struct IssueModel;
struct ItemModel;
struct StatModel;
}

namespace gtfoklahoma {
class Ui {
public:
  Ui();
  ~Ui();

  // Call when the into is over to initialize all the windows
  void renderBeginGame(const libgtfoklahoma::StatModel &initialStats, int32_t initialMile, int32_t initialHour);

  /** Things that happen in the main window **/
  [[nodiscard]] const Window &mainWindow() const;
  void renderEvent(const libgtfoklahoma::EventModel &event,
                   const std::vector<std::reference_wrapper<libgtfoklahoma::ActionModel>> &actions);

  void renderIssue(const libgtfoklahoma::IssueModel &issue,
                   const std::vector<std::reference_wrapper<libgtfoklahoma::ActionModel>> &actions);

  void renderStore(const libgtfoklahoma::ActionModel &action,
                   const std::vector<std::reference_wrapper<libgtfoklahoma::ItemModel>> &items);

  /** Things that happen in the inventory window **/
  [[nodiscard]] const Window &inventoryWindow() const;

  /** Things that happen in the stats window **/
  [[nodiscard]] const Window &statusWindow() const;
  void renderStats(const libgtfoklahoma::StatModel &stats,
                   int32_t mile,
                   int32_t hour);

  [[nodiscard]] const Window &inputBar() const;
  [[nodiscard]] const Window &bottomBar() const;

private:
  struct NumberedListViewModel {
      /*
       * ---------------------------------------
       * - HEADER                              -
       * ---------------------------------------
       *
       * ---------------------------------------
       * - BODY (wrapped)                      -
       * -                                     -
       * -                                     -
       * ---------------------------------------
       *
       * ---------------------------------------
       * - LIST_HEADER                         -
       * -    0. Item                          -
       * -    1. Item                          -
       * -    ...                              -
       * ---------------------------------------
       *
       */
      explicit NumberedListViewModel(Window &window)
      : window(window) {}
      std::string header;
      std::string body;
      std::string list_header;
      std::vector<std::string> items;
      Window &window;
  };

  static void renderNumberedListView(const NumberedListViewModel &viewModel);

private:
  int32_t m_termHeight;
  int32_t m_termWidth;

  UIUtils m_utils;

  Window m_mainWindow;
  Window m_inventoryWindow;
  Window m_statsWindow;
  Window m_inputBar;
  Window m_bottomBar;
};
}