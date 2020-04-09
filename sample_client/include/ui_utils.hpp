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

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace gtfoklahoma {

struct Window;
class UIUtils {
public:
  using BlitBuffer = std::vector<std::string>;

  /**
   *
   * @param buffer Buffer ot characters to blit.
   * @param window The window to blit in. Transfer starts at top left of this.
   * @param clear If true, the window will be cleared before the blit.
   */
  static void blitWindow(const BlitBuffer &buffer, const Window &window,
                         bool clear = true);

  /**
   *
   * @param window The window to clear. Each cell will be set to the window's
   * background color.
   */
  static void clearWindow(const Window &window);

  /**
 *
 * @param window The window where the action happens. The prompt is printed
 * here, the input is echoed here, and the retry prompt is printed here if
 * necessary. There is no word wrapping.
 * @param prompt The text to display to the user asking for input
 * @param validator A function that validates the input. The value passed to
 * the validator is guaranteed to be a valid int32_t. If this function returns
 * false, the window will show the `retry prompt` briefly and then display the
 * `prompt` again. This repeats until the validator returns true.
 * @param retry_prompt This prompt is displayed to the user briefly (~2
 * seconds) if the validator fails or if the user enters something that isn't
 * an int.
 * @return The validated int received from the user.
 */
  static int32_t
  getInputInt(const Window &window, const std::string &prompt,
              const std::function<bool(int32_t i)> &validator,
              const std::string &retry_prompt = "INVALID CHOICE!");

  /**
   *
   * @param A string with \n characters in it.
   * @return A BlitBuffer that will blit an empty line where \n characters in `str` are.
   */
  static BlitBuffer insertLineBreaks(const std::string &str);

  /**
   *
   * @param str The string to wrap.
   * @param window The window to use as a reference for width dimensions.
   * @param delimiter Will attempt to wrap on this delimiter. If a string is
   * longer than the width of the window, it will be elided.
   * @return A BlitBuffer than can be provided to `blitWindow` for rendering.
   */
  static BlitBuffer wrapString(const std::string &str, const Window &window,
                               char delimiter = ' ');

private:
  static void blitRaw(const BlitBuffer &buffer, int32_t x, int32_t y,
                      uint16_t fg_color, uint16_t bg_color);

  static std::string elide(const std::string &str, int32_t result_length,
                           const std::string &elideText = "...");

private:
  inline static const uint32_t kEmptyChar = static_cast<uint32_t>(' ');
};
}