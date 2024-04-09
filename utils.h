/****************************************************************************/
/* utils.h: utility definitions for FRASCR application                      */
/*   Defines structs for Datum as well as Palette, though this is right     */
/*   now unused.                                                            */
/*   Gives typedefs for integer types. These will need to be altered so     */
/*   that the whole of the application, including the libraries, can        */
/*   switch between 8, 16, and 32 bit storage, and 8 & 16 bit output.       */
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



#ifndef UTILS_H
#define UTILS_H


typedef unsigned short short_f;
typedef unsigned int int_f;
typedef unsigned int counter_f;
typedef double real_f;


struct datum {
  real_f re;
  real_f im;
  counter_f n;
};

typedef struct datum Datum;



struct canvas_f {
  int_f w;
  int_f h;
};

struct palette_f {
  real_f x;
  real_f y;
  real_f x0;
  real_f y0;
  counter_f n;
};


#endif /* UTILS_H */
