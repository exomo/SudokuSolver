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

#include <vector>
#include <string>
#include <windows.h>
#include <algorithm>
#include "sudoku.h"

template <class T>
bool isInList(const std::vector<T> &vec, T k) {
    return (find(vec.begin(), vec.end(), k) != vec.end());
}

template <class T>
bool removeFromList(std::vector<T> &vec, T k) {
    typename std::vector<T>::iterator it = find(vec.begin(), vec.end(), k);
    if(it != vec.end()) {
        vec.erase(it);
    }
    return false;
}

Sudoku::Sudoku(int size) {
    _size = size;
    _difficulty = 0;
    _level = 0;

    child = 0;
    numPos=0;
    aktPos=0;

    for(int i=0; i<size; i++) {
        std::vector<int> idRow;
        std::vector<char> numRow;
        for(int j=0; j<size; j++) {
            idRow.push_back(0);
            numRow.push_back(0);
        }
        fieldId.push_back(idRow);
        numbers.push_back(numRow);
    }
}

Sudoku::Sudoku(const Sudoku& alt) {
    _difficulty = 0;
    _size = alt._size;
    _level = alt._level + 1;

    child = 0;
    numPos=0;
    aktPos=0;

    numbers = alt.numbers;
    fieldId = alt.fieldId;
    fields  = alt.fields;
    posList = alt.posList;

    symbols = alt.symbols;
}

int Sudoku::getSize()
{
    return _size;
}

char Sudoku::getFieldId(unsigned int row, unsigned int col)
{
    if(fieldId.size() > row && fieldId[row].size() > col) {
        return fieldId[row][col];
    } else {
        return 0;
    }
}

void Sudoku::setFieldId(unsigned int row, unsigned int col, char id)
{
    if(fieldId.size() > row && fieldId[row].size() > col) {
        fieldId[row][col] = id;
    }
}

char Sudoku::getNumber(unsigned int row, unsigned int col)
{
    if(numbers.size() > row && numbers[row].size() > col) {
        return numbers[row][col];
    } else {
        return 0;
    }
}

void Sudoku::setNumber(unsigned int row, unsigned int col, char num)
{
    if(numbers.size() > row && numbers[row].size() > col) {
        numbers[row][col] = num;
    }
}

int Sudoku::getDifficulty()
{
    return _difficulty;
}

Sudoku* Sudoku::solve()
{
    symbols.clear();
    fields.clear();

    // check for errors in fieldIds and create fields
    for(unsigned int row = 0; row < fieldId.size(); row++)
    {
        for(unsigned int col = 0; col < fieldId[row].size(); col++)
        {
            bool matchFieldId = false;
            Cell cell;
            cell.row = row;
            cell.col = col;
            cell.fieldId = fieldId[row][col];

            for(std::vector<Field>::iterator it = fields.begin(); it != fields.end(); ++it) {
                if( (*it).fieldId == cell.fieldId ) {
                    (*it).cells.push_back(cell);
                    matchFieldId = true;
                }
            }
            if(matchFieldId == false) {
                Field field;
                field.fieldId = cell.fieldId;
                field.cells.push_back(cell);
                fields.push_back(field);
            }
        }
    }

    if(fields.size() != _size) {

        MessageBox(0, "Anzahl der Felder muss der Größe des Sudokus entsprechen", "Fehler", MB_OK | MB_ICONERROR);
        return 0;
    }

    for(unsigned int i=0; i<fields.size(); i++) {
        if(fields[i].cells.size() != _size) {
            MessageBox(0, "Alle Felder müssen gleich groß sein", "Fehler", MB_OK | MB_ICONERROR);
            return 0;
        }
    }

    // read the used symbols
    for(unsigned int i=0; i<numbers.size(); i++) {
        for(unsigned int j=0; j<numbers[i].size(); j++) {
            if(numbers[i][j] > 0 && !isInList(symbols, numbers[i][j])) {
                symbols.push_back(numbers[i][j]);
            }
        }
    }

    std::string moresymbols = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    unsigned int msp = 0;
    while(symbols.size() < _size && msp < moresymbols.size()) {
        if(!isInList(symbols, moresymbols[msp])) {
            symbols.push_back(moresymbols[msp]);
        }
        msp++;
    }

    if(symbols.size() > _size) {
        MessageBox(0, "Anzahl der verwendeten Werte ist größer als die Größe des Sudokus", "Fehler", MB_OK | MB_ICONERROR);
        return 0;
    }

//    std::string tmp = "Verwendete Symbole:                 ";
//    for(int i=0; i<symbols.size(); i++) {
//        tmp[20+i] = symbols[i];
//    }

//    MessageBox(0, tmp.c_str(), "Symbole", MB_OK);

    Sudoku *newSudoku = new Sudoku(*this);
    newSudoku->makePosList();
    child = newSudoku;
    numPos=1;
    aktPos=0;
    bool solved = newSudoku->dancing_links();
    if(solved) {
        //newSudoku->setNumber(1,1,'X');
        return newSudoku;
    } else {
        delete newSudoku;
        return 0;
    }
}

