/****************************************************************************/
/* debug.c: debugging / verbose output utilities for FRASCR application     */
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


#include "debug.h"



void debug(DParam* param, 
	   int level,
	   char * msg,
	   ... )
{
  va_list args;
  if (param->out){
    if (level <= param->mask) {
      va_start(args, msg);
      vfprintf(param->out, msg, args);
      va_end(args);
    }
  }
}


void debugflush(DParam * param)
{
  if (param->out) {
    fflush(param->out);
  }
}
