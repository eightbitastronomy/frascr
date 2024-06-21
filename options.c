/****************************************************************************/
/* options.c: commandline and config file reading for FRASCR application    */
/*   To be built with cmake: options.h.in -> options.h.                     */
/*   Configuration file is json format.                                     */
/*   Json functionality provided by json-c library.                         */
/*   Command-line parsing done with standard library get_opt type calls.    */
/*   Last updated: 2024 May                                                 */
/****************************************************************************/
/*  NEEDS: command-line switches must be updated to reflect changes to      */
/*   config files. Possibly adopt only the new "visualization" switches,    */
/*   while for the rest taking a default set of values (for swatches, etc)  */
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


#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <stdlib.h>
#include <json-c/json.h>
#include "options.h"
#include "color.h"


#define CHECK(rt,obj) {if (json_object_get_type(obj) == json_type_null) { json_object_put(rt); return OPT_CONF_JSON;}}


static inline void location_concat(const char * loc, const char * file, char ** out)
{
  int lenL, lenF;
  char * buffer = NULL;

  lenL = strlen(loc);
  lenF = strlen(file);

  if (loc[lenL-1] == '/') {
    buffer = malloc(sizeof(char)*(lenL+lenF));
    strncpy(buffer, loc, lenL);
    strncpy((char*)(buffer+lenL), file, lenF);
  } else {
    buffer = malloc(sizeof(char)*(lenL+lenF+1));
    strncpy(buffer, loc, lenL);
    buffer[lenL] = '/';
    strncpy((char*)(buffer+lenL+1), file, lenF);
  }
  *out = buffer;
}


static inline void read_ints(json_object * obj, int n, void * swatch) {
  int i;
  json_object * item;
  json_object * field;
  for (i=0; i<n; i++) {
    item = json_object_array_get_idx(obj, i);
    field = json_object_object_get(item, "caxisa");
    ((BaseI *)swatch)[i].a = json_object_get_int(field);
    field = json_object_object_get(item, "caxisb");
    ((BaseI *)swatch)[i].b = json_object_get_int(field);
    field = json_object_object_get(item, "caxisc");
    ((BaseI *)swatch)[i].c = json_object_get_int(field);
  }
}


static inline void read_dbls(json_object * obj, int n, void * swatch) {
  int i;
  json_object * item;
  json_object * field;
  for (i=0; i<n; i++) {
    item = json_object_array_get_idx(obj, i);
    field = json_object_object_get(item, "caxisa");
    ((BaseD *)swatch)[i].a = json_object_get_double(field);
    field = json_object_object_get(item, "caxisb");
    ((BaseD *)swatch)[i].b = json_object_get_double(field);
    field = json_object_object_get(item, "caxisc");
    ((BaseD *)swatch)[i].c = json_object_get_double(field);
  }
}


static inline void read_chrs8(json_object * obj, int n, void * swatch) {
  int i;
  json_object * item;
  json_object * field;
  for (i=0; i<n; i++) {
    item = json_object_array_get_idx(obj, i);
    field = json_object_object_get(item, "caxisa");
    ((BaseC8 *)swatch)[i].rgba.r = (unsigned char)json_object_get_int(field);
    field = json_object_object_get(item, "caxisb");
    ((BaseC8 *)swatch)[i].rgba.g = (unsigned char)json_object_get_int(field);
    field = json_object_object_get(item, "caxisc");
    ((BaseC8 *)swatch)[i].rgba.b = (unsigned char)json_object_get_int(field);
  }
}


static inline void read_chrs16(json_object * obj, int n, void * swatch) {
  int i;
  json_object * item;
  json_object * field;
  for (i=0; i<n; i++) {
    item = json_object_array_get_idx(obj, i);
    field = json_object_object_get(item, "caxisa");
    ((BaseC16 *)swatch)[i].rgba.r = (uint16)json_object_get_int(field);
    field = json_object_object_get(item, "caxisb");
    ((BaseC16 *)swatch)[i].rgba.g = (uint16)json_object_get_int(field);
    field = json_object_object_get(item, "caxisc");
    ((BaseC16 *)swatch)[i].rgba.b = (uint16)json_object_get_int(field);
  }
}