bool Sudoku::findfixed() {
    bool changed;
    bool solved;

    do {
        changed = false;
        solved = true;
        for (unsigned int i=0; i<_size; i++)
        for (unsigned int j=0; j<_size; j++)
        {

            if(numbers[i][j] == 0) {
                solved = false;
                if (posList[i][j].size() == 1)
                {
                    numbers[i][j] = symbols[posList[i][j][0]];
                    removeFromPosList(i, j, posList[i][j][0]);
                    changed = true;
                    //char msg[100];
                    //sprintf(msg, "Feld %d, %d: eindeutige Zahl %c", i+1, j+1, numbers[i][j]);
//                    solution.addLine(einrueckung + msg);
                    //MessageBox(0, msg, "Tipp", MB_OK);
                    _difficulty += 1;
                }
            }
        }
    } while (changed);
    return solved;
}

bool Sudoku::findfixed2() {

    bool changed;
    bool solved;

    do {

        // return as soon as the sudoku can be solved by findfixed
        if(findfixed()) {
            return true;
        }

        changed = false;
        solved = true;

        for(unsigned int i=0; i<_size && (!changed); i++)
        {
            // check rows for single number
            for(unsigned int k=0; k<_size && (!changed); k++)
            {
                bool single = false;
                int pos = -1;
                if (isInList(numbers[i], symbols[k]) ) continue;
                for(unsigned int j=0; j<_size; j++)
                {
                    if(numbers[i][j] != 0) continue;
                    //std::vector<char> pl(poslist[i][j]);
                    if (isInList(posList[i][j], k)) {
                        if (single) {
                            single = false;
                            break;
                        } else {
                            single = true;
                            pos = j;
                        }
                    }
                }
                if (single) {
                    numbers[i][pos] = symbols[k];
                    removeFromPosList(i, pos, k);
                    changed = true;
                    _difficulty += 3;
//                    char msg[100];
//                    sprintf(msg, "Feld %d, %d: einzige Möglichkeit für Zahl %c in Zeile", i+1, pos+1, symbols[k]);
//                    solution.addLine(einrueckung + msg);
                    //MessageBox(0, msg, "Tipp", MB_OK);

                }
            }
        }


        for(unsigned int j=0; j<_size && (!changed); j++)
        {
            // check columns for single number
            for(unsigned int k=0; k<=_size && (!changed); k++)
            {
                bool single = false;
                int pos = -1;

                bool isInCol = false;
                for(unsigned int i=0; i<_size; i++) {
                    if (numbers[i][j] == symbols[k]) {
                        isInCol = true;
                        break;
                    }
                }
                if(isInCol) continue;

                for(int i=0; i<_size; i++)
                {
                    if (numbers[i][j] == 0 && isInList(posList[i][j], k)) {
                        if (single) {
                            single = false;
                            break;
                        } else {
                            single = true;
                            pos = i;
                        }
                    }
                }
                if (single) {
                    numbers[pos][j] = symbols[k];
                    removeFromPosList(pos, j, k);
                    changed = true;
                    _difficulty += 2;
//                    char msg[100];
//                    sprintf(msg, "Feld %d, %d: einzige Möglichkeit für Zahl %c in Spalte", pos+1, j+1, symbols[k]);
//                    solution.addLine(einrueckung + msg);
                    //MessageBox(0, msg, "Tipp", MB_OK);
                }
            }
        }

        for(unsigned int i=0; i<_size && (!changed); i++)
        {
            // check fields for single number
            Field currentField = fields[i];

            for(unsigned int k=0; k<_size && (!changed); k++)
            {
                bool single = false;
                int pos = -1;

                bool isInField = false;
                for(int j=0; j<_size; j++) {
                    if (numbers[currentField.cells[j].row][currentField.cells[j].col] == symbols[k]) {
                        isInField = true;
                        break;
                    }
                }
                if(isInField) continue;

                for(unsigned int j=0; j<_size; j++)
                {
                    if (numbers[currentField.cells[j].row][currentField.cells[j].col] == 0 && isInList(posList[currentField.cells[j].row][currentField.cells[j].col], k)) {
                        if (single) {
                            single = false;
                            break;
                        } else {
                            single = true;
                            pos = j;
                        }
                    }
                }
                if (single) {
                    numbers[currentField.cells[pos].row][currentField.cells[pos].col] = symbols[k];
                    removeFromPosList(currentField.cells[pos].row, currentField.cells[pos].col, k);
                    changed = true;
                    _difficulty += 3;
//                    char msg[100];
//                    sprintf(msg, "Feld %d, %d: einzige Möglichkeit für Zahl %c in Feld", currentField.cells[pos].row, currentField.cells[pos].col, symbols[k]);
//                    solution.addLine(einrueckung + msg);
                    //MessageBox(0, msg, "Tipp", MB_OK);
                }
            }
        }

    } while (changed);

    return false;
}

