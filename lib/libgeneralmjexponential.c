/****************************************************************************/
/* Libgeneralmjexponential.c: shared object for the FRASCR application      */
/*   Provides an EXECUTE function: computation of the generalized sets for  */
/*   the complex functions given by eqns 1-3 in "The generalized            */
/*   Mandelbort-Julia [sic] sets from a class of complex exponential map,"  */
/*   by W Xingyuan and S Qijiang, 2006. Implements a secondary-option       */
/*   holding lambda and w (see paper), as well as the iteration type. The   */
/*   iteration types are carried out in separate functions: Mandelbrot or   */
/*   Julia set computation for each of the functions. UNDER CONSTRUCTION.   */
/*   For a finishing library, this outputs only a single double array of    */
/*   unsigned ints.                                                         */
/*   Last updated: 2024-05-02                                               */
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


#include "libgeneralmjexponential.h"
#include <stdlib.h>
#include <math.h>


#define FREE_DBL_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { free((arr)[l]); } free((arr)); (arr)=NULL; } }
#define CLOSE_FILE_ARRAY(arr,l,m) { if ((arr)) { for (l=0; l<m; l++) { if ((arr)[l]) fclose((arr)[l]); free((arr)); (arr)=NULL; } } }
#define ABS(x) (x < 0 ? -1.*x : x)
#define NEARZERO(x,w) (ABS(x) < w ? 1 : 0)
#define MIN(x,y) (x < y ? x : y)


struct secondary_option {
  real_f wre, wim, lre, lim;
  void (*iterfunc)();
};
typedef struct secondary_option SecondaryOpts;


void type1_julia(CanvasOpts * canvopts,
		 SecondaryOpts * secopts,
		 Datum *** canv);

void type2_julia(CanvasOpts * canvopts,
		 SecondaryOpts * secopts,
		 Datum *** canv);

void type3_julia(CanvasOpts * canvopts,
		 SecondaryOpts * secopts,
		 Datum *** canv);


static inline void * process_type(int x) {
  switch (x) {
  case 1:
    return type1_julia;
    break;
  case 2:
    return type2_julia;
    break;
  case 3:
    return type3_julia;
    break;
  default:
    return NULL;
  }
}


static inline int process_sec_opts(char ** const src, const int_f l, SecondaryOpts * targ) {
  if ((src==NULL) || (targ==NULL))
    return LIBBADCALL;
  if (l < 5)
    return LIBBADAUXLEN;
  targ->wre = atof(src[0]);
  targ->wim = atof(src[1]);
  targ->lre = atof(src[2]);
  targ->lim = atof(src[3]);
  targ->iterfunc = process_type(atoi(src[4]));
  if (targ->iterfunc == NULL)
    return LIBBADAUXOPT;
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
  int i, j;
  SecondaryOpts secopts;
  int ret;

  if ( (canvopts==NULL) || (finfunc==NULL) || (validfunc==NULL) || (outfn==NULL) )
    return LIBBADCALL;

  if (canvopts->secondary == NULL)
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

  /* Execute iteration type */

  secopts.iterfunc(canvopts, &secopts, &canv);

  /* output results */

  finfunc(canvopts, canva, canvl, outfa, outfl);

  FREE_DBL_ARRAY(canv,i,canvopts->nwidth);
  if (canva)
    free(canva);
  CLOSE_FILE_ARRAY(outfa,i,outfl);
  
  return 0;
  
}



void type1_julia(CanvasOpts * canvopts,
		 SecondaryOpts * secopts,
		 Datum *** canv){
  real_f x, y, expbuf, modbuf, prodbuf, inner, x0, y0, left, bottom, width, height;
  real_f logrhoz;
  real_f thetaz, rhoz, thetal, rhol;
  counter_f n, max;
  real_f w_re, w_im;
  real_f lam_re, lam_im;
  real_f smallerinterval;
  int_f nx, ny;
  int i, j;

  left = canvopts->left;
  nx = canvopts->nwidth;
  width = canvopts->width;
  bottom = canvopts->bottom;
  ny = canvopts->nheight;
  height = canvopts->height;
  max = canvopts->escape;
  smallerinterval = MIN((width/(double)nx),(height/(double)ny));
  w_re = secopts->wre;
  w_im = secopts->wim;
  lam_re = secopts->lre;
  lam_im = secopts->lim;
  //julia set: lambda doesn't change
  rhol = sqrt(lam_re*lam_re + lam_im*lam_im);
  thetal = atan2(lam_im, lam_re);
  
  /* core functionality, execute */

  for (i=0; i<nx; i++) {
    for (j=0; j<ny; j++) {

      //julia set: iterate x0, y0, not lambda
      x0 = left + ((real_f)i) * width / ((real_f)nx);
      y0 = bottom + ((real_f)j) * height / ((real_f)ny);
      x = 0.;
      y = 0.;
      rhoz = sqrt(x0*x0 + y0*y0);
      thetaz = atan2(y0,x0);
      n = 0;
      if ( x0 > 50. ) {

	continue;

      } else {

	while ( n < max ) {

	  if (NEARZERO(rhoz,smallerinterval) == 1) {
	    x = lam_re;
	    y = lam_im;
	    n += 1;
	  } else {
	    if ( x <= 50. ) {
	      logrhoz = log(rhoz);
	      expbuf = exp(w_re*logrhoz-w_im*thetaz);
	      prodbuf = w_im*logrhoz+w_re*thetaz;
	      modbuf = rhol*exp(expbuf*cos(prodbuf));
	      inner = thetal + expbuf*sin(prodbuf);
	      x = modbuf*cos(inner);
	      y = modbuf*sin(inner);
	      rhoz = sqrt(x*x + y*y);
	      thetaz = atan2(y,x);
	      n += 1;
	    }
	    else
	      break;
	  }
	  
	} /* while n < max */
	
      } /* if x < 50 */
      
      (*canv)[i][j].re = x0;
      (*canv)[i][j].im = y0;
      (*canv)[i][j].n = n;

    } /* for j */
  } /* for i */

  return;
  
}


