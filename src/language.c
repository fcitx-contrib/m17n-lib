/* language.c -- language (and script) module.
   Copyright (C) 2003, 2004, 2006
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
   Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   02111-1307, USA.  */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "m17n.h"
#include "m17n-misc.h"
#include "internal.h"
#include "language.h"
#include "symbol.h"
#include "plist.h"
#include "mtext.h"

#if !defined (FOR_DOXYGEN) || defined (DOXYGEN_INTERNAL_MODULE)

static MPlist *language_list;
static MPlist *script_list;
static MPlist *langname_list;

static MPlist *
load_lang_script_list (MSymbol tag0, MSymbol tag1, MSymbol tag2, MSymbol tag3)
{
  MDatabase *mdb = mdatabase_find (tag0, tag1, tag2, tag3);
  MPlist *plist, *pl, *p;

  if (! mdb
      || ! (plist = mdatabase_load (mdb)))
    return NULL;
  /* Check at least if the plist is ((SYMBOL ...) ...).  */
  for (pl = plist; ! MPLIST_TAIL_P (pl);)
    {
      if (! MPLIST_PLIST_P (pl))
	mplist__pop_unref (pl);
      else
	{
	  p = MPLIST_PLIST (pl);
	  if (! MPLIST_SYMBOL_P (p))
	    mplist__pop_unref (pl);
	  else
	    pl = MPLIST_NEXT (pl);
	}
    }
  return plist;
}

static int
init_language_list (void)
{
  language_list = load_lang_script_list (msymbol ("standard"), Mlanguage,
					 msymbol ("iso639"), Mnil);
  if (! language_list)
    {
      language_list = mplist ();
      MERROR (MERROR_DB, -1);
    }
  return 0;
}


static int
init_script_list (void)
{
  script_list = load_lang_script_list (msymbol ("standard"), Mscript,
				       msymbol ("unicode"), Mnil);
  if (! script_list)
    {
      script_list = mplist ();
      MERROR (MERROR_DB, -1);
    }
  return 0;
}


/* Internal API */

int
mlang__init ()
{
  msymbol_put_func (Mlanguage, Mtext_prop_serializer,
		    M17N_FUNC (msymbol__serializer));
  msymbol_put_func (Mlanguage, Mtext_prop_deserializer,
		    M17N_FUNC (msymbol__deserializer));
  Miso639_2 = msymbol ("iso639-2");
  Miso639_1 = msymbol ("iso639-1");

  language_list = script_list = langname_list = NULL;
  return 0;
}

void
mlang__fini (void)
{
  M17N_OBJECT_UNREF (language_list);
  M17N_OBJECT_UNREF (script_list);
  M17N_OBJECT_UNREF (langname_list);
}

/*=*/

/***en
    @brief Get information about a language.

    The mlanguage_info () function returns a well-formed @e plist that
    contains information about $LANGUAGE.  $LANGUAGE is a symbol whose
    name is an ISO639-2 3-letter language code, an ISO639-1 2-letter
    language codes, or an English word.

    The format of the plist is:

@verbatim
        (ISO639-2 [ISO639-1 | nil] ENGLISH-NAME ["NATIVE-NAME" | nil]
                  ["REPRESENTATIVE-CHARACTERS"])
@endverbatim

    where, ISO639-2 is a symbol whose name is 3-letter language code
    of ISO639-2, ISO639-1 is a symbol whose name is 2-letter language
    code of ISO639-1, ENGLISH-NAME is a symbol whose name is the
    English name of the language, "NATIVE-NAME" is an M-text written
    by the most natural way in the language,
    "REPRESENTATIVE-CHARACTERS" is an M-text that contains
    representative characters used by the language.

    It is assured that the formats of both M-texts are
    #MTEXT_FORMAT_UTF_8.

    @return
    If the information is available, this function returns a plist
    that should not be modified nor freed.  Otherwise, it returns
    @c NULL.

    @seealso
    mlanguage_list ()  */

