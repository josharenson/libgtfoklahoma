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

// 3P includes
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

// Local includes
#include <event_observer.hpp>
#include <ui.hpp>

void signalHandler(int sigNum) {
  spdlog::info("SIGINT");
  spdlog::default_logger()->flush();
  exit(sigNum);
}

using namespace gtfoklahoma;

int main(int argc, char *argv[]) {
  signal(SIGINT, signalHandler);

  auto file_logger = spdlog::basic_logger_st("basic_logger", "logs/basic.txt");
  spdlog::set_default_logger(file_logger);
  spdlog::set_level(spdlog::level::debug);

  // Draws intro UI - blocks until player starts game
  Ui ui;

  libgtfoklahoma::Game game("");
  auto eventObserver = std::make_unique<gtfoklahoma::EventObserver>(game, ui);
  game.registerEventObserver(std::move(eventObserver));
  ui.renderBeginGame(game.getStats().getPlayerStatsModel(), game.getCurrentMile(), game.getCurrentHour());

  auto engine = std::make_unique<libgtfoklahoma::Engine>(game);
  engine->start();

  // UI thread?
  auto delay = std::chrono::milliseconds(250);
  for (;;) {
    spdlog::default_logger()->flush();
    std::this_thread::sleep_for(delay);
  }
}
