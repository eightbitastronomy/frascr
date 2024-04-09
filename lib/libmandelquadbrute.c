/****************************************************************************/
/* Libmandelquadbrute.c: shared object for the FRASCR application           */
/*   Provides an EXECUTE function: computation of the Mandelbrot set for    */
/*   the quadratic function z^2 + c.                                        */
/*   For a finishing library, this outputs only a single double array of    */
/*   unsigned ints.                                                         */
/*   Last updated: 2024-04-09                                               */
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


#include "libmandelquadbrute.h"
#include <stdlib.h>


#define FREE_DBL_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { free((arr)[l]); } free((arr)); (arr)=NULL; } }
#define CLOSE_FILE_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { if ((arr)[l]) fclose((arr)[l]); free((arr)); (arr)=NULL; } } }


int EXECUTE(CanvasOpts * canvopts,
	    void (*finfunc)(),
	    int (*validfunc)(),
	    char ** outfn,
	    int_f outfl)
{
  /* The data holders used in execute */
  Datum ** canv;
  /* Validator will check dataa and datal, outfa and outfl.
     dataa must be one spot for each data holder used above, datal the total num.
     outfa is the array of file pointers, and outfl the total num. */
  Datum *** canva = NULL;
  const int_f canvl = 1;
  FILE ** outfa = NULL;
  /* variables local to execute */
  real_f x, y, xsq, ysq, x0, y0, left, bottom, width, height;
  counter_f n, max;
  int_f nx, ny;
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

  /* core functionality, execute */

  for (i=0; i<nx; i++) {
    for (j=0; j<ny; j++) {

      x0 = left + ((real_f)i) * width / ((real_f)nx);
      y0 = bottom + ((real_f)j) * height / ((real_f)ny);
      
      if ( x0*x0+y0*y0 > 4. ) {
	n = 0;
      } else {
	
	x = 0.;
	y = 0.;
	n = 0;

	while ( n < max ) {
	  
	  xsq = x*x;
	  ysq = y*y;
	  if (xsq+ysq <= 4.) {
	    y = (x+x)*y + y0;
	    x = xsq - ysq + x0;
	    n += 1;
	  }
	  else
	    break; 
	} /* while n < max */
	
      } /* else */
      
      canv[i][j].re = x0;
      canv[i][j].im = y0;
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
