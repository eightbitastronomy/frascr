/****************************************************************************/
/* color.c: color library for FRASCR application                            */
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


#include "color.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>



#define ALLOCO(ob,tp,n)   (ob) = malloc(sizeof(tp)*n)
#define ALLOCSW(ob,tp,n)  ALLOCO((*ob)->swatch,tp,n)
#define GAMMACORRECT(x)   (x) <= .0031308 ? 12.92*(x) : 1.055*pow(x,1.0/2.4)-0.055
#define FINV(x)           ( x > 6.0/29.0 ? pow(x, 3.0) : 27./24389.*(116.*(x) - 16.) )
#define INTERPS1(tp,mbr,targ,src,fr,n) (*((tp **)(targ)))->mbr = interpolate_S1(((tp *)((src)->swatch))[n].mbr,fr,((tp *)((src)->swatch))[n+1].mbr)
#define INTERPS1D(tp,mbr,targ,src,fr,n) (*((tp **)(targ)))->mbr = interpolate_S1D(((tp *)(src)->swatch)[n].mbr,fr,((tp *)(src)->swatch)[n+1].mbr)
#define INTERPC(tp,mbr,targ,src,fr,n) (*((tp **)(targ)))->rgba.mbr = interpolate_C(((tp *)(src)->swatch)[n].rgba.mbr,fr,((tp *)(src)->swatch)[n+1].rgba.mbr)
#define DUPE_I(targ,src)  dupe_i_n(targ,src,1)
#define DUPE_D(targ,src)  dupe_d_n(targ,src,1)
#define DUPE_C(targ,src)  dupe_c_n(targ,src,1)
#define ABS(x)            (x < 0 ? -1*x : x)
#define MAXS1(x,y)        (x < 180 ? x : (y < 180 ? y : 180))
#define MAXS1D(x,y)       (x < 180.0 ? x : (y < 180.0 ? y : 180.0))

#define GAMMACORRALT(x)   (x) <= 0.0031308 ? 3294.6*(x) : 269.025*pow(x,1.0/2.4)-14.025
#define FINVOLD(x)        ( x < 6.0/29.0 ? pow(x, 3.0) : 3.0*36.0/29.0/29.0*((x) - 4.0/29.0) )
//#define EVEN(x)           (ceil((double)x)/2 == x/2 ? 1 : 0)
//#define MAX(x,y)          (x < y ? y : x)  //looks obsolete
//#define MIDDLE(x)         (floor((double)x)/2)
//#define INTERP(l,f,r)     (left + (int)((right-left)*frac)) //looks obsolete
//#define INTERPD(l,f,r)    (left + ((right-left)*frac)) //looks obsolete



static inline void dupe_i_n(BaseI * targ, BaseI * src, int n) {
  int i;
  for (i=0; i<n; i++) {
    targ[i].a = src[i].a;
    targ[i].b = src[i].b;
    targ[i].c = src[i].c;
  }
}



static inline void dupe_d_n(BaseD * targ, BaseD * src, int n) {
  int i;
  for (i=0; i<n; i++) {
    targ[i].a = src[i].a;
    targ[i].b = src[i].b;
    targ[i].c = src[i].c;
  }
}



static inline void dupe_c_n(BaseC * targ, BaseC * src, int n) {
  int i;
  for (i=0; i<n; i++) {
    targ[i].rgba.r = src[i].rgba.r;
    targ[i].rgba.g = src[i].rgba.g;
    targ[i].rgba.b = src[i].rgba.b;
  }
}




static inline int s1(const int x)
{
  int r = x % 360;
  return r > 0 ? r : r + 360;
}


static inline double s1D(const double x)
{
  double r = fmod(x, 360.);
  return r > 0 ? r : r + 360;
}



