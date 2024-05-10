/****************************************************************************/
/* options.c: commandline and config file reading for FRASCR application    */
/*   To be built with cmake: options.h.in -> options.h.                     */
/*   Configuration file is json format.                                     */
/*   Json functionality provided by json-c library.                         */
/*   Command-line parsing done with standard library get_opt type calls.    */
/*   Last updated: 2024-04-09                                               */
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


static inline void read_chrs(json_object * obj, int n, void * swatch) {
  int i;
  json_object * item;
  json_object * field;
  for (i=0; i<n; i++) {
    item = json_object_array_get_idx(obj, i);
    field = json_object_object_get(item, "caxisa");
    ((BaseC *)swatch)[i].rgba.r = (unsigned char)json_object_get_int(field);
    field = json_object_object_get(item, "caxisb");
    ((BaseC *)swatch)[i].rgba.g = (unsigned char)json_object_get_int(field);
    field = json_object_object_get(item, "caxisc");
    ((BaseC *)swatch)[i].rgba.b = (unsigned char)json_object_get_int(field);
  }
}


int file_reader(CoreOpts * core,
		CanvasOpts * canv,
		DParam * debug,
		char * conff)
{
  json_object * root;
  json_object * major, * minor, * sub;
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
  minor = json_object_object_get(major, "file");
  CHECK(root,minor);
  filel = json_object_array_length(minor);
  core->outs = malloc(sizeof(char *)*filel);
  core->outl = filel;
  for (i=0; i<filel; i++) {
    sub = json_object_array_get_idx(minor, i);
    if (json_object_get_type(sub) == json_type_null) {
      free(core->outs);
      json_object_put(root);
      return OPT_CONF_FILES;
    }
    core->outs[i] = strndup(json_object_get_string(sub), 255);
  }

  /* canvas options */

  major = json_object_object_get(root, "canvas");
  if (json_object_get_type(major) == json_type_null) {
     json_object_put(root);
    if (core->outs)
      free(core->outs);
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
  minor = json_object_object_get(major, "compression");
  canv->compression = (uint32)json_object_get_int(minor);

  /* secondary canvas information: will be passed to execute fctn, which must know how to use it */
  /* secondary is optional and might not be present */
  
  minor = json_object_object_get(major, "secondary");
  if (json_object_get_type(minor) != json_type_null) {
    filel = json_object_array_length(minor);
    canv->secondary = malloc(filel*sizeof(char *));
    if (canv->secondary == NULL) {
      json_object_put(root);
      if (core->outs)
	free(core->outs);
      return OPT_CONF_CANV;
    }
    for (i=0; i<filel; i++) {
      sub = json_object_array_get_idx(minor, i);
      canv->secondary[i] = strndup(json_object_get_string(sub),255);
    }
    canv->secondaryl = filel;
  } 

  /* colorization options */

  major = json_object_object_get(root, "color");
  if (json_object_get_type(major) == json_type_null) {
    json_object_put(root);
    if (core->outs)
      free(core->outs);
    return OPT_CONF_COLOR;
  }
  minor = json_object_object_get(major, "space");
  canv->colors.space = space_to_space(json_object_get_string(minor));
  if (canv->colors.space != MONO) {
    minor = json_object_object_get(major, "algorithm");
    sub = json_object_object_get(minor, "type");
    canv->colors.mode = mode_to_mode(json_object_get_string(sub));
    sub = json_object_object_get(minor, "n");
    canv->colors.swatch_n = json_object_get_int(sub);
    minor = json_object_object_get(major, "swatches");
    filel = json_object_array_length(minor);
    if (filel != canv->colors.swatch_n) {
      json_object_put(root);
      if (core->outs)
	free(core->outs);
      return OPT_CONF_N_MISMATCH;
    }
    switch (canv->colors.space) {
    case LCH:
      canv->colors.swatch = malloc(sizeof(BaseI *)*filel);
      read_ints(minor, filel, canv->colors.swatch);
      break;
    case CIELUV:
      canv->colors.swatch = malloc(sizeof(BaseD *)*filel);
      read_dbls(minor, filel, canv->colors.swatch);
      break;
    case CIELAB:
      canv->colors.swatch = malloc(sizeof(BaseD *)*filel);
      read_dbls(minor, filel, canv->colors.swatch);
      break;
    case CIEXYZ:
      canv->colors.swatch = malloc(sizeof(BaseD *)*filel);
      read_dbls(minor, filel, canv->colors.swatch);
      break;
    case SRGB:
      canv->colors.swatch = malloc(sizeof(BaseC *)*filel);
      read_chrs(minor, filel, canv->colors.swatch);
      break;
    default:
      json_object_put(root);
      if (core->outs)
	free(core->outs);
      return OPT_CONF_CLR_SPACE;
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
      {"algorithm", required_argument, 0, 'a'},
      {"bottom", required_argument, 0, 'b'},
      {"escape", required_argument, 0, 'e'},
      {"file", required_argument, 0, 'f'},
      {"help", no_argument, 0, 'h'},
      {"pixelheight", required_argument, 0, 'i'},
      {"realheight", required_argument, 0, 'I'},
      {"left", required_argument, 0, 'l'},
      {"output", required_argument, 0, 'o'},
      {"verbose", no_argument, 0, 'v'},
      {"pixelwidth", required_argument, 0, 'j'},
      {"realwidth", required_argument, 0, 'J'},
      {"offsetre", required_argument, 0, 'x'},
      {"offsetim", required_argument, 0, 'y'},
      {0, 0, 0, 0}
    };

    option_index = 0;

    ret = getopt_long(num,
		      args,
		      "a:b:e:f:hi:I:j:J:l:o:vw:W:x:y:",
		      long_options,
		      &option_index);

    if (ret < 0)
      break;

    switch (ret)
      {
      case 'a':
	if (optarg)
	  core->execs = strndup(optarg, 255);
	break;
      case 'b':
	if (optarg)
	  canv->bottom = atof(optarg);
	break;
      case 'e':
	if (optarg)
	  canv->escape = atoi(optarg);
	break;
      case 'f':
	if (optarg) {
	  files = strndup(optarg, 255);
	  fileflag = 1;
	}
	break;
      case 'h':
	HELP_FOR_OPTIONS(debug->out);
	return 0;
      case 'i':
	if (optarg)
	  canv->nheight = atoi(optarg);
	break;
      case 'I':
	if (optarg)
	  canv->height = atof(optarg);
	break;
      case 'j':
	if (optarg)
	  canv->nwidth = atoi(optarg);
	break;
      case 'J':
	if (optarg)
	  canv->width = atof(optarg);
	break;
      case 'l':
	if (optarg)
	  canv->left = atof(optarg);
	break;
      case 'o':
	if (optarg)
	  core->fins = strndup(optarg, 255);
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
  canv->colors.swatch = NULL;
  canv->secondary = NULL;
  canv->secondaryl = -1;
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



void options_canvas_cleanup(CanvasOpts * canv)
{
  int i;
  if (canv->colors.swatch) {
    switch(canv->colors.space) {
    case LCH:
      free((BaseI *)(canv->colors.swatch));
      break;
    case CIELUV:
      free((BaseD *)(canv->colors.swatch));
      break;
    case CIELAB:
      free((BaseD *)(canv->colors.swatch));
      break;
    case CIEXYZ:
      free((BaseD *)(canv->colors.swatch));
      break;
    case SRGB:
      free((BaseC *)(canv->colors.swatch));
      break;
    default:
      free((BaseI *)(canv->colors.swatch));
      break;
    }
    canv->colors.swatch = NULL;
  }
  if (canv->secondary) {
    while (canv->secondaryl > 0) {
      free(canv->secondary[--(canv->secondaryl)]);
    }
    free(canv->secondary);
    canv->secondary = NULL;
  }
  return;
}
