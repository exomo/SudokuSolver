# SudokuSolver
This is a program that is able to solve Sudoku puzzles. It can not only solve the standard 9x9 sudoku, but also larger ones and sudokus with non-rectangular fields (chaos sudoku).

## License
License: GPL Version 3

## Current status 
The project is in a somewhat poor state. The code is from a time when I just started programming, so there is some room for improvement. I am currently working on some "modernization" (See Next Steps), but concerning features, the project is finished.

## How to install
Currently there is only one binary release for 64 bit Windows. Download the archive SudokuSolverWin64.7z from the repository and extract all the files to a directory of your choice. You should then be able to start SudokuSolver.exe.

## How to use
### Basic usage
To solve a sudoku, enter the given numbers in the text boxes. When you click "Lösen", all the missing numbers are filled in. A MessageBox will tell you if the sudoku was solved or not (if it is unsolvable), and a calculated difficulty value.

Click on "Clear" to remove all the numbers and start over.

### Chosing a different size
It is also possible to solve sudoku puzzles with sizes other than 9x9. For 12x12 puzzles, chose one of the 12x12 options from the "Größe" menu, to resize the field. You can also chose "Andere Größe (Chaos)", to enter any size in the popup-dialog. This will start "chaos mode" (see next section).

Hint: Since there are only 10 digits, you have to use other values for larger fields. You can use any character you want, you don't even have to use any digits. Just make sure that you use a fixed set of characters (as many as the field size is).

### Chaos Mode, changing the layout
The Chaos Mode allows to change the layout of the sudoku fields. To start the chaos mode, chose "Chaoseingabe" from the "Rätsel" menu.
In this mode, the cells do not contain regular values, but sub-field identifiers. All cells that have the same identifier belong to the same sub-field. You can use any character as identifier. Individual sub-fields can be identified by the borders around them. Make sure to have exactly N sub-fields of N cells each, for a sudoku of size NxN. Cells of a sub-fields do not necessarily have to be all connected.

When you are done specifying the sub-fields, click the "Chaos Ok" button to return to normal input.

### Loading ans saving
If you want to reuse a sudoku or resume input at a later time, you can save it to a file and later load from it.
Chose "Speichern" or "Laden" from the "Rätsel" menu to save or load the current state of input. 
Both the values from regular input (cell values) and chaos input are saved. 

## How to build
The current code is only made for Windows (using WinAPI and resource files), so it will most likely not compile for any other system. I am using a Code::Blocks project, configured to use a custom compiler setup. The compiler I use is Mingw-W64.
If you don't use Code::Blocks, set up the build environment of your choice to compile all the .cpp and the .rc file into a single exe. The manifest is made for x64 architecture, if you want to use a 32 bit compiler, make sure to patch the processorArchitecture properties in the manifest first.

## Next steps
* Use cmake to make build easier
* Port GUI to Qt
* General stability improvement and code clean-up