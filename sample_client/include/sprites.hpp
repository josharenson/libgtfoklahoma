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

#include <string>
#include <vector>

namespace gtfoklahoma::sprites {
    const std::vector<std::string> fullscreenLogo = {
        {R"( ____    ______  ____    _____   __       ___             __                                      )"},
        {R"(/\  _`\ /\__  _\/\  _`\ /\  __`\/\ \     /\_ \           /\ \                                     )"},
        {R"(\ \ \L\_\/_/\ \/\ \ \L\_\ \ \/\ \ \ \/'\ \//\ \      __  \ \ \___     ___     ___ ___      __     )"},
        {R"( \ \ \L_L  \ \ \ \ \  _\/\ \ \ \ \ \ , <   \ \ \   /'__`\ \ \  _ `\  / __`\ /' __` __`\  /'__`\   )"},
        {R"(  \ \ \/, \ \ \ \ \ \ \/  \ \ \_\ \ \ \\`\  \_\ \_/\ \L\.\_\ \ \ \ \/\ \L\ \/\ \/\ \/\ \/\ \L\.\_ )"},
        {R"(   \ \____/  \ \_\ \ \_\   \ \_____\ \_\ \_\/\____\ \__/.\_\\ \_\ \_\ \____/\ \_\ \_\ \_\ \__/.\_\)"},
        {R"(    \/___/    \/_/  \/_/    \/_____/\/_/\/_/\/____/\/__/\/_/ \/_/\/_/\/___/  \/_/\/_/\/_/\/__/\/_/)"}
    };

    const std::vector<std::string> gameOverLogo = {
        {R"(  ______    ______   __       __  ________         ______   __     __  ________  _______  )"},
        {R"( /      \  /      \ |  \     /  \|        \       /      \ |  \   |  \|        \|       \ )"},
        {R"(|  $$$$$$\|  $$$$$$\| $$\   /  $$| $$$$$$$$      |  $$$$$$\| $$   | $$| $$$$$$$$| $$$$$$$\)"},
        {R"(| $$ __\$$| $$__| $$| $$$\ /  $$$| $$__          | $$  | $$| $$   | $$| $$__    | $$__| $$)"},
        {R"(| $$|    \| $$    $$| $$$$\  $$$$| $$  \         | $$  | $$ \$$\ /  $$| $$  \   | $$    $$)"},
        {R"(| $$ \$$$$| $$$$$$$$| $$\$$ $$ $$| $$$$$         | $$  | $$  \$$\  $$ | $$$$$   | $$$$$$$\)"},
        {R"(| $$__| $$| $$  | $$| $$ \$$$| $$| $$_____       | $$__/ $$   \$$ $$  | $$_____ | $$  | $$)"},
        {R"( \$$    $$| $$  | $$| $$  \$ | $$| $$     \       \$$    $$    \$$$   | $$     \| $$  | $$)"},
        {R"(  \$$$$$$  \$$   \$$ \$$      \$$ \$$$$$$$$        \$$$$$$      \$     \$$$$$$$$ \$$   \$$)"}
    };

    const std::vector<std::string> oklahomaState = {
        {R"(===============================\ )"},
        {R"(|__________.                   | )"},
        {R"(           |                   \ )"},
        {R"(           |                   \ )"},
        {R"(           |                    |)"},
        {R"(           |                    |)"},
        {R"(           vvv                  |)"},
        {R"(              ^vvvvVvVVV   vvv  |)"},
        {R"(                        ^v^   V^|)"}
    };

    const std::vector<std::string> texasState = {
        {R"(      eeeeee,            )"},
        {R"(      l     l            )"},
        {R"(      l     l            )"},
        {R"(      l     `~e-_~_-eb.. )"},
        {R"(      l                A )"},
        {R"(......l                L )"},
        {R"(`q                      >)"},
        {R"(  `Y                    C)"},
        {R"(   \    --            .,J)"},
        {R"(    `vd' `\        .e/   )"},
        {R"(           `l.    .'     )"},
        {R"(             l   Y       )"},
        {R"(             `l._ q      )"}
    };

}
