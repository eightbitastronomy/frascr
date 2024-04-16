/****************************************************************************/
/* mand_v2: main for FRASCR application                                     */
/*   Sets up option structures, then calls option library for processing    */
/*   of commandline and config-file settings, then calls execution and      */
/*   finishing functions, then cleans up and exits.                         */
/*   Execution of algorithms is provided by the "lib_exec" shared object.   */
/*   Preparation and output to image/text files provided by the "lib_fin"   */
/*   shared object. Each finisher library must be compatible at a basic     */
/*   level with the execution library. This is checked by passing the       */
/*   VALIDATE function to the EXECUTE function. VALIDATE will only verify   */
/*   that a workable number of data arrays and open file pointers           */
/*   are present.                                                           */
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


#include <stdlib.h>
#include <stdio.h>
#include "debug.h"
#include "utils.h"
#include "options.h"
#include "libopen.h"



int main(int argc, char ** argv) {

  int_f i,j;
  int retbuf;
  DParam debug;
  CanvasOpts palette;
  CoreOpts general;
  Datum ** canv = NULL;

  debug.mask = D0;
  debug.out = stderr; 
  debug.outs = NULL;
  general.execs = NULL;
  general.execute = NULL;
  general.lib_exec = NULL;
  general.finish = NULL;
  general.lib_fin = NULL;
  general.fins = NULL;
  general.validate = NULL;
  general.outs = NULL;
  palette.bottom = 0.0;
  palette.escape = 100;
  palette.nheight = 100;
  palette.height = 1.0;
  palette.nwidth = 100;
  palette.width = 1.0;
  palette.left = 0.0;
  palette.coord_Re = 0.0;
  palette.coord_Im = 0.0;
  palette.colors.swatch = NULL;

  /* cmdline arg processing */
  if (argc > 1) {

    retbuf = process_options(&general, &palette, &debug, argc, argv);

    if (retbuf) {
      switch (retbuf) {
      case OPT_BAD_CALL:
	DEBUG(&debug, D0, "frascr::main: option processing encountered error: coder was careless with function call\n", retbuf);
	break;
      case OPT_BAD_OPTION:
	DEBUG(&debug, D0, "frascr::main: option processing encountered error: invalid command-line option used\n", retbuf);
	break;
      case OPT_TOO_FEW:
	DEBUG(&debug, D0, "frascr::main: option processing encountered error: too few command-line arguments not associated with switches\n", retbuf);
	break;
      default:
	DEBUG(&debug, D0, "frascr::main: option processing encountered error %d: unknown\n", retbuf);
	break;
      }
      HELP_FOR_OPTIONS(debug.out);
      return EXIT_SUCCESS;
    } else {
      DEBUG(&debug, DS, "frascr::main: Debug mask %d\nfrascr::main: Debug out %s\
                         \nfrascr::main: library file %s\
                         \nfrascr::main: output file %s\nfrascr::main: nheight %d\
                         \nfrascr::main: nwidth %d\nfrascr::main: left %f\
                         \nfrascr::main: width %f\nfrascr::main: bottom %f\
                         \nfrascr::main: coord_Re %f\nfrascr::main: coord_Im %f\
                         \nfrascr::main: escape %d\n",
	    debug.mask, debug.outs,
	    general.execs,
	    general.fins, palette.nheight,
	    palette.nwidth, palette.left,
	    palette.width, palette.bottom,
	    palette.coord_Re, palette.coord_Im,
	    palette.escape);
    }
    DEBUGFLUSH(&debug);
  }
  else {
    DEBUG(&debug, D2, "frascr::main: no command-line arguments.\n");
    DEBUG(&debug, D0, "No command-line arguments given. Use -h or --help for usage info.\n");
  }

  DEBUG(&debug, DS, "frascr::main: sizeof unsigned short short_f: %d\
                     \nfrascr::main: sizeof unsigned int int_f: %d\
                     \nfrascr::main: sizeof unsigned int counter_f: %d\
	             \nfrascr::main: sizeof double real_f: %d\n",
	sizeof(short_f), sizeof(int_f), sizeof(counter_f), sizeof(real_f));
  DEBUGFLUSH(&debug);

  /* open library/libraries */
  
  if (general.execs) {
    if ( (retbuf=load_libraries(&general,&debug)) != 0 ) {
      DEBUG(&debug, D0, "frascr::main: unable to open libraries: %d\n", retbuf);
      return EXIT_SUCCESS;
    }
  } else {
    /* nothing to do. just walk away. */
    DEBUG(&debug, D2, "frascr::main: nothing to do. Exiting...\n");
    return EXIT_SUCCESS;
  }

  /* Call EXECUTE */
  
  DEBUG(&debug, D1, "frascr::main: executing library algorithm\n");
  retbuf = (*(general.execute))(&palette,
				general.finish,
				general.validate,
				general.outs,
				general.outl);
  if (retbuf != 0) 
    DEBUG(&debug, D0, "frascr::main: error in library executing algorithm: %d\n", retbuf);

  /* clean up & exit */

  DEBUG(&debug, D1, "frascr::main: Closing libraries.\n");
  if (general.execute) {
    close_libraries(&general, &debug);
  }
  DEBUG(&debug, D1, "frascr::main: Cleaning up core.\n");
  options_core_cleanup(&general);
  DEBUG(&debug, D1, "frascr::main: Cleaning up canvas.\n");
  options_canvas_cleanup(&palette);

  DEBUG(&debug, D1, "frascr::main: Cleaning up debug and exiting.\n");
  DEBUGFLUSH(&debug);

  DEBUGCLEANUP(&debug);

  return EXIT_SUCCESS;
  
};
