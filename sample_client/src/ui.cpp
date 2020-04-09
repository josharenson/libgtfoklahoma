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

// Self include
#include <ui.hpp>

// System includes
#include <functional>


// 3P includes
#include <spdlog/spdlog.h>
#include <termbox.h>

// Library includes
#include <libgtfoklahoma/action_model.hpp>
#include <libgtfoklahoma/event_model.hpp>
#include <libgtfoklahoma/stats.hpp>

// Local includes
#include <libgtfoklahoma/issue_model.hpp>
#include <libgtfoklahoma/item_model.hpp>
#include <ui_utils.hpp>

using namespace gtfoklahoma;
using namespace libgtfoklahoma;

Ui::Ui()
: m_termHeight(0)
, m_termWidth(0)
, m_utils(UIUtils()) {

  tb_init();
  tb_select_output_mode(TB_OUTPUT_NORMAL);
  m_termHeight = tb_height();
  m_termWidth = tb_width();

  //m_termHeight = 100;
  //m_termWidth = 100;

  m_mainWindow.height = (.6 * m_termHeight);
  m_mainWindow.width = (.95 * m_termWidth);
  m_mainWindow.anchors.horizontalCenter = m_termWidth / 2;
  m_mainWindow.anchors.top = 0;
  m_mainWindow.margins = Margins(1);
  m_mainWindow.fg_color = TB_BLACK;
  UIUtils::clearWindow(m_mainWindow);

  m_statsWindow.height = (.3 * m_termHeight);
  m_statsWindow.width = (.45 * m_termWidth);
  m_statsWindow.margins = Margins(1);
  m_statsWindow.anchors.top = m_mainWindow.bottom();
  m_statsWindow.anchors.left = m_mainWindow.left();
  m_statsWindow.bg_color = TB_BLUE;
  m_statsWindow.fg_color = TB_WHITE;
  UIUtils::clearWindow(m_statsWindow);

  m_inventoryWindow.height = (.3 * m_termHeight);
  m_inventoryWindow.width = (.45 * m_termWidth);
  m_inventoryWindow.margins = Margins(1);
  m_inventoryWindow.anchors.top = m_mainWindow.bottom();
  m_inventoryWindow.anchors.right = m_mainWindow.right();
  m_inventoryWindow.bg_color = TB_WHITE;
  m_inventoryWindow.fg_color = TB_CYAN;
  UIUtils::clearWindow(m_inventoryWindow);

  m_bottomBar.height = 1;
  m_bottomBar.width = m_termWidth;
  m_bottomBar.anchors.left = 0;
  m_bottomBar.anchors.bottom = m_termHeight;
  m_bottomBar.bg_color = TB_RED;
  m_bottomBar.fg_color = TB_WHITE;
  UIUtils::clearWindow(m_bottomBar);

  m_inputBar.height = 1;
  m_inputBar.width = m_termWidth;
  m_inputBar.anchors.left = 0;
  m_inputBar.anchors.bottom = m_mainWindow.bottom();
  m_inputBar.bg_color = TB_BLACK;
  m_inputBar.fg_color = TB_WHITE;
  UIUtils::clearWindow(m_inputBar);

}

Ui::~Ui() { tb_shutdown(); }

void Ui::renderEvent(
    const libgtfoklahoma::EventModel &event,
    const std::vector<std::reference_wrapper<libgtfoklahoma::ActionModel>>
        &actions) {

  std::vector<std::string> actionNames;
  actionNames.reserve(actions.size());
  for (const auto &action : actions) {
    actionNames.emplace_back(action.get().display_name);
  }

  NumberedListViewModel viewModel(m_mainWindow);
  viewModel.header = "Point of Interest: " + event.display_name;
  viewModel.body = "Description: " + event.description;
  viewModel.list_header = "Available Actions:";
  viewModel.items = actionNames;

  renderNumberedListView(viewModel);
}

void Ui::renderIssue(
    const libgtfoklahoma::IssueModel &issue,
    const std::vector<std::reference_wrapper<libgtfoklahoma::ActionModel>>
        &actions) {

  std::vector<std::string> actionNames;
  actionNames.reserve(actions.size());
  for (const auto &action : actions) {
    actionNames.emplace_back(action.get().display_name);
  }

  NumberedListViewModel viewModel(m_mainWindow);
  viewModel.header = "Something unexpected happened: " + issue.display_name;
  viewModel.body = "Description: " + issue.description;
  viewModel.list_header = "Available Actions:";
  viewModel.items = actionNames;

  renderNumberedListView(viewModel);
}

void Ui::renderStore(
    const libgtfoklahoma::ActionModel &action,
    const std::vector<std::reference_wrapper<libgtfoklahoma::ItemModel>>
        &items) {

  std::vector<std::string> itemNames;
  itemNames.reserve(items.size());
  for (const auto &item : items) {
    itemNames.push_back(item.get().display_name + " - $" +
                        std::to_string(item.get().cost));
  }
  itemNames.emplace_back("GTFO");

  NumberedListViewModel viewModel(m_mainWindow);
  viewModel.header = "Going shopping?";
  viewModel.body = "";
  viewModel.list_header = "Available Items:";
  viewModel.items = itemNames;

  renderNumberedListView(viewModel);
}

void Ui::renderStats(const std::unique_ptr<StatModel> &stats, int32_t mile, int32_t hour) {
  std::string _hour = "Hour: " + std::to_string(hour);
  std::string _mile = "Mile: " + std::to_string(mile);
  std::string _health = "Health: " + std::to_string(stats->health);
  std::string _money = "Money: " + std::to_string(stats->money_remaining);

  UIUtils::BlitBuffer buf = {_hour, _mile, _health, _money};
  UIUtils::blitWindow(buf, m_statsWindow);
}

const Window &Ui::mainWindow() const { return m_mainWindow; }
const Window &Ui::inventoryWindow() const { return m_inventoryWindow; }
const Window &Ui::statusWindow() const { return m_statsWindow; }
const Window &Ui::inputBar() const { return m_inputBar; }
const Window &Ui::bottomBar() const { return m_bottomBar; }

void Ui::renderNumberedListView(const NumberedListViewModel &viewModel) {

  std::string header = viewModel.header;

  auto lines = UIUtils::insertLineBreaks(viewModel.body);
  UIUtils::BlitBuffer wrappedBody;
  for (const auto &line : lines) {
    auto result = UIUtils::wrapString(line, viewModel.window);
    wrappedBody.insert(wrappedBody.end(), result.begin(), result.end());
  }

  int32_t choice = 0;
  std::vector<std::string> items = {viewModel.list_header};
  for(const auto &item : viewModel.items) {
    items.push_back("    " + std::to_string(choice) + ". " + item);
    choice++;
  }

  // Header
  UIUtils::BlitBuffer buffer = {header, ""};

  // Body
  buffer.insert(buffer.end(), wrappedBody.begin(), wrappedBody.end());
  buffer.push_back("");

  // List Items
  buffer.insert(buffer.end(), items.begin(), items.end());
  UIUtils::blitWindow(buffer, viewModel.window);
}
