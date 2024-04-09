/****************************************************************************/
/* libopen.h: dynamic library handling for FRASCR application               */
/*   Functions for opening, loading, and cleaning up shared objects /       */
/*   libraries.                                                             */
/*   Last updated 2024-04-09                                                */
/****************************************************************************/
/*  Author: Miguel Abele                                                    */
/*  Copyrighted by Miguel Abele, 2024.                                      */
/*                                                                          */
/*  License information:                                                    */
/*                                                                          */
/*  This file is a part of the FRASCR application.                          */
/*                                                                          */
/*  FRASCR is free software; you can redistribute it and/or                 */
/*  modify it under the terms of the GNU General Public License             */
/*  as published by the Free Software Foundation; either version 3          */
/*  of the License, or (at your option) any later version.                  */
/*                                                                          */
/*  FRASCR is distributed in the hope that it will be useful,               */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           */
/*  GNU General Public License for more details.                            */
/*                                                                          */
/*  You should have received a copy of the GNU General Public License       */
/*  along with this program; if not, write to the Free Software             */
/*  Foundation, Inc., 51 Franklin Street, Fifth Floor,                      */
/*  Boston, MA  02110-1301, USA.                                            */
/****************************************************************************/


#ifndef LIBOPEN_H
#define LIBOPEN_H

#include "debug.h"
#include "options.h"

#define LO_EXECUTE   "EXECUTE"
#define LO_FINISH    "FINISH"
#define LO_VALIDATE  "VALIDATE"
#define LONULLARG    -10
#define LOOPENFILEE  -20
#define LOOPENFILEF  -30
#define LOLOADEXEC   -21
#define LOLOADFINS   -31

int load_libraries(CoreOpts * opts, DParam  * debug);

void close_libraries(CoreOpts * opts, DParam * debug);

#endif /* LIBOPEN_H */
