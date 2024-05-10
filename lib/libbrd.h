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


#ifndef LIBBRD_H
#define LIBBRD_H


#include "utils.h"
#include "options.h"


#define LIBBADCALL    -1
#define LIBMALLOC     -2
#define LIBFILE       -3
#define LIBVALIDATE   -4
#define LIBBADAUXLEN  -5


int EXECUTE(CanvasOpts * canvopts,
	    void (*finfunc)(),
	    int (*validfunc)(),
	    char ** outfn,
	    uint32 outfl); 



#endif /* LIBBRD_H */
