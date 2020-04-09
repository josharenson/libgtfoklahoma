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
#include <cstdint>
#include <memory>
#include <thread>

// 3P includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// Local includes
#include <event_observer.hpp>
#include <ui.hpp>
#include <window.hpp>

void signalHandler(int sigNum) { exit(sigNum); }

using namespace gtfoklahoma;

int main(int argc, char *argv[]) {
  signal(SIGINT, signalHandler);

  auto file_logger = spdlog::basic_logger_st("basic_logger", "logs/basic.txt");
  spdlog::set_default_logger(file_logger);
  spdlog::set_level(spdlog::level::debug);

  // Draws empty UI
  Ui ui;


  auto bottomBarText = "[F1] - HELP | [F2] - INVENTORY | [F3] - SETTINGS | [F4] - SAVE | [F5] - QUIT";
  // ui.setTextRaw(ui.bottomBar(), bottomBarText);

  // std::string logo = R"(*____    ______  ____    _____   __       ___             __**************************************/\  _`\ /\__  _\/\  _`\ /\  __`\/\ \     /\_ \           /\ \*************************************\ \ \L\_\/_/\ \/\ \ \L\_\ \ \/\ \ \ \/'\ \//\ \      __  \ \ \___     ___     ___ ___      __******\ \ \L_L  \ \ \ \ \  _\/\ \ \ \ \ \ , <   \ \ \   /'__`\ \ \  _ `\  / __`\ /' __` __`\  /'__`\*****\ \ \/, \ \ \ \ \ \ \/  \ \ \_\ \ \ \\`\  \_\ \_/\ \L\.\_\ \ \ \ \/\ \L\ \/\ \/\ \/\ \/\ \L\.\_****\ \____/  \ \_\ \ \_\   \ \_____\ \_\ \_\/\____\ \__/.\_\\ \_\ \_\ \____/\ \_\ \_\ \_\ \__/.\_\****\/___/    \/_/  \/_/    \/_____/\/_/\/_/\/____/\/__/\/_/ \/_/\/_/\/___/  \/_/\/_/\/_/\/__/\/_/)";


  libgtfoklahoma::Game game("");
  auto engine = std::make_unique<libgtfoklahoma::Engine>(game);
  auto eventObserver = std::make_unique<gtfoklahoma::EventObserver>(game, ui);
  engine->registerEventObserver(std::move(eventObserver));
  engine->start();

  // UI thread?
  auto delay = std::chrono::milliseconds(250);
  for (;;) {
    std::this_thread::sleep_for(delay);
  }
}
