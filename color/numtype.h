/****************************************************************************/
/* numtype.h: color library for FRASCR application                          */
/*   This library may eventually be expanded.                               */
/*   Currently, implements functionality for making a color wheel, working  */
/*   with swatches of a few different color spaces, and a few conversions.  */
/*   However, currently, it really is mostly useful for starting in LCH and */
/*   moving over to sRGB.                                                   */
/*   Some of the algorithms implemented here follow work found online. A    */
/*   URL has been provided below.                                           */
/*   Last updated 2024 May                                                  */
/****************************************************************************/
/*  Author: Miguel Abele                                                    */
/*  Copyrighted by Miguel Abele, 2024.                                      */
/*  Contains mathematical and empirical results for which the               */
/*  intellectual rights belong to the respective authors. I have tried to   */
/*  adequately cite their work based on the information available to me.    */
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


#ifndef NUMTYPE_H
#define NUMTYPE_H


/* numerical type defs used throughout the color library */

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long uint64;
typedef double float64;


#endif //NUMTYPE_H
