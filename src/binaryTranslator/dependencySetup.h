/*******************************************************************************
 * dependencySetup.h
 ******************************************************************************/

#ifndef _DEP_SETUP_H_
#define _DEP_SETUP_H_


#include "global.h"
#include "list.h"
#include "basicblock.h"
#include "instruction.h"
#include "make_subblock.h"
#include "dependencyTable.h"

void dependencySetup (List<basicblock*>*, List<instruction*>*);

#endif