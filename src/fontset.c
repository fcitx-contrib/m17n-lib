/* fontset.c -- fontset module.
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
    @addtogroup m17nFontset
    @brief A fontset is an object that maps a character to fonts.

    A @e fontset is an object of the type @c MFontset.  When drawing an
    M-text, a fontset provides rules to select a font for each
    character in the M-text according to the following information.

    @li The script character property of a character.
    @li The language text property of a character.
    @li The charset text property of a character.

    The documentation of mdraw_text () describes how that information is
    used.  */

/***ja @addtogroup m17nFontset 

    @brief �ե���ȥ��åȤ�ʸ������ե���Ȥؤ��б��դ���Ԥ����֥������ȤǤ���.

    @e �ե���ȥ��å� �� @c MFontset ���Υ��֥������ȤǤ��롣M-text ��
    ɽ������ݡ��ե���ȥ��åȤ� M-text ��θġ���ʸ�����Ф��ƤɤΥե�
    ��Ȥ��Ѥ��뤫�ε�§�򡢰ʲ��ξ���˽��ä�Ϳ���롣

    @li ʸ����ʸ���ץ�ѥƥ� "������ץ�"
    @li ʸ���Υƥ����ȥץ�ѥƥ� "����"
    @li ʸ���Υƥ����ȥץ�ѥƥ� "ʸ�����å�"

    �����ξ��󤬤ɤΤ褦���Ѥ����뤫�� mdraw_text () �������򻲾�
    �Τ��ȡ�

    */

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
#include "symbol.h"
#include "plist.h"
#include "character.h"
#include "charset.h"
#include "internal-gui.h"
#include "font.h"
#include "fontset.h"

struct MFontset
{
  M17NObject control;

  /* Name of the fontset.  */
  MSymbol name;

  /* Initialized to 0, and incremented by one each time the fontset is
     modified.  */
  unsigned tick;

  /* Database from which to load the contents of the fontset.  Once
     loaded, this member is set to NULL.  */
  MDatabase *mdb;

  /* SCRIPT vs PER-LANGUAGE (which is a plist LANGUAGE vs FONT-GROUP) */
  MPlist *per_script;

  /* CHARSET vs FONT-GROUP */
  MPlist *per_charset;

  /* FONT-GROUP */
  MPlist *fallback;

  /* Plist of Mt vs font specs. */
  MPlist *font_spec_list;
};

static MFontset *default_fontset;

static MPlist *fontset_list;

struct MRealizedFontset
{
  /* Fontset from which the realized fontset is realized.  */
  MFontset *fontset;

  /* Initialized to <fontset>->tick.  */
  unsigned tick;

  /* Font spec extracted from a face.  */
  MFont spec;

  /* The frame on which the realized fontset is realized.  */
  MFrame *frame;

  MPlist *per_script;

  MPlist *per_charset;

  MPlist *fallback;
};


static MPlist *
load_font_group (MPlist *plist, MPlist *elt, MPlist *spec_list)
{
  MPLIST_DO (elt, elt)
    {
      /* ELT ::= ( FONT-SPEC-LIST [ LAYOUTER ] ) ...  */
      MPlist *elt2, *p;
      MFont font, *spec = NULL;
      MSymbol layouter_name;

      if (! MPLIST_PLIST_P (elt))
	MWARNING (MERROR_FONTSET);
      elt2 = MPLIST_PLIST (elt);
      if (! MPLIST_PLIST_P (elt2))
	MWARNING (MERROR_FONTSET);
      mfont__set_spec_from_plist (&font, MPLIST_PLIST (elt2));
      MPLIST_DO (p, spec_list)
        {
	  if (! memcmp (MPLIST_VAL (p), &font, sizeof (MFont)))
	    {
	      spec = MPLIST_VAL (p);
	      break;
	    }
        }
      if (! spec)
	{
	  MSTRUCT_MALLOC (spec, MERROR_FONTSET);
	  *spec = font;
	  mplist_add (spec_list, Mt, spec);
	}
      elt2 = MPLIST_NEXT (elt2);
      layouter_name = Mt;
      if (MPLIST_SYMBOL_P (elt2))
	layouter_name = MPLIST_SYMBOL (elt2);
      if (layouter_name == Mnil)
	layouter_name = Mt;
      plist = mplist_add (plist, layouter_name, spec);
      continue;
    warning:
      /* ANSI-C requires some statement after a label.  */
      continue;
    }
  return plist;
}