MPlist *
mlanguage__info (MSymbol language)
{
  MPlist *plist;

  if (! language_list
      && init_language_list () < 0)
    return NULL;

  MPLIST_DO (plist, language_list)
    {
      MPlist *pl = MPLIST_PLIST (plist);

      if (MPLIST_SYMBOL (pl) == language)
	return pl;
      if (MPLIST_TAIL_P (pl))
	continue;
      pl = MPLIST_NEXT (pl);
      if (MPLIST_SYMBOL_P (pl) && MPLIST_SYMBOL (pl) == language)
	return MPLIST_PLIST (plist);
      if (MPLIST_TAIL_P (pl))
	continue;
      pl = MPLIST_NEXT (pl);
      if (MPLIST_MTEXT_P (pl))
	{
	  MText *mt = MPLIST_MTEXT (pl);

	  if (mtext_nbytes (mt) == MSYMBOL_NAMELEN (language)
	      && memcmp (MTEXT_DATA (MPLIST_MTEXT (pl)),
			 MSYMBOL_NAME (language),
			 MSYMBOL_NAMELEN (language)) == 0)
	    return MPLIST_PLIST (plist);
	}
    }
  return NULL;
}

static MPlist *
mscript__info (MSymbol script)
{
  MPlist *plist;

  if (! script_list
      && init_script_list () < 0)
    return NULL;
  MPLIST_DO (plist, script_list)
    {
      MPlist *pl = MPLIST_PLIST (plist);

      if (MPLIST_SYMBOL (pl) == script)
	return pl;
    }
  return NULL;
}

MPlist *
mscript__char_list (MSymbol name)
{
  MPlist *plist = mscript__info (name);

  if (plist			/* script name */
      && (plist = MPLIST_NEXT (plist)) /* language list */
      && ! MPLIST_TAIL_P (plist)
      && (plist = MPLIST_NEXT (plist)) /* char list */
      && MPLIST_PLIST_P (plist))
    return MPLIST_PLIST (plist);
  return NULL;
}

MSymbol
mscript__otf_tag (MSymbol script)
{
  MPlist *plist = mscript__info (script);

  if (plist			/* script name */
      && (plist = MPLIST_NEXT (plist)) /* language list */
      && ! MPLIST_TAIL_P (plist)
      && (plist = MPLIST_NEXT (plist)) /* char list */
      && ! MPLIST_TAIL_P (plist)
      && (plist = MPLIST_NEXT (plist)) /* otf tag */
      && MPLIST_SYMBOL_P (plist))
    return MPLIST_SYMBOL (plist);
  return NULL;
}

MSymbol
mscript__from_otf_tag (MSymbol otf_tag)
{
  MPlist *plist;
  /* As it is expected that this function is called in a sequence with
     the same argument, we use a cache.  */
  static MSymbol last_otf_tag, script;

  if (! script_list)
    {
      last_otf_tag = script = Mnil;
      if (init_script_list () < 0)
	return Mnil;
    }
  if (otf_tag == last_otf_tag)
    return script;
  last_otf_tag = otf_tag;
  script = Mnil;
  MPLIST_DO (plist, script_list)
    {
      MPlist *pl = MPLIST_PLIST (plist), *p;

      if (pl			       /* script name */
	  && (p = MPLIST_NEXT (pl))    /* language tag */
	  && ! MPLIST_TAIL_P (p)
	  && (p = MPLIST_NEXT (p)) /* char list */
	  && ! MPLIST_TAIL_P (p)
	  && (p = MPLIST_NEXT (p)) /* otf tag */
	  && MPLIST_SYMBOL_P (p)
	  && otf_tag == MPLIST_SYMBOL (p))
	{
	  script = MPLIST_SYMBOL (pl);
	  break;
	}
    }
  return script;
}

#endif /* !FOR_DOXYGEN || DOXYGEN_INTERNAL_MODULE */


/* External API */

MSymbol Miso639_1, Miso639_2;

/*=*/

/***en
    @brief List 3-letter language codes.

    The mlanguage_list () funciton returns a well-formed plist whose
    keys are #Msymbol and values are symbols whose names are ISO639-2
    3-letter language codes.

    @return
    This function returns a plist.  The caller should free it by
    m17n_object_unref ().

    @seealso
    mscript_list ().  */