static inline int extend_the_str_list(char *** target, int * len) {
  int i, newlen;
  newlen = 2*(*len);
  char ** buffer = malloc(newlen*sizeof(char *));
  if (buffer == NULL)
    return -1;
  for (i=0; i<(*len); i++) {
    buffer[i] = strdup((*target)[i]);
    if (buffer[i] == NULL) {
      for (i; i>0; --i)
	free(buffer[i]);
      return -1;
    }
  }
  for (i=0; i<(*len); i++) {
    free((*target)[i]);
  }
  free(*target);
  *target = buffer;
  *len = newlen;
  return 0;
}


static inline void trim_the_str_list(CanvasOpts * canv, int len) {
  char ** buffer;
  int i;
  buffer = malloc(len*sizeof(char *));
  for (i=0; i<len; i++) {
    buffer[i] = canv->secondary[i];
  }
  free(canv->secondary);
  canv->secondary = buffer;
  return;
}


static inline int parse_secondary_args_from_cmdline(CanvasOpts * canv, char * input) {
  int len = strlen(input);
  int ncurrent = len/2 + 1; //this should be large enough even for the worst case, "1 2 3 4" etc
  int i_input = 0;
  int j_buffer = -1;
  int copyflag = 0;
  int start, stop;
  canv->secondary = malloc(ncurrent*sizeof(char *));
  do {
    if (input[i_input] != ' ') {
      if (copyflag == 0) {
	copyflag = 1;
	j_buffer++;
	if (j_buffer == ncurrent) {
	  if (extend_the_str_list(&(canv->secondary), &ncurrent)) {
	    for (j_buffer; j_buffer>=0; --j_buffer) {
	      free(canv->secondary[j_buffer]);
	    }
	    free(canv->secondary);
	    canv->secondary == NULL;
	    return -1; 
	  }
	}
	start = i_input;
	stop = i_input;
      } else {
	stop = i_input;
      } 
    } else {
      if (copyflag != 0) {
	copyflag = 0;
	canv->secondary[j_buffer] = strndup(&input[start], stop-start+1);
      }
    }
    i_input++;
  } while (i_input < len);
  if (copyflag != 0) {
    canv->secondary[j_buffer] = strndup(&input[start], stop-start+1);
  }
  canv->secondaryl = j_buffer + 1;
  trim_the_str_list(canv, j_buffer+1);
  return 0;
}