/* Load FONTSET->per_script from the data in FONTSET->mdb.  */

static void
load_fontset_contents (MFontset *fontset)
{
  MPlist *per_script, *per_charset, *fallback, *spec_list, *font_group;
  MSymbol script, lang;
  MPlist *fontset_def, *plist;

  fontset->per_script = per_script = mplist ();
  fontset->per_charset = per_charset = mplist ();
  fontset->fallback = fallback = mplist ();
  fontset->font_spec_list = spec_list = mplist ();
  if (! (fontset_def = (MPlist *) mdatabase_load (fontset->mdb)))
    return;

  MPLIST_DO (plist, fontset_def)
    {
      /* PLIST ::= ( SCRIPT ( LANGUAGE FONT-SPEC-ELT ... ) ... )
		   | (CHARSET FONT-SPEC-ELT ...)
		   | FONT-SPEC-ELT  */
      MPlist *elt;

      if (! MPLIST_PLIST_P (plist))
	MWARNING (MERROR_FONTSET);
      elt = MPLIST_PLIST (plist);
      if (! MPLIST_SYMBOL_P (elt))
	MWARNING (MERROR_FONTSET);
      script = MPLIST_SYMBOL (elt);
      elt = MPLIST_NEXT (elt);
      if (! MPLIST_PLIST_P (elt))
	MWARNING (MERROR_FONTSET);
      if (script == Mnil)
	fallback = load_font_group (fallback, elt, spec_list);
      else if (MPLIST_PLIST_P (MPLIST_PLIST (elt)))
	{
	  font_group = mplist_find_by_key (fontset->per_charset, script);
	  if (! font_group)
	    {
	      font_group = mplist ();
	      per_charset = mplist_add (per_charset, script, font_group);
	    }
	  load_font_group (font_group, elt, spec_list);
	}
      else
	{
	  MPlist *per_lang = mplist_find_by_key (fontset->per_script, script);

	  if (! per_lang)
	    {
	      per_lang = mplist ();
	      per_script = mplist_add (per_script, script, per_lang);
	    }

	  MPLIST_DO (elt, elt)
	    {
	      /* ELT ::= ( LANGUAGE FONT-DEF ...) ... */
	      MPlist *elt2;

	      if (! MPLIST_PLIST_P (elt))
		MWARNING (MERROR_FONTSET);
	      elt2 = MPLIST_PLIST (elt);
	      if (! MPLIST_SYMBOL_P (elt2))
		MWARNING (MERROR_FONTSET);
	      lang = MPLIST_SYMBOL (elt2);
	      if (lang == Mnil)
		lang = Mt;
	      font_group = mplist_find_by_key (per_lang, lang);
	      if (! font_group)
		{
		  font_group = mplist ();
		  mplist_add (per_lang, lang, font_group);
		}
	      elt2 = MPLIST_NEXT (elt2);
	      load_font_group (font_group, elt2, spec_list);
	    }
	}
      continue;

    warning:
      /* ANSI-C requires some statement after a label.  */
      continue;
    }

  M17N_OBJECT_UNREF (fontset_def);
  fontset->mdb = NULL;
}

static void
free_fontset (void *object)
{
  MFontset *fontset = (MFontset *) object;
  MPlist *plist, *pl, *p;

  if (fontset->per_script)
    {
      MPLIST_DO (plist, fontset->per_script)
	{
	  MPLIST_DO (pl, MPLIST_PLIST (plist))
	    {
	      p = MPLIST_PLIST (pl);
	      M17N_OBJECT_UNREF (p);
	    }
	  pl = MPLIST_PLIST (plist);
	  M17N_OBJECT_UNREF (pl);
	}
      M17N_OBJECT_UNREF (fontset->per_script);
    }
  if (fontset->per_charset)
    {
      MPLIST_DO (plist, fontset->per_charset)
	{
	  pl = MPLIST_PLIST (plist);
	  M17N_OBJECT_UNREF (pl);
	}
      M17N_OBJECT_UNREF (fontset->per_charset);
    }
  if (fontset->fallback)
    M17N_OBJECT_UNREF (fontset->fallback);
  plist = mplist_find_by_key (fontset_list, fontset->name);
  if (! plist)
    mdebug_hook ();
  mplist_pop (plist);
  if (fontset->font_spec_list)
    {
      if (((M17NObject *) (fontset->font_spec_list))->ref_count == 1)
	MPLIST_DO (plist, fontset->font_spec_list)
	  free (MPLIST_VAL (plist));
      M17N_OBJECT_UNREF (fontset->font_spec_list);
    }
  free (object);
}

