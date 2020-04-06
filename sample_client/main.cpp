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

// Library includes
#include <libgtfoklahoma/engine.hpp>
#include <libgtfoklahoma/game.hpp>

// System Includes
#include <chrono>
#include <csignal>
#include <memory>
#include <thread>

// 3P includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// Local includes
#include <event_observer.hpp>
#include <ui_utils.hpp>
#include <window.hpp>

void signalHandler(int sigNum) { exit(sigNum); }
#include <iostream>
#include <cstdint>

using namespace gtfoklahoma;
int main(int argc, char *argv[]) {
  signal(SIGINT, signalHandler);

  auto file_logger = spdlog::basic_logger_st("basic_logger", "logs/basic.txt");
  spdlog::set_default_logger(file_logger);
  spdlog::set_level(spdlog::level::debug);

  gtfoklahoma::UIUtils ui;

  int32_t h = ui.termHeight(); int32_t w = ui.termWidth();
  // h = 100; w = 100;
  Window mainWindow(h, w);
  mainWindow.anchors.top = 0;
  mainWindow.anchors.left = 0;
  mainWindow.heightPercent = .5;
  mainWindow.widthPercent = .5;
  mainWindow.margins = Margins(1);
  ui.clearWindow(mainWindow);

  Window statsWindow(h, w);
  statsWindow.anchors.top = mainWindow.bottom();
  statsWindow.anchors.left = mainWindow.left();
  statsWindow.heightPercent = .4;
  statsWindow.widthPercent = .4;
  statsWindow.bg_color = 0x05;
  ui.clearWindow(statsWindow);

  Window inventoryWindow(h, w);
  inventoryWindow.anchors.verticalCenter = mainWindow.verticalCenter();
  inventoryWindow.anchors.horizontalCenter = mainWindow.horizontalCenter();
  inventoryWindow.heightPercent = .2;
  inventoryWindow.widthPercent = .2;
  inventoryWindow.bg_color = 0x02;
  ui.clearWindow(inventoryWindow);

  Text text(h, w);
  text.text = "Hello World";
  text.wrapOnWord = false;
  text.fg_color = mainWindow.fg_color;
  text.bg_color = mainWindow.bg_color;
  text.anchors.top = mainWindow.x();
  text.anchors.left = mainWindow.y();
  text.height = mainWindow.rawHeight();
  text.width = mainWindow.rawWidth();
  ui.drawText(text);

  file_logger->flush();
  //ui.setMenuText("[F1] - HELP | [F2] - INVENTORY | [F3] - SETTINGS | [F4] - SAVE | [F5] - QUIT");

  // Prototype
  std::string logo = R"(*____    ______  ____    _____   __       ___             __**************************************/\  _`\ /\__  _\/\  _`\ /\  __`\/\ \     /\_ \           /\ \*************************************\ \ \L\_\/_/\ \/\ \ \L\_\ \ \/\ \ \ \/'\ \//\ \      __  \ \ \___     ___     ___ ___      __******\ \ \L_L  \ \ \ \ \  _\/\ \ \ \ \ \ , <   \ \ \   /'__`\ \ \  _ `\  / __`\ /' __` __`\  /'__`\*****\ \ \/, \ \ \ \ \ \ \/  \ \ \_\ \ \ \\`\  \_\ \_/\ \L\.\_\ \ \ \ \/\ \L\ \/\ \/\ \/\ \/\ \L\.\_****\ \____/  \ \_\ \ \_\   \ \_____\ \_\ \_\/\____\ \__/.\_\\ \_\ \_\ \____/\ \_\ \_\ \_\ \__/.\_\****\/___/    \/_/  \/_/    \/_____/\/_/\/_/\/____/\/__/\/_/ \/_/\/_/\/___/  \/_/\/_/\/_/\/__/\/_/)";
  //ui.rawBlit(logo, 3, 1, 98);

  std::string stats = R"(HEALTH: 100 | MILE: 0 | MONEY REMAINING: 2000)";
  //ui.setStatSWindowText(stats);

  /*libgtfoklahoma::Game game("");
  auto engine = std::make_unique<libgtfoklahoma::Engine>(game);
  auto eventObserver = std::make_unique<gtfoklahoma::EventObserver>(game);
  engine->registerEventObserver(std::move(eventObserver));
  engine->start();*/

  // UI thread?
  auto delay = std::chrono::milliseconds(250);
  for (;;) {
    std::this_thread::sleep_for(delay);
  }
}