/***ja
    @brief 3$BJ8;z8@8l%3!<%I$r%j%9%H$9$k(B.

    $B4X?t(B mlanguage_list () $B$O!"@07A<0(B (well-formed) plist $B$rJV$9!#3F%-!<(B
    $B$O(B #Msymbol $B$G$"$j!"8D!9$NCM$O(B ISO639-2 $B$KDj$a$i$l$?(B3$BJ8;z8@8l%3!<(B
    $B%I$rL>A0$H$9$k%7%s%\%k$G$"$k!#(B

    @return
    $B$3$N4X?t$,JV$9(B plist $B$O!"8F$S=P$7B&$,(B m17n_object_unref () $B$r;H$C(B
    $B$F2rJ|$9$kI,MW$,$"$k!#(B

    @seealso
    mscript_list ().  */

MPlist *
mlanguage_list (void)
{
  MPlist *plist, *pl, *p, *p0;

  if (! language_list
      && init_language_list () < 0)
    return NULL;
  plist = pl = mplist ();
  MPLIST_DO (p, language_list)
    {
      p0 = MPLIST_PLIST (p);
      pl = mplist_add (pl, Msymbol, MPLIST_VAL (p0));
    }
  return plist;
}

/*=*/

/***en
    @brief Get a language code.

    The mlanguage_code () function returns a symbol whose name is the
    ISO639 language code of $LANGUAGE. $LANGUAGE is a symbol whose
    name is an ISO639-2 3-letter language code, an ISO639-1 2-letter
    language codes, or an English word.

    $LEN specifies the type of the returned language code.  If it is
    3, an ISO639-2 3-letter language code is returned.  If it is 2, an
    ISO639-1 2-letter language code is returned when defined;
    otherwise #Mnil is returned.  If it is 0, a 2-letter code is
    returned when defined; otherwise a 3-letter code is returned.

    @return
    If the information is available, this function returns a non-#Mnil
    symbol.  Otherwise, it returns #Mnil.

    @seealso
    mlanguage_names (), mlanguage_text ().  */

/***ja
    @brief $B8@8l%3!<%I$rF@$k(B.

    $B4X?t(B mlanguage_code () $B$O!"(B$LANGUAGE $B$KBP1~$7$?(B ISO-639 $B8@8l%3!<%I(B
    $B$,L>A0$G$"$k$h$&$J%7%s%\%k$rJV$9!#(B$LANGUAGE $B$O%7%s%\%k$G$"$j!"$=$N(B
    $BL>A0$O!"(BISO639-2 3$BJ8;z8@8l%3!<%I!"(BISO639-1 2$BJ8;z8@8l%3!<%I!"1Q8lL>!"(B
    $B$N$$$:$l$+$G$"$k!#(B

    $LEN $B$OJV$5$l$k8@8l%3!<%I$N<oN`$r7hDj$9$k!#(B$LEN $B$,(B3$B$N>l9g$O(B 
    ISO639-2 3$BJ8;z8@8l%3!<%I$,JV$5$l$k!#(B2$B$N>l9g$O!"$b$7Dj5A$5$l$F$$$l(B
    $B$P(B ISO639-1 2$BJ8;z8@8l%3!<%I$,!"$=$&$G$J$1$l$P(B #Mnil $B$,JV$5$l$k!#(B0 
    $B$N>l9g$O!"$b$7Dj5A$5$l$F$$$l$P(B2$BJ8;z%3!<%I$,!"$=$&$G$J$1$l$P(B3$BJ8;z%3!<(B
    $B%I$,JV$5$l$k!#(B

    @return
    $B$b$7>pJs$,F@$i$l$l$P!"$3$N4X?t$O(B #Mnil $B0J30$N%7%s%\%k$rJV$9!#$=$&(B
    $B$G$J$1$l$P(B #Mnil $B$rJV$9!#(B

    @seealso
    mlanguage_name (), mlanguage_text ().  */

