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


#ifndef LIBGENERALMJEXPONENTIAL_H
#define LIBGENERALMJEXPONENTIAL_H


#include "utils.h"
#include "options.h"


#define LIBBADCALL    -1
#define LIBMALLOC     -2
#define LIBFILE       -3
#define LIBVALIDATE   -4
#define LIBBADAUXLEN  -5
#define LIBBADAUXOPT  -6


int EXECUTE(CanvasOpts * canvopts,
	    void (*finfunc)(),
	    int (*validfunc)(),
	    char ** outfn,
	    int_f outfl); 



#endif /* LIBGENERALMJEXPONENTIAL_H */
