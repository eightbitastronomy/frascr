/****************************************************************************/
/* reference.c: color library for FRASCR application                        */
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


#include "reference.h"
#include <string.h>


RefType illum_to_illum(const char * type)
{
  if (strcmp("D65 2deg", type) == 0)
    return D65_2DEG;
  if (strcmp("D65 10deg", type) == 0)
    return D65_10DEG;
  if (strcmp("D50 2deg", type) == 0)
    return D50_2DEG;
  if (strcmp("D50 10deg", type) == 0)
    return D50_10DEG;
  return UNKNOWN;
}


/* in the following standard-illuminant and matrix setting functions,
   I would like to have used the following reference:
   - Schanda, János (2007). "3. CIE Colorimetry".
     In Schanda, János (ed.). Colorimetry: understanding the CIE system.
     John Wiley & Sons. Appendix A, p. 74.
   However, I did not have access to it through my academic institution.
   I may return to this and try harder to get it...Because, especially
   because, the matrix elements are not ubiquitous across all sources,
   while I need to have consistency in this library. */

/* Note: https://web.archive.org/web/20171204115107/http://www.cie.co.at/publ/abst/s005.html
   There should only be two standard illuminants, A and D65.
   Having others would be helpful, though.
   https://cie.co.at/publications/international-standards
   see ISO/CIE 11664-3:2019
   (Can't access at this time without paying) */


void std_illuminant_for_D65_2deg(RefValues *rv) {
  /* not sure the source for these */
  rv->white.tristimx = 0.950489;
  rv->white.tristimy = 1.0;
  rv->white.tristimz = 1.088840;
  /* from https://en.wikipedia.org/wiki/Standard_illuminant */
  rv->white.coordx = 0.31272;
  rv->white.coordy = 0.32903;
}


void std_illuminant_for_D65_10deg(RefValues *rv) {
  /* from https://en.wikipedia.org/wiki/Standard_illuminant */
  rv->white.tristimx = 0.94811;
  rv->white.tristimy = 1.0;
  rv->white.tristimz = 1.07304;
  /* from https://en.wikipedia.org/wiki/Standard_illuminant */
  rv->white.coordx = 0.31382;
  rv->white.coordy = 0.33100;
}


void std_illuminant_for_D50_2deg(RefValues *rv) {
  /* from https://www.color.org/sRGB.pdf (sRGB_for_ICC_profiles.pdf) */
  rv->white.tristimx = 0.9642;
  rv->white.tristimy = 1.0;
  rv->white.tristimz = 0.8249;
  /* from https://en.wikipedia.org/wiki/Standard_illuminant */
  rv->white.coordx = 0.34567;
  rv->white.coordy = 0.35850;
}


void std_illuminant_for_D50_10deg(RefValues *rv) {
  /* NEED! (if they exist) */
  rv->white.tristimx = 0.;
  rv->white.tristimy = 0.;
  rv->white.tristimz = 0.;
  /* from https://en.wikipedia.org/wiki/Standard_illuminant */
  rv->white.coordx = 0.34773;
  rv->white.coordy = 0.35952;
}


void matrix_for_XYZ_sRGB_D65(RefValues *rv) {
  /* values for the matrix elements are taken from www.brucelindbloom.com */
  /*rv->matrix.m11 = 3.2404542;
  rv->matrix.m12 = -1.5371385;
  rv->matrix.m13 = -0.4985314;
  rv->matrix.m21 = -0.9692660;
  rv->matrix.m22 = 1.8760108;
  rv->matrix.m23 = 0.0415560;
  rv->matrix.m31 = 0.0556434;
  rv->matrix.m32 = -0.2040259;
  rv->matrix.m33 = 1.0572252;*/
  /* values found in multiple locations, specifically https://www.color.org/sRGB.pdf
     (sRGB_for_ICC_profiles.pdf) */
  rv->matrix.m11 = 3.2406255;
  rv->matrix.m12 = -1.537208;
  rv->matrix.m13 = -0.4986286;
  rv->matrix.m21 = -0.9689307;
  rv->matrix.m22 = 1.8757561;
  rv->matrix.m23 = 0.0415175;
  rv->matrix.m31 = 0.0557101;
  rv->matrix.m32 = -0.2040211;
  rv->matrix.m33 = 1.0569959;
}


void matrix_for_sRGB_XYZ_D65(RefValues *rv) {
  /* values for the matrix elements are taken from www.brucelindbloom.com */
  /* I don't think these will be consistent with the more widespread values used
     in the above XYZ_sRGB function, following from the fact that the two
     matrices ought to be inverses of each other */
  rv->matrix.m11 = 0.4124564;
  rv->matrix.m12 = 0.3575761;
  rv->matrix.m13 = 0.1804375;
  rv->matrix.m21 = 0.2126729;
  rv->matrix.m22 = 0.7151522;
  rv->matrix.m23 = 0.0721750;
  rv->matrix.m31 = 0.0193339;
  rv->matrix.m32 = 0.1191920;
  rv->matrix.m33 = 0.9503041;
}


void matrix_for_XYZ_wgRGB_D50(RefValues *rv) {
  /* values for the matrix elements are taken from www.brucelindbloom.com */
  rv->matrix.m11 = 1.4628067;
  rv->matrix.m12 = -0.1840623;
  rv->matrix.m13 = -0.2743606;
  rv->matrix.m21 = -0.5217933;
  rv->matrix.m22 = 1.4472381;
  rv->matrix.m23 = 0.0677227;
  rv->matrix.m31 = 0.0349342;
  rv->matrix.m32 = -0.0968930;
  rv->matrix.m33 = 1.2884099;
}


void matrix_for_wgRGB_XYZ_D50(RefValues *rv) {
  /* values for the matrix elements are taken from www.brucelindbloom.com */
  rv->matrix.m11 = 0.7161046;
  rv->matrix.m12 = 0.1009296;
  rv->matrix.m13 = 0.1471858;
  rv->matrix.m21 = 0.2581874;
  rv->matrix.m22 = 0.7249378;
  rv->matrix.m23 = 0.0168748;
  rv->matrix.m31 = 0.0;
  rv->matrix.m32 = 0.0517813;
  rv->matrix.m33 = 0.7734287;
}
