/****************************************************************************/
/* Libbrd.h: shared object for the FRASCR application                       */
/*   Provides an EXECUTE function: computation of the Baker-Rippon-         */
/*   Devaney (BRD) explored in "Growth in Complex Exponential Dynamics" by  */
/*   M Romera, G Pastor, G Alvarez, F Montoya, 2000. This is set generated  */
/*   by lambda exp(z), in particular the Mandelbrot set. This code has been */
/*   adopted from libgeneralmjexponential: when that library is finished,   */
/*   this library will just be a special case of that library's func-       */
/*   tionality.                                                             */
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


#include "libbrd.h"
#include <stdlib.h>
#include <math.h>


#define FREE_DBL_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { free((arr)[l]); } free((arr)); (arr)=NULL; } }
#define CLOSE_FILE_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { if ((arr)[l]) fclose((arr)[l]); free((arr)); (arr)=NULL; } } }


struct secondary_option {
  real_f wre, wim, lre, lim;
  void (*iterfunc)();
};
typedef struct secondary_option SecondaryOpts;


static inline int process_sec_opts(char ** const src, const int_f l, SecondaryOpts * targ) {
  if ((src==NULL) || (targ==NULL))
    return LIBBADCALL;
  if (l < 2)
    return LIBBADAUXLEN;
  targ->wre = atof(src[0]);
  targ->wim = atof(src[1]);
  return 0;
}



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
  real_f x, y, expbuf, inner, x0, y0, left, bottom, width, height;
  real_f lamexp1, lamexp2, cosy, siny;
  counter_f n, max;
  real_f w_re, w_im;
  real_f lam_re, lam_im, rhol, thetal;
  real_f biggerbound;
  int_f nx, ny;
  int i, j;
  SecondaryOpts secopts;
  int ret;

  if ( (canvopts==NULL) || (finfunc==NULL) || (validfunc==NULL) || (outfn==NULL) )
    return LIBBADCALL;

  ret = process_sec_opts(canvopts->secondary, canvopts->secondaryl, &secopts);
  if (ret)
    return ret;

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
  w_re = secopts.wre;
  w_im = secopts.wim;

  /* core functionality, execute */

  for (i=0; i<nx; i++) {
    for (j=0; j<ny; j++) {

      lam_re = left + ((real_f)i) * width / ((real_f)nx);
      lam_im = bottom + ((real_f)j) * height / ((real_f)ny);
      rhol = sqrt(lam_re*lam_re + lam_im*lam_im);
      thetal = atan2(lam_im, lam_re);

      if ( lam_re > 50. ) {
	n = 0;
      } else {

	x = 0.;
	y = 0.;
	n = 0;

	while ( n < max ) {
	  
	  if ( x <= 50. ) {
	    expbuf = rhol*exp(x);
	    inner = thetal + sqrt(x*x+y*y)*sin(atan2(y,x));
	    x = expbuf*cos(inner);
	    y = expbuf*sin(inner);
	    n += 1;
	  }
	  else
	    break; 

	} /* while n < max */

      } /* else */
      
      canv[i][j].re = lam_re;
      canv[i][j].im = lam_im;
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

