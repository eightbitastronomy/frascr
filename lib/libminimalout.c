/****************************************************************************/
/* Libminimalout.c: printing shared object for the FRASCR application.      */
/*   Provides a FINISH function and VALIDATE function.                      */
/*   FINISH outputs text only. VALIDATE always returns 0 (not useful).      */
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


#include "libminimalout.h"


void FINISH(CanvasOpts * opts,
	    Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel)
{

  int i, j;
  Datum ** canvas = dataa[0];
  FILE * output = filea[0];
  
  for (i=0; i<opts->nwidth; i++) {
    for (j=0; j<opts->nheight; j++) {
      fprintf(output, "%f %f %d\n", canvas[i][j].re, canvas[i][j].im, canvas[i][j].n);
    }
  }

  return;
}


int VALIDATE(Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel)
{
  return 0;
}