int file_reader(CoreOpts * core,
		CanvasOpts * canv,
		DParam * debug,
		char * conff)
{
  json_object * root;
  json_object * major, * minor, * sub, * batboy, * intern;
  char * loc;
  int filel;
  int i;

  root = json_object_from_file(conff);

  /* debug options */

  major = json_object_object_get(root, "debug");
  CHECK(root,major);
  minor = json_object_object_get(major, "verbose");
  CHECK(root,minor);
  debug->mask = json_object_get_int(minor);
  
  /* core options */

  major = json_object_object_get(root, "core");
  CHECK(root,major);
  minor = json_object_object_get(major, "location");
  CHECK(root,minor);
  loc = strndup(json_object_get_string(minor), 255);
  minor = json_object_object_get(major, "algorithm");
  CHECK(root,minor);
  location_concat(loc, json_object_get_string(minor), &(core->execs));
  minor = json_object_object_get(major, "output");
  CHECK(root,minor);
  location_concat(loc, json_object_get_string(minor), &(core->fins));
  loc = NULL;
  free(loc);
  minor = json_object_object_get(major, "file");
  CHECK(root,minor);
  filel = json_object_array_length(minor);
  core->outs = malloc(sizeof(char *)*filel);
  if (core->outs == NULL) {
    free(core->execs);
    free(core->fins);
    json_object_put(root);
  }
  core->outl = filel;
  for (i=0; i<filel; i++) {
    sub = json_object_array_get_idx(minor, i);
    if (json_object_get_type(sub) == json_type_null) {
      free(core->outs);
      free(core->execs);
      free(core->fins);
      json_object_put(root);
      return OPT_CONF_FILES;
    }
    core->outs[i] = strndup(json_object_get_string(sub), 255);
  }

  /* canvas options */

  major = json_object_object_get(root, "canvas");
  if (json_object_get_type(major) == json_type_null) {
     json_object_put(root);
     free(core->outs);
     free(core->execs);
     free(core->fins);
     return OPT_CONF_CANV;
  }
  minor = json_object_object_get(major, "bottom");
  canv->bottom = json_object_get_double(minor);
  minor = json_object_object_get(major, "left");
  canv->left = json_object_get_double(minor);
  minor = json_object_object_get(major, "realheight");
  canv->height = json_object_get_double(minor);
  minor = json_object_object_get(major, "realwidth");
  canv->width = json_object_get_double(minor);
  minor = json_object_object_get(major, "pixelheight");
  canv->nheight = json_object_get_int(minor);
  minor = json_object_object_get(major, "pixelwidth");
  canv->nwidth = json_object_get_int(minor);
  minor = json_object_object_get(major, "offset_Re");
  canv->coord_Re = json_object_get_double(minor);
  minor = json_object_object_get(major, "offset_Im");
  canv->coord_Im = json_object_get_double(minor);
  minor = json_object_object_get(major, "escape");
  canv->escape = (uint32)json_object_get_int(minor);

  /* secondary canvas information: will be passed to execute fctn, which must know how to use it */
  /* secondary is optional and might not be present */
  
  minor = json_object_object_get(major, "secondary");
  if (json_object_get_type(minor) != json_type_null) {
    filel = json_object_array_length(minor);
    canv->secondary = malloc(filel*sizeof(char *));
    if (canv->secondary == NULL) {
      json_object_put(root);
      free(core->outs);
      free(core->execs);
      free(core->fins);
      return OPT_CONF_CANV;
    }
    for (i=0; i<filel; i++) {
      sub = json_object_array_get_idx(minor, i);
      canv->secondary[i] = strndup(json_object_get_string(sub),255);
    }
    canv->secondaryl = filel;
  }

  /* visualization options */

  major = json_object_object_get(root, "visualization");
  if (json_object_get_type(major) == json_type_null) {
    free(core->outs);
    free(core->execs);
    free(core->fins);
    if (canv->secondary) {
      while (canv->secondaryl > 0) {
	free(canv->secondary[--(canv->secondaryl)]);
      }
      free(canv->secondary);
    }
    json_object_put(root);
    return OPT_BAD_OPTION;
  }
  minor = json_object_object_get(major, "compression");
  canv->visuals.compression = json_object_get_int(minor);
  minor = json_object_object_get(major, "channeldepth");
  canv->visuals.depth = json_object_get_int(minor);

  /* colorization options -- optional */

  minor = json_object_object_get(major, "colorization");
  if (json_object_get_type(minor) != json_type_null) {
    canv->visuals.colors = malloc(sizeof(ColorOpts));
    if (canv->visuals.colors == NULL) {
      free(core->outs);
      free(core->execs);
      free(core->fins);
      if (canv->secondary) {
	while (canv->secondaryl > 0) {
	  free(canv->secondary[--(canv->secondaryl)]);
	}
	free(canv->secondary);
      }
      json_object_put(root);
      return OPT_CONF_MALLOC;
    }
    sub = json_object_object_get(minor, "space");
    canv->visuals.colors->space = space_to_space(json_object_get_string(sub));
    if (canv->visuals.colors->space != MONO) {
      sub = json_object_object_get(minor, "illuminant");
      canv->visuals.colors->reference = illum_to_illum(json_object_get_string(sub));
      if (canv->visuals.colors->reference == UNKNOWN) {
	json_object_put(root);
	free(core->outs);
	free(core->execs);
	free(core->fins);
	if (canv->secondary) {
	  while (canv->secondaryl > 0) {
	    free(canv->secondary[--(canv->secondaryl)]);
	  }
	  free(canv->secondary);
	}
	return OPT_CONF_CLR_REF;
      }
      sub = json_object_object_get(minor, "algorithm");
      batboy = json_object_object_get(sub, "type");
      canv->visuals.colors->mode = mode_to_mode(json_object_get_string(batboy));
      batboy = json_object_object_get(sub, "n");
      canv->visuals.colors->swatch_n = json_object_get_int(batboy);
      sub = json_object_object_get(minor, "swatches");
      filel = json_object_array_length(sub);
      if (filel != canv->visuals.colors->swatch_n) {
	json_object_put(root);
	free(core->outs);
	free(core->execs);
	free(core->fins);
	if (canv->secondary) {
	  while (canv->secondaryl > 0) {
	    free(canv->secondary[--(canv->secondaryl)]);
	  }
	  free(canv->secondary);
	}
	return OPT_CONF_N_MISMATCH;
      }
      switch (canv->visuals.colors->space) {
      case LCH:
	canv->visuals.colors->swatch = malloc(sizeof(BaseI *)*filel);
	read_ints(sub, filel, canv->visuals.colors->swatch);
	break;
      case CIELUV:
	canv->visuals.colors->swatch = malloc(sizeof(BaseD *)*filel);
	read_dbls(sub, filel, canv->visuals.colors->swatch);
	break;
      case CIELAB:
	canv->visuals.colors->swatch = malloc(sizeof(BaseD *)*filel);
	read_dbls(sub, filel, canv->visuals.colors->swatch);
	break;
      case CIEXYZ:
	canv->visuals.colors->swatch = malloc(sizeof(BaseD *)*filel);
	read_dbls(sub, filel, canv->visuals.colors->swatch);
	break;
      case SRGB8:
	canv->visuals.colors->swatch = malloc(sizeof(BaseC8 *)*filel);
	read_chrs8(sub, filel, canv->visuals.colors->swatch);
	break;
      case SRGB16:
	canv->visuals.colors->swatch = malloc(sizeof(BaseC16 *)*filel);
	read_chrs16(sub, filel, canv->visuals.colors->swatch);
	break;
      default:
	json_object_put(root);
	free(core->outs);
	free(core->execs);
	free(core->fins);
	if (canv->secondary) {
	  while (canv->secondaryl > 0) {
	    free(canv->secondary[--(canv->secondaryl)]);
	  }
	  free(canv->secondary);
	}
	return OPT_CONF_CLR_SPACE;
      }
    }
  }

  /* done */

  json_object_put(root);
  return 0;
}





