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

#include <iostream>
#include <sstream>
#include <vector>

#include <spdlog/spdlog.h>
#include <termbox.h>

#include <window.hpp>

using namespace gtfoklahoma;

UIUtils::UIUtils()
: m_height(0)
, m_width(0) {

    if(tb_init() != 0) {
      spdlog::error("Terminal doesn't support magic!");
      abort();
    }
    tb_select_output_mode(TB_OUTPUT_NORMAL);

    m_height = tb_height();
    m_width = tb_width();
}

UIUtils::~UIUtils() { tb_shutdown(); }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"
void UIUtils::clearWindow(const Window &window) {
  spdlog::debug("x:{}, y:{}, top:{}, bottom:{}, left:{}, right:{}, rawHeight:{}",
      window.x(), window.y(), window.top(), window.bottom(), window.left(), window.right(), window.rawHeight());

  auto col_begin = window.x();
  auto col_end = col_begin + window.rawWidth();

  auto row_begin = window.y();
  auto row_end = row_begin + window.rawHeight();

  for (int row = row_begin; row < row_end; row++) {
    for (int col = col_begin; col < col_end; col++) {
      tb_change_cell(col, row, kEmptyChar, window.fg_color, window.bg_color);
    }
  }
  tb_present();
}
#pragma clang diagnostic pop


int32_t UIUtils::termHeight() const { return m_height; }
int32_t UIUtils::termWidth() const { return m_width; }

void UIUtils::drawText(const Text &text) {
  auto rowBegin = text.y();
  auto colBegin = text.x();
  auto colEnd = colBegin + text.rawWidth();

  if (text.wrapOnWord) {
    std::vector<std::string> tokens;
    std::stringstream ss(text.text);
    std::string token;
    while (std::getline(ss, token, ' ')) {
      tokens.push_back(token);
    }

    // Write words, wrapping lines on word boundaries and eliding if necessary
    int32_t curIdx = 0;
    int32_t rowWidth = text.x() + text.rawWidth();

    while (curIdx < tokens.size()) {
      // Elide the token if necessary
      auto paddedWord = tokens.at(curIdx);
      if (paddedWord.length() > rowWidth) {
        paddedWord = elide(paddedWord, rowWidth, text.elideText);
      } else {
        paddedWord += " ";
      }

      // Try printing the toke on the current line and increment if
      // it won't fit
      if (!writeWord(paddedWord, rowBegin, colBegin, colEnd, text.fg_color,
                     text.bg_color)) {
        colBegin = text.x();
        rowBegin++;
      } else {
        colBegin += paddedWord.length();
        curIdx++;
      }
    }
  } else {
    // No word wrap, just write and elide if necessary
    auto realText = text.text;
    if (text.text.length() > (colEnd - colBegin)) {
      realText = elide(realText, (colEnd - colBegin), text.elideText);
    }
    int32_t idx = 0;
    for (int col = colBegin; col < colEnd; col++) {
      if (idx == realText.length()) { break; }
      tb_change_cell(col, rowBegin, realText.at(idx), text.fg_color, text.bg_color);
      idx++;
    }
  }
  tb_present();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"
std::string UIUtils::elide(const std::string &str, int32_t result_length,
                           const std::string &elideText) {

  return str.substr(0, result_length - elideText.length()) + elideText;
}
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma ide diagnostic ignored "MemberFunctionCanBeStatic"
bool UIUtils::writeWord(const std::string &word,
                        int32_t row,
                        int32_t col_begin,
                        int32_t col_end,
                        uint16_t fg_color,
                        uint16_t bg_color) {

  if (word.length() > col_end - col_begin) { return false; }
  int idx = 0;
  for (int col = col_begin; col < col_begin + word.length(); col++) {
    tb_change_cell(col, row, (uint32_t)word.at(idx), fg_color, bg_color);
    idx++;
  }

  return true;
}
#pragma clang diagnostic pop