void type2_julia(CanvasOpts * canvopts,
		 SecondaryOpts * secopts,
		 Datum *** canv){
  real_f x, y, expbuf, modbuf, prodbuf, inner, x0, y0, left, bottom, width, height;
  real_f logrhoz;
  real_f thetaz, rhoz, thetal, rhol;
  counter_f n, max;
  real_f w_re, w_im;
  real_f lam_re, lam_im;
  real_f smallerinterval;
  int_f nx, ny;
  int i, j;

  left = canvopts->left;
  nx = canvopts->nwidth;
  width = canvopts->width;
  bottom = canvopts->bottom;
  ny = canvopts->nheight;
  height = canvopts->height;
  max = canvopts->escape;
  smallerinterval = MIN((width/(double)nx),(height/(double)ny));
  w_re = secopts->wre;
  w_im = secopts->wim;
  lam_re = secopts->lre;
  lam_im = secopts->lim;
  //julia set: lambda doesn't change
  
  /* core functionality, execute */

  for (i=0; i<nx; i++) {
    for (j=0; j<ny; j++) {

      //julia set: iterate x0, y0, not lambda
      x0 = left + ((real_f)i) * width / ((real_f)nx);
      y0 = bottom + ((real_f)j) * height / ((real_f)ny);
      x = 0.;
      y = 0.;
      rhoz = sqrt(x0*x0 + y0*y0);
      thetaz = atan2(y0,x0);
      n = 0;
      if ( x0 > 50. ) {

	continue;

      } else {

	while ( n < max ) {

	  if (NEARZERO(rhoz,smallerinterval) == 1) {
	    x = lam_re;
	    y = lam_im;
	    n += 1;
	  } else {
	    if ( x <= 50. ) {
	      logrhoz = log(rhoz);
	      expbuf = exp(w_re*logrhoz-w_im*thetaz);
	      prodbuf = w_im*logrhoz+w_re*thetaz;
	      modbuf = exp(expbuf*cos(prodbuf)+lam_re);
	      inner = expbuf*sin(prodbuf)+lam_im;
	      x = modbuf*cos(inner);
	      y = modbuf*sin(inner);
	      rhoz = sqrt(x*x + y*y);
	      thetaz = atan2(y,x);
	      n += 1;
	    }
	    else
	      break;
	  }
	  
	} /* while n < max */
	
      } /* if x < 50 */
      
      (*canv)[i][j].re = x0;
      (*canv)[i][j].im = y0;
      (*canv)[i][j].n = n;

    } /* for j */
  } /* for i */

  return;
}



void type3_julia(CanvasOpts * canvopts,
		 SecondaryOpts * secopts,
		 Datum *** canv){
  real_f x, y, expbuf, modbuf, prodbuf, inner, x0, y0, left, bottom, width, height;
  real_f logrhoz;
  real_f thetaz, rhoz, thetal, rhol;
  counter_f n, max;
  real_f w_re, w_im;
  real_f lam_re, lam_im;
  real_f smallerinterval;
  int_f nx, ny;
  int i, j;

  left = canvopts->left;
  nx = canvopts->nwidth;
  width = canvopts->width;
  bottom = canvopts->bottom;
  ny = canvopts->nheight;
  height = canvopts->height;
  max = canvopts->escape;
  smallerinterval = MIN((width/(double)nx),(height/(double)ny));
  w_re = secopts->wre;
  w_im = secopts->wim;
  lam_re = secopts->lre;
  lam_im = secopts->lim;
  //julia set: lambda doesn't change
  
  /* core functionality, execute */

  for (i=0; i<nx; i++) {
    for (j=0; j<ny; j++) {

      //julia set: iterate x0, y0, not lambda
      x0 = left + ((real_f)i) * width / ((real_f)nx);
      y0 = bottom + ((real_f)j) * height / ((real_f)ny);
      x = 0.;
      y = 0.;
      rhoz = sqrt(x0*x0 + y0*y0);
      thetaz = atan2(y0,x0);
      n = 0;
      if ( x0 > 50. ) {

	continue;

      } else {

	while ( n < max ) {

	  if (NEARZERO(rhoz,smallerinterval) == 1) {
	    x = lam_re;
	    y = lam_im;
	    n += 1;
	  } else {
	    if ( x <= 50. ) {
	      logrhoz = log(rhoz);
	      expbuf = exp(w_re*logrhoz-w_im*thetaz)/rhol;
	      prodbuf = w_im*logrhoz+w_re*thetaz-thetal;
	      modbuf = exp(expbuf*cos(prodbuf));
	      inner = expbuf*sin(prodbuf);
	      x = modbuf*cos(inner);
	      y = modbuf*sin(inner);
	      rhoz = sqrt(x*x + y*y);
	      thetaz = atan2(y,x);
	      n += 1;
	    }
	    else
	      break;
	  }
	  
	} /* while n < max */
	
      } /* if x < 50 */
      
      (*canv)[i][j].re = x0;
      (*canv)[i][j].im = y0;
      (*canv)[i][j].n = n;

    } /* for j */
  } /* for i */

  return;
}