int process_options(CoreOpts * core,
		    CanvasOpts * canv,
		    DParam * debug,
		    int num,
		    char ** args)
{
  int option_index;
  int ret;
  int verbose = 0;
  int num_files, i;
  int fileflag = 0;
  char * files = NULL;

  if ((core==NULL) || (canv==NULL) || (debug==NULL))
    return OPT_BAD_CALL;
  
  while (fileflag == 0) {

    static struct option long_options[] = {
      {"EXECUTE", required_argument, 0, 'E'},
      {"bottom", required_argument, 0, 'b'},
      {"escape", required_argument, 0, 'e'},
      {"file", required_argument, 0, 'f'},
      {"help", no_argument, 0, 'h'},
      {"pixelheight", required_argument, 0, 'm'},
      {"realheight", required_argument, 0, 'i'},
      {"left", required_argument, 0, 'l'},
      {"FINISH", required_argument, 0, 'F'},
      {"verbose", no_argument, 0, 'v'},
      {"pixelwidth", required_argument, 0, 'n'},
      {"realwidth", required_argument, 0, 'j'},
      {"offsetre", required_argument, 0, 'x'},
      {"offsetim", required_argument, 0, 'y'},
      {"secondary", required_argument, 0, 's'},
      {0, 0, 0, 0}
    };

    option_index = 0;

    ret = getopt_long(num,
		      args,
		      "b:e:f:hi:j:l:m:n:s:vx:y:E:F:",
		      long_options,
		      &option_index);

    if (ret < 0)
      break;

    switch (ret)
      {
      case 'b':
	if (optarg)
	  canv->bottom = atof(optarg);
	break;
      case 'e':
	if (optarg)
	  canv->escape = atoi(optarg);
	break;
      case 'E':
	if (optarg)
	  core->execs = strndup(optarg, 255);
	break;
      case 'f':
	if (optarg) {
	  files = strndup(optarg, 255);
	  fileflag = 1;
	}
	break;
      case 'F':
	if (optarg)
	  core->fins = strndup(optarg, 255);
	break;
      case 'h':
	HELP_FOR_OPTIONS(debug->out);
	return 0;
      case 'i':
	if (optarg)
	  canv->height = atof(optarg);
	break;
      case 'j':
	if (optarg)
	  canv->width = atof(optarg);
	break;
      case 'l':
	if (optarg)
	  canv->left = atof(optarg);
	break;
      case 'm':
	if (optarg)
	  canv->nheight = atoi(optarg);
	break;
      case 'n':
	if (optarg)
	  canv->nwidth = atoi(optarg);
	break;
      case 's':
	if (optarg)
	  if (parse_secondary_args_from_cmdline(canv, optarg))
	    return OPT_BAD_OPTION;
	break;
      case 'v':
	verbose++;
	break;
      case 'x':
	if (optarg)
	  canv->coord_Re = atof(optarg);
	break;
      case 'y':
	if (optarg)
	  canv->coord_Im = atof(optarg);
	break;
      default:
	return OPT_BAD_OPTION;
	break;
      } /* switch ret */

  } /* while true */

  /* config file found. Read from there and skip rest of this function. */

  if (fileflag == 1)
    {
      return file_reader(core, canv, debug, files);
    }
  
  /* options were from the command line. Keep going. */
  
  if (verbose > DS)
    debug->mask = DS;
  else
    debug->mask = verbose;

  if (optind == num)
    {
      /* no filename was given, as required */
      return OPT_TOO_FEW;
    }
  num_files = num - optind;
  core->outl = num_files;
  core->outs = malloc(num_files*sizeof(char *));
  i = 0;
  while (optind < num) {
    core->outs[i++] = strndup(args[optind++], 255);
  }

  return 0;

}