static void
realize_font_group (MFrame *frame, MFont *request, MPlist *font_group,
		    int size)
{
  MPlist *plist = MPLIST_VAL (font_group), *pl, *p;

  mplist_set (font_group, Mnil, NULL);
  MPLIST_DO (pl, plist)
    {
      MSymbol layouter = MPLIST_KEY (pl);
      MFont this_request = *request;
      MRealizedFont *rfont;

      mfont__resize (MPLIST_VAL (pl), &this_request);
      rfont = mfont__select (frame, MPLIST_VAL (pl), &this_request,
			     size, layouter == Mt ? Mnil : layouter);

      if (rfont)
	{
	  MPLIST_DO (p, font_group)
	    if (((MRealizedFont *) (MPLIST_VAL (p)))->score > rfont->score)
	      break;
	  mplist_push (p, Mt, rfont);
	}
    }
}



/* Internal API */

int
mfont__fontset_init ()
{
  Mfontset = msymbol ("fontset");
  Mfontset->managing_key = 1;
  fontset_list = mplist ();
  default_fontset = mfontset ("default");
  if (! default_fontset->mdb)
    {
      MFont font;

      MFONT_INIT (&font);
      mfont_put_prop (&font, Mregistry, msymbol ("iso8859-1"));
      mfontset_modify_entry (default_fontset, Mnil, Mnil, Mnil,
			     &font, Mnil, 1);
      mfont_put_prop (&font, Mregistry, msymbol ("iso10646-1"));
      mfontset_modify_entry (default_fontset, Mnil, Mnil, Mnil,
			     &font, Mnil, 1);
    }
  return 0;
}


void
mfont__fontset_fini ()
{
  while (! MPLIST_TAIL_P (fontset_list))
    free_fontset ((MFontset *) MPLIST_VAL (fontset_list));
  M17N_OBJECT_UNREF (fontset_list);
  fontset_list = NULL;
}


MRealizedFontset *
mfont__realize_fontset (MFrame *frame, MFontset *fontset, MFace *face)
{
  MRealizedFontset *realized;
  MFont request;
  MPlist *per_script, *per_lang, *per_charset, *font_group;
  MPlist *plist, *pl, *p;

  if (fontset->mdb)
    load_fontset_contents (fontset);

  mfont__set_spec_from_face (&request, face);
  if (request.property[MFONT_SIZE] <= 0)
    {
      mdebug_hook ();
      request.property[MFONT_SIZE] = 120;
    }
  MPLIST_DO (p, frame->realized_fontset_list)
    {
      realized = (MRealizedFontset *) MPLIST_VAL (p);
      if (fontset->name == MPLIST_KEY (p)
	  && ! memcmp (&request, &realized->spec, sizeof (request)))
	return realized;
    }

  MSTRUCT_MALLOC (realized, MERROR_FONTSET);
  realized->fontset = fontset;
  realized->tick = fontset->tick;
  realized->spec = request;
  realized->frame = frame;
  realized->per_script = per_script = mplist ();
  MPLIST_DO (plist, fontset->per_script)
    {
      per_lang = mplist ();
      per_script = mplist_add (per_script, MPLIST_KEY (plist), per_lang);
      MPLIST_DO (pl, MPLIST_PLIST (plist))
	{
	  font_group = mplist ();
	  mplist_add (font_group, Mplist, MPLIST_VAL (pl));
	  per_lang = mplist_add (per_lang, MPLIST_KEY (pl), font_group);
	}
    }

  realized->per_charset = per_charset = mplist ();
  MPLIST_DO (plist, fontset->per_charset)
    {
      font_group = mplist ();
      mplist_add (font_group, Mplist, MPLIST_VAL (plist));
      per_charset = mplist_add (per_charset, MPLIST_KEY (plist), font_group);
    }

  realized->fallback = mplist ();
  mplist_add (realized->fallback, Mplist, fontset->fallback);

  mplist_add (frame->realized_fontset_list, fontset->name, realized);
  return realized;
}


