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

#ifndef SUDOKU_H_INCLUDED
#define SUDOKU_H_INCLUDED

#include <vector>
#include <string>

enum SudokuMessageType
{
    ERR = 1,
    SOLUTION = 2
};

class SudokuMessageListener
{
    public:
        virtual void message(SudokuMessageType type, std::string msg) = 0;
};

struct Cell
{
    int row;
    int col;
    int fieldId;
};

struct Field
{
    char fieldId;
    std::vector<Cell> cells;
};


class Sudoku
{

    private:
        unsigned int _size;
        int _difficulty;
        int _level;

        std::vector<std::vector<char> > numbers;
        std::vector<std::vector<int> > fieldId;
        std::vector<std::vector<std::vector<unsigned int> > > posList;

        std::vector<char> symbols;
        std::vector< Field > fields;

        bool findfixed();
        bool findfixed2();
        bool dancing_links();

        void makePosList();
        void removeFromPosList(unsigned int row, unsigned int col, unsigned int pos);

        Sudoku* child;
        int numPos;
        int aktPos;

        std::vector<SudokuMessageListener *> listeners;
        void dispatchMessage(SudokuMessageType type, std::string msg);


    public:
        Sudoku(int size);
        Sudoku(const Sudoku&);
        int getSize();

        char getFieldId(unsigned int row, unsigned int col);
        void setFieldId(unsigned int row, unsigned int col, char id);

        char getNumber(unsigned int row, unsigned int col);
        void setNumber(unsigned int row, unsigned int col, char num);

        int getDifficulty();

        void addMessageListener(SudokuMessageListener *listener);
        void removeMessageListener(SudokuMessageListener *listener);

        Sudoku* solve();

};

#endif // SUDOKU_H_INCLUDED
