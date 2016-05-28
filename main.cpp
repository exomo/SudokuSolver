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

#include <windows.h>
#include <process.h>
#include <commctrl.h>
#include <PostCreationObjects.hpp>
#include <vector>
#include <cstdio>
#include "gui.h"
#include "sudoku.h"

/*  Declare Windows procedure  */
LPARAM CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LPARAM CALLBACK About(HWND, UINT, WPARAM, LPARAM);
LPARAM CALLBACK ChooseSizeProc(HWND, UINT, WPARAM, LPARAM);

HINSTANCE hInst;

std::vector<std::vector<pco::Edit> > numbers;
pco::Memo solution;
pco::Button btnOk;
pco::Button btnClear;

bool solve(HWND);
bool load(HWND);
bool save(HWND);
void cancelSolve(HWND);


Sudoku *sudoku, *oldSudoku;
unsigned int threadId = 0;
HANDLE th;

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nFunsterStil)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */

    hInst = hThisInstance;

    InitCommonControls();

    hwnd = CreateDialog(hInst, MAKEINTRESOURCE(IDD_SUDOKU), 0, WindowProcedure);

    HICON icon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON));
    HICON sicon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICONSMALL));
	SendMessage(hwnd,WM_SETICON,(WPARAM)ICON_SMALL,(LPARAM) sicon);
	SendMessage(hwnd,WM_SETICON,(WPARAM)ICON_BIG,(LPARAM) icon);

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nFunsterStil);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        if(!IsDialogMessage(hwnd, &messages) ){

            /* Translate virtual-key messages into character messages */
            TranslateMessage(&messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&messages);
        }
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

HFONT font1, font2;

bool destroyField()
{
    for(std::vector<std::vector<pco::Edit> >::iterator numberLine = numbers.begin(); numberLine != numbers.end(); ++numberLine)
    {
        for(std::vector<pco::Edit>::iterator number = numberLine->begin(); number != numberLine->end(); ++number)
        {
            DestroyWindow(*number);
        }
    }
    numbers.clear();
    delete sudoku;

    return true;
}

bool createField(HWND hwnd, int num)
{

    for (int i=0; i<num; i++) {
        std::vector<pco::Edit> numberLine;
        for(int j=0; j<num; j++) {
            pco::Edit number = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_TABSTOP, (j+1)*30, (i+1)*30, 25, 25, hwnd, (HMENU) 1337, hInst, NULL);
            number.setLimitText(1);
            SendMessage(number, WM_SETFONT, (WPARAM) font1, 0);
            numberLine.push_back(number);
        }
        numbers.push_back(numberLine);
    }
    sudoku = new Sudoku(num);
    return true;
}

bool clearField()
{
    for(std::vector<std::vector<pco::Edit> >::iterator numberLine = numbers.begin(); numberLine != numbers.end(); ++numberLine)
    {
        for(std::vector<pco::Edit>::iterator number = numberLine->begin(); number != numberLine->end(); ++number)
        {
            number->clear();
        }
    }
    return true;
}

bool saveFieldIds()
{
    int i=0;
    for(std::vector<std::vector<pco::Edit> >::iterator numberLine = numbers.begin(); numberLine != numbers.end(); ++numberLine)
    {
        int j=0;
        for(std::vector<pco::Edit>::iterator number = numberLine->begin(); number != numberLine->end(); ++number)
        {
            std::string txt = (*number).getText();
            if(txt.length() > 0) sudoku->setFieldId(i,j, txt[0]);
            else sudoku->setFieldId(i,j, 0);
            j++;
        }
        i++;
    }
    return true;
}

bool loadFieldIds()
{
    int i=0;
    for(std::vector<std::vector<pco::Edit> >::iterator numberLine = numbers.begin(); numberLine != numbers.end(); ++numberLine)
    {
        int j=0;
        for(std::vector<pco::Edit>::iterator number = numberLine->begin(); number != numberLine->end(); ++number)
        {
            if(sudoku->getFieldId(i,j) > 0) {
                std::string txt;
                txt.push_back(sudoku->getFieldId(i,j));
                (*number).setText(txt);
            } else {
                (*number).setText("");
            }
            j++;
        }
        i++;
    }
    return true;
}

