/****************************************************************************/
/* Libjuliaquadbrute.c: shared object for the FRASCR application            */
/*   Provides an EXECUTE function: computation of Kc (filled julia set)     */
/*   for the quadratic function z^2 + c.                                    */
/*   For a finishing library, this outputs only a single double array of    */
/*   unsigned ints.                                                         */
/*   Last updated: 2024 May                                                 */
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


#include "libjuliaquadbrute.h"
#include <stdlib.h>


#define FREE_DBL_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { free((arr)[l]); } free((arr)); (arr)=NULL; } }
#define CLOSE_FILE_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { if ((arr)[l]) fclose((arr)[l]); free((arr)); (arr)=NULL; } } }


int EXECUTE(CanvasOpts * canvopts,
	    void (*finfunc)(),
	    int (*validfunc)(),
	    char ** outfn,
	    uint32 outfl)
{
  /* The data holders used in execute */
  Datum ** canv;
  /* Validator will check dataa and datal, outfa and outfl.
     dataa must be one spot for each data holder used above, datal the total num.
     outfa is the array of file pointers, and outfl the total num. */
  Datum *** canva = NULL;
  const uint32 canvl = 1;
  FILE ** outfa = NULL;
  /* variables local to execute */
  float64 x, y, xtmp, ytmp, x0, y0, left, bottom, width, height;
  uint32 n, max;
  uint32 nx, ny;
  int i, j;

  if ( (canvopts==NULL) || (finfunc==NULL) || (validfunc==NULL) || (outfn==NULL) )
    return LIBBADCALL;

  /* setup memory and organize for validator */

  canv = malloc(sizeof(Datum *)*canvopts->nwidth);
  if (canv == NULL)
    return LIBMALLOC;
  for (i=0; i<canvopts->nwidth; i++) {
    canv[i] = malloc(sizeof(Datum)*canvopts->nheight);
    if (canv[i-1] == NULL) {
      FREE_DBL_ARRAY(canv,j,i-2);
      return LIBMALLOC;
    }
  }

  canva = malloc(sizeof(Datum **)*canvl);
  if (canva == NULL) {
    FREE_DBL_ARRAY(canv,j,canvopts->nwidth);
    return LIBMALLOC;
  }
  canva[0] = canv;

  outfa = malloc(sizeof(FILE *)*outfl);
  if (outfa == NULL) {
    FREE_DBL_ARRAY(canv,j,canvopts->nwidth);
    free(canva);
    return LIBMALLOC;
  }
  for (i=0; i<outfl; i++) {
    outfa[i] = fopen(outfn[i], "wb");
    if (!outfa[i-1]) {
      CLOSE_FILE_ARRAY(outfa,j,i-2);
      FREE_DBL_ARRAY(canv,j,canvopts->nwidth);
      free(canva);
      return LIBFILE;
    }
  }

  if (validfunc(canva, canvl, outfa, outfl) != 0) {
    FREE_DBL_ARRAY(canv,j,canvopts->nwidth);
    free(canva);
    CLOSE_FILE_ARRAY(outfa,j,outfl);
    return LIBVALIDATE;
  }

  /* need to "palettize" here */

  left = canvopts->left;
  nx = canvopts->nwidth;
  width = canvopts->width;
  bottom = canvopts->bottom;
  ny = canvopts->nheight;
  height = canvopts->height;
  max = canvopts->escape;
  x0 = canvopts->coord_Re;
  y0 = canvopts->coord_Im;
  
  /* core functionality, execute */

  for (i=0; i<nx; i++) {
    for (j=0; j<ny; j++) {

      x = left + ((float64)i) * width / ((float64)nx);
      y = bottom + ((float64)j) * height / ((float64)ny);
      n = 0;
      canv[i][j].re = x;
      canv[i][j].im = y;

      while ( n < max ) {
	
	ytmp = (x+x)*y + y0;
	x = x*x - y*y + x0;
	y = ytmp;

	if (x*x + y*y > 4.0) {
	  break;
	}
	
	n += 1;

      }

      canv[i][j].n = n;

    } /* for j */
  } /* for i */

  /* output results */

  finfunc(canvopts, canva, canvl, outfa, outfl);

  FREE_DBL_ARRAY(canv,i,canvopts->nwidth);
  if (canva)
    free(canva);
  CLOSE_FILE_ARRAY(outfa,i,outfl);
  
  return 0;
  
}