MSymbol
mlanguage_code (MSymbol language, int len)
{
  MPlist *plist = mlanguage__info (language);
  MSymbol code;

  if (! plist)
    return Mnil;
  if (! MPLIST_SYMBOL_P (plist))
    return Mnil;
  code = MPLIST_SYMBOL (plist);
  if (len == 3)
    return code;
  plist = MPLIST_NEXT (plist);
  return ((MPLIST_SYMBOL_P (plist) && MPLIST_SYMBOL (plist) != Mnil)
	  ? MPLIST_SYMBOL (plist)
	  : len == 0 ? code : Mnil);
}

/*=*/

/***en
    @brief Return the language names written in the specified language.

    The mlanguage_name_list () function returns a plist of LANGUAGE's
    names written in TARGET language.

    LANGUAGE and TARGET must be a symbol whose name is an ISO639-2
    3-letter language code or an ISO639-1 2-letter language codes.
    TARGET may be #Mnil, in which case, the language of the current
    locale is used.  If locale is not set or is C, English is used.

    @return
    If the information is available, this function returns a non-empty
    plist whose keys are #Mtext and values are M-texts of the
    translated language names.  Otherwise, @c NULL is returned.
    The returned plist should not be modified nor freed.

    @seealso
    mlanguage_code (), mlanguage_text ().  */

MPlist *
mlanguage_name_list (MSymbol language, MSymbol target)
{
  MPlist *plist;

  plist = mlanguage__info (language);
  if (! plist)
    return NULL;
  language = mplist_value (plist);
  if (target != Mnil)
    {
      plist = mlanguage__info (target);
      if (! plist)
	return NULL;
      target = mplist_value (plist);
    }
  else
    {
      MLocale *locale = mlocale_set (LC_MESSAGES, NULL);

      if (! locale)
	target = msymbol ("eng");
      else
	{
	  target = mlocale_get_prop (locale, Mlanguage);
	  plist = mlanguage__info (target);
	  if (! plist)
	    return NULL;
	  target = mplist_value (plist);
	}
    }
  /* Now both LANGUAGE and TARGET are 3-letter codes.  */

  if (langname_list)
    plist = mplist_get (langname_list, target);
  else
    langname_list = mplist (), plist = NULL;
  if (! plist)
    {
      MDatabase *mdb = mdatabase_find (Mlanguage, Mname, target, Mnil);

      if (! mdb
	  || ! (plist = mdatabase_load (mdb)))
	plist = mplist ();
      else
	mplist__pop_unref (plist);
      langname_list = mplist_push (langname_list, target, plist);
      MPLIST_SET_NESTED_P (langname_list);
    }
  /* PLIST == ((LANGUAGE TRANSLATED) ...) */
  plist = mplist__assq (plist, language);
  if (! plist || MPLIST_TAIL_P (plist))
    return NULL;
  plist = MPLIST_PLIST (plist);
  plist = MPLIST_NEXT (plist);
  return plist;
}

/*=*/

/***en
    @brief Return the language name written in that language.

    The mlanguage_text () function returns, in the form of M-text, the
    language name of $LANGUAGE written in $LANGUAGE.  If the
    representative characters of the language are known, the
    characters of the returned M-text has a text property whose key is
    #Mtext and whose value is an M-text that contains the
    representative characters.

    @return
    If the information is available, this function returns an M-text
    that should not be modified nor freed.  Otherwise, it returns @c
    NULL.

    @seealso
    mlanguage_code (), mlanguage_name ().  */