void
mfont__free_realized_fontset (MRealizedFontset *realized)
{
  MPlist *plist, *pl, *p;
  MRealizedFont *rfont;

  if (realized->per_script)
    {
      MPLIST_DO (plist, realized->per_script)
	{
	  MPLIST_DO (pl, MPLIST_PLIST (plist))
	    {
	      MPLIST_DO (p, MPLIST_PLIST (pl))
		if ((rfont = MPLIST_VAL (p)) && ! rfont->frame)
		  free (rfont);
	      p = MPLIST_PLIST (pl);
	      M17N_OBJECT_UNREF (p);
	    }
	  pl = MPLIST_PLIST (plist);
	  M17N_OBJECT_UNREF (pl);
	}
      M17N_OBJECT_UNREF (realized->per_script);
    }
  if (realized->per_charset)
    {
      MPLIST_DO (plist, realized->per_charset)
	{
	  MPLIST_DO (pl, MPLIST_PLIST (plist))
	    if ((rfont = MPLIST_VAL (pl)) && ! rfont->frame)
	      free (rfont);
	  pl = MPLIST_PLIST (plist);
	  M17N_OBJECT_UNREF (pl);
	}
      M17N_OBJECT_UNREF (realized->per_charset);
    }
  if (realized->fallback)
    {
      MPLIST_DO (plist, realized->fallback)
	if ((rfont = MPLIST_VAL (plist)) && ! rfont->frame)
	  free (rfont);
      M17N_OBJECT_UNREF (realized->fallback);
    }

  free (realized);
}


MRealizedFont *
mfont__lookup_fontset (MRealizedFontset *realized, MGlyph *g, int *num,
		       MSymbol script, MSymbol language, MSymbol charset,
		       int size)
{
  MFrame *frame = realized->frame;
  MCharset *preferred_charset = (charset == Mnil ? NULL : MCHARSET (charset));
  MPlist *per_charset, *per_script, *per_lang;
  MPlist *font_groups[256], *plist;
  int n_font_group = 0;
  MRealizedFont *rfont;
  int i;

  if (preferred_charset
      && (per_charset = mplist_get (realized->per_charset, charset)) != NULL)
    font_groups[n_font_group++] = per_charset;
  if (script != Mnil
      && ((per_script = mplist_find_by_key (realized->per_script, script))
	  != NULL))
    {
      /* We prefer font groups in this order:
	  (1) group matching LANGUAGE
	  (2) group for generic LANGUAGE
	  (3) group non-matching LANGUAGE  */
      if (language == Mnil)
	language = Mt;
      per_lang = mplist_find_by_key (MPLIST_PLIST (per_script), language);
      if (per_lang)
	{
	  font_groups[n_font_group++] = MPLIST_PLIST (per_lang);
	  if (language == Mt)
	    {
	      MPLIST_DO (per_lang, MPLIST_PLIST (per_script))
		if (MPLIST_KEY (per_lang) != language)
		  font_groups[n_font_group++] = MPLIST_PLIST (per_lang);
	    }
	}
      if (language != Mt)
	{
	  plist = mplist_get (MPLIST_PLIST (per_script), Mt);
	  if (plist)
	    font_groups[n_font_group++] = plist;
	}
      MPLIST_DO (per_lang, MPLIST_PLIST (per_script))
	if (MPLIST_KEY (per_lang) != language
	    && MPLIST_KEY (per_lang) != Mt)
	  font_groups[n_font_group++] = MPLIST_PLIST (per_lang);
    }
  font_groups[n_font_group++] = realized->fallback;

  if (n_font_group == 1)
    {
      /* As we only have a fallback font group, try all the other
	 fonts too.  */
      MPLIST_DO (per_script, realized->per_script)
	MPLIST_DO (per_lang, MPLIST_PLIST (per_script))
	  font_groups[n_font_group++] = MPLIST_PLIST (per_lang);
      MPLIST_DO (per_charset, realized->per_charset)
	font_groups[n_font_group++] = MPLIST_PLIST (per_charset);
    }

  for (i = 0; i < n_font_group; i++)
    {
      int j;
      
      if (MPLIST_PLIST_P (font_groups[i]))
	realize_font_group (frame, &realized->spec, font_groups[i], size);

      MPLIST_DO (plist, font_groups[i])
        {
	  rfont = (MRealizedFont *) MPLIST_VAL (plist);
	  if (rfont->status < 0)
	    continue;
	  /* Check if this font can display all glyphs.  */
	  for (j = 0; j < *num; j++)
	    {
	      g[j].code = mfont__encode_char (rfont, g[j].c);
	      if (g[j].code == MCHAR_INVALID_CODE)
		break;
	    }
	  if (j == *num)
	    {
	      if (rfont->status > 0
		  || mfont__open (rfont) == 0)
		/* We found a font that can display all glyphs.  */
		break;
	    }
	}
      if (! MPLIST_TAIL_P (plist))
	break;
    }

  if (i < n_font_group) 
    return rfont;

  /* We couldn't find a font that can display all glyphs.  Find one
     that can display at least the first glyph.  */
  for (i = 0; i < n_font_group; i++)
    {
      MPLIST_DO (plist, font_groups[i])
	{
	  rfont = (MRealizedFont *) MPLIST_VAL (plist);
	  if (rfont->status < 0)
	    continue;
	  g->code = mfont__encode_char (rfont, g->c);
	  if (g->code != MCHAR_INVALID_CODE)
	    {
	      if (rfont->status > 0
		  || mfont__open (rfont) == 0)
		break;
	    }
	}
      if (! MPLIST_TAIL_P (plist))
	break;
    }
  return (i < n_font_group ? rfont : NULL);
}

