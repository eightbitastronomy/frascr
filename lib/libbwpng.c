/****************************************************************************/
/* Libbwpng.c: printing shared object for the FRASCR application.           */
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


#include "libbwpng.h"
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include <zlib.h>


void FINISH(CanvasOpts * opts,
	    Datum *** dataa,
	    int datal,
	    FILE ** filea,
	    int filel)
{

  int i, j;
  int max, rno;
  Datum ** canvas;
  FILE * output;
  unsigned short ** storage = NULL;
  short_f MAX_VAL = ~(unsigned short)(0);
  short_f storeval;
  png_voidp errorptr = NULL;
  png_structp pngptr = NULL;
  png_infop infoptr = NULL;
  png_text * textptr = NULL;
  const int textfields = 3;
  char texttmp[511];
  int datasize = sizeof(short_f);

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
    png_set_compression_level(pngptr, Z_NO_COMPRESSION); // if compile err, try include zlib.h 
    png_set_IHDR(pngptr, infoptr, opts->nwidth, opts->nheight, 16,
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
    
    /* transpose input data for libpng, and (for now) scale data into black & white */

    storage = malloc(sizeof(short_f *)*opts->nheight);
    for (i=0; i<opts->nheight; i++)
      storage[i] = malloc(sizeof(short_f)*datasize*opts->nwidth);
    png_byte ** rows = malloc(sizeof(png_byte *)*opts->nheight);
    for (i=0; i<opts->nheight; i++) {
      rows[i] = (png_byte *)(storage[i]);
      for (j=0; j<opts->nwidth; j++) {
	storeval = (short_f)((double)MAX_VAL * (double)canvas[j][i].n / (double)opts->escape);
	storage[opts->nheight-i-1][j] = storeval;
      }
    }
    
    png_write_info(pngptr, infoptr);
    png_set_rows(pngptr, infoptr, rows);  
    
    /* Write/Output */
    
    png_write_png(pngptr, infoptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_image(pngptr, rows);
    png_write_end(pngptr, infoptr);
    png_destroy_write_struct(&pngptr, &infoptr);
    
    /* Cleanup */
    
    if (storage) {
      for (i=0; i<opts->nheight; i++)
	if (storage[i]) {
	  free(storage[i]);
	  storage[i] = NULL;
	}
      free(storage);
    }
    if (rows)
      free(rows);
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
