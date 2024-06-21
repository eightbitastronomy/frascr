/****************************************************************************/
/* Libbwpng.c: printing shared object for the FRASCR application.           */
/*   Provides a FINISH function and VALIDATE function.                      */
/*   FINISH outputs black and white png files by converting unsigned        */
/*   integer data into an intensity.                                        */
/*  Last updated: 2024 May                                                  */
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


#include "libbwpng.h"
#include "bitorder.h"
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <zlib.h>


static inline void * byte_n_switch_16(void * destn, const void * source, size_t sz) {
  int i;
  unsigned char * dest = destn;
  const unsigned char * src = source;
  *(dest) = *(src + 1);
  *(dest + 1) = *(src);
  return NULL;
}


static inline uint32 find_max_intensity(Datum ** const dat,
					   const int rows,
					   const int cols)
{
  int i, j;
  uint32 n;
  uint32 max = 0;
  for (i=0; i<rows; i++) {
    for (j=0; j<cols; j++) {
      n = dat[i][j].n;
      if (n > max)
	max = n;
    }
  }

  return max;
}


void FINISH(CanvasOpts * opts,
	    Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel)
{

  int i, j;

  /* Data & control variables */
  int max, rno;
  Datum ** canvas;
  FILE * output;

  /* libpng variables */ 
  png_voidp errorptr = NULL;
  png_structp pngptr = NULL;
  png_infop infoptr = NULL;
  png_text * textptr = NULL;
  const int textfields = 3;
  char texttmp[511];
  png_byte ** rows;

  /* Intensity conversion helpers */
  uint16 MAX_VAL = ~(unsigned short)(0);
  int datasize;
  uint32 intensitymax;
  double storevald;

  /* Intensity-to-libpng helpers */
  void *(*bytecopy)(void *, const void *, size_t);

  max = (datal <= filel ? datal : filel);

  for (rno=0; rno<max; rno++) {

    output = filea[rno];
    canvas = dataa[rno];
    
    pngptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
				     errorptr,
				     NULL, NULL);
    if (!pngptr)
      return;

    infoptr = png_create_info_struct(pngptr);
    if (!infoptr)
      {
	png_destroy_write_struct(&pngptr, (png_infopp)NULL);
	return;
      }

    if (setjmp(png_jmpbuf(pngptr)))
      {
	png_destroy_write_struct(&pngptr, &infoptr);
	return;
      }

    png_init_io(pngptr, output);

    /* only support on/off for compression at the moment */
    if (opts->visuals.compression == 0) {
      png_set_compression_level(pngptr, Z_NO_COMPRESSION);
    } else {
      png_set_compression_level(pngptr, Z_BEST_COMPRESSION); // if compile err, try include zlib.h
    }
    
    png_set_IHDR(pngptr, infoptr, opts->nwidth, opts->nheight, opts->visuals.depth,
		 PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    
    /* set text fields */
    textptr = malloc(sizeof(png_text)*textfields);
    if (textptr) {
      textptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
      textptr[0].key = "Title";
      textptr[0].text = "Frascr output image";
      textptr[0].text_length = strlen(textptr[0].text);
      textptr[0].lang = NULL;
      textptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
      textptr[1].key = "Author";
      textptr[1].text = "eightbitastronomy";
      textptr[1].text_length = strlen(textptr[1].text);
      textptr[1].lang = NULL;
      //fprintf(stderr, "text 1: [key] %s [len] %d [text] %s [len] %d\n",
      //	    textptr[1].key, strlen(textptr[1].key), textptr[1].text, textptr[1].text_length);
      //fprintf(stderr, "setting text 2\n");
      textptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
      textptr[2].key = "Description";
      //fprintf(stderr,"about to sprintf\n");
      //sprintf(texttmp, "Size %d x %d. Color type %s. Re domain: [ %f , %f ]. Im domain: [ %f , %f ]. Offset: %f + i %f. Escape: %d",
      //opts->nwidth, opts->nheight, "Gray", opts->left, opts->left + opts->width,
      //opts->bottom, opts->bottom + opts->height, opts->coord_Re, opts->coord_Im,
      //opts->escape);
      //1, 2, "Gray", 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10);
      //textptr[2].text_length = strlen(texttmp);
      //strncpy(textptr[2].text, texttmp, textptr[2].text_length);
      textptr[2].text = "Some shit";
      textptr[2].text_length = strlen(textptr[2].text);
      textptr[2].lang = NULL;
      //fprintf(stderr, "text 2: [key] %s [len] %d [text] %s [len] %d\n",
      //	    textptr[2].key, strlen(textptr[2].key), textptr[2].text, textptr[2].text_length);
      //fflush(stderr);
      //fprintf(stderr, "png_set_text\n");
      png_set_text(pngptr, infoptr, textptr, textfields);
    }

    /* find maximum intensity */

    intensitymax = find_max_intensity(canvas, opts->nwidth, opts->nheight);
    
    /* transpose input data for libpng, and (for now) scale data into black & white */

    rows = malloc(sizeof(png_byte *)*opts->nheight);
    if (opts->visuals.depth == 8) {
      bytecopy = memcpy;
      datasize = sizeof(uint8);\
    } else {
      datasize = sizeof(uint16);
      switch (O32_HOST_ORDER) {
      case O32_LITTLE_ENDIAN:
	bytecopy = byte_n_switch_16;
	break;
      case O32_BIG_ENDIAN:
	bytecopy = memcpy;
	break;
      default:
	return; //I'm not handling PDP or HONEYWELL at the moment
      }

    }
    for (i=0; i<opts->nheight; i++)
      rows[i] = malloc(sizeof(png_byte)*opts->nwidth*datasize);

    for (i=0; i<opts->nheight; i++) {
      for (j=0; j<opts->nwidth; j++) {
	storevald = intensitymax == 0 ? 0.0 : (double)(canvas[j][i].n) / (double)(intensitymax);
	bytecopy(&(rows[opts->nheight-i-1][datasize*j]), (void *)(&storevald), datasize);
      }
    }
    
    png_write_info(pngptr, infoptr);
    png_set_rows(pngptr, infoptr, rows);  
    
    /* Write/Output */
    
    png_write_png(pngptr, infoptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_image(pngptr, rows);
    png_write_end(pngptr, infoptr);
    
    /* Cleanup */

    png_destroy_write_struct(&pngptr, &infoptr);
    if (rows) {
      for (i=0; i<opts->nheight; i++)
	if (rows[i]) {
	  free(rows[i]);
	}
      free(rows);
    }
    if (textptr)
      free(textptr);
    
  } // for rno

  return;
}


int VALIDATE(Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel)
{
  int i;

  if ((dataa==NULL) || (filea==NULL))
    return PW_BAD_CALL;

  if ((datal < 1) || (filel < 1))
    return PW_BAD_CALL;

  for (i=0; i<datal; i++) {
    if (dataa[i] == NULL)
      return PW_BAD_CALL;
  }

  for (i=0; i<filel; i++) {
    if (filea[i] == NULL)
      return PW_BAD_CALL;
    if (ftell(filea[0]) < 0)
      return PW_BAD_CALL;
  }

  return 0;
}