/*** @} */
#endif /* !FOR_DOXYGEN || DOXYGEN_INTERNAL_MODULE */


/* External API */

/*** @addtogroup m17nFontset */
/*** @{ */

/*=*/
/***en
    @brief Return a fontset.

    The mfontset () function returns a pointer to a fontset object of
    name $NAME.  If $NAME is @c NULL, it returns a pointer to the
    default fontset.

    If no fontset has the name $NAME, a new one is created.  At that
    time, if there exists a data \<@c fontset, $NAME\> in the m17n
    database, the fontset contents are initialized according to the
    data.  If no such data exists, the fontset contents are left
    vacant.

    The macro M17N_INIT () creates the default fontset.  An
    application program can modify it before the first call of 
    mframe ().

    @return
    This function returns a pointer to the found or newly created
    fontset.  */
/***ja 
    @brief �ե���ȥ��åȤ��֤�.

    �ؿ� mfontset () ��̾�� $NAME ����ĥե���ȥ��åȥ��֥������Ȥؤ�
    �ݥ��󥿤��֤��� $NAME �� @c NULL �ʤ�С��ǥե���ȥե���ȥ��å�
    �ؤΥݥ��󥿤��֤���

    $NAME �Ȥ���̾������ĥե���ȥ��åȤ��ʤ���С���������Τ�����
    �롣���κݡ�m17n �ǡ����١����� \<@c fontset, $NAME\> �Ȥ����ǡ���
    ������С��ե���ȥ��åȤϤ��Υǡ����˱�äƽ��������롣�ʤ���С�
    ���Τޤޤˤ���롣

    �ޥ��� M17N_INIT () �ϥǥե���ȤΥե���ȥ��åȤ��롣���ץꥱ��
    �����ץ����� mframe () ����ƸƤ֤ޤǤϥǥե���ȥե����
    ���åȤ��ѹ����뤳�Ȥ��Ǥ��롣

    @return
    ���δؿ��ϸ��Ĥ��ä������뤤�Ϻ�ä��ե���ȥ��åȤؤΥݥ��󥿤��֤���
     */

MFontset *
mfontset (char *name)
{
  MSymbol sym;
  MFontset *fontset;

  if (! name)
    fontset = default_fontset;
  else
    {
      sym = msymbol (name);
      fontset = mplist_get (fontset_list, sym);
      if (! fontset)
	{
	  M17N_OBJECT (fontset, free_fontset, MERROR_FONTSET);
	  fontset->name = sym;
	  fontset->mdb = mdatabase_find (Mfontset, sym, Mnil, Mnil);
	  if (! fontset->mdb)
	    {
	      fontset->per_script = mplist ();
	      fontset->per_charset = mplist ();
	      fontset->fallback = mplist ();
	    }
	  mplist_put (fontset_list, sym, fontset);
	}
    }
  M17N_OBJECT_REF (fontset);
  return fontset;
}

