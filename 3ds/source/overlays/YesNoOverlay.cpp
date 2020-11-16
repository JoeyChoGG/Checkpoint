/*
 *   This file is part of Checkpoint
 *   Copyright (C) 2017-2020 Bernardo Giordano, FlagBrew
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#include "YesNoOverlay.hpp"

YesNoOverlay::YesNoOverlay(
    Screen& screen, const std::string& mtext, const std::function<void(InputDataHolder&)>& callbackYes, const std::function<void(InputDataHolder&)>& callbackNo)
    : DualScreenOverlay(screen),
    buttonYes(42, 162, 116, 36, COLOR_GREY_DARK, COLOR_WHITE, "\uE000 Yes", true),
    buttonNo(162, 162, 116, 36, COLOR_GREY_DARK, COLOR_WHITE, "\uE001 No", true),
    hid(2, 2)
{
    textBuf = C2D_TextBufNew(64);
    C2D_TextParse(&text, textBuf, mtext.c_str());
    C2D_TextOptimize(&text);

    yesFunc = callbackYes;
    noFunc  = callbackNo;

    posx = ceilf(320 - text.width * 0.6) / 2;
    posy = 40 + ceilf(120 - 0.6f * fontGetInfo(NULL)->lineFeed) / 2;
}

YesNoOverlay::~YesNoOverlay()
{
    C2D_TextBufDelete(textBuf);
}

void YesNoOverlay::drawTop(DrawDataHolder& d) const
{
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, COLOR_OVERLAY);
}

void YesNoOverlay::drawBottom(DrawDataHolder& d) const
{
    C2D_DrawRectSolid(0, 0, 0.5f, 400, 240, COLOR_OVERLAY);
    C2D_DrawRectSolid(40, 40, 0.5f, 240, 160, COLOR_GREY_DARK);
    C2D_DrawText(&text, C2D_WithColor, posx, posy, 0.5f, 0.6f, 0.6f, COLOR_WHITE);
    C2D_DrawRectSolid(40, 160, 0.5f, 240, 40, COLOR_GREY_LIGHT);

    buttonYes.draw(d, 0.7, 0);
    buttonNo.draw(d, 0.7, 0);

    if (hid.index() == 0) {
        d.citro.drawPulsingOutline(42, 162, 116, 36, 2, COLOR_BLUE);
    }
    else {
        d.citro.drawPulsingOutline(162, 162, 116, 36, 2, COLOR_BLUE);
    }
}

void YesNoOverlay::update(InputDataHolder& input)
{
    hid.update(input, 2);

    hid.index(buttonYes.held(input) ? 0 : buttonNo.held(input) ? 1 : hid.index());
    buttonYes.selected(hid.index() == 0);
    buttonNo.selected(hid.index() == 1);

    if (buttonYes.released(input) || ((input.kDown & KEY_A) && hid.index() == 0)) {
        yesFunc(input);
    }
    else if (buttonNo.released(input) || (input.kDown & KEY_B) || ((input.kDown & KEY_A) && hid.index() == 1)) {
        noFunc(input);
    }
}