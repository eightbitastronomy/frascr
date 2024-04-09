/****************************************************************************/
/* color.h: color library for FRASCR application                            */
/*   This library may eventually be expanded.                               */
/*   Currently, implements functionality for making a color wheel, working  */
/*   with swatches of a few different color spaces, and a few conversions.  */
/*   However, currently, it really is mostly useful for starting in LCH and */
/*   moving over to sRGB.                                                   */
/*   Some of the algorithms implemented here follow work found online. A    */
/*   URL has been provided below.                                           */
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


#ifndef COLOR_H
#define COLOR_H


#define PW_BAD_CALL    -1
#define PW_MALLOC      -2
#define PW_BAD_COLOR   -3
#define PW_NEED_ODD_N  -4
#define PW_BAD_WHEEL   -5
#define PW_COLOR_CONV  -6
#define PW_UNK_MODE    -7


enum color_space {
  MONO,
  LCH,
  CIELAB,
  CIELUV,
  CIEXYZ,
  SRGB
};
typedef enum color_space ColorSpace;


enum swatch_generation {
  OTHER,
  SAMPLE,
  LINEAR
};
typedef enum swatch_generation SwatchGenMode;


struct palette_wheel {
  void * swatch;
  int n;
  ColorSpace space;
};
typedef struct palette_wheel Wheel;


struct palette_base_int {
  int a;
  int b;
  int c;
};
typedef struct palette_base_int BaseI;


struct palette_base_dbl {
  double a;
  double b;
  double c;
};
typedef struct palette_base_dbl BaseD;


union palette_base_char {
  struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char alpha;
  } rgba;
  unsigned int word;
};
typedef union palette_base_char BaseC;



int initialize_wheel(Wheel ** w,
		     int n,
		     ColorSpace space,
		     SwatchGenMode mode,
		     void * swatches);
  
void destroy_wheel(Wheel ** w);

void sample_by_intensity_norm(const Wheel * w,
			      const double intensity,
			      void ** output);

void linear_by_intensity_norm(const Wheel * w,
			      const double intensity,
			      void ** output);
  
ColorSpace space_to_space(const char * space);

SwatchGenMode mode_to_mode(const char * mode);
  
int convert_lch_to_lab(BaseD * bluv, BaseI * blch);

int convert_lab_to_xyz_old(BaseD * bxyz, BaseD * blab);

int convert_lab_to_xyz(BaseD * bxyz, BaseD * blab);
  
int convert_luv_to_xyz(BaseD * bxyz, BaseD * bluv);

int convert_luv_to_xyz_1(BaseD * bxyz, BaseD * bluv);

int convert_xyz_to_sRGB_old(BaseC * brgb, BaseD * bxyz, unsigned char alpha);

int convert_xyz_to_sRGB(BaseC * brgb, BaseD * bxyz, unsigned char alpha);

int convert_xyz_to_RGB(BaseC * brgb, BaseD * bxyz, unsigned char alpha);


#endif /* COLOR_H */