bool saveNumbers()
{
    int i=0;
    for(std::vector<std::vector<pco::Edit> >::iterator numberLine = numbers.begin(); numberLine != numbers.end(); ++numberLine)
    {
        int j=0;
        for(std::vector<pco::Edit>::iterator number = numberLine->begin(); number != numberLine->end(); ++number)
        {
            std::string txt = (*number).getText();
            if(txt.length() > 0) sudoku->setNumber(i,j, txt[0]);
            else sudoku->setNumber(i,j, 0);
            j++;
        }
        i++;
    }
    return true;
}

bool loadNumbers()
{
    int i=0;
    for(std::vector<std::vector<pco::Edit> >::iterator numberLine = numbers.begin(); numberLine != numbers.end(); ++numberLine)
    {
        int j=0;
        for(std::vector<pco::Edit>::iterator number = numberLine->begin(); number != numberLine->end(); ++number)
        {
            if(sudoku->getNumber(i,j) > 0) {
                std::string txt;
                txt.push_back(sudoku->getNumber(i,j));
                (*number).setText(txt);
            } else {
                (*number).setText("");
            }
            j++;
        }
        i++;
    }
    return true;
}

/*  This function is called by the Windows function DispatchMessage()  */
LPARAM CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    DWORD tmp;
    int size;
    static bool chaosMode = false;
    static LARGE_INTEGER startTime, endTime;

    switch (message)                  /* handle the messages */
    {
        case WM_INITDIALOG:
//            cout << "init dialog" << endl;
            btnOk = GetDlgItem(hwnd, IDOK);
            btnClear = GetDlgItem(hwnd, IDC_CLEAR);

            //     CreateFont(höhe, breite, ?, ?,    ?,   kursiv, unterstreichen, durchstreichen,
            font1 = CreateFont(20,    10,    0, 0,    0,      0 ,       0,               0,          0, 0, 0, 0, 0, "Arial");
            font2 = CreateFont(18,     9,    0, 0,    0,      1 ,       0,               0,          0, 0, 0, 0, 0, "Arial");
            tmp = RGB(255,0,0);

            solution = GetDlgItem(hwnd, IDC_SOLUTION);
            // create default 9x9 sudoku
            SendMessage(hwnd, WM_COMMAND, IDC_9x9, 0);
            break;

        case WM_CLOSE:
        case WM_DESTROY:
            DeleteObject(font1);
            DeleteObject(font2);
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            return TRUE;
            cancelSolve(hwnd);
            break;

        case WM_SOLVED:
            {
                QueryPerformanceCounter(&endTime);
                loadNumbers();
                LARGE_INTEGER frequency;
                QueryPerformanceFrequency(&frequency);
                unsigned seconds = unsigned(double( endTime.QuadPart - startTime.QuadPart) / double(frequency.QuadPart));
                unsigned minutes = seconds / 60;
                seconds = seconds % 60;
                unsigned hours = minutes / 60;
                minutes = minutes % 60;
                char msg[100];
                sprintf(msg, "Sudoku wurde gelöst.\nSchwierigkeit: %d\nZeit: %d:%d:%d", wParam, hours, minutes, seconds);
                MessageBox(hwnd, msg, "Exomo Sudoku Solver", MB_OK);
            }
            break;

        case WM_UNSOLVED:
            {
                char msg[100];
                sprintf(msg, "Sudoku wurde nicht gelöst.");
                MessageBox(hwnd, msg, "Exomo Sudoku Solver", MB_OK);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wParam))
            {
                case IDC_END:
                    PostQuitMessage(0);
                    break;

                case IDM_ABOUT:
                    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
                    break;

                case IDC_CLEAR:
                    clearField();
                    solution.clear();
                    if(chaosMode) InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case IDOK:
                    if(!chaosMode) {
                        // Solve it
                        QueryPerformanceCounter(&startTime);
                        if(solve(hwnd)) {

                        } else {
                            MessageBox(hwnd, "Der Thread kann nicht erstellt werden.", "Exomo Sudoku Solver", MB_OK);
                        }
                    } else {
                        chaosMode = false;
                        CheckMenuItem(GetMenu(hwnd), IDC_CHAOS, MF_UNCHECKED);
                        btnOk.setText("Lösen");
                        saveFieldIds();
                        loadNumbers();
                    }
                    break;

                case IDC_SAVE:
                    if(chaosMode) saveFieldIds();
                    else saveNumbers();
                    save(hwnd);
                    break;

                case IDC_LOAD:
                    {
                        chaosMode = false;
                        Sudoku *oldSudoku = sudoku;
                        if(load(hwnd)) {
                            Sudoku *newSudoku = sudoku;
                            sudoku = oldSudoku;
                            destroyField();
                            size = newSudoku->getSize();
                            createField(hwnd, size);
                            delete sudoku;
                            sudoku = newSudoku;
                            SetWindowPos(hwnd, HWND_TOP, 0, 0, (size + 1) * 30 + 400, (size + 1) * 30 + 150, SWP_NOMOVE | SWP_DRAWFRAME);
                            solution.move((size+1) * 30 + 30, 10, 350, (size + 1) * 30 + 80);
                            SetWindowPos(GetDlgItem(hwnd, IDC_CLEAR), HWND_TOP, 100, (size + 1) * 30 + 10, 50, 25, SWP_NOSIZE);
                            SetWindowPos(GetDlgItem(hwnd, IDOK), HWND_TOP, 20, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                            CheckMenuItem(GetMenu(hwnd), IDC_4x4, size==4?MF_CHECKED:MF_UNCHECKED);
                            CheckMenuItem(GetMenu(hwnd), IDC_9x9, MF_UNCHECKED);
                            CheckMenuItem(GetMenu(hwnd), IDC_12x12_1, MF_UNCHECKED);
                            CheckMenuItem(GetMenu(hwnd), IDC_12x12_2, MF_UNCHECKED);
                            CheckMenuItem(GetMenu(hwnd), IDC_OTHERSIZE, MF_UNCHECKED);
                            loadNumbers();
                            InvalidateRect(hwnd, NULL, TRUE);
                        } else {
                            // delete the new sudoku, it is invalid
                            delete sudoku;
                            sudoku = oldSudoku;
                        }
                    }
                    break;

                case IDC_4x4:
                    size = 4;
                    destroyField();
                    createField(hwnd, size);
                    SetWindowPos(hwnd, HWND_TOP, 0, 0, (size + 1) * 30 + 400, (size + 1) * 30 + 150, SWP_NOMOVE | SWP_DRAWFRAME);
                    solution.move((size+1) * 30 + 30, 10, 350, (size + 1) * 30 + 80);
                    SetWindowPos(GetDlgItem(hwnd, IDC_CLEAR), HWND_TOP, 100, (size + 1) * 30 + 10, 50, 25, SWP_NOSIZE);
                    SetWindowPos(GetDlgItem(hwnd, IDOK), HWND_TOP, 20, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                    CheckMenuItem(GetMenu(hwnd), IDC_4x4, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_9x9, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_1, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_2, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_OTHERSIZE, MF_UNCHECKED);
                    for(int i=0; i<size*size; i++) sudoku->setFieldId(i/size,i%size, '0' + i/size/2*2 + i%size/2);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case IDC_9x9:
                    size = 9;
                    destroyField();
                    createField(hwnd, size);
                    SetWindowPos(hwnd, HWND_TOP, 0, 0, (size + 1) * 30 + 400, (size + 1) * 30 + 150, SWP_NOMOVE | SWP_DRAWFRAME);
                    solution.move((size+1) * 30 + 30, 10, 350, (size + 1) * 30 + 80);
                    SetWindowPos(GetDlgItem(hwnd, IDC_CLEAR), HWND_TOP, 100, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                    SetWindowPos(GetDlgItem(hwnd, IDOK), HWND_TOP, 20, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                    CheckMenuItem(GetMenu(hwnd), IDC_4x4, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_9x9, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_1, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_2, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_OTHERSIZE, MF_UNCHECKED);
                    for(int i=0; i<size*size; i++) sudoku->setFieldId(i/size,i%size,'0' + i/size/3*3 + i%size/3);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case IDC_12x12_1:
                    size = 12;
                    destroyField();
                    createField(hwnd, size);
                    SetWindowPos(hwnd, HWND_TOP, 0, 0, (size + 1) * 30 + 400, (size + 1) * 30 + 150, SWP_NOMOVE | SWP_DRAWFRAME);
                    solution.move((size+1) * 30 + 30, 10, 350, (size + 1) * 30 + 80);
                    SetWindowPos(GetDlgItem(hwnd, IDC_CLEAR), HWND_TOP, 100, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                    SetWindowPos(GetDlgItem(hwnd, IDOK), HWND_TOP, 20, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                    CheckMenuItem(GetMenu(hwnd), IDC_4x4, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_9x9, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_1, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_2, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_OTHERSIZE, MF_UNCHECKED);
                    for(int i=0; i<size*size; i++) sudoku->setFieldId(i/size,i%size,'0' + i/size/4*4 + i%size/3);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case IDC_12x12_2:
                    size = 12;
                    destroyField();
                    createField(hwnd, size);
                    SetWindowPos(hwnd, HWND_TOP, 0, 0, (size + 1) * 30 + 400, (size + 1) * 30 + 150, SWP_NOMOVE | SWP_DRAWFRAME);
                    solution.move((size+1) * 30 + 30, 10, 350, (size + 1) * 30 + 80);
                    SetWindowPos(GetDlgItem(hwnd, IDC_CLEAR), HWND_TOP, 100, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                    SetWindowPos(GetDlgItem(hwnd, IDOK), HWND_TOP, 20, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                    CheckMenuItem(GetMenu(hwnd), IDC_4x4, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_9x9, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_1, MF_UNCHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_12x12_2, MF_CHECKED);
                    CheckMenuItem(GetMenu(hwnd), IDC_OTHERSIZE, MF_UNCHECKED);
                    for(int i=0; i<size*size; i++) sudoku->setFieldId(i/size,i%size,'0' + i/size/3*3 + i%size/4);
                    InvalidateRect(hwnd, NULL, TRUE);
                    break;

                case IDC_OTHERSIZE:
                    size=DialogBox(hInst, MAKEINTRESOURCE(IDD_CHOOSESIZE), hwnd, ChooseSizeProc);
                    if(size > 4) {
                        destroyField();
                        createField(hwnd, size);
                        SetWindowPos(hwnd, HWND_TOP, 0, 0, (size + 1) * 30 + 400, (size + 1) * 30 + 150, SWP_NOMOVE | SWP_DRAWFRAME);
                        solution.move((size+1) * 30 + 30, 10, 350, (size + 1) * 30 + 80);
                        SetWindowPos(GetDlgItem(hwnd, IDC_CLEAR), HWND_TOP, 100, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                        SetWindowPos(GetDlgItem(hwnd, IDOK), HWND_TOP, 20, (size + 1) * 30 + 10, 0, 0, SWP_NOSIZE);
                        CheckMenuItem(GetMenu(hwnd), IDC_4x4, MF_UNCHECKED);
                        CheckMenuItem(GetMenu(hwnd), IDC_9x9, MF_UNCHECKED);
                        CheckMenuItem(GetMenu(hwnd), IDC_12x12_1, MF_UNCHECKED);
                        CheckMenuItem(GetMenu(hwnd), IDC_12x12_2, MF_UNCHECKED);
                        CheckMenuItem(GetMenu(hwnd), IDC_OTHERSIZE, MF_CHECKED);
                        chaosMode = true;
                        btnOk.setText("Chaos OK");
                        CheckMenuItem(GetMenu(hwnd), IDC_CHAOS, MF_CHECKED);
                        InvalidateRect(hwnd, NULL, TRUE);
                    }
                    break;

                case IDC_CHAOS:
                    if(chaosMode) {
                        chaosMode = false;
                        CheckMenuItem(GetMenu(hwnd), IDC_CHAOS, MF_UNCHECKED);
                        btnOk.setText("Lösen");
                        saveFieldIds();
                        loadNumbers();
                    } else {

                        CheckMenuItem(GetMenu(hwnd), IDC_CHAOS, MF_CHECKED);
                        btnOk.setText("Chaos Ok");
                        saveNumbers();
                        loadFieldIds();
                        chaosMode = true;
                    }
                    break;

                case 1337:
                    if (HIWORD(wParam) == EN_CHANGE ) {
                        if(chaosMode) {
                            saveFieldIds();
                            InvalidateRect(hwnd, NULL, TRUE);
                        }
                    }
                    break;

                default: return FALSE;
            }
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            if(sudoku != 0) {
                MoveToEx(hdc, 27, 27, 0);
                LineTo(hdc, 27, 27 + (sudoku->getSize()  * 30));
                MoveToEx(hdc, 27 + (sudoku->getSize()  * 30), 27, 0);
                LineTo(hdc, 27 + (sudoku->getSize()  * 30), 27 + (sudoku->getSize()  * 30));
                MoveToEx(hdc, 27, 27, 0);
                LineTo(hdc, 27 + (sudoku->getSize()  * 30), 27 );
                MoveToEx(hdc, 27, 27 + (sudoku->getSize()  * 30), 0);
                LineTo(hdc, 27 + (sudoku->getSize()  * 30), 27 + (sudoku->getSize()  * 30));

                for(int i=0; i<sudoku->getSize(); i++) {
                    for(int j=0; j<sudoku->getSize(); j++) {
                        if(sudoku->getFieldId(i,j) != sudoku->getFieldId(i, j+1)) {
                            MoveToEx(hdc, 27 + ((j+1)*30), 27 + ((i)*30), 0);
                            LineTo(hdc, 27 + ((j+1)*30), 27 + ((i+1)*30));
                        }
                        if(sudoku->getFieldId(i,j) != sudoku->getFieldId(i+1, j)) {
                            MoveToEx(hdc, 27 + ((j)*30), 27 + ((i+1)*30), 0);
                            LineTo(hdc, 27 + ((j+1)*30), 27 + ((i+1)*30));
                        }
                    }
                }
            }
            EndPaint(hwnd, &ps);

        default:                      /* for messages that we don't deal with */
            return FALSE;
    }
    return TRUE;
}

// Meldungshandler für Infofeld.
LPARAM CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

LPARAM CALLBACK ChooseSizeProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static pco::SpinEdit spin;
    static pco::Edit sizeNum;
	switch (message)
	{
	case WM_INITDIALOG:
        spin = GetDlgItem(hDlg, IDC_SPINNER);
        sizeNum = GetDlgItem(hDlg, IDC_SIZENUMBER);
        SendMessage(spin, UDM_SETBUDDY, (WPARAM) sizeNum.getHwnd(), 0);
        SendMessage(spin, UDM_SETRANGE, 0,MAKELPARAM(4,20));
        SendMessage(spin, UDM_SETPOS, 0, 12);


        spin.setValue(5);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
		    pco::Edit sizeText(GetDlgItem(hDlg, IDC_SIZENUMBER));
		    int s = sizeText.getInt();
		    if(s < 4 || s > 20) s = 0;
			EndDialog(hDlg, s);
			return TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

struct ThreadParam
{
    HWND hwnd;
};

unsigned solveThread(void *param)
{
    HWND hwnd = ((ThreadParam*)param)->hwnd;
    delete (ThreadParam*) param;

    sudoku = sudoku->solve();

    if(sudoku != 0) {
        SendMessage(hwnd, WM_SOLVED, (WPARAM) sudoku->getDifficulty(), 0);
        return 0;
    } else {
        SendMessage(hwnd, WM_UNSOLVED, 0, 0);
        sudoku = oldSudoku;
        return 0;
    }
}

bool solve(HWND hwnd) {
    saveNumbers();

    delete oldSudoku;
    oldSudoku = sudoku;

    ThreadParam *tp = new ThreadParam;
    tp->hwnd = hwnd;

    th = (HANDLE) _beginthreadex( 0, 0, solveThread,  tp, 0, &threadId);

    if(th == 0) return false;
    return true;
}

void cancelSolve(HWND hwnd)
{
    TerminateThread(th, 0);
    CloseHandle(th);
}

bool load(HWND hwnd)
{
    char fn[MAX_PATH] = "";

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fn;
    ofn.lpstrFilter = "Sudoku Dateien (.sud)\0*.sud\0Alle Dateien\0*\0\0";
    ofn.nFilterIndex = 1;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = "sud";

    if(GetOpenFileName(&ofn)) {
        HANDLE file = CreateFile(fn, GENERIC_READ, 0, NULL, OPEN_EXISTING,0, 0);
        if (file == INVALID_HANDLE_VALUE) {
            MessageBox(hwnd, "Kann Datei nicht öffnen", "Fehler", MB_OK);
            return false;
        }

        DWORD read;

        // write a version number, current version is 1
        int version = 0;
        ReadFile(file, &version, sizeof(version), &read, NULL);
        if(read != sizeof(version)) {
            MessageBox(hwnd, "Die Datei ist keine Sudoku Datei\n(Lesefehler)", "Fehler", MB_OK | MB_ICONERROR);
            CloseHandle(file);
            return false;
        }
        if(version != 1) {
            MessageBox(hwnd, "Die Datei ist keine Sudoku Datei\n(falsche Version)", "Fehler", MB_OK | MB_ICONERROR);
            CloseHandle(file);
            return false;
        }

        // write the size of the sudoku
        int size = 0;
        ReadFile(file, &size, sizeof(size), &read, NULL);
        if(read != sizeof(size)) {
            MessageBox(hwnd, "Die Datei ist keine Sudoku Datei\n(Lesefehler)", "Fehler", MB_OK | MB_ICONERROR);
            CloseHandle(file);
            return false;
        }
        if(size < 4 || size > 16) {
            MessageBox(hwnd, "Die Datei ist keine Sudoku Datei\n(falsche Größe)", "Fehler", MB_OK | MB_ICONERROR);
            CloseHandle(file);
            return false;
        }

        sudoku = new Sudoku(size);

        // read the FieldIds
        for (int i=0; i<size; i++)
        for (int j=0; j<size; j++) {
            char tmp;
            ReadFile(file, &tmp, sizeof(tmp), &read, NULL);
            if(read != sizeof(tmp)) {
                MessageBox(hwnd, "Die Datei ist keine Sudoku Datei\n(Lesefehler)", "Fehler", MB_OK | MB_ICONERROR);
                CloseHandle(file);
                return false;
            }
            sudoku->setFieldId(i,j, tmp);
        }

        // read the numbers
        for (int i=0; i<size; i++)
        for (int j=0; j<size; j++) {
            char tmp;
            ReadFile(file, &tmp, sizeof(tmp), &read, NULL);
            if(read != sizeof(tmp)) {
                MessageBox(hwnd, "Die Datei ist keine Sudoku Datei\n(Lesefehler)", "Fehler", MB_OK | MB_ICONERROR);
                CloseHandle(file);
                return false;
            }
            sudoku->setNumber(i,j, tmp);
        }

        CloseHandle(file);
    }
    else
    {
        return false;
    }
    return true;
}

bool save(HWND hwnd)
{
    char fn[MAX_PATH] = "";

    OPENFILENAME ofn;
    memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fn;
    ofn.lpstrFilter = "Sudoku Dateien (.sud)\0*.sud\0Alle Dateien\0*\0\0";
    ofn.nFilterIndex = 1;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "sud";

    if(GetSaveFileName(&ofn)) {
        HANDLE file = CreateFile(fn, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,0, 0);
        if (file == INVALID_HANDLE_VALUE) {
            MessageBox(hwnd, "Kann nicht speichern", "Fehler", MB_OK);
            return false;
        }

        DWORD written;

        // write a version number, current version is 1
        int version = 1;
        WriteFile(file, &version, sizeof(version), &written, NULL);

        // write the size of the sudoku
        int size = sudoku->getSize();
        WriteFile(file, &size, sizeof(size), &written, NULL);

        // write the FieldIds
        for (int i=0; i<size; i++)
        for (int j=0; j<size; j++) {
            char tmp = sudoku->getFieldId(i,j);
            WriteFile(file, &tmp, sizeof(tmp), &written, NULL);
        }

        // write the numbers
        for (int i=0; i<size; i++)
        for (int j=0; j<size; j++) {
            char tmp = sudoku->getNumber(i,j);
            WriteFile(file, &tmp, sizeof(tmp), &written, NULL);
        }
        CloseHandle(file);
    }
    //GetSaveFileName(&ofn);

    return true;
}