static inline double interpolate_S1D(const double left, const double frac, const double right)
{
  double RL   = right - left;
  double RLS1 = s1D(RL);
  double LR   = left - right;
  double LRS1 = s1D(LR);
  double interval; 
  if (LR < 0.) {
    interval = MAXS1D(LRS1,RLS1);
    if (RL==RLS1)
      /* left < right */ 
      return left + interval*frac;
    else
      /* left > right, wraparound */
      return s1D(left - interval*frac);
  } else if (LR > 0.) {
    interval = MAXS1D(LRS1,RLS1);
    if (LR==LRS1)
      /* left > right */
      return left - interval*frac;
    else
      /* right > left, wraparound */
      return s1D(left + interval*frac);
  } else {
    return left;
  }
}


static inline int interpolate_S1(const int left, const double frac, const int right)
{
  int RL   = right - left;
  int RLS1 = s1(RL);
  int LR   = left - right;
  int LRS1 = s1(LR);
  double interval;
  if (LR<0) {
    interval = (double)MAXS1(LRS1,RLS1);
    if (LRS1 > 180)
      /* left < right */ 
      return left + (int)(interval*frac);
    else
      /* left > right, wraparound */
      return s1(left - (int)(interval*frac));
  } else if (LR > 0) {
    interval = (double)MAXS1(LRS1,RLS1);
    if (RLS1 > 180)
      /* left > right */
      return left - (int)(interval*frac);
    else
      /* right > left, wraparound */
      return s1(left + (int)(interval*frac));
  } else {
    return left;
  }
}



/* note, interpolation of BaseC cannot use the S1 function since unsigned char cannot even reach 360. */
static inline unsigned char interpolate_C(const unsigned char left, const double frac, const unsigned char right)
{
  double LR = (double)left - (double)right;
  double interval = ABS(LR);
  if (LR<0) 
    /* left < right */ 
    return (unsigned char)(left + interval*frac);
  else
    /* left > right */
    return (unsigned char)(left - interval*frac);
}



ColorSpace space_to_space(const char * space)
{
  if (strcmp("lch", space) == 0)
    return LCH;
  if (strcmp("cieluv", space) == 0)
    return CIELUV;
  if (strcmp("cielab", space) == 0) 
    return CIELAB;
  if (strcmp("ciexyz", space) == 0)
    return CIEXYZ;
  if (strcmp("srgb", space) == 0)
    return SRGB;
  return MONO;
}



SwatchGenMode mode_to_mode(const char * mode)
{
  if (strcmp("sample", mode))
    return SAMPLE;
  if (strcmp("linear", mode))
    return LINEAR;
  return OTHER;
}



int convert_lch_to_lab(BaseD * blab, BaseI * blch)
{
  double l, c, h;

  if ((blab==NULL) || (blch==NULL))
    return PW_BAD_CALL;

  l = (double) blch->a;
  c = (double) blch->b;
  h = (double) blch->c;

  /* L */
  blab->a = l;
  /* u */
  blab->b = c * cos(h*M_PI/180.0);
  /* v */
  blab->c = c * sin(h*M_PI/180.0);

  return 0;
}



/* Based on https://mina86.com/2021/srgb-lab-lchab-conversions/ algorithm */
int convert_lab_to_xyz(BaseD * bxyz, BaseD * blab)
{
  /* D65 values */
  const double Xn = 0.950489;//95.0489;//
  const double Yn = 1.0;// 100.0;//
  const double Zn = 1.088840;//108.8840;//
  const double kappa = 24389. / 27.;
  double buffer, r;
  
  if ((bxyz==NULL) || (blab==NULL))
    return PW_BAD_CALL;
  
  /* pre-Y */
  r = (blab->a+16.0)/116.0;
  buffer = r;
  /* X */
  r = buffer + blab->b/500.0;
  bxyz->a = Xn*FINV(r);
  /* Z */
  r = buffer - blab->c/200.0;
  bxyz->c = Zn*FINV(r);
  /* Y */
  if (buffer < 8.0) {
    buffer = pow(buffer, 3.0);
  } else {
    buffer = blab->a / kappa;
  }
  bxyz->b = buffer;

  return 0;
}