static inline void options_visuals_initialize(VisualizationOpts * v) {
  v->colors = NULL;
  v->compression = 1;
  v->depth = 8;
}



void options_canvas_initialize(CanvasOpts * canv) {
  canv->bottom = 0.0;
  canv->escape = 100;
  canv->nheight = 100;
  canv->height = 1.0;
  canv->nwidth = 100;
  canv->width = 1.0;
  canv->left = 0.0;
  canv->coord_Re = 0.0;
  canv->coord_Im = 0.0;
  canv->secondary = NULL;
  canv->secondaryl = -1;
  options_visuals_initialize(&(canv->visuals));
}


void options_core_initialize(CoreOpts * core) {
  core->execs = NULL;
  core->execute = NULL;
  core->lib_exec = NULL;
  core->finish = NULL;
  core->lib_fin = NULL;
  core->fins = NULL;
  core->validate = NULL;
  core->outs = NULL;
}



void options_core_cleanup(CoreOpts * core)
{
  int i;
  
  if (core->execs)
    free(core->execs);
  if (core->fins)
    free(core->fins);
  if (core->outs) {
    for (i=0; i<core->outl; i++)
      if (core->outs[i])
	free(core->outs[i]);
  }
}



static inline void options_visuals_cleanup(VisualizationOpts * v) {
  if (v->colors) {
    if (v->colors->swatch) {
      switch(v->colors->space) {
      case LCH:
	free((BaseI *)(v->colors->swatch));
	break;
      case CIELUV:
	free((BaseD *)(v->colors->swatch));
	break;
      case CIELAB:
	free((BaseD *)(v->colors->swatch));
	break;
      case CIEXYZ:
	free((BaseD *)(v->colors->swatch));
	break;
      case SRGB8:
	free((BaseC8 *)(v->colors->swatch));
	break;
      case SRGB16:
	free((BaseC16 *)(v->colors->swatch));
	break;
      default:
	free((BaseI *)(v->colors->swatch));
	break;
      }
      v->colors->swatch = NULL;
    }
    free(v->colors);
    v->colors = NULL;
  }
}



void options_canvas_cleanup(CanvasOpts * canv)
{
  int i;
  options_visuals_cleanup(&(canv->visuals));
  if (canv->secondary) {
    while (canv->secondaryl > 0) {
      free(canv->secondary[--(canv->secondaryl)]);
    }
    free(canv->secondary);
    canv->secondary = NULL;
  }
  return;
}