/*=*/

/***en
    @brief Return the name of a fontset.

    The mfontset_name () function returns the name of fontset $FONTSET.  */
/***ja
    @brief �ե���ȥ��åȤ�̾�����֤�.

    �ؿ� mfontset_name () �ϥե���ȥ��å� $FONTSET ��̾�����֤���  */
MSymbol
mfontset_name (MFontset *fontset)
{
  return fontset->name;
}

/*=*/

/***en
    @brief Make a copy of a fontset.

    The mfontset_copy () function makes a copy of fontset $FONTSET, gives it a
    name $NAME, and returns a pointer to the created copy.  $NAME must
    not be a name of existing fontset.  In such case, this function
    returns NULL without making a copy.  */
/***ja
    @brief �ե���ȥ��åȤΥ��ԡ�����.

    �ؿ� mfontset_copy () �ϥե���ȥ��å� $FONTSET �Υ��ԡ����äơ�
    ̾�� $NAME ��Ϳ�������Υ��ԡ��ؤΥݥ��󥿤��֤���$NAME �ϴ�¸��
    �ե���ȥ��åȤ�̾���Ǥ��äƤϤʤ�ʤ������ξ��ˤϥ��ԡ����餺
    NULL ���֤���  */

MFontset *
mfontset_copy (MFontset *fontset, char *name)
{
  MSymbol sym = msymbol (name);
  MFontset *copy = mplist_get (fontset_list, sym);
  MPlist *plist, *pl;

  if (copy)
    return NULL;
  M17N_OBJECT (copy, free_fontset, MERROR_FONTSET);
  copy->name = sym;

  if (fontset->per_script)
    {
      copy->per_script = mplist ();
      MPLIST_DO (plist, fontset->per_script)
        {
	  MPlist *new = mplist ();

	  MPLIST_DO (pl, MPLIST_PLIST (plist))
	    mplist_add (new, MPLIST_KEY (pl), mplist_copy (MPLIST_PLIST (pl)));
	  mplist_add (copy->per_script, MPLIST_KEY (plist), new);
	}
    }
  if (fontset->per_charset)
    {
      copy->per_charset = mplist ();
      MPLIST_DO (plist, fontset->per_charset)
	mplist_add (copy->per_charset, MPLIST_KEY (plist),
		    mplist_copy (MPLIST_PLIST (plist)));
    }
  if (fontset->fallback)
    copy->fallback = mplist_copy (fontset->fallback);

  copy->font_spec_list = fontset->font_spec_list;
  M17N_OBJECT_REF (copy->font_spec_list);

  mplist_put (fontset_list, sym, copy);
  M17N_OBJECT_REF (copy);
  return copy;
}

/*=*/

/***en
    @brief Modify the contents of a fontset.

    The mfontset_modify_entry () function associates, in fontset
    $FONTSET, a copy of $FONT with the $SCRIPT / $LANGUAGE pair or
    with $CHARSET.

    Each font in a fontset is associated with a particular
    script/language pair, with a particular charset, or with the
    symbol @c Mnil.  The fonts that are associated with the same item
    make a group.

    If $SCRIPT is not @c Mnil, it must be a symbol identifying a
    script.  In this case, $LANGUAGE is either a symbol identifying a
    language or @c Mnil, and $FONT is associated with the $SCRIPT /
    $LANGUAGE pair.

    If $CHARSET is not @c Mnil, it must be a symbol representing a
    charset object.  In this case, $FONT is associated with that
    charset.

    If both $SCRIPT and $CHARSET are not @c Mnil, two copies of $FONT
    are created.  Then one is associated with the $SCRIPT / $LANGUAGE
    pair and the other with that charset.

    If both $SCRIPT and $CHARSET are @c Mnil, $FONT is associated with
    @c Mnil.  This kind of fonts are called @e fallback @e fonts.

    The argument $HOW specifies the priority of $FONT.  If $HOW is
    positive, $FONT has the highest priority in the group of fonts
    that are associated with the same item.  If $HOW is negative,
    $FONT has the lowest priority.  If $HOW is zero, $FONT becomes the
    only available font for the associated item; all the other fonts
    are removed from the group.

    If $LAYOUTER_NAME is not @c Mnil, it must be a symbol
    representing a @ref flt.  In that case, if $FONT is selected for
    drawing an M-text, that font layout table is used to generate a
    glyph code sequence from a character sequence.

    @return
    If the operation was successful, mfontset_modify_entry () returns 0.
    Otherwise it returns -1 and assigns an error code to the external
    variable #merror_code.  */