int convert_xyz_to_sRGB(BaseC * brgb,
			BaseD * bxyz,
			unsigned char alpha)
{
  const double m11 = 3.2406;
  const double m12 = -1.5372;
  const double m13 = -.4986;
  const double m21 = -.9689;
  const double m22 = 1.8758;
  const double m23 = .0415;
  const double m31 = .0557;
  const double m32 = -.2040;
  const double m33 = 1.0570;
  double x = bxyz->a;
  double y = bxyz->b;
  double z = bxyz->c;
  double r;

  if ((bxyz==NULL) || (brgb==NULL))
    return -1;

  /* R */
  r = x*m11 + y*m12 + z*m13;
  r = GAMMACORRECT(r);
  if (r < 0.0) {
    r = 0;
  } else if ( r > 1.0 ) {
    r = 0.9999;
  } 
  brgb->rgba.r = (unsigned char)((unsigned int)(255.0*r));

  /* G */
  r = x*m21 + y*m22 + z*m23;
  r = GAMMACORRECT(r);
  if (r < 0.0) {
    r = 0;
  } else if ( r > 1.0 ) {
    r = 0.9999;
  }
  brgb->rgba.g = (unsigned char)((unsigned int)(255.0*r));

  /* B */
  r = x*m31 + y*m32 + z*m33;
  r = GAMMACORRECT(r);
  if (r < 0.0) {
    r = 0;
  } else if ( r > 1.0 ) {
    r = 0.9999;
  }
  brgb->rgba.b = (unsigned char)((unsigned int)(255.0*r));

  /* alpha */
  brgb->rgba.alpha = alpha;

  return 0;
}



void sample_by_intensity_norm(const Wheel * w, const double intensity, void ** output)
{
  /* set up bins based on swatch_n, then use intensity float [0,1] to bin. */
  /* Need a switch-case to set up bins, convert to float, and need a switch case to finish. In-between, no. */
  /* This fctn can benefit from an all-int version since binning can be done without floats... */
  int n;
  double landing, remainder;
  int below, landing_bin;
  void * outswatch;
  
  if ((intensity<0.) || (intensity>1.0))
    return;
  if ((output==NULL) || (w==NULL))
    return;

  outswatch = *output;
  n = w->n - 1;
  landing = n*intensity;
  below = (int)landing;
  remainder = landing - (double)below;
  if (remainder < 0.5)
    landing_bin = below;
  else
    landing_bin = below + 1;
  switch(w->space) {
  case LCH:
    ALLOCO(outswatch,BaseI,1);
    DUPE_I(&(((BaseI *)w->swatch)[landing_bin]),outswatch);
    break;
  case CIELUV:
    ALLOCO(outswatch,BaseD,1);
    DUPE_D(&(((BaseD *)w->swatch)[landing_bin]),outswatch);
    break;
  case CIELAB:
    ALLOCO(outswatch,BaseD,1);
    DUPE_D(&(((BaseD *)w->swatch)[landing_bin]),outswatch);
    break;
  case CIEXYZ:
    ALLOCO(outswatch,BaseD,1);
    DUPE_D(&(((BaseD *)w->swatch)[landing_bin]),outswatch);
    break;
  case SRGB:
    ALLOCO(outswatch,BaseC,1);
    DUPE_C(&(((BaseC *)w->swatch)[landing_bin]),outswatch);
    break;
  default:
    ALLOCO(outswatch,BaseI,1);
    DUPE_I(&(((BaseI *)w->swatch)[landing_bin]),outswatch);
  }
  
  return;
}



