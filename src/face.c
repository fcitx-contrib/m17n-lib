/* face.c -- face module.
   Copyright (C) 2003, 2004
     National Institute of Advanced Industrial Science and Technology (AIST)
     Registration Number H15PRO112

   This file is part of the m17n library.

   The m17n library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   The m17n library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the m17n library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.  */

/***en
    @addtogroup m17nFace
    @brief A face is an object to control appearance of M-text.

    A @e face is an object of the type #MFace and controls how to
    draw M-texts.  A face has a fixed number of @e face @e properties.
    Like other types of properties, a face property consists of a key
    and a value.  A key is one of the following symbols:

    #Mforeground, #Mbackground, #Mvideomode, #Mhline, #Mbox,
    #Mfoundry, #Mfamily, #Mweight, #Mstyle, #Mstretch, #Madstyle,
    #Msize, #Mfontset, #Mratio, #Mhook_func, #Mhook_arg

    "The face property that belongs to face F and whose key is @c xxx"
    may be shortened to "the xxx property of F".

    The M-text drawing functions first search an M-text for the text
    property whose key is the symbol #Mface, then draw the M-text
    using the value of that text property.  This value must be a
    pointer to a face object.

    If there are multiple text properties whose key is @c Mface, and
    they are not conflicting one another, properties of those faces
    are merged and used.

    If no faces specify a certain property, the value of the default
    face is used.  */

/***ja
    @addtogroup m17nFace
    @brief �ե������Ȥϡ�M-text ��ɽ�������椹�륪�֥������ȤǤ���.

    @e �ե����� �� #MFace ���Υ��֥������ȤǤ��ꡢM-text ��ɽ����ˡ
    �����椹�롣�ե������ϸ���Ŀ��� @e �ե������ץ�ѥƥ� ����ġ�
    �ե������ץ�ѥƥ��ϥ������ͤ���ʤ롣�����ϥ���ܥ�Ǥ��ꡢ

    #Mforeground, #Mbackground, #Mvideomode, #Mhline, #Mbox, 
    #Mfoundry, #Mfamily, #Mweight, #Mstyle, #Mstretch, #Madstyle, 
    #Msize, #Mfontset, #Mratio, #Mhook_func, #Mhook_arg

    �֥ե����� F �Υե������ץ�ѥƥ��Τ��������� @c Mxxx �Ǥ����Ρ�
    �Τ��Ȥ��ñ�ˡ�F �� xxx �ץ�ѥƥ��פȸƤ֤��Ȥ����롣

    M-text ��ɽ���ؿ��ϡ��ޤ��ǽ�ˤ��� M-text ���饭��������ܥ� 
    #Mface �Ǥ���褦�ʥƥ����ȥץ�ѥƥ���õ�������ˤ����ͤ˽��ä� 
    M-text ��ɽ�����롣�����ͤϥե��������֥������ȤؤΥݥ��󥿤Ǥʤ�
    ��Фʤ�ʤ���

     M-text ����#Mface �򥭡��Ȥ���ƥ����ȥץ�ѥƥ���ʣ�����äƤ��ꡢ
    ���Ĥ������ͤδ֤˾��ͤ��ʤ��ʤ�С��ե�����������Ȥ߹�蘆���
    �Ѥ����롣

    ����ƥ�����°�����ɤΥե������ˤ�äƤ���ꤵ��Ƥ��ʤ����ϡ���
    �ե���ȥե��������ͤ��Ѥ����롣  */

/*=*/

#if !defined (FOR_DOXYGEN) || defined (DOXYGEN_INTERNAL_MODULE)
/*** @addtogroup m17nInternal
     @{ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "m17n-gui.h"
#include "m17n-misc.h"
#include "internal.h"
#include "charset.h"
#include "symbol.h"
#include "plist.h"
#include "mtext.h"
#include "textprop.h"
#include "internal-gui.h"
#include "face.h"
#include "font.h"
#include "fontset.h"

static M17NObjectArray face_table;

static MSymbol Mlatin;

static MSymbol M_face_prop_index;

/** Find a realized face registered on FRAME that is realized from
    FACE and using font RFONT.  If RFONT is NULL, find any that
    matches FACE.  */

static MRealizedFace *
find_realized_face (MFrame *frame, MFace *face, MRealizedFont *rfont)
{
  MPlist *rface_list;
  MRealizedFace *rface;
  int i;

  MPLIST_DO (rface_list, frame->realized_face_list)
    {
      rface = MPLIST_VAL (rface_list);
      if (! rfont
	  || rface->rfont == rfont)
	{
	  for (i = 0; i < MFACE_RATIO; i++)
	    if (rface->face.property[i] != face->property[i])
	      break;
	  if (i == MFACE_RATIO)
	    return rface;
	}
    }
  return NULL;
}

static void
free_face (void *object)
{
  MFace *face = (MFace *) object;

  if (face->property[MFACE_FONTSET])
    M17N_OBJECT_UNREF (face->property[MFACE_FONTSET]);
  if (face->property[MFACE_HLINE])
    free (face->property[MFACE_HLINE]);
  if (face->property[MFACE_BOX])
    free (face->property[MFACE_BOX]);
  M17N_OBJECT_UNREGISTER (face_table, face);
  free (object);
}


static MPlist *
serialize_hline (MPlist *plist, MFaceHLineProp *hline)
{
  MPlist *pl = mplist ();

  mplist_add (pl, Minteger, (void *) hline->type);
  mplist_add (pl, Minteger, (void *) hline->width);
  mplist_add (pl, Msymbol, hline->color);
  plist = mplist_add (plist, Mplist, pl);
  M17N_OBJECT_UNREF (pl);
  return plist;
}

static MPlist *
serialize_box (MPlist *plist, MFaceBoxProp *box)
{
  MPlist *pl = mplist ();

  mplist_add (pl, Minteger, (void *) box->width);
  mplist_add (pl, Minteger, (void *) box->inner_hmargin);
  mplist_add (pl, Minteger, (void *) box->inner_vmargin);
  mplist_add (pl, Minteger, (void *) box->outer_hmargin);
  mplist_add (pl, Minteger, (void *) box->outer_vmargin);
  mplist_add (pl, Msymbol, box->color_top);
  mplist_add (pl, Msymbol, box->color_bottom);
  mplist_add (pl, Msymbol, box->color_left);
  mplist_add (pl, Msymbol, box->color_right);
  plist = mplist_add (plist, Mplist, pl);
  M17N_OBJECT_UNREF (pl);
  return plist;
}

static MPlist *
serialize_face (void *val)
{
  MFace *face = val;
  MPlist *plist = mplist (), *pl = plist;
  int i;
  struct {
    MSymbol *key;
    MSymbol *type;
    MPlist *(*func) (MPlist *plist, void *val);
  } serializer[MFACE_PROPERTY_MAX]
      = { { &Mfoundry,		&Msymbol },
	  { &Mfamily,		&Msymbol },
	  { &Mweight,		&Msymbol },
	  { &Mstyle,		&Msymbol },
	  { &Mstretch,		&Msymbol },
	  { &Madstyle,		&Msymbol },
	  { &Msize,		&Minteger },
	  { &Mfontset,		NULL },
	  { &Mforeground,	&Msymbol },
	  { &Mbackground,	&Msymbol },
	  { &Mhline,		NULL },
	  { &Mbox,		NULL },
	  { &Mvideomode, 	&Msymbol },
	  { NULL,		NULL}, /* MFACE_HOOK_FUNC */
	  { NULL,		NULL}, /* MFACE_HOOK_ARG */
	  { &Mratio,		&Minteger } };
  
  for (i = 0; i < MFACE_PROPERTY_MAX; i++)
    if (face->property[i] && serializer[i].key)
      {
	pl = mplist_add (pl, Msymbol, *serializer[i].key);
	if (serializer[i].type)
	  pl = mplist_add (pl, *serializer[i].type, face->property[i]);
	else if (i == MFACE_FONTSET)
	  pl = mplist_add (pl, Msymbol, mfontset_name ((MFontset *)
						       face->property[i]));
	else if (i == MFACE_HLINE)
	  pl = serialize_hline (pl, (MFaceHLineProp *) face->property[i]);
	else if (i == MFACE_BOX)
	  pl = serialize_box (pl, (MFaceBoxProp *) face->property[i]);
      }

  return plist;
}