/***ja
    @brief �ե���ȥ��åȤ����Ƥ��ѹ�����.

    �ؿ� mfontset_modify_entry () �ϡ�$LANGUAGE �� $SCRIPT ���Ȥ߹��
    ���ޤ��� $CHARSET ���Ф��� $FONT �Υ��ԡ���Ȥ��褦�ˡ��ե���ȥ���
    �� $FONTSET �����ꤹ�롣

    �ե���ȥ��åȤγƥե���Ȥϡ�����Υ�����ץȤȸ���Υڥ��������
    ʸ�����åȡ�����ܥ� @c Mnil �Τ����줫�ȴ�Ϣ�դ����Ƥ��롣Ʊ��
    ��Τȴ�Ϣ�դ���줿�ե���Ȥϥ��롼�פ������롣

    $SCRIPT �� @c Mnil �Ǥ��뤫��������ץȤ����ꤹ�륷��ܥ�Ǥ��롣
    ����ܥ�Ǥ�����ˤϡ�$LANGUAGE �ϸ�������ꤹ�륷��ܥ뤫 @c
    Mnil �Ǥ��ꡢ$FONT ��the $SCRIPT / $LANGUAGE �ڥ��˴�Ϣ�դ����롣

    $CHARSET �� @c Mnil �Ǥ��뤫��ʸ�����åȥ��֥������Ȥ�ɽ������ܥ�
    �Ǥ��롣����ܥ�Ǥ�����ˤ� $FONT �Ϥ���ʸ�����åȤȴ�Ϣ�դ����
    �롣

    $SCRIPT �� $CHARSET �������� @c Mnil �Ǥʤ����ˤ� $FONT �Υ��ԡ�
    �����ĺ��졢���줾�� $SCRIPT / $LANGUAGE �ڥ���ʸ�����åȤ˴�Ϣ
    �դ����롣

    $SCRIPT �� $CHARSET �������� @c Mnil �ʤ�С� $FONT �� @c Mnil ��
    ��Ϣ�դ����롣���μ�Υե���Ȥ� @e fallback @e font �ȸƤФ�롣

    ���� $HOW �� $FONT ��ͥ���٤���ꤹ�롣$HOW �����ʤ�С�$FONT ��Ʊ
    ����Τȴ�Ϣ�դ���줿���롼����Ǻǹ��ͥ���٤���ġ�$HOW �����
    ��С������ͥ���٤���ġ�$HOW �� 0 �ʤ�С�$FONT �ϴ�Ϣ�դ���줿
    ��Τ��Ф���ͣ������Ѳ�ǽ�ʥե���ȤȤʤꡢ¾�Υե���Ȥϥ��롼��
    �����������롣

    $LAYOUTER_NAME �� @c Mnil �Ǥ��뤫��@ref flt �򼨤�����ܥ�Ǥ��롣
    ����ܥ�Ǥ���С�$FONT ���Ѥ���M-text ��ɽ������ݤˤϡ����� FONT
    LAYOUT TABLE ��Ȥä�ʸ���󤫤饰��ե���������������롣

    @return 
    ���������������Ȥ���mfontset_modify_entry () �� 0 ���֤���
    ���Ԥ����Ȥ��� -1 ���֤��������ѿ� #merror_code �˥��顼�����ɤ�
    ���ꤹ�롣  */

/***
    @errors
    @c MERROR_SYMBOL  */