void linear_by_intensity_norm(const Wheel * w, const double intensity, void ** output)
{
  /* linearly interpolate between swatches. Needs a minimum of two swatches. */
  /* Need a switch-case to set up partitions, convert to float, and need a switch case to finish. In-between, no. */
  /* Using different versions of this fctn is not that useful, since even in the integer case,
     floats must still be used to interpolate...? */
  int n;
  double landing, remainder;
  int below, landing_bin;
  double a, b, c;
  
  if ((intensity<0.) || (intensity>1.0))
    return;
  if ((output==NULL) || (w==NULL))
    return;

  if (n == 2) {
    below = 0;
    remainder = intensity;
  } else {
    n = w->n - 1;
    landing = n*intensity;
    below = (int)landing;
    remainder = landing - (double)below;
  }

  switch( w->space ) {
  case LCH:
    ALLOCO(*((BaseI **)(output)),BaseI,1);
    INTERPS1(BaseI, a, output, w, remainder, below);
    INTERPS1(BaseI, b, output, w, remainder, below);
    INTERPS1(BaseI, c, output, w, remainder, below);
    break;
  case CIELUV:
    ALLOCO(*((BaseD **)(output)),BaseD,1);
    INTERPS1D(BaseD, a, output, w, remainder, below);
    INTERPS1D(BaseD, b, output, w, remainder, below);
    INTERPS1D(BaseD, c, output, w, remainder, below);
    break;
  case CIELAB:
    ALLOCO(*((BaseD **)(output)),BaseD,1);
    INTERPS1D(BaseD, a, output, w, remainder, below);
    INTERPS1D(BaseD, b, output, w, remainder, below);
    INTERPS1D(BaseD, c, output, w, remainder, below);
    break;
  case CIEXYZ:
    ALLOCO(*((BaseD **)(output)),BaseD,1);
    INTERPS1D(BaseD, a, output, w, remainder, below);
    INTERPS1D(BaseD, b, output, w, remainder, below);
    INTERPS1D(BaseD, c, output, w, remainder, below);
    break;
  case SRGB:
    ALLOCO(*((BaseC **)(output)),BaseC,1);
    INTERPC(BaseC, r, output, w, remainder, below);
    INTERPC(BaseC, g, output, w, remainder, below);
    INTERPC(BaseC, b, output, w, remainder, below);
    break;
  default:
    ALLOCO(*((BaseI **)(output)),BaseI,1);
    INTERPS1(BaseI, a, output, w, remainder, below);
    INTERPS1(BaseI, b, output, w, remainder, below);
    INTERPS1(BaseI, c, output, w, remainder, below);
  }
  return;
}


int initialize_wheel(Wheel ** w,
		     int n,
		     ColorSpace space,
		     SwatchGenMode mode,
		     void * swatches)
{
  *w = malloc(sizeof(Wheel));
  if (*w == NULL)
    return PW_MALLOC;
  (*w)->n = n;
  (*w)->space = space;
  switch (space) {
  case LCH:
    ALLOCSW(w,BaseI,n);
    dupe_i_n((BaseI *)((*w)->swatch), (BaseI *)(swatches), n);
    break;
  case CIELUV:
    ALLOCSW(w,BaseD,n);
    dupe_d_n((BaseD *)((*w)->swatch), (BaseD *)(swatches), n);
    break;
  case CIELAB:
    ALLOCSW(w,BaseD,n);
    dupe_d_n((BaseD *)((*w)->swatch), (BaseD *)(swatches), n);
    break;
  case CIEXYZ:
    ALLOCSW(w,BaseD,n);
    dupe_d_n((BaseD *)((*w)->swatch), (BaseD *)(swatches), n);
    break;
  case SRGB:
    ALLOCSW(w,BaseC,n);
    dupe_c_n((BaseC *)((*w)->swatch), (BaseC *)(swatches), n);
    break;
  default:
    ALLOCSW(w,BaseI,n);
    dupe_i_n((BaseI *)((*w)->swatch), (BaseI *)(swatches), n);
  }
  if ((*w)->swatch == NULL)
    return PW_MALLOC;
  return 0;
}



