/****************************************************************************/
/* debug.h: debugging / verbose output utilities for FRASCR application     */
/*   Defines verbosity levels as an enum, as well as printing macros.       */
/*   Printing functions still present, though are extraneous.               */
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


#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>
#include <stdio.h>

enum {
  D0,
  D1,
  D2,
  DS
};


struct dstruct {
  FILE * out;
  char * outs;
  int mask;
};

typedef struct dstruct DParam;


/* Printer fctn for debug information */
void debug(DParam * param,  /* struct containing output file stream, etc., for debug printing */
	   int level,       /* debug level for masking debug output */
	   char * msg,      /* string output to be passed to vfprintf */
	   ...              /* va_arg list to be passed to vfprintf */
	   );

/* Flush debug stream */
void debugflush(DParam * param);


/* alternative to an actual debug function */
/* using VA_OPT and VA_ARGS, can accept no args past msg, or can accept a var-length list.
   This depends on the compiler implementing C++20 or GNU C. But, since I can't overload macros,
   unsure how to implement in the older-C cases. */
#define DEBUG(param,level,msg,...) if (level <= (param)->mask) { fprintf((param)->out, msg __VA_OPT__(,) __VA_ARGS__); }

#define DEBUGFLUSH(param) if ((param)->out) { fflush((param)->out); }

#define DEBUGCLEANUP(param) { if ((param)->outs) { free((param)->outs); fclose((param)->out); } }

#endif /* DEBUG_H */