int
mfontset_modify_entry (MFontset *fontset,
		       MSymbol script, MSymbol language, MSymbol charset,
		       MFont *spec, MSymbol layouter_name,
		       int how)
{
  MPlist *per_lang, *plist[3], *pl;
  MFont *font = NULL;
  int i;

  if (fontset->mdb)
    load_fontset_contents (fontset);

  if (! fontset->font_spec_list)
    fontset->font_spec_list = mplist ();
  else
    MPLIST_DO (pl, fontset->font_spec_list)
      {
	if (! memcmp (MPLIST_VAL (pl), spec, sizeof (MFont)))
	  {
	    font = MPLIST_VAL (pl);
	    break;
	  }
      }
  if (! font)
    {
      font = mfont ();
      *font = *spec;
      mplist_add (fontset->font_spec_list, Mt, font);
    }

  i = 0;
  if (script != Mnil)
    {
      if (language == Mnil)
	language = Mt;
      per_lang = mplist_get (fontset->per_script, script);
      if (! per_lang)
	mplist_add (fontset->per_script, script, per_lang = mplist ());
      plist[i] = mplist_get (per_lang, language);
      if (! plist[i])
	mplist_add (per_lang, language, plist[i] = mplist ());
      i++;
    }
  if (charset != Mnil)
    {
      plist[i] = mplist_get (fontset->per_charset, charset);
      if (! plist[i])
	mplist_add (fontset->per_charset, charset, plist[i] = mplist ());
      i++;
    }
  if (script == Mnil && charset == Mnil)
    {
      plist[i++] = fontset->fallback;
    }

  if (layouter_name == Mnil)
    layouter_name = Mt;
  for (i--; i >= 0; i--)
    {
      if (how == -1)
	mplist_push (plist[i], layouter_name, font);
      else if (how == 1)
	mplist_add (plist[i], layouter_name, font);
      else
	{
	  mplist_set (plist[i], Mnil, NULL);
	  mplist_add (plist[i], layouter_name, font);
	}
    }

  return 0;
}

/*** @} */

/*** @addtogroup m17nDebug */
/*=*/
/*** @{  */

/***en
    @brief Dump a fontset.

    The mdebug_dump_fontset () function prints fontset $FONTSET in a human readable
    way to the stderr.  $INDENT specifies how many columns to indent
    the lines but the first one.

    @return
    This function returns $FONTSET.  */
/***ja
    @brief �ե���ȥ��åȤ����פ���.

    �ؿ� mdebug_dump_face () �ϥե���ȥ��å� $FONTSET �� stderr �˿�
    �֤˲��ɤʷ��ǰ������롣 $INDENT �ϣ����ܰʹߤΥ���ǥ�Ȥ���ꤹ
    �롣

    @return
    ���δؿ��� $FONTSET ���֤���  */

MFontset *
mdebug_dump_fontset (MFontset *fontset, int indent)
{
  char *prefix = (char *) alloca (indent + 1);
  MPlist *plist, *pl, *p;

  memset (prefix, 32, indent);
  prefix[indent] = 0;

  fprintf (stderr, "(fontset %s", fontset->name->name);
  if (fontset->per_script)
    MPLIST_DO (plist, fontset->per_script)
      {
	fprintf (stderr, "\n  %s(%s", prefix, MPLIST_KEY (plist)->name);
	MPLIST_DO (pl, MPLIST_PLIST (plist))
	  {
	    fprintf (stderr, "\n    %s(%s", prefix, MPLIST_KEY (pl)->name);
	    MPLIST_DO (p, MPLIST_PLIST (pl))
	      {
		fprintf (stderr, "\n      %s(%s ", prefix,
			 MPLIST_KEY (p)->name);
		mdebug_dump_font (MPLIST_VAL (p));
		fprintf (stderr, ")");
	      }
	    fprintf (stderr, ")");
	  }
	fprintf (stderr, ")");
      }
  if (fontset->per_charset)
    MPLIST_DO (pl, fontset->per_charset)
      {
	fprintf (stderr, "\n  %s(%s", prefix, MPLIST_KEY (pl)->name);
	MPLIST_DO (p, MPLIST_PLIST (pl))
	  {
	    fprintf (stderr, "\n    %s(%s ", prefix, MPLIST_KEY (p)->name);
	    mdebug_dump_font (MPLIST_VAL (p));
	    fprintf (stderr, ")");
	  }
	fprintf (stderr, ")");
      }

  if (fontset->fallback)
    MPLIST_DO (p, fontset->fallback)
      {
	fprintf (stderr, "\n  %s(%s ", prefix, MPLIST_KEY (p)->name);
	mdebug_dump_font (MPLIST_VAL (p));
	fprintf (stderr, ")");
      }

  fprintf (stderr, ")");
  return fontset;
}

/*** @} */

/*
  Local Variables:
  coding: euc-japan
  End:
*/
