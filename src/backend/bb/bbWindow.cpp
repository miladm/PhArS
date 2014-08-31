/*******************************************************************************
 * bbWindow.cpp
 ******************************************************************************/

#include "bbWindow.h"

bbWindow::bbWindow (string bbWin_id, sysClock* clk)
    : unit ("bbWindow_" + bbWin_id, clk),
      _win (100, 8, 8, clk, "bbWindow_" + bbWin_id), //TODO configure and more real numbers
      _id (atoi (bbWin_id.c_str ()))
{ }


/* deploy the code for rfManager
 * remove the bbWindow creation from sysCore back to scheduler
 * add an inherited class for bbInstruction for BB extra functionalities and update the frontend
 * integrate the functionality of RF_MGR in the core
 * TODO: delte this text
 */