void destroy_wheel(Wheel ** w)
{
  int i;
  if (*w) {
    if ((*w)->swatch) {
      free((*w)->swatch);
      (*w)->swatch = NULL;
    }
    free(*w);
    *w = NULL;
  }
}



/************** obsolete - remove *****************/


static inline int adjust_hue(int val)
{
  if (val < 0) {
    val += ceil(-1.0f*(double)val/360.0f)*360.0f;
  }
  return val % 360;
}



static inline int wheelmap(int n, int start1, int end1, int start2, int end2) 
{
  return (int)( (((double)n - (double)start1) / ((double)end1 - (double)start1)) * ((double)end2 - (double)start2) + (double)start2 );
}



/***************    old conversions    ***************/




/* Based on CIELab->XYZ wikipedia algorithm */
int convert_lab_to_xyz_old(BaseD * bxyz, BaseD * blab)
{
  /* D65 values */
  double Xn = 0.950489;//95.0489;
  double Yn = 1.0;// 100.0;
  double Zn = 1.088840;//108.8840;

  if ((bxyz==NULL) || (blab==NULL))
    return PW_BAD_CALL;
  
  /* X */
  bxyz->a = Xn*FINVOLD(((blab->a+16.0)/116.0 + blab->b/500.0));
  /* Y */
  bxyz->b = Yn*FINVOLD(((blab->a+16.0)/116.0));
  /* Z */
  bxyz->c = Zn*FINVOLD(((blab->a+16.0)/116.0 + blab->c/200.0));

  return 0;
}



int convert_luv_to_xyz(BaseD * bxyz, BaseD * bluv)
{
  double upn = 0.2009;
  double vpn = 0.4610;
  double y_wh_pt = 1.0; //100.0;
  double up, vp;
  double l, u, v;
  
  if ((bxyz==NULL) || (bluv==NULL))
    return PW_BAD_CALL;

  l = bluv->a;
  u = bluv->b;
  v = bluv->c;
  
  up = u/13.0/l + upn;
  vp = v/13.0/l + vpn;

  /* Y */
  if (l <= 8.0) {
    bxyz->b = y_wh_pt*l*pow(3.0/29.0, 3.0);
  } else {
    bxyz->b = y_wh_pt*pow((l+16.0)/116.0, 3.0);
  }
  /* X */
  bxyz->a = bxyz->b * 9.0*up/4.0/vp;
  /* Z */
  bxyz->c = bxyz->b * (12.0-3.0*up-20.0*vp)/4.0/vp;

  return 0;
}


int convert_luv_to_xyz_1(BaseD * bxyz, BaseD * bluv)
{
  /* XYZ tristimulus values for 2 deg observer and D65 */
  const double xr = 0.9505; //1.205; //illuminant E? //73000. for each for E
  const double yr = 1.0; //0.948;
  const double zr = 1.0890; //0.909;
  const double kappa = 903.3;
  double l, u, v, Y;
  
  
  if ((bxyz==NULL) || (bluv==NULL))
    return PW_BAD_CALL;

  l = bluv->a;
  u = bluv->b;
  v = bluv->c;

  double vnot = 9.0*yr/(xr+15.0*yr+3.0*zr);
  double unot = 4.0*xr/(xr+15.0*yr+3.0*zr);
  double db = 39.0*l/(v + 13.0*l*vnot);
  double ac = 52.0*l/(u + 13.0*l*unot)/3.0;

  /* Y */
  if (l <= 8.0) {
    Y = l/kappa;
  } else {
    Y = pow((l+16.0)/116.0, 3.0);
  }
  bxyz->b = Y;
  /* X */
  bxyz->a = Y*db/ac;
  /* Z */
  bxyz->c = (bxyz->a)*(ac-1./3.)-5.0*Y;

  return 0;
}



