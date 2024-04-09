/****************************************************************************/
/* Libbwpng.h: printing shared object for the FRASCR application.           */
/*   Provides a FINISH function and VALIDATE function.                      */
/*   FINISH outputs black and white png files by converting unsigned        */
/*   integer data into an intensity.                                        */
/****************************************************************************/
/* Update 2024-04-09: modified FINISH to output any # of canvases so long   */
/*    as there is an output file for it.                                    */
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


#ifndef LIBBWPNG_H
#define LIBBWPNG_H


#include "options.h"
#include "utils.h"
#include <stdio.h>


void FINISH(CanvasOpts * opts,
	    Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel);


int VALIDATE(Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel);



#endif /* LIBBWPNG_H */
