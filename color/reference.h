/****************************************************************************/
/* reference.h: color library for FRASCR application                        */
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


#ifndef REFERENCE_H
#define REFERENCE_H


#include "numtype.h"


enum reference_value_type {
  D65_2DEG,
  D65_10DEG,
  D50_2DEG,
  D50_10DEG,
  UNKNOWN
};
typedef enum reference_value_type RefType;


struct standard_illuminant_values {
  float64 tristimx, tristimy, tristimz;
  float64 coordx, coordy;
};
typedef struct standard_illuminant_values StdIllum;


struct conversion_matrix {
  float64 m11, m12, m13;
  float64 m21, m22, m23;
  float64 m31, m32, m33;
};
typedef struct conversion_matrix ConvMatrix;


struct reference_value_holder {
  StdIllum white;
  ConvMatrix matrix;
};
typedef struct reference_value_holder RefValues;


RefType illum_to_illum(const char * type);

void std_illuminant_for_D65_2deg(RefValues *rv);

void std_illuminant_for_D65_10deg(RefValues *rv);

void std_illuminant_for_D50_2deg(RefValues *rv);

void std_illuminant_for_D50_10deg(RefValues *rv);

void matrix_for_XYZ_sRGB_D65(RefValues *rv);

void matrix_for_sRGB_XYZ_D65(RefValues *rv);

void matrix_for_XYZ_wgRGB_D50(RefValues *rv);

void matrix_for_wgRGB_XYZ_D50(RefValues *rv);



#endif //REFERENCE_H