int convert_xyz_to_sRGB_alt(BaseC * brgb,
			    BaseD * bxyz,
			    unsigned char alpha)
{
  const double m11 = 3.2406;
  const double m12 = -1.5372;
  const double m13 = -.4986;
  const double m21 = -.9689;
  const double m22 = 1.8758;
  const double m23 = .0415;
  const double m31 = .0557;
  const double m32 = -.2040;
  const double m33 = 1.0570;
  double x = bxyz->a;
  double y = bxyz->b;
  double z = bxyz->c;
  double r;

  if ((bxyz==NULL) || (brgb==NULL))
    return -1;

  // Unclear why final swatch is way off, although none are perfectly right
  
  /* R */
  r = x*m11 + y*m12 + z*m13;
  r = GAMMACORRALT(r);
  if (r < 0.0) {
    brgb->rgba.r = (unsigned char)0;
  } else if ( r > 255.0 ) {
    brgb->rgba.r = (unsigned char)255.0;
  } else
    brgb->rgba.r = (unsigned char)r;

  /* G */
  r = x*m21 + y*m22 + z*m23;
  r = GAMMACORRALT(r);
  if (r < 0.0) {
    brgb->rgba.r = (unsigned char)0;
  } else if ( r > 255.0 ) {
    brgb->rgba.r = (unsigned char)255.0;
  } else
    brgb->rgba.r = (unsigned char)r;

  /* B */
  r = x*m31 + y*m32 + z*m33;
  r = GAMMACORRALT(r);
  if (r < 0.0) {
    brgb->rgba.r = (unsigned char)0;
  } else if ( r > 255.0 ) {
    brgb->rgba.r = (unsigned char)255.0;
  } else
    brgb->rgba.r = (unsigned char)r;
  
  /* alpha */
  brgb->rgba.alpha = alpha;

  return 0;
}



int convert_xyz_to_RGB(BaseC * brgb, BaseD * bxyz, unsigned char alpha)
{
  double m11 = 2.36461385;
  double m12 = -0.89654057;
  double m13 = -0.46807328;
  double m21 = -0.51516621;
  double m22 = 1.4264081;
  double m23 = 0.0887581;
  double m31 = 0.0052037;
  double m32 = -0.01440816;
  double m33 = 1.00920446;
  double x = bxyz->a;// / 100.0;
  double y = bxyz->b;// / 100.0;
  double z = bxyz->c;// / 100.0;
  double r;

  if ((bxyz==NULL) || (brgb==NULL))
    return -1;

  /* R */
  r = x*m11 + y*m12 + z*m13;
  //fprintf(stderr,"                %f ",r);
  r = GAMMACORRECT(x*m11 + y*m12 + z*m13);
  //fprintf(stderr,"--> %f",r);
  if (r < 0.0) {
    r = 0;
  } else if ( r > 1.0 ) {
    r = 0.9999;
  } 
  //fprintf(stderr,"--> %f\n",r);
  brgb->rgba.r = (unsigned char)((unsigned int)(255.0*r));

  /* G */
  r = x*m21 + y*m22 + z*m23;
  //fprintf(stderr,"                %f ",r);
  r = GAMMACORRECT(x*m21 + y*m22 + z*m23);
  //fprintf(stderr,"--> %f",r);
  if (r < 0.0) {
    r = 0;
  } else if ( r > 1.0 ) {
    r = 0.9999;
  }
  //fprintf(stderr,"--> %f\n",r);
  brgb->rgba.g = (unsigned char)((unsigned int)(255.0*r));

  /* B */
  r = x*m31 + y*m32 + z*m33;
  //fprintf(stderr,"                %f ",r);
  r = GAMMACORRECT(x*m31 + y*m32 + z*m33);
  //fprintf(stderr,"--> %f",r);
  if (r < 0.0) {
    r = 0;
  } else if ( r > 1.0 ) {
    r = 0.9999;
  }
  //fprintf(stderr,"--> %f\n",r);
  brgb->rgba.b = (unsigned char)((unsigned int)(255.0*r));

  /* alpha */
  brgb->rgba.alpha = alpha;

  return 0;
}
