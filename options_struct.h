/****************************************************************************/
/* options_struct.h: struct definitions for FRASCR application              */
/*   Canvas and Core structures.                                            */
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

#ifndef OPTIONS_STRUCT_H
#define OPTIONS_STRUCT_H

#include "color.h"
#include "options.h"

struct coreopts {
  void * lib_exec;
  int (*execute)();
  char * execs;
  void * lib_fin;
  int (*finish)();
  char * fins;
  int (*validate)();
  char ** outs;
  int outl;
};
typedef struct coreopts CoreOpts;


struct coloropts {
  SwatchGenMode mode;
  ColorSpace space;
  int swatch_n;
  void * swatch;
};
typedef struct coloropts ColorOpts;


struct visualizationopts {
  int compression;
  int depth;
  ColorOpts * colors;
};
typedef struct visualizationopts VisualizationOpts;


struct canvasopts {
  uint32 nheight, nwidth;
  float64 left, width;
  float64 bottom, height;
  float64 coord_Re, coord_Im;
  uint32 escape;
  uint32 secondaryl;
  char ** secondary;
  VisualizationOpts visuals;
};
typedef struct canvasopts CanvasOpts;



#endif /* OPTIONS_STRUCT_H */

