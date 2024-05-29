/****************************************************************************/
/* Libcolorpng.c: printing shared object for the FRASCR application         */
/*   Provides a FINISH function and VALIDATE function.                      */
/*   FINISH (currently) outputs sRGB png files by mapping unsigned int data */
/*   using a user-provided color palette (LCH/CIELAB, linear interpolation  */
/*   provided via color library shared object.                              */
/*  Last updated: 2024 May                                                  */
/****************************************************************************/
/* NEEDS: o  16-bit channel implementation gives wrong colors               */
/*           - I've unit-tested the color.c library calls as well as the    */
/*             bytecopy. 99.9% sure the problem lies in how I call libpng   */
/*        o  possibly, to handle sampling palettes, since I started them    */
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


#include "options.h"
#include "utils.h"
#include "color.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <png.h>
#include <zlib.h>


#define MAX_SHORT      ((unsigned short)~(0))
#define MAX_INT        ((unsigned int)~(0)) 


static inline void bytecopy(void * destn, void * const source, const int sz) {
  int i;
  unsigned char * dest = destn;
  unsigned char * src = source;
  for (i=0; i<sz; i++) {
    *(dest + i) = *(src + i);
  }
  /*for (i=0; i<sz; i++) 
    if (i % 2 == 0)
    *(dest + i/2) = *(src + i);*/
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
  int i, j, k, ret;
  int max, rno;
  uint16 max_uint16 = MAX_SHORT; /* see below */
  uint32 max_uint32 = MAX_INT;
  Datum ** canvas = dataa[0];
  FILE * output = filea[0];
  uint32 intensitymax;
  uint16 storeval;
  double storevald;
  png_voidp errorptr = NULL;
  png_structp pngptr = NULL;
  png_infop infoptr = NULL;
  png_text * textptr = NULL;
  png_color_8 sig_bit;
  const int textfields = 3;
  char texttmp[511];
  int datasize;
  Wheel * colors = NULL;
  BaseI basecolor;
  BaseC8 converted;
  void * swatchI;
  BaseD swatchluv;
  BaseD swatchxyz;
  png_byte ** rows;
  void * swatchrgb;
  int (*convertptr)(void *, BaseD *, unsigned short) = NULL;

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
    if (!infoptr) {
      png_destroy_write_struct(&pngptr, (png_infopp)NULL);
      return;
    }

    if (setjmp(png_jmpbuf(pngptr))) {
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
		 PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /*sig_bit.red = opts->visuals.depth;
    sig_bit.green = opts->visuals.depth;
    sig_bit.blue = opts->visuals.depth;
    sig_bit.alpha = opts->visuals.depth;
    png_set_sBIT(pngptr, infoptr, &sig_bit);*/
    
    /* set text fields -- this section isn't working the way I want it to */
    
    textptr = malloc(sizeof(png_text)*textfields);
    if (textptr) {
      textptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
      textptr[0].key = "Title";
      textptr[0].text = "Frascr output image with pixelart palette";
      textptr[0].text_length = strlen(textptr[0].text);
      textptr[0].lang = NULL;
      textptr[1].compression = PNG_TEXT_COMPRESSION_NONE;
      textptr[1].key = "Author";
      textptr[1].text = "eightbitastronomy";
      textptr[1].text_length = strlen(textptr[1].text);
      textptr[1].lang = NULL;
      textptr[2].compression = PNG_TEXT_COMPRESSION_NONE;
      textptr[2].key = "Description";
      textptr[2].text = "Some shit";
      textptr[2].text_length = strlen(textptr[2].text);
      textptr[2].lang = NULL;
      png_set_text(pngptr, infoptr, textptr, textfields);
    }
    
    /* Create a LCH color wheel. */

    if (ret = initialize_wheel(&colors,
			       opts->visuals.colors->swatch_n,
			       opts->visuals.colors->space,
			       opts->visuals.colors->mode,
			       opts->visuals.colors->swatch)) {
      png_destroy_write_struct(&pngptr, &infoptr);
      return;
    }

    /* find maximum intensity */

    intensitymax = find_max_intensity(canvas, opts->nwidth, opts->nheight);
    
    /* transpose input data for libpng while converting from black & white to color */

    rows = malloc(sizeof(png_byte *)*opts->nheight);
    if (opts->visuals.depth == 8) {
      convertptr = convert_xyz_to_sRGB8;
      datasize = sizeof(BaseC8);\
      swatchrgb = malloc(sizeof(datasize));
      for (i=0; i<opts->nheight; i++)
	rows[i] = malloc(sizeof(png_byte)*opts->nwidth*datasize);
    } else {
      convertptr = convert_xyz_to_sRGB16;
      datasize = sizeof(BaseC16);
      swatchrgb = malloc(sizeof(datasize));
      for (i=0; i<opts->nheight; i++) 
	rows[i] = malloc(sizeof(png_byte)*opts->nwidth*datasize);
    }

    /* Here, something goes wrong. 29 May 2024: After posting the issue on pnggroup's libpng github page,
       I'm told that somehow my data gets garbled or something in the 16-bit output. They quickly ruled
       out a bug in their library. I find that the converted data is okay. So somehow, as I already
       suspected, the problem must lie either in how I write to the rows array, or in how libpng reads
       the rows array. However, attempts to modify how libpng reads rows have not proved helpful.
     */

    for (i=0; i<opts->nheight; i++) {
      for (j=0; j<opts->nwidth; j++) {
	storevald = intensitymax == 0 ? 0.0 : (double)(canvas[j][i].n) / (double)(intensitymax);
	linear_by_intensity_norm(colors, storevald, &swatchI);
	convert_lch_to_lab(&swatchluv, (BaseI *)swatchI);
	convert_lab_to_xyz(&swatchxyz, &swatchluv);
	convertptr(swatchrgb, &swatchxyz, max_uint16);
	//Next line was the original, before BaseC8 & BaseC16 required swatchrgb to be void * ptr
	//storage[opts->nheight-i-1][j] = swatchrgb.word; 
	//Bytecopy was written so that I could manipulate the bytes of data to help debug.
	//bytecopy(&(rows[opts->nheight-i-1][datasize*j]), swatchrgb, datasize); //use 8 for last arg for upper-8
	/* Debug printing. Fucking hell. 
	  if (opts->visuals.depth == 8) {
	  fprintf(stderr,"%d %d:  (%d)   %d %d %d %d   ..   %04x %04x %04x %04x\n",opts->nheight-i-1,j,storevald,((BaseC8 *)swatchrgb)->rgba.r,((BaseC8 *)swatchrgb)->rgba.g,((BaseC8 *)swatchrgb)->rgba.b,((BaseC8 *)swatchrgb)->rgba.alpha,((BaseC8 *)swatchrgb)->rgba.r,((BaseC8 *)swatchrgb)->rgba.g,((BaseC8 *)swatchrgb)->rgba.b,((BaseC8 *)swatchrgb)->rgba.alpha);
	} else {
	  fprintf(stderr,"%d %d:  (%d)   %d %d %d %d   ..   %04x %04x %04x %04x\n",opts->nheight-i-1,j,storevald,((BaseC16 *)swatchrgb)->rgba.r,((BaseC16 *)swatchrgb)->rgba.g,((BaseC16 *)swatchrgb)->rgba.b,((BaseC16 *)swatchrgb)->rgba.alpha,((BaseC16 *)swatchrgb)->rgba.r,((BaseC16 *)swatchrgb)->rgba.g,((BaseC16 *)swatchrgb)->rgba.b,((BaseC16 *)swatchrgb)->rgba.alpha);
	}
	*/
	memcpy(&(rows[opts->nheight-i-1][datasize*j]), swatchrgb, datasize); 
	free((BaseI *)swatchI);
      }
    }

    png_write_info(pngptr, infoptr);
    png_set_rows(pngptr, infoptr, rows);  

    /* Write/Output */
    
    png_write_png(pngptr, infoptr, PNG_TRANSFORM_IDENTITY, NULL);
    png_write_image(pngptr, rows);
    png_write_end(pngptr, infoptr);

    // Apparently pnggroup tells me that weird data appears after IEND in the file. How is that even
    // possible? If I've miswritten data to the rows array, why would that cause libpng to completely
    // botch the job? Fucking hell. 29 May 2024

    /* Cleanup */
    png_destroy_write_struct(&pngptr, &infoptr);
    if (swatchrgb)
      free(swatchrgb);
    if (rows) {
      for (i=0; i<opts->nheight; i++)
	free(rows[i]);
      free(rows);
    }
    destroy_wheel(&colors);
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