void Sudoku::makePosList() {
    posList.clear();

    for (int i=0; i<_size; i++) {
        std::vector<std::vector<unsigned int> > posListRow;
        for (int j=0; j<_size; j++) {
            std::vector<unsigned int> posCell;
            if (numbers[i][j] == 0) {

                Field currentField;
                for(int c=0; c<fields.size(); c++) {
                    if(fields[c].fieldId == fieldId[i][j]) {
                        currentField = fields[c];
                        break;
                    }
                }

                for (int k=0; k<_size; k++) {

                    bool possible = true;
                    for(int l=0; l<_size; l++) {
                        if( symbols[k] == numbers[i][l] || symbols[k] == numbers[l][j] || symbols[k] == numbers[currentField.cells[l].row][currentField.cells[l].col]) {
                            possible = false;
                            break;
                        }
                    }
                    if(possible)
                    {
                        posCell.push_back(k);
                    }
                }

//                char msg[100];
//                sprintf(msg, "Feld %d,%d: %d Möglichkeiten:       ", i+1, j+1, poslist[i][j].size());
//                for(int kl=0; kl<poslist[i][j].size(); kl++) msg[28+kl] = symbols[poslist[i][j][kl]];
                //MessageBox(0, msg,  "test", MB_OK);
            }
            posListRow.push_back(posCell);
        }
        posList.push_back(posListRow);
    }
}

void Sudoku::removeFromPosList(unsigned int row, unsigned int col, unsigned int pos) {
    Field currentField;
    for(int c=0; c<fields.size(); c++) {
        if(fields[c].fieldId == fieldId[row][col]) {
            currentField = fields[c];
            break;
        }
    }

    for(unsigned int i=0; i<_size; i++) {
        removeFromList(posList[i][col], pos);
        removeFromList(posList[row][i], pos);
        removeFromList(posList[currentField.cells[i].row][currentField.cells[i].col], pos);
    }
}

bool Sudoku::dancing_links()
{

    // first try the simple algorithms
    if(findfixed2()) {
        return true;
    }

    // find the cell where the fewest numbers are possible
    int minlist = _size+1;
    int min_i = 0;
    int min_j = 0;

    for (int i=0; i<_size; i++)
    for (int j=0; j<_size; j++)
    {
        int tmp = posList[i][j].size();


        if (tmp < minlist && numbers[i][j] == 0) {
            if(tmp == 0) {
//                char msg[100];
//                sprintf(msg, "Feld %d,%d: Keine Möglichkeiten", i+1, j+1);
//                MessageBox(0, msg,  "test", MB_OK);
                return false; // sudoku is not solvable
            }
            minlist = tmp;
            min_i = i;
            min_j = j;
        }
    }

    numPos = minlist;
    // try all the possible values
    for (aktPos=0; aktPos<numPos; aktPos++) {
        // copy the sudoku structure
        child = new Sudoku(*this);
        child->numbers[min_i][min_j] = symbols[posList[min_i][min_j][aktPos]];
        child->removeFromPosList(min_i, min_j, posList[min_i][min_j][aktPos]);

//        if(_level == 20) {
//        char msg[100];
//
//        sprintf(msg, "Feld %d,%d: Rate Zahl %c auf Level %d", min_i+1, min_j+1, symbols[poslist[min_i][min_j][i]], level);
////        solution.addLine(einrueckung + msg);
////        einrueckung = einrueckung + "  ";
//        MessageBox(0, msg,  "test", MB_OK);
//        }

        if (child->dancing_links()) {
            //MessageBox(0, "gelöst", "test", MB_OK);
            numbers = child->numbers;
            _difficulty += child->_difficulty * minlist;
            delete child;
            child = 0;
            return true;
        } else {
            delete child;
            child=0;
        }

//        einrueckung = einrueckung.substr(0, einrueckung.size()-2);
//        solution.addLine(einrueckung + "Geratene Zahl war falsch");
    }

//    char msg[100];

//    sprintf(msg, "Feld %d,%d: Alle Möglichkeiten durchprobiert", min_i+1, min_j+1);
//        solution.addLine(einrueckung + msg);
//        einrueckung = einrueckung + "  ";
    //MessageBox(0, msg,  "test", MB_OK);

    return false;
}

/* handle the message listeners */

void Sudoku::addMessageListener(SudokuMessageListener *listener)
{
    if(listener == NULL) return;
    listeners.push_back(listener);
}

void Sudoku::removeMessageListener(SudokuMessageListener *listener)
{
    if(listener == NULL) return;
    removeFromList(listeners, listener);
}

void Sudoku::dispatchMessage(SudokuMessageType type, std::string msg)
{
    for(std::vector<SudokuMessageListener *>::iterator it = listeners.begin(); it != listeners.end(); ++it)
    {
        (*it)->message(type, msg);
    }
}
