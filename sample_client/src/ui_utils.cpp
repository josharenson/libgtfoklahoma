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

#include <ui_utils.hpp>

#include <cstdlib>
#include <sstream>
#include <vector>

#include <spdlog/spdlog.h>
#include <termbox.h>

#include <window.hpp>

using namespace gtfoklahoma;

void UIUtils::blitWindow(const BlitBuffer &buffer, const Window &window, bool clear) {
  if (clear) { UIUtils::clearWindow(window);}
  blitRaw(buffer, window.x(), window.y(), window.fg_color, window.bg_color);
}

void UIUtils::clearWindow(const Window &window) {
  auto col_begin = window.x();
  auto col_end = col_begin + window.width;

  auto row_begin = window.y();
  auto row_end = row_begin + window.height;

  for (int row = row_begin; row < row_end; row++) {
    for (int col = col_begin; col < col_end; col++) {
      tb_change_cell(col, row, kEmptyChar, window.fg_color, window.bg_color);
    }
  }
  tb_present();
}

int32_t UIUtils::getInputInt(const Window &window, const std::string &prompt,
                             const std::function<bool(int32_t)> &validator,
                             const std::string &retry_prompt) {
  struct tb_event event{};
  blitWindow({prompt}, window);

  std::vector<uint32_t> accumulator;
  int32_t charsInputSoFar = 0;

  auto convert = [](std::vector<uint32_t> &asciiVals){
    int32_t result = 0;
    std::reverse(asciiVals.begin(), asciiVals.end());
    for (int i = 0; i < asciiVals.size(); i++) {
      auto digit = asciiVals[i] - 0x30;
      digit *= pow(10, i);
      result += digit;
    }
    return result;
  };

  auto echo = [&charsInputSoFar, prompt, window](uint32_t ch){
    auto x = window.x() + prompt.length() + charsInputSoFar;
    tb_change_cell(x, window.y(), ch, window.fg_color, window.bg_color);
    tb_present();
    charsInputSoFar++;
  };

  auto retry = [&accumulator, &charsInputSoFar, prompt, retry_prompt, window](){
    spdlog::debug("Entering retry workflow");
    std::chrono::milliseconds delay(2000);
    blitWindow({retry_prompt}, window);
    std::this_thread::sleep_for(delay);
    blitWindow({prompt}, window);
    charsInputSoFar = 0;
    accumulator.clear();
  };

  auto inputIsNumber = [](const std::vector<uint32_t> &accumulator){
    for (const auto &ch : accumulator) {
      if (ch < 48 || ch > 57) {
        spdlog::debug("Input failed validation");
        return false;
      }
    }
    spdlog::debug("Input validated");
    spdlog::default_logger()->flush();
    return true;
  };

  while (tb_poll_event(&event)) {
    // Ignore non key events
    if (event.type != TB_EVENT_KEY) { continue; }

    // If submitted, validate and break
    if (event.key == TB_KEY_ENTER) {
      if (inputIsNumber(accumulator) && validator(convert(accumulator))) {
        break;
      } else {
        retry();
      }

      // Otherwise we have a key event that isn't "enter" so echo it.
    } else {
      accumulator.push_back(event.ch);
      echo (event.ch);
    }
  }

  return convert(accumulator);
}

UIUtils::BlitBuffer UIUtils::insertLineBreaks(const std::string &str) {
  // Split on newlines - not carriage returns cause those are stupid.
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, '\n')) {
    tokens.push_back(token);
    tokens.emplace_back("");
  }

  // If there were _no_ newline characters, remove the trailing line
  if (tokens.size() == 2)
    tokens.pop_back(); // Remove trailing line break

  return tokens;
}

std::string UIUtils::elide(const std::string &str, int32_t result_length,
                           const std::string &elideText) {

  return str.substr(0, result_length - elideText.length()) + elideText;
}

UIUtils::BlitBuffer UIUtils::wrapString(const std::string &str,
                                        const Window &window,
                                        const char delimiter) {
  BlitBuffer result;

  // Tokenize by delimiter
  std::vector<std::string> tokens;
  std::stringstream ss(str);
  std::string token;
  while (std::getline(ss, token, delimiter)) {
    tokens.push_back(token);
  }

  auto shouldWrap = [](const std::string &word, int32_t x_begin, int32_t x_end){
    return (word.length() > (x_end - x_begin));
  };

  int32_t curIdx = 0;
  int32_t rowWidth = window.width;
  auto rowBegin = window.y();
  auto colBegin = window.x();
  auto colEnd = colBegin + window.width;

  std::string currentLine;
  while (curIdx < tokens.size()) {
    // Elide the token if necessary
    auto paddedWord = tokens.at(curIdx);
    // +1 is to account for the delimiter
    if (paddedWord.length() + 1 > rowWidth) {
      paddedWord = elide(paddedWord, rowWidth, "...");
    } else {
      paddedWord += delimiter;
    }

    // Try printing the token on the current line and increment if
    // it won't fit
    if (shouldWrap(paddedWord, colBegin, colEnd)) {
      result.push_back(currentLine);
      currentLine = paddedWord;
      colBegin = window.x() + paddedWord.length();
      rowBegin++;
      curIdx++;
    } else {
      currentLine += paddedWord;
      colBegin += paddedWord.length();
      curIdx++;
    }
  }

  result.push_back(currentLine);
  return result;
}

void UIUtils::blitRaw(const BlitBuffer &buffer, int32_t x, int32_t y,
                 uint16_t fg_color, uint16_t bg_color) {
  /*BlitBuffer  jam = {
      {"-------- __@ "},
      {"----- _`\\<,_ "},
      {"---- (*)/ (*)"}
  };*/
  auto num_cols = buffer[0].length();
  auto num_rows = buffer.size();
  int32_t colIdx = 0;
  int32_t rowIdx = 0;
  for (int row = y; row < (y + num_rows); row++) {
    for (int col = x; col < (x + num_cols); col++) {
      tb_change_cell(col, row, buffer[rowIdx][colIdx], fg_color, bg_color);
      colIdx++;
    }
    colIdx = 0;
    rowIdx++;
    num_cols = buffer[rowIdx].length();
  }
  tb_present();
}
