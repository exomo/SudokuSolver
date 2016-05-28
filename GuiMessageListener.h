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

#ifndef GUIMESSAGELISTENER_H
#define GUIMESSAGELISTENER_H

#include "sudoku.h"
 #include <PostCreationObjects.hpp>

class GuiMessageListener : public SudokuMessageListener
{
    public:
        GuiMessageListener(HWND window, pco::Edit edit);
        virtual ~GuiMessageListener();

        virtual void message(SudokuMessageType type, std::string msg);
    protected:
    private:
        HWND window;
        pco::Edit edit;
};

#endif // GUIMESSAGELISTENER_H
