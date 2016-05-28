/**************************************************************************
 *  Copyright 2016 Kai Bublitz (exomo)                                    *
 *                                                                        *
 *  This file is part of the program SudokuSolver.                        *
 *                                                                        *
 *  SudokuSolver is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  SudokuSolver is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with SudokuSolver.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                        *
 **************************************************************************/

#include "GuiMessageListener.h"

GuiMessageListener::GuiMessageListener(HWND window, pco::Edit edit) : window(window), edit(edit)
{
    //ctor
}

GuiMessageListener::~GuiMessageListener()
{
    //dtor
}

void GuiMessageListener::message(SudokuMessageType type, std::string msg)
{
    edit.appendText(msg + "\n");

}