/***ja
    @brief $BM?$($i$l$?8@8l<+?H$G=q$+$l$?8@8lL>$rJV$9(B.

    $B4X?t(B mlanguage_text () $B$O!"8@8l(B $LANGUAGE $B$G=q$+$l$?(B $LANGUAGE $B$N(B
    $BL>A0$r(B M-text $B$N7A<0$GJV$9!#$=$N8@8l$NBeI=E*$JJ8;z$,$o$+$C$F$$$k>l(B
    $B9g$O!"JV$5$l$k(B M-text $B$N3FJ8;z$K!"%-!<$,(B #Mtext $B$GCM$,$=$NBeI=E*$J(B
    $BJ8;z$r4^$`(B M-text $B$G$"$k$h$&$J%F%-%9%H%W%m%Q%F%#$,IU2C$5$l$k!#(B

    @return
    $B5a$a$k>pJs$,F@$i$l$?>l9g!"$3$N4X?t$,JV$9(B M-text $B$rJQ99$7$?$j2rJ|$7(B
    $B$?$j$7$F$O$$$1$J$$!#>pJs$,F@$i$l$J$+$C$?>l9g$O(B @c NULL $B$,JV$5$l$k!#(B

    @seealso
    mlanguage_code (), mlanguage_name ().  */

MText *
mlanguage_text (MSymbol language)
{
  MPlist *plist = mlanguage__info (language);
  MText *mt;

  if (! plist)
    return NULL;
  plist = MPLIST_NEXT (plist);
  if (MPLIST_TAIL_P (plist))
    return NULL;
  plist = MPLIST_NEXT (plist);
  if (MPLIST_TAIL_P (plist))
    return NULL;
  plist = MPLIST_NEXT (plist);		  
  if (! MPLIST_MTEXT_P (plist))
    return NULL;
  mt = MPLIST_MTEXT (plist);
  if (mtext_nchars (mt) == 0)
    return NULL;
  plist = MPLIST_NEXT (plist);
  if (MPLIST_MTEXT_P (plist)
      && ! mtext_get_prop (mt, 0, Mtext))
    mtext_put_prop (mt, 0, mtext_nchars (mt), Mtext, MPLIST_MTEXT (plist));
  return mt;
}

/***en
    @brief List script names.

    The mscript_list () funciton returns a well-formed plist whose
    keys are #Msymbol and values are symbols whose names are script
    names.

    @return
    This function returns a plist.  The caller should free it by
    m17n_object_unref ().

    @seealso
    mscript_language_list (), mlanguage_list ().  */

/***ja
    @brief $B%9%/%j%W%HL>$r%j%9%H$9$k(B.

    $B4X?t(B mscript_list () $B$O!"@07A<0(B (well-formed) plist $B$rJV$9!#3F%-!<(B
    $B$O(B #Msymbol $B$G$"$j!"8D!9$NCM$O%9%/%j%W%HL>$rL>A0$H$9$k%7%s%\%k$G$"(B
    $B$k!#(B

    @return
    $B$3$N4X?t$,JV$9(B plist $B$O!"8F$S=P$7B&$,(B m17n_object_unref () $B$r;H$C(B
    $B$F2rJ|$9$kI,MW$,$"$k!#(B

    @seealso
    mscript_language_list (), mlanguage_list ().  */

MPlist *
mscript_list (void)
{
  MPlist *plist, *pl, *p, *p0;

  if (! script_list
      && init_script_list () < 0)
    return NULL;
  plist = pl = mplist ();
  MPLIST_DO (p, script_list)
    {
      p0 = MPLIST_PLIST (p);
      pl = mplist_add (pl, Msymbol, MPLIST_VAL (p0));
    }
  return plist;
}

/*=*/

/***en
    @brief List languages that use a specified script.

    The mscript_language_list () function lists languages that use
    $SCRIPT.  $SCRIPT is a symbol whose name is the lower-cased
    version of a script name that appears in the Unicode Character
    Database.

    @return

    This function returns a well-formed plist whose keys are #Msymbol
    and values are symbols whose names are ISO639-1 2-letter codes (or
    ISO639-2 3-letter codes, if the former is not available).  The
    caller should not modify nor free it.  If the m17n library does
    not know about $SCRIPT, it returns @ c NULL.

    @seealso
    mscript_list (), mlanguage_list ().  */

