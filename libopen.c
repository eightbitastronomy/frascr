/****************************************************************************/
/* libopen.c: dynamic library handling for FRASCR application               */
/*   Functions for opening, loading, and cleaning up shared objects /       */
/*   libraries.                                                             */
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


#include "libopen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>



static inline void * open_shared_ob(char * libs, DParam * debug)
{
  char * strerror = NULL;
  void * libptr = NULL;
  libptr = dlopen(libs, RTLD_LAZY);
  strerror = dlerror();
  if ( strerror ) {
    DEBUG(debug, D0, "libopen::open_shared_ob: library import error: %s.\n", strerror );
    libptr = NULL;
  }
  return libptr;
}


static inline int close_shared_ob(void ** libptr, DParam * debug)
{
  if (*libptr) {
    dlclose(*libptr);
    *libptr = NULL;
  }
  return 0;
}





int load_libraries(CoreOpts * opts, DParam * debug) 
{
  char * strerror = NULL;
  void * altlib = NULL;

  /* exec_alg must be present, so this part isn't optional */
  if ( opts->execs == NULL )
    return LONULLARG;
  
  opts->lib_exec = open_shared_ob(opts->execs, debug);
  if ( opts->lib_exec == NULL )
    return LOOPENFILEE;
  
  opts->execute = dlsym(opts->lib_exec, LO_EXECUTE);
  strerror = dlerror();
  if ( strerror ) {
    DEBUG(debug, D0, "libopen::open_shared_ob: exec function import error: %s.\n", strerror );
    opts->lib_exec = NULL;
    return LOLOADEXEC;
  }
  
  if ( opts->fins != NULL ) {
    opts->lib_fin = open_shared_ob(opts->fins, debug);
    if ( opts->lib_fin == NULL ) {
      close_shared_ob(&(opts->lib_exec), debug);
      return LOOPENFILEF;
    }
    altlib = opts->lib_fin;
  } else {
    altlib = opts->lib_exec;
  }
      
  opts->finish = dlsym(altlib, LO_FINISH);
  strerror = dlerror();
  if ( strerror ) {
    DEBUG(debug, D0, "libopen::open_shared_ob: fin function import error: %s.\n", strerror );
    opts->finish = NULL;
  }
  opts->validate = dlsym(altlib, LO_VALIDATE);
  if ( strerror ) {
    DEBUG(debug, D0, "libopen::open_shared_ob: function import error: %s.\n", strerror );
    opts->validate = NULL;
  }
  if ( (opts->finish==NULL) || (opts->validate==NULL) ) {
    close_shared_ob(&(opts->lib_exec), debug);
    close_shared_ob(&(opts->lib_fin), debug);
    return LOLOADFINS;
  }
  
  return 0;
}


void close_libraries(CoreOpts * opts, DParam * debug)
{
  close_shared_ob(&(opts->lib_exec), debug);
  close_shared_ob(&(opts->lib_fin), debug);
}
