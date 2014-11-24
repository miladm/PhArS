/*******************************************************************************
 * inoBkEnd.h
 *******************************************************************************/

#ifndef _G_INO_BACKEND
#define _G_INO_BACKEND

#include <stdint.h>
#include "sysCore.h"
#include "../unit/sysClock.h"

void inoBkEnd_init ();
void inoBkEndRun (FRONTEND_STATUS);
void inoBkEnd_fini ();

#endif