/***ja
    @brief $BM?$($i$l$?%9%/%j%W%H$rMQ$$$k8@8l$r%j%9%H$9$k(B.

    $B4X?t(B mscript_language_list () $B$O!"(B$SCRIPT $B$rMQ$$$k8@8l$r%j%9%H$9$k!#(B
    $SCRIPT $B$O%7%s%\%k$G!"$=$NL>A0$O(B Unicode Character Database $B$K<($5(B
    $B$l$F$$$k%9%/%j%W%HL>$r$9$Y$F>.J8;z$K$7$?$b$N$G$"$k!#(B

    @return $B$3$N4X?t$O!"@07A<0(B (well-formed) plist $B$rJV$9!#3F%-!<$O(B 
    #Msymbol $B$G$"$j!"8D!9$NCM$O(B ISO639-1 $B$KDj$a$i$l$?(B2$BJ8;z8@8l%3!<%I(B
    ($BDj5A$5$l$F$$$J$$>l9g$O(B ISO639-2 $B$KDj$a$i$l$?(B3$BJ8;z8@8l%3!<%I(B) $B$rL>(B
    $BA0$H$9$k%7%s%\%k$G$"$k!#JV$5$l$k(B plist $B$OJQ99$7$?$j2rJ|$7$?$j$7$F(B
    $B$O$J$i$J$$!#(B$SCRIPT $B$,L$CN$N>l9g$O(B @c NULL $B$,JV$5$l$k!#(B

    @seealso
    mscript_list (), mlanguage_list ().  */
    

MPlist *
mscript_language_list (MSymbol script)
{
  MPlist *plist = mscript__info (script);

  if (plist			/* script name */
      && (plist = MPLIST_NEXT (plist)) /* language list */
      && MPLIST_PLIST_P (plist))
    return MPLIST_PLIST (plist);
  return NULL;
}

/*** @} */
/*=*/
/***en
    @name Obsolete functions
*/
/***ja
    @name Obsolete $B$J4X?t(B
*/
/*** @{ */

/***en
    @brief Get an English language name.

    This function is obsolete.  Use mlanguage_name_list () instead.

    The mlanguage_name () function returns a symbol whose name is an
    English name of $LANGUAGE.  $LANGUAGE is a symbol whose name is an
    ISO639-2 3-letter language code, an ISO639-1 2-letter language
    codes, or an English word.

    @return
    If the information is available, this function returns a non-#Mnil
    symbol.  Otherwise, it returns #Mnil.

    @seealso
    mlanguage_code (), mlanguage_text ().  */

/***ja
    @brief $B8@8l$N1Q8lL>$rF@$k(B.

    $B4X?t(B mlanguage_name () $B$O!"(B$LANGUAGE $B$N1Q8lL>$rL>A0$H$9$k$h$&$J%7(B
    $B%s%\%k$rJV$9!#(B$LANGUAGE $B$O%7%s%\%k$G$"$j!"$=$NL>A0$O!"(BISO639-2 3$BJ8(B
    $B;z8@8l%3!<%I!"(BISO639-1 2$BJ8;z8@8l%3!<%I!"1Q8lL>!"$N$$$:$l$+$G$"$k!#(B

    @return
    $B5a$a$F$$$k>pJs$,F@$i$l$k$J$i!"$3$N4X?t$O(B #Mnil $B0J30$N%7%s%\%k$rJV(B
    $B$9!#$=$&$G$J$1$l$P(B #Mnil $B$rJV$9!#(B

    @seealso
    mlanguage_code (), mlanguage_text ().  */

MSymbol
mlanguage_name (MSymbol language)
{
  MPlist *plist = mlanguage__info (language);
  MText *mt;

  if (! plist)			/* 3-letter code */
    return Mnil;
  plist = MPLIST_NEXT (plist);	/* 2-letter code */
  if (MPLIST_TAIL_P (plist))
    return Mnil;
  plist = MPLIST_NEXT (plist);	/* english name */
  if (MPLIST_MTEXT_P (plist))
    return Mnil;
  mt = MPLIST_MTEXT (plist);
  if (mtext_nbytes (mt) != MSYMBOL_NAMELEN (language)
      || memcmp (MTEXT_DATA (MPLIST_MTEXT (plist)),
		 MSYMBOL_NAME (language),
		 MSYMBOL_NAMELEN (language)))
    return Mnil;
  return language;
}

/*=*/