static void *
deserialize_hline (MPlist *plist)
{
  MFaceHLineProp hline, *hline_ret;

  if (! MPLIST_INTEGER_P (plist))
    MERROR (MERROR_FACE, NULL);
  hline.type = MPLIST_INTEGER_P (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_INTEGER_P (plist))
    MERROR (MERROR_FACE, NULL);
  hline.width = MPLIST_INTEGER_P (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_SYMBOL_P (plist))
    MERROR (MERROR_FACE, NULL);
  hline.color = MPLIST_SYMBOL (plist);
  MSTRUCT_MALLOC (hline_ret, MERROR_FACE);
  *hline_ret = hline;
  return hline_ret;
}

static void *
deserialize_box (MPlist *plist)
{
  MFaceBoxProp box, *box_ret;

  if (! MPLIST_INTEGER_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.width = MPLIST_INTEGER (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_INTEGER_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.inner_hmargin = MPLIST_INTEGER (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_INTEGER_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.inner_vmargin = MPLIST_INTEGER (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_INTEGER_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.outer_hmargin = MPLIST_INTEGER (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_INTEGER_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.outer_vmargin = MPLIST_INTEGER (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_SYMBOL_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.color_top = MPLIST_SYMBOL (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_SYMBOL_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.color_bottom = MPLIST_SYMBOL (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_SYMBOL_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.color_left = MPLIST_SYMBOL (plist);
  plist = MPLIST_NEXT (plist);
  if (! MPLIST_SYMBOL_P (plist))
    MERROR (MERROR_FACE, NULL);
  box.color_right = MPLIST_SYMBOL (plist);
  MSTRUCT_MALLOC (box_ret, MERROR_FACE);
  *box_ret = box;
  return box_ret;
}

static void *
deserialize_face (MPlist *plist)
{
  MFace *face = mface ();

  MPLIST_DO (plist, plist)
    {
      MSymbol key;
      int index;
      void *val;

      if (! MPLIST_SYMBOL_P (plist))
	break;
      key = MPLIST_SYMBOL (plist);
      index = (int) msymbol_get (key, M_face_prop_index) - 1;
      plist = MPLIST_NEXT (plist);
      if (MPLIST_TAIL_P (plist))
	break;
      if (index < 0 || index >= MFACE_PROPERTY_MAX)
	continue;
      if (key == Mfoundry || key == Mfamily || key == Mweight || key == Mstyle
	  || key == Mstretch || key == Madstyle
	  || key == Mforeground || key == Mbackground || key == Mvideomode)
	{
	  if (! MPLIST_SYMBOL_P (plist))
	    continue;
	  val = MPLIST_VAL (plist);
	}
      else if (key == Msize || key == Mratio)
	{
	  if (! MPLIST_INTEGER_P (plist))
	    continue;
	  val = MPLIST_VAL (plist);
	}
      else if (key == Mfontset)
	{
	  if (! MPLIST_SYMBOL_P (plist))
	    continue;
	  val = mfontset (MSYMBOL_NAME (MPLIST_SYMBOL (plist)));
	}
      else if (key == Mhline)
	{
	  if (! MPLIST_PLIST_P (plist))
	    continue;
	  val = deserialize_hline (MPLIST_PLIST (plist));
	}
      else if (key == Mbox)
	{
	  if (! MPLIST_PLIST_P (plist))
	    continue;
	  val = deserialize_box (MPLIST_PLIST (plist));
	}
      face->property[index] = val;
    }
  return face;
}

static MGlyphString work_gstring;



/* Internal API */

MFace *mface__default;

int
mface__init ()
{
  int i;

  face_table.count = 0;
  Mface = msymbol_as_managing_key ("face");
  msymbol_put (Mface, Mtext_prop_serializer, (void *) serialize_face);
  msymbol_put (Mface, Mtext_prop_deserializer, (void *) deserialize_face);

  Mforeground = msymbol ("foreground");
  Mbackground = msymbol ("background");
  Mvideomode = msymbol ("videomode");
  Mnormal = msymbol ("normal");
  Mreverse = msymbol ("reverse");
  Mratio = msymbol ("ratio");
  Mhline = msymbol ("hline");
  Mbox = msymbol ("box");
  Mhook_func = msymbol ("hook-func");
  Mhook_arg = msymbol ("hook-arg");

  Mlatin = msymbol ("latin");
  M_face_prop_index = msymbol ("  face-prop-index");

  {
    struct {
      /* Pointer to the key symbol of the face property.  */
      MSymbol *key;
      /* Index (enum face_property) of the face property. */
      int index;
    } mface_prop_data[MFACE_PROPERTY_MAX] =
	{ { &Mfoundry,		MFACE_FOUNDRY },
	  { &Mfamily,		MFACE_FAMILY },
	  { &Mweight,		MFACE_WEIGHT },
	  { &Mstyle,		MFACE_STYLE },
	  { &Mstretch,		MFACE_STRETCH },
	  { &Madstyle,		MFACE_ADSTYLE },
	  { &Msize,		MFACE_SIZE },
	  { &Mfontset,		MFACE_FONTSET },
	  { &Mforeground,	MFACE_FOREGROUND },
	  { &Mbackground,	MFACE_BACKGROUND },
	  { &Mhline,		MFACE_HLINE },
	  { &Mbox,		MFACE_BOX },
	  { &Mvideomode, 	MFACE_VIDEOMODE },
	  { &Mhook_func,	MFACE_HOOK_FUNC },
	  { &Mhook_arg,		MFACE_HOOK_ARG },
	  { &Mratio,		MFACE_RATIO },
	};

    for (i = 0; i < MFACE_PROPERTY_MAX; i++)
      /* We add one to distinguish it from no-property.  */
      msymbol_put (*mface_prop_data[i].key, M_face_prop_index,
		   (void *) (mface_prop_data[i].index + 1));
  }

  mface__default = mface ();
  mface__default->property[MFACE_WEIGHT] = msymbol ("medium");
  mface__default->property[MFACE_STYLE] = msymbol ("r");
  mface__default->property[MFACE_STRETCH] = msymbol ("normal");
  mface__default->property[MFACE_SIZE] = (void *) 120;
  mface__default->property[MFACE_FONTSET] = mfontset (NULL);
  M17N_OBJECT_REF (mface__default->property[MFACE_FONTSET]);
  /* mface__default->property[MFACE_FOREGROUND] =msymbol ("black"); */
  /* mface__default->property[MFACE_BACKGROUND] =msymbol ("white"); */

  mface_normal_video = mface ();
  mface_normal_video->property[MFACE_VIDEOMODE] = (void *) Mnormal;

  mface_reverse_video = mface ();
  mface_reverse_video->property[MFACE_VIDEOMODE] = (void *) Mreverse;

  {
    MFaceHLineProp *hline_prop;

    MSTRUCT_MALLOC (hline_prop, MERROR_FACE);
    hline_prop->type = MFACE_HLINE_UNDER;
    hline_prop->width = 1;
    hline_prop->color = Mnil;
    mface_underline = mface ();
    mface_underline->property[MFACE_HLINE] = (void *) hline_prop;
  }

  mface_medium = mface ();
  mface_medium->property[MFACE_WEIGHT] = (void *) msymbol ("medium");
  mface_bold = mface ();
  mface_bold->property[MFACE_WEIGHT] = (void *) msymbol ("bold");
  mface_italic = mface ();
  mface_italic->property[MFACE_STYLE] = (void *) msymbol ("i");
  mface_bold_italic = mface_copy (mface_bold);
  mface_bold_italic->property[MFACE_STYLE]
    = mface_italic->property[MFACE_STYLE];

  mface_xx_small = mface ();
  mface_xx_small->property[MFACE_RATIO] = (void *) 50;
  mface_x_small = mface ();
  mface_x_small->property[MFACE_RATIO] = (void *) 67;
  mface_small = mface ();
  mface_small->property[MFACE_RATIO] = (void *) 75;
  mface_normalsize = mface ();
  mface_normalsize->property[MFACE_RATIO] = (void *) 100;
  mface_large = mface ();
  mface_large->property[MFACE_RATIO] = (void *) 120;
  mface_x_large = mface ();
  mface_x_large->property[MFACE_RATIO] = (void *) 150;
  mface_xx_large = mface ();
  mface_xx_large->property[MFACE_RATIO] = (void *) 200;

  mface_black = mface ();
  mface_black->property[MFACE_FOREGROUND] = (void *) msymbol ("black");
  mface_white = mface ();
  mface_white->property[MFACE_FOREGROUND] = (void *) msymbol ("white");
  mface_red = mface ();
  mface_red->property[MFACE_FOREGROUND] = (void *) msymbol ("red");
  mface_green = mface ();
  mface_green->property[MFACE_FOREGROUND] = (void *) msymbol ("green");
  mface_blue = mface ();
  mface_blue->property[MFACE_FOREGROUND] = (void *) msymbol ("blue");
  mface_cyan = mface ();
  mface_cyan->property[MFACE_FOREGROUND] = (void *) msymbol ("cyan");
  mface_yellow = mface ();
  mface_yellow->property[MFACE_FOREGROUND] = (void *) msymbol ("yellow");
  mface_magenta = mface ();
  mface_magenta->property[MFACE_FOREGROUND] = (void *) msymbol ("magenta");

  work_gstring.glyphs = malloc (sizeof (MGlyph) * 2);
  work_gstring.size = 2;
  work_gstring.used = 0;
  work_gstring.inc = 1;
  return 0;
}

void
mface__fini ()
{
  M17N_OBJECT_UNREF (mface__default);
  M17N_OBJECT_UNREF (mface_normal_video);
  M17N_OBJECT_UNREF (mface_reverse_video);
  M17N_OBJECT_UNREF (mface_underline);
  M17N_OBJECT_UNREF (mface_medium);
  M17N_OBJECT_UNREF (mface_bold);
  M17N_OBJECT_UNREF (mface_italic);
  M17N_OBJECT_UNREF (mface_bold_italic);
  M17N_OBJECT_UNREF (mface_xx_small);
  M17N_OBJECT_UNREF (mface_x_small);
  M17N_OBJECT_UNREF (mface_small);
  M17N_OBJECT_UNREF (mface_normalsize);
  M17N_OBJECT_UNREF (mface_large);
  M17N_OBJECT_UNREF (mface_x_large);
  M17N_OBJECT_UNREF (mface_xx_large);
  M17N_OBJECT_UNREF (mface_black);
  M17N_OBJECT_UNREF (mface_white);
  M17N_OBJECT_UNREF (mface_red);
  M17N_OBJECT_UNREF (mface_green);
  M17N_OBJECT_UNREF (mface_blue);
  M17N_OBJECT_UNREF (mface_cyan);
  M17N_OBJECT_UNREF (mface_yellow);
  M17N_OBJECT_UNREF (mface_magenta);
  free (work_gstring.glyphs);

  mdebug__report_object ("Face", &face_table);
}

/** Return a realized face for ASCII characters from NUM number of
    base faces pointed by FACES on the frame FRAME.  */

MRealizedFace *
mface__realize (MFrame *frame, MFace **faces, int num,
		MSymbol language, MSymbol charset, int size)
{
  MRealizedFace *rface;
  MRealizedFont *rfont;
  MFace merged_face = *(frame->face);
  void **props;
  int i, j;
  unsigned tick;
  MGlyph g;

  if (num == 0 && language == Mnil && charset == Mnil && frame->rface)
    return frame->rface;

  for (i = 0; i < MFACE_PROPERTY_MAX; i++)
    for (j = num - 1; j >= 0; j--)
      if (faces[j]->property[i])
	{
	  merged_face.property[i] = faces[j]->property[i];
	  break;
	}

  for (i = 0, tick = 0; i < num; i++)
    tick += faces[i]->tick;

  if (merged_face.property[MFACE_RATIO])
    {
      int font_size = (int) merged_face.property[MFACE_SIZE];

      font_size *= (int) merged_face.property[MFACE_RATIO];
      font_size /= 100;
      merged_face.property[MFACE_SIZE] = (void *) font_size;
    }

  rface = find_realized_face (frame, &merged_face, NULL);
  if (rface && rface->tick == tick)
    return rface->ascii_rface;

  MSTRUCT_CALLOC (rface, MERROR_FACE);
  rface->frame = frame;
  rface->face = merged_face;
  rface->tick = tick;
  props = rface->face.property;

  rface->rfontset = mfont__realize_fontset (frame,
					    (MFontset *) props[MFACE_FONTSET],
					    &merged_face);
  g.c = ' ';
  num = 1;
  rfont = mfont__lookup_fontset (rface->rfontset, &g, &num,
				 msymbol ("latin"), language, Mnil,
				 size);

  if (rfont)
    {
      rface->rfont = rfont;
      g.otf_encoded = 0;
      work_gstring.glyphs[0] = g;
      work_gstring.glyphs[0].rface = rface;
      work_gstring.glyphs[1].code = MCHAR_INVALID_CODE;
      work_gstring.glyphs[1].rface = rface;
      mfont__get_metric (&work_gstring, 0, 2);
      rface->space_width = work_gstring.glyphs[0].width;
      rface->ascent = work_gstring.glyphs[1].ascent;
      rface->descent = work_gstring.glyphs[1].descent;
    }
  else
    {
      rface->rfont = NULL;
      rface->space_width = frame->space_width;
    }

  rface->hline = (MFaceHLineProp *) props[MFACE_HLINE];
  rface->box = (MFaceBoxProp *) props[MFACE_BOX];
  rface->ascii_rface = rface;
  mwin__realize_face (rface);

  mplist_add (frame->realized_face_list, Mt, rface);

  if (rface->rfont)
    {
      MSTRUCT_CALLOC (rface->nofont_rface, MERROR_FACE);
      *rface->nofont_rface = *rface;
      rface->nofont_rface->rfont = NULL;
    }
  else
    rface->nofont_rface = rface;

  return rface;
}


MGlyph *
mface__for_chars (MSymbol script, MSymbol language, MSymbol charset,
		  MGlyph *from_g, MGlyph *to_g, int size)
{
  MRealizedFace *rface;
  MRealizedFont *rfont;
  int num = to_g - from_g, i;

  rfont = mfont__lookup_fontset (from_g->rface->rfontset, from_g, &num,
				 script, language, charset, size);
  if (! rfont)
    {
      from_g->rface = from_g->rface->nofont_rface;
      return (from_g + 1);
    }
  rface = find_realized_face (from_g->rface->frame, &(from_g->rface->face),
			      rfont);
  if (! rface)
    {
      MSTRUCT_MALLOC (rface, MERROR_FACE);
      *rface = *from_g->rface->ascii_rface;
      rface->rfont = rfont;
      {
	work_gstring.glyphs[0].code = MCHAR_INVALID_CODE;
	work_gstring.glyphs[0].rface = rface;
	mfont__get_metric (&work_gstring, 0, 1);
	rface->ascent = work_gstring.glyphs[0].ascent;
	rface->descent = work_gstring.glyphs[0].descent;
      }
      mwin__realize_face (rface);
      mplist_add (from_g->rface->frame->realized_face_list, Mt, rface);
    }

  for (i = 0; i < num; i++, from_g++)
    from_g->rface = rface;
  return from_g;
}


void
mface__free_realized (MRealizedFace *rface)
{
  mwin__free_realized_face (rface);
  if (rface == rface->ascii_rface)
    {
      if (! rface->nofont_rface)
	mdebug_hook ();
      else
	free (rface->nofont_rface);
      rface->nofont_rface = NULL;
    }
  free (rface);
}

/*** @} */
#endif /* !FOR_DOXYGEN || DOXYGEN_INTERNAL_MODULE */


/* External API  */
/*** @addtogroup m17nFace */
/*** @{ */
/*=*/

/***en @name Variables: Keys of face property */
/***ja @name �ѿ�: �ե������ץ�ѥƥ��Υ���  */
/*** @{ */
/*=*/

/***en
    @brief Key of a face property specifying foreground color

    The variable #Mforeground is used as a key of face property.  The
    property value must be a symbol whose name is a color name, or
    #Mnil.

    #Mnil means that the face does not specify a foreground color.
    Otherwise, the foreground of an M-text is drawn by the specified
    color.  */

/***ja
    @brief ���ʿ�����ꤹ��ե������ץ�ѥƥ����Υ���

    �ѿ� #Mforeground �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣
    �ץ�ѥƥ����ͤϡ���̾��̾���Ȥ��ƻ��ĥ���ܥ뤫 #Mnil �Ǥ��롣

    #Mnil �ξ�硢���ʿ��ϻ��ꤵ��ʤ���������ʤ���� M-text �����ʤ�
    ���ꤵ�줿����ɽ������롣  */

MSymbol Mforeground;

/***en
    @brief Key of a face property specifying background color

    The variable #Mbackground is used as a key of face property.  The
    property value must be a symbol whose name is a color name, or
    #Mnil.

    #Mnil means that the face does not specify a background color.
    Otherwise, the background of an M-text is drawn by the specified
    color.  */

/***ja
    @brief �طʿ�����ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mbackground �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣
    �ץ�ѥƥ����ͤϡ���̾��̾���Ȥ��ƻ��ĥ���ܥ뤫 #Mnil �Ǥ��롣

    #Mnil �ξ�硢�طʿ��ϻ��ꤵ��ʤ���������ʤ���� M-text ���طʤ�
    ���ꤵ�줿����ɽ������롣  */

MSymbol Mbackground;

/***en
    @brief Key of a face property specifying video mode

    The variable #Mvideomode is used as a key of face property.  The
    property value must be #Mnormal, #Mreverse, or #Mnil.

    #Mnormal means that an M-text is drawn in normal video mode
    (i.e. the foreground is drawn by foreground color, the background
    is drawn by background color).

    #Mreverse means that an M-text is drawn in reverse video mode
    (i.e. the foreground is drawn by background color, the background
    is drawn by foreground color).

    #Mnil means that the face does not specify a video mode.  */

/***ja
    @brief �ӥǥ��⡼�ɤ���ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mvideomode �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣
    �ץ�ѥƥ����ͤϡ�#Mnormal, #Mreverse, #Mnil �Τ����줫�Ǥʤ��ƤϤʤ�ʤ���

    #Mnormal �ξ��ϡ�M-text ��ɸ��Υӥǥ��⡼�ɡ����ʤ����ʿ��ǡ���
    �ʤ��طʿ��ǡˤ�ɽ�����롣

    #Mreverse �ξ��ϥ�С����ӥǥ��⡼�ɤǡ����ʤ��طʿ��ǡ��طʤ���
    �ʿ��ǡ�ɽ�����롣

    #Mnil �ξ��ϥӥǥ��⡼�ɤϻ��ꤵ��ʤ���
    */

MSymbol Mvideomode;

/***en
    @brief Key of a face property specifying font size ratio

    The variable #Mratio is used as a key of face property.  The value
    RATIO must be an integer.

    The value 0 means that the face does not specify a font size
    ratio.  Otherwise, an M-text is drawn by a font of size (FONTSIZE
    * RATIO / 100) where FONTSIZE is a font size specified by the face
    property #Msize.  */
/***ja
    @brief ����������Ψ����ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mratio �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�� RATIO 
    �������ͤǤʤ��ƤϤʤ�ʤ���

    �ͤ�0�ʤ�С��ե���ȥ������ϻ��ꤵ��ʤ��������Ǥʤ���С�M-text 
    ��(FONTSIZE * RATIO / 100) �Ȥ����������Υե���Ȥ�ɽ������롣��
    ���� FONTSIZE �ϥե������ץ�ѥƥ��� #Msize �ǻ��ꤵ�줿�������Ǥ�
    �롣 */

MSymbol Mratio;

/***en
    @brief Key of a face property specifying horizontal line

    The variable #Mhline is used as a key of face property.  The value
    must be a pointer to an object of type #MFaceHLineProp, or @c
    NULL.

    The value @c NULL means that the face does not specify this
    property.  Otherwise, an M-text is drawn with a horizontal line by
    a way specified by the object that the value points to.  */

/***ja
    @brief ��ʿ������ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mhline �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤ� 
    #MFaceHLineProp �����֥������ȤؤΥݥ��󥿤� @c NULL �Ǥʤ��ƤϤ�
    ��ʤ���

    �ͤ� @c NULL �ʤ�С����Υץ�ѥƥ��ϻ��ꤵ��ʤ��������Ǥ������
   ���ؤ����֥������Ȥ˻��ꤵ�줿�褦�˿�ʿ�����ղä��� M-text ��ɽ��
   ���롣*/

MSymbol Mhline;

/***en
    @brief Key of a face property specifying box

    The variable #Mbox is used as a key of face property.  The value
    must be a pointer to an object of type #MFaceBoxProp, or @c NULL.

    The value @c NULL means that the face does not specify a box.
    Otherwise, an M-text is drawn with a surrounding box by a way
    specified by the object that the value points to.  */

/***ja
    @brief �Ϥ��Ȥ���ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mbox �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤ� 
    #MFaceBoxProp �����֥������ȤؤΥݥ��󥿤� @c NULL �Ǥʤ��ƤϤʤ�
    �ʤ���

    �ͤ� @c NULL �ʤ�С����Υץ�ѥƥ��ϻ��ꤵ��ʤ��������Ǥ������
    ���ؤ����֥������Ȥ˻��ꤵ�줿�褦�˰Ϥ��Ȥ��ղä��� M-text ��ɽ��
    ���롣*/

MSymbol Mbox;

/***en
    @brief Key of a face property specifying fontset 

    The variable #Mfontset is used as a key of face property.  The
    value must be a pointer to an object of type #Mfontset, or @c
    NULL.

    The value @c NULL means that the face does not specify a fontset.
    Otherwise, an M-text is drawn with a font selected from what
    specified in the fontset.  */

/***ja
    @brief �ե���ȥ��åȤ���ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mfontset �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤ� 
    #Mfontset �����֥������ȤؤΥݥ��󥿤� @c NULL �Ǥʤ��ƤϤʤ�ʤ���

    �ͤ� @c NULL �ʤ�С��ե���ȥ��åȤϻ��ꤵ��ʤ��������Ǥ������
    ���ؤ����֥������Ȥ˻��ꤵ�줿�ե���ȥ��åȤ���������ե���Ȥ� 
    M-text ��ɽ�����롣*/
    
MSymbol Mfontset;

/***en
    @brief Key of a face property specifying hook

    The variable #Mhook_func is used as a key of face property.  The
    value must be a function of type #MFaceHookFunc, or @c NULL.

    The value @c NULL means that the face does not specify a hook.
    Otherwise, the specified function is called before the face is
    realized.  */
/***ja
    @brief �եå�����ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mhook_func �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤ� 
    #MFaceHookFunc ���δؿ��� @c NULL �Ǥʤ��ƤϤʤ�ʤ���

    �ͤ� @c NULL �ʤ�С��եå��ϻ��ꤵ��ʤ��������Ǥ���Хե�������
    �¸��������˻��ꤷ���ؿ����ƤФ�롣      */
MSymbol Mhook_func;

/***en
    @brief Key of a face property specifying argument of hook

    The variable #Mhook_arg is used as a key of face property.  The
    value can be anything that is passed a hook function specified by
    the face property #Mhook_func.  */
/***ja
    @brief �եå��ΰ�������ꤹ�뤿��Υե������ץ�ѥƥ����Υ���

    �ѿ� #Mhook_arg �ϥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤ� 
    ���Ǥ�褯���ե������ץ�ѥƥ� #Mhook_func �ǻ��ꤵ���ؿ����Ϥ�
    ��롣 */
MSymbol Mhook_arg;

/*** @} */
/*=*/

/*** @ingroup m17nFace */
/***en @name Variables: Possible values of #Mvideomode property of face */
/***ja @name �ѿ���  �ե������� #Mvideomode �ץ�ѥƥ��β�ǽ���� */
/*** @{ */
/*=*/

/***en
    See the documentation of the variable #Mvideomode.  */ 
/***ja
    �ѿ� #Mvideomode �������򻲾ȤΤ��ȡ�  */ 
MSymbol Mnormal;
MSymbol Mreverse;
/*** @} */
/*=*/

/*** @ingroup m17nFace */
/***en @name Variables: Predefined faces  */
/***ja @name �ѿ�: ����Ѥߥե�����  */
/*** @{ */
/*=*/

/***en
    @brief Normal video face

    The variable #mface_normal_video points to a face that has the
    #Mvideomode property with value #Mnormal.  The other properties
    are not specified.  An M-text drawn with this face appear normal
    colors (i.e. the foreground is drawn by foreground color, and
    background is drawn by background color).  */
/***en
    @brief ɸ��ӥǥ��ե�����

    �ѿ� #mface_normal_video �� #Mvideomode �ץ�ѥƥ����ͤ� #Mnormal 
    �Ǥ���ե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ���
    ���Υե�������ɽ�������M-text ��ɸ��ο� (���ʤ�����ʤ����ʿ���
    �طʤ��طʿ��ˤ�������롣  */

MFace *mface_normal_video;

/***en
    @brief Reverse video face

    The variable #mface_reverse_video points to a face that has the
    #Mvideomode property with value #Mreverse.  The other properties
    are not specified.  An M-text drawn with this face appear in
    reversed colors (i.e. the foreground is drawn by background
    color, and background is drawn by foreground color).  */
/***ja
    @brief ��С����ӥǥ��ե�����

    �ѿ� #mface_reverse_video �� #Mvideomode �ץ�ѥƥ����ͤ� 
    #Mreverse �Ǥ���ե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ���
    ����ʤ������Υե�������ɽ�������M-text �����ʿ����طʿ���������
    ��ä� (���ʤ�����ʤ��طʿ����طʤ����ʿ���������롣  */

MFace *mface_reverse_video;

/***en
    @brief Underline face

    The variable #mface_underline points to a face that has the
    #Mhline property with value a pointer to an object of type
    #MFaceHLineProp.  The members of the object are as follows:

@verbatim
    member  value
    -----   -----
    type    MFACE_HLINE_UNDER
    width   1
    color   Mnil
@endverbatim

    The other properties are not specified.  An M-text that has this
    face is drawn with an underline.  */ 
/***ja
    @brief �����ե�����

    �ѿ� #mface_underline �� #Mhline �ץ�ѥƥ����ͤ� #MFaceHLineProp 
    �����֥������ȤؤΥݥ��󥿤Ǥ���ե�������ؤ��ݥ��󥿤Ǥ��롣����
    �������ȤΥ��Фϰʲ����̤ꡣ

@verbatim
    ����  ��
    -----   -----
    type    MFACE_HLINE_UNDER
    width   1
    color   Mnil
@endverbatim

    ¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �ϲ����դ�
    ��ɽ������롣*/ 

MFace *mface_underline;

/***en
    @brief Medium face

    The variable #mface_medium points to a face that has the #Mweight
    property with value a symbol of name "medium".  The other
    properties are not specified.  An M-text that has this face is
    drawn with a font of medium weight.  */
/***ja
    @brief �ߥǥ�����ե�����

    �ѿ� #mface_medium �� #Mweight �ץ�ѥƥ����ͤ� "medium" �Ȥ���̾
    �����ĥ���ܥ�Ǥ���褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�
    �ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �ϡ��ߥǥ����०��
    ���ȤΥե���Ȥ�ɽ������롣  */
MFace *mface_medium;

/***en
    @brief Bold face

    The variable #mface_bold points to a face that has the #Mweight
    property with value a symbol of name "bold".  The other properties
    are not specified.  An M-text that has this face is drawn with a
    font of bold weight.  */

/***ja
    @brief �ܡ���ɥե�����

    �ѿ� #mface_bold �� #Mweight �ץ�ѥƥ����ͤ� "bold" �Ȥ���̾����
    ��ĥ���ܥ�Ǥ���褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ�
    �ϻ��ꤵ��ʤ������Υե���������� M-text �ϡ��ܡ���ɤΥե���Ȥ�
    ɽ������롣
     */

MFace *mface_bold;

/***en
    @brief Italic face

    The variable #mface_italic points to a face that has the #Mstyle
    property with value a symbol of name "italic".  The other
    properties are not specified.  An M-text that has this face is
    drawn with a font of italic style.  */

/***ja
    @brief ������å��ե�����

    �ѿ� #mface_italic �� #Mstyle �ץ�ѥƥ����ͤ� "italic" �Ȥ���̾��
    ���ĥ���ܥ�Ǥ���褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ��
    �ƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �ϡ�������å��Τ�ɽ
    ������롣
     */

MFace *mface_italic;

/***en
    @brief Bold italic face

    The variable #mface_bold_italic points to a face that has the
    #Mweight property with value a symbol of name "bold", and #Mstyle
    property with value a symbol of name "italic".  The other
    properties are not specified.  An M-text that has this face is
    drawn with a font of bold weight and italic style.  */

/***ja
    @brief �ܡ���ɥ�����å��ե�����

    �ѿ� #mface_bold_italic �ϡ�#Mweight �ץ�ѥƥ����ͤ� "bold" �Ȥ�
    ��̾�����ĥ���ܥ�Ǥ��ꡢ���� #Mstyle �ץ�ѥƥ����ͤ� "italic" 
    �Ȥ���̾�����ĥ���ܥ�Ǥ���褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣
    ¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �ϡ��ܡ���
    �ɥ�����å��Τ�ɽ������롣
    */

MFace *mface_bold_italic;

/***en
    @brief Smallest face

    The variable #mface_xx_small points to a face that has the #Mratio
    property with value 50.  The other properties are not specified.
    An M-text that has this face is drawn with a font whose size is
    50% of a normal font.  */

/***ja
    @brief �Ǿ��Υե�����

    �ѿ� #mface_xx_small �ϡ�#Mratio �ץ�ѥƥ����ͤ� 50 �Ǥ���ե���
    ����ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե�����
    ����� M-text ��ɸ��� 50% ���礭���Υե���Ȥ��Ѥ���ɽ������롣
     */

MFace *mface_xx_small;

/***en
    @brief Smaller face

    The variable #mface_x_small points to a face that has the #Mratio
    property with value 66.  The other properties are not specified.
    An M-text that has this face is drawn with a font whose size is
    66% of a normal font.  */

/***ja
    @brief ��äȾ������ե�����

    �ѿ� #mface_x_small �ϡ�#Mratio �ץ�ѥƥ����ͤ� 66 �Ǥ���ե���
    ����ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե�����
    ����� M-text ��ɸ��� 66% ���礭���Υե���Ȥ��Ѥ���ɽ������롣
     */

MFace *mface_x_small;

/***en
    @brief Small face

    The variable #mface_x_small points to a face that has the #Mratio
    property with value 75.  The other properties are not specified.
    An M-text that has this face is drawn with a font whose size is
    75% of a normal font.  */

/***ja
    @brief �������ե�����

    �ѿ� #mface_small �ϡ�#Mratio �ץ�ѥƥ����ͤ� 75 �Ǥ���ե�������
    �ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե��������
    �� M-text ��ɸ��� 75% ���礭���Υե���Ȥ��Ѥ���ɽ������롣
     */

MFace *mface_small;

/***en
    @brief Normalsize face

    The variable #mface_normalsize points to a face that has the
    #Mratio property with value 100.  The other properties are not
    specified.  An M-text that has this face is drawn with a font
    whose size is the same as a normal font.  */

/***ja
    @brief ɸ����礭���Υե�����

    �ѿ� #mface_normalsize �ϡ�#Mratio �ץ�ѥƥ����ͤ� 100 �Ǥ���ե���
    ����ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե�����
    ����� M-text ��ɸ���Ʊ���礭���Υե���Ȥ��Ѥ���ɽ������롣
     */

MFace *mface_normalsize;

/***en
    @brief Large face

    The variable #mface_large points to a face that has the #Mratio
    property with value 120.  The other properties are not specified.
    An M-text that has this face is drawn with a font whose size is
    120% of a normal font.  */

/***ja
    @brief �礭���ե�����

    �ѿ� #mface_large �ϡ�#Mratio �ץ�ѥƥ����ͤ� 120 �Ǥ���ե�����
    ��ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե�������
    ���� M-text ��ɸ��� 120% ���礭���Υե���Ȥ��Ѥ���ɽ������롣
     */

MFace *mface_large;

/***en
    @brief Larger face

    The variable #mface_x_large points to a face that has the #Mratio
    property with value 150.  The other properties are not specified.
    An M-text that has this face is drawn with a font whose size is
    150% of a normal font.  */

/***ja
    @brief ��ä��礭���ե�����

    �ѿ� #mface_x_large �ϡ�#Mratio �ץ�ѥƥ����ͤ� 150 �Ǥ���ե���
    ����ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե�����
    ����� M-text ��ɸ��� 150% ���礭���Υե���Ȥ��Ѥ���ɽ������롣
     */

MFace *mface_x_large;

/***en
    @brief Largest face

    The variable #mface_xx_large points to a face that has the #Mratio
    property with value 200.  The other properties are not specified.
    An M-text that has this face is drawn with a font whose size is
    200% of a normal font.  */

/***ja
    @brief ����Υե�����

    �ѿ� #mface_xx_large �ϡ�#Mratio �ץ�ѥƥ����ͤ� 200 �Ǥ���ե���
    ����ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե�����
    ����� M-text ��ɸ��� 200% ���礭���Υե���Ȥ��Ѥ���ɽ������롣
     */

MFace *mface_xx_large;

/***en
    @brief Black face

    The variable #mface_black points to a face that has the
    #Mforeground property with value a symbol of name "black".  The
    other properties are not specified.  An M-text that has this face
    is drawn with black foreground.  */

/***ja @brief ���ե�����

    �ѿ� #mface_black �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� "black" ��
    ����̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υ�
    ��ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �����ʿ�����Ȥ�
    ��ɽ������롣     */

MFace *mface_black;

/***en
    @brief White face

    The variable #mface_white points to a face that has the
    #Mforeground property with value a symbol of name "white".  The
    other properties are not specified.  An M-text that has this face
    is drawn with white foreground.  */

/***ja
    @brief ��ե�����

    �ѿ� #mface_white �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� "white" ��
    ����̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υ�
    ��ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �����ʿ�����Ȥ�
    ��ɽ������롣  */

MFace *mface_white;

/***en
    @brief Red face

    The variable #mface_red points to a face that has the
    #Mforeground property with value a symbol of name "red".  The
    other properties are not specified.  An M-text that has this face
    is drawn with red foreground.  */

/***ja
    @brief �֥ե�����

    �ѿ� #mface_red �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� "red" �Ȥ���
    ̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ��
    �ƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �����ʿ����֤Ȥ���ɽ
    ������롣  */

MFace *mface_red;

/***en
    @brief Green face

    The variable #mface_green points to a face that has the
    #Mforeground property with value a symbol of name "green".  The
    other properties are not specified.  An M-text that has this face
    is drawn with green foreground.  */

/***ja
    @brief �Хե�����

    �ѿ� #mface_green �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� "green" ��
    ����̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υ�
    ��ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �����ʿ����ФȤ�
    ��ɽ������롣  */

MFace *mface_green;

/***en
    @brief Blue face

    The variable #mface_blue points to a face that has the
    #Mforeground property with value a symbol of name "blue".  The
    other properties are not specified.  An M-text that has this face
    is drawn with blue foreground.  */

/***ja
    @brief �ĥե�����

    �ѿ� #mface_blue �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� "blue" �Ȥ�
    ��̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�
    �ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �����ʿ����ĤȤ���
    ɽ������롣  */

MFace *mface_blue;

/***en
    @brief Cyan face

    The variable #mface_cyan points to a face that has the
    #Mforeground property with value a symbol of name "cyan".  The
    other properties are not specified.  An M-text that has this face
    is drawn with cyan foreground.  */

/***ja
    @brief ������ե�����

    �ѿ� #mface_cyan �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� "cyan" �Ȥ�
    ��̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾�Υץ�
    �ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �����ʿ��򥷥����
    ����ɽ������롣  */

MFace *mface_cyan;

/***en
    @brief yellow face

    The variable #mface_yellow points to a face that has the
    #Mforeground property with value a symbol of name "yellow".  The
    other properties are not specified.  An M-text that has this face
    is drawn with yellow foreground.  */

/***ja
    @brief ���ե�����

    �ѿ� #mface_yellow �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� "yellow" 
    �Ȥ���̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤Ǥ��롣¾��
    �ץ�ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text �����ʿ��򲫿�
    �Ȥ���ɽ������롣  */

MFace *mface_yellow;

/***en
    @brief Magenta face

    The variable #mface_magenta points to a face that has the
    #Mforeground property with value a symbol of name "magenta".  The
    other properties are not specified.  An M-text that has this face
    is drawn with magenta foreground.  */

/***ja
    @brief �ޥ��󥿥ե�����

    �ѿ� #mface_magenta �ϡ�#Mforeground �ץ�ѥƥ����ͤȤ��� 
    "magenta" �Ȥ���̾���Υ���ܥ����Ĥ褦�ʥե�������ؤ��ݥ��󥿤�
    ���롣¾�Υץ�ѥƥ��ϻ��ꤵ��ʤ������Υե���������� M-text ����
    �ʿ���ޥ��󥿤Ȥ���ɽ������롣  */

MFace *mface_magenta;

/*** @} */
/*=*/

/***en @name Variables: The other symbols for face handling.  */
/***ja @name �ѿ�: �ե��������갷������Τ���¾�Υ���ܥ�  */
/*** @{ */
/*=*/

/***en
    @brief Key of a text property specifying a face.

    The variable #Mface is a symbol of name <tt>"face"</tt>.  A text
    property whose key is this symbol must have a pointer to an object
    of type #MFace.  This is a managing key.  */

/***ja
    @brief �ե���������ꤹ��ƥ����ȥץ�ѥƥ��Υ���

    �ѿ� #Mface �� <tt>"face"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ��롣��
    �Υ���ܥ�򥭡��Ȥ���ƥ����ȥץ�ѥƥ��ϡ�#MFace ���Υ��֥�����
    �ȤؤΥݥ��󥿤�����ʤ���Фʤ�ʤ�������ϴ��������Ǥ��롣  */

MSymbol Mface;
/*=*/
/*** @} */
/*=*/

/***en
    @brief Create a new face.

    The mface () function creates a new face object that specifies no
    property.

    @return
    This function returns a pointer to the created face.  */

/***ja
    @brief �������ե�������Ĥ��� 

    �ؿ� mface () �ϥץ�ѥƥ�����ڻ����ʤ��������ե��������֥�������
    ���롣

    @return
    ���δؿ��Ϻ�ä��ե������ؤΥݥ��󥿤��֤���  */

MFace *
mface ()
{
  MFace *face;

  M17N_OBJECT (face, free_face, MERROR_FACE);
  M17N_OBJECT_REGISTER (face_table, face);
  return face;
}

/*=*/

/***en
    @brief Make a copy of a face.

    The mface_copy () function makes a copy of $FACE and returns a
    pointer to the created copy.  */

/***ja
    @brief �ե������Υ��ԡ�����.

    �ؿ� mface_copy () �ϥե����� $FACE �Υ��ԡ����ꡢ���Υ��ԡ��ؤ�
    �ݥ��󥿤��֤���  */

MFace *
mface_copy (MFace *face)
{
  MFace *copy;

  MSTRUCT_CALLOC (copy, MERROR_FACE);
  *copy = *face;
  copy->control.ref_count = 1;
  M17N_OBJECT_REGISTER (face_table, copy);
  if (copy->property[MFACE_FONTSET])
    M17N_OBJECT_REF (copy->property[MFACE_FONTSET]);
  if (copy->property[MFACE_HLINE])
    {
      MFaceHLineProp *val;

      MSTRUCT_MALLOC (val, MERROR_FACE); 
      *val = *((MFaceHLineProp *) copy->property[MFACE_HLINE]);
      copy->property[MFACE_HLINE] = val;
    }
  if (copy->property[MFACE_BOX])
    {
      MFaceBoxProp *val;

      MSTRUCT_MALLOC (val, MERROR_FACE); 
      *val = *((MFaceBoxProp *) copy->property[MFACE_BOX]);
      copy->property[MFACE_BOX] = val;
    }

  return copy;
}

/*=*/
/***en
    @brief Merge faces.

    The mface_merge () functions merges the properties of face $SRC
    into $DST.

    @return
    This function returns $DST.  */

/***ja
    @brief �ե����������礹��.

    �ؿ� mface_merge () �ϡ��ե����� $SRC �Υץ�ѥƥ���ե����� $DST 
    �����礹�롣

    @return
    ���δؿ��� $DST ���֤���  */

MFace *
mface_merge (MFace *dst, MFace *src)
{
  int i;

  for (i = 0; i < MFACE_PROPERTY_MAX; i++)
    if (src->property[i])
      {
	dst->property[i] = src->property[i];
	if (i == MFACE_FONTSET)
	  M17N_OBJECT_REF (dst->property[i]);
	else if (i == MFACE_HLINE)
	  {
	    MFaceHLineProp *val;

	    MSTRUCT_MALLOC (val, MERROR_FACE); 
	    *val = *((MFaceHLineProp *) dst->property[MFACE_HLINE]);
	    dst->property[MFACE_HLINE] = val;
	  }
	else if (i == MFACE_BOX)
	  {
	    MFaceBoxProp *val;

	    MSTRUCT_MALLOC (val, MERROR_FACE); 
	    *val = *((MFaceBoxProp *) dst->property[MFACE_BOX]);
	    dst->property[MFACE_BOX] = val;
	  }
      }
  return dst;
}

/*=*/

/***en
    @brief Make a face from a font.

    The mface_from_font () function return a newly created face while
    reflecting the properties of $FONT in its properties.   */

/***ja
    @brief �ե���Ȥ���ե���������.

    �ؿ� mface_from_font () �ϥե���� $FONT �Υץ�ѥƥ���ץ�ѥƥ�
    �Ȥ��ƻ��Ŀ������ե��������ꡢ������֤���  */

MFace *
mface_from_font (MFont *font)
{
  MFace *face = mface ();

  face->property[MFACE_FOUNDRY] = mfont_get_prop (font, Mfoundry);
  face->property[MFACE_FAMILY] = mfont_get_prop (font, Mfamily);
  face->property[MFACE_WEIGHT] = mfont_get_prop (font, Mweight);
  face->property[MFACE_STYLE] = mfont_get_prop (font, Mstyle);
  face->property[MFACE_STRETCH] = mfont_get_prop (font, Mstretch);
  face->property[MFACE_ADSTYLE] = mfont_get_prop (font, Madstyle);
  face->property[MFACE_SIZE] = mfont_get_prop (font, Msize);
  return face;
}

/*=*/

/***en
    @brief Get the value of a face property.

    The mface_get_prop () function returns the value of the face
    property whose key is $KEY in face $FACE.  $KEY must be one of the
    followings:

        #Mforeground, #Mbackground, #Mvideomode, #Mhline, #Mbox,
        #Mfoundry, #Mfamily, #Mweight, #Mstyle, #Mstretch, #Madstyle,
        #Msize, #Mfontset, #Mratio, #Mhook_func, #Mhook_arg

    @return
    The actual type of the returned value depends of $KEY.  See
    documentation of the above keys.  If an error is detected, it
    returns @c NULL and assigns an error code to the external variable
    #merror_code.  */

/***ja
    @brief �ե������Υץ�ѥƥ����ͤ�����

    �ؿ� mface_get_prop () �ϡ��ե����� $FACE �����ĥե������ץ�ѥƥ�
    ���⡢������ $KEY �Ǥ����Τ��ͤ��֤���$KEY �ϲ����Τ����줫�Ǥ�
    ����Фʤ�ʤ���

        #Mforeground, #Mbackground, #Mvideomode, #Mhline, #Mbox,
        #Mfoundry, #Mfamily, #Mweight, #Mstyle, #Mstretch, #Madstyle,
        #Msize, #Mfontset, #Mratio, #Mhook_func, #Mhook_arg

    @return 
    ����ͤη��� $KEY �˰�¸���롣�嵭�Υ����������򻲾Ȥ��뤳
    �ȡ����顼�����Ф��줿���� @c NULL ���֤��������ѿ� #merror_code 
    �˥��顼�����ɤ����ꤹ�롣  */

/***
    @seealso
    mface_put_prop ()

    @errors
    @c MERROR_FACE  */

void *
mface_get_prop (MFace *face, MSymbol key)
{
  int index = (int) msymbol_get (key, M_face_prop_index) - 1;

  if (index < 0)
    MERROR (MERROR_FACE, NULL);
  return face->property[index];
}

/*=*/

/***en
    @brief Set a value of a face property.

    The mface_put_prop () function assigns $VAL to the property whose
    key is $KEY in face $FACE.  $KEY must be one the followings:

        #Mforeground, #Mbackground, #Mvideomode, #Mhline, #Mbox,
        #Mfoundry, #Mfamily, #Mweight, #Mstyle, #Mstretch, #Madstyle,
        #Msize, #Mfontset, #Mratio, #Mhook_func, #Mhook_arg

    Among them, font related properties (#Mfoundry through #Msize) are
    used as the default values when a font in the fontset of $FACE
    does not specify those values.

    The actual type of the returned value depends of $KEY.  See
    documentation of the above keys.

    @return
    If the operation was successful, mface_put_prop returns () 0.
    Otherwise it returns -1 and assigns an error code to the external
    variable #merror_code.  */

/***ja
    @brief �ե������ץ�ѥƥ����ͤ����ꤹ��

    �ؿ� mface_put_prop () �ϡ��ե����� $FACE ��ǥ����� $KEY �Ǥ����
    ��ѥƥ����ͤ� $VAL �����ꤹ�롣$KEY �ϰʲ��Τ����줫�Ǥʤ��ƤϤ�
    ��ʤ���

        #Mforeground, #Mbackground, #Mvideomode, #Mhline, #Mbox,
        #Mfoundry, #Mfamily, #Mweight, #Mstyle, #Mstretch, #Madstyle,
        #Msize, #Mfontset, #Mratio, #Mhook_func, #Mhook_arg.

    �����Τ����Ρ��ե���ȴ�Ϣ�Υץ�ѥƥ� (#Mfamily ���� #Msize 
    �ޤ�) �ϡ��ե������Υե���ȥ��å���Υե���Ȥ˴ؤ���ǥե������
    �Ȥʤꡢ�ġ��Υե���Ȥ��ͤ���ꤷ�ʤ��ä������Ѥ����롣

    ����ͤη��� $KEY �˰�¸���롣�嵭�Υ����������򻲾Ȥ��뤳�ȡ�

    @return
    ����������������硢mface_put_prop () �� 0 ���֤������Ԥ������� 
    -1 ���֤��������ѿ� #merror_code �˥��顼�����ɤ����ꤹ�롣  */

/***
    @seealso
    mface_get_prop ()

    @errors
    @c MERROR_FACE  */

int
mface_put_prop (MFace *face, MSymbol key, void *val)
{
  int index = (int) msymbol_get (key, M_face_prop_index) - 1;

  if (index < 0)
    MERROR (MERROR_FACE, -1);
  if (key == Mfontset)
    M17N_OBJECT_REF (val);
  else if (key == Mhline)
    {
      MFaceHLineProp *newval;

      MSTRUCT_MALLOC (newval, MERROR_FACE);
      *newval = *((MFaceHLineProp *) val);
      val = newval;
    }
  else if (key == Mbox)
    {
      MFaceBoxProp *newval;

      MSTRUCT_MALLOC (newval, MERROR_FACE);
      *newval = *((MFaceBoxProp *) val);
      val = newval;
    }
  face->property[index] = val;
  face->tick++;

  return 0;
}

/*=*/

/***en
    @brief Update a face.

    The mface_update () function update face $FACE on frame $FRAME by
    calling a hook function of $FACE (if any).  */

/***ja
    @brief �ե������򹹿�����.

    �ؿ� mface_update () �ϥե졼�� $FRAME �Υե����� $FACE �� $FACE 
    �Υեå��ؿ���ʤ���С˸Ƥ�ǹ������롣  */

void
mface_update (MFrame *frame, MFace *face)
{
  MFaceHookFunc func = (MFaceHookFunc) face->property[MFACE_HOOK_FUNC];
  MPlist *rface_list;
  MRealizedFace *rface;

  if (func)
    {
      MPLIST_DO (rface_list, frame->realized_face_list)
	{
	  rface = MPLIST_VAL (rface_list);
	  if ((MFaceHookFunc) rface->face.property[MFACE_HOOK_FUNC] == func)
	    (func) (&(rface->face), rface->face.property[MFACE_HOOK_ARG],
		    rface->info);
	}
    }
}
/*=*/

/*** @} */
/*=*/

/*** @addtogroup m17nDebug */
/*** @{  */
/*=*/

/***en
    @brief Dump a face.

    The mdebug_dump_face () function prints face $FACE in a human readable
    way to the stderr.  $INDENT specifies how many columns to indent
    the lines but the first one.

    @return
    This function returns $FACE.  */

/***ja
    @brief �ե����������פ���.

    �ؿ� mdebug_dump_face () �ϥե����� $FACE �� stderr �˿ʹ֤˲��ɤ�
    ���ǰ������롣 $INDENT �ϣ����ܰʹߤΥ���ǥ�Ȥ���ꤹ�롣

    @return
    ���δؿ��� $FACE ���֤���  */

MFace *
mdebug_dump_face (MFace *face, int indent)
{
  char *prefix = (char *) alloca (indent + 1);
  MFont spec;

  memset (prefix, 32, indent);
  prefix[indent] = 0;
  mfont__set_spec_from_face (&spec, face);
  fprintf (stderr, "(face font:\"");
  mdebug_dump_font (&spec);
  fprintf (stderr, "\"\n %s  fore:%s back:%s", prefix,
	   msymbol_name ((MSymbol) face->property[MFACE_FOREGROUND]),
	   msymbol_name ((MSymbol) face->property[MFACE_BACKGROUND]));
  if (face->property[MFACE_FONTSET])
    fprintf (stderr, " non-default-fontset");
  fprintf (stderr, " hline:%s", face->property[MFACE_HLINE] ? "yes" : "no");
  fprintf (stderr, " box:%s)", face->property[MFACE_BOX] ? "yes" : "no");
  return face;
}

/*** @} */

/*
  Local Variables:
  coding: euc-japan
  End:
*/
