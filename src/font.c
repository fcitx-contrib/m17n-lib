/* font.c -- font module.
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
    @addtogroup m17nFont
    @brief Font object.

    The m17n GUI API represents a font by an object of the type @c
    MFont.  A font can have @e font @e properties.  Like other types
    of properties, a font property consists of a key and a value.  The
    key of a font property must be one of the following symbols:

    @c Mfoundry, @c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
    @c Madstyle, @c Mregistry, @c Msize, @c Mresolution.

    When the key of a font property is @c Msize or @c Mresolution, its
    value is an integer.  Otherwise the value is a symbol.  "The font
    property that belongs to font F and whose key is @c Mxxx" may be
    shortened to "the xxx property of F".

    The value of a foundry property is a symbol representing font
    foundry information, e.g. adobe, misc, etc.

    The value of a family property is a symbol representing font family
    information, e.g. times, helvetica, etc.

    The value of a weight property is a symbol representing weight
    information, e.g. normal, bold, etc.

    The value of a style property is a symbol representing slant
    information, e.g. normal, italic, etc.

    The value of a stretch property is a symbol representing width
    information, e.g. normal, semicondensed, etc.

    The value of an adstyle property is a symbol representing abstract
    font family information, e.g. serif, sans-serif, etc.

    The value of a registry property is a symbol representing registry
    information, e.g. iso10646-1, iso8895-1, etc.

    The value of a size property is an integer representing design
    size in the unit of 1/10 point.

    The value of a resolution property is an integer representing
    assumed device resolution in the unit of dots per inch (dpi)

    The m17n library uses font objects for two purposes: to receive
    font specification from an application program, and to present
    available fonts to an application program.  When the m17n library
    presents an available font to an application program, all font
    properties have a concrete value.

    The m17n library supports three kinds of fonts: Window system fonts,
    FreeType fonts, and OpenType fonts.

    <ul>

    <li> Window system fonts

    The m17n-X library supports all fonts handled by an X server and
    an X font server.  The correspondence between XLFD fields and font
    properties are shown below.

@verbatim
    XLFD field                                  property
    ---------------                             --------
    FOUNDRY                                     foundry
    FAMILY_NAME                                 family
    WEIGHT_NAME                                 weight
    SLANT                                       style
    SETWIDTH_NAME                               stretch
    ADD_STYLE_NAME                              adstyle
    POINT_SIZE                                  size
    RESOLUTION_Y                                resolution
    CHARSET_REGISTRY-CHARSET_ENCODING           registry
@endverbatim

    XLFD fields not listed in the above table are ignored.

    <li> FreeType fonts

    The m17n library, if configured to use the FreeType library,
    supports all fonts that can be handled by the FreeType library.
    The variable #mfont_freetype_path is initialized properly according
    to the configuration of the m17n library and the environment
    variable @c M17NDIR.  See the documentation of the variable for
    details.

    If the m17n library is configured to use the Xft librray, in
    addition to #mfont_freetype_path, all fonts available via
    fontconfig are supported.

    The family name of a FreeType font corresponds to the family
    property.  Style names of FreeType fonts correspond to the weight,
    style, and stretch properties as below.

@verbatim
    style name          weight  style   stretch
    ----------          ------  -----   -------
    Regular             medium  r       normal
    Italic              medium  i       normal
    Bold                bold    r       normal
    Bold Italic         bold    i       normal
    Narrow              medium  r       condensed
    Narrow Italic       medium  i       condensed
    Narrow Bold         bold    r       condensed
    Narrow Bold Italic  bold    i       condensed
    Black               black   r       normal
    Black Italic        black   i       normal
    Oblique		medium	o	normal
    BoldOblique		bold	o	normal
@endverbatim

    Style names not listed in the above table are treated as
    "Regular".

    Combination of a platform ID and an encoding ID corresponds to the
    registry property.  For example, if a font has the combination (1
    1), the registry property is 1-1.  Some frequent combinations have
    a predefined registry property as below.

@verbatim
    platform ID         encoding ID     registry property
    -----------         -----------     -----------------
    0                   3               unicode-bmp
    0                   4               unicode-full
    1                   0               apple-roman
    3                   1               unicode-bmp
    3                   1               unicode-full
@endverbatim

    Thus, a font that has two combinations (1 0) and (3 1) corresponds
    to four font objects whose registries are 1-0, apple-roman, 3-1,
    and unicode-bmp.

    <li> OpenType fonts 

    The m17n library, if configured to use both the FreeType library
    and the OTF library, supports any OpenType fonts.  The list of
    actually available fonts is created in the same way as in the case
    of FreeType fonts.  If a fontset instructs to use an OpenType font
    via an FLT (Font Layout Table), and the FLT has an OTF-related
    command (e.g. otf:deva), the OTF library converts a character
    sequence to a glyph code sequence according to the OpenType layout
    tables of the font, and the FreeType library gives a bitmap image
    for each glyph.

    </ul>

  */

/***ja
    @addtogroup m17nFont
    @brief �ե���ȥ��֥�������.

    m17n GUI API �ϥե���Ȥ� @c MFont ���Υ��֥������ȤȤ���ɽ�����롣
    �ե���Ȥ� @e �ե���ȥץ�ѥƥ� ����Ĥ��Ȥ��Ǥ��롣¾�Υ����פ�
    �ץ�ѥƥ�Ʊ�ͥե���ȥץ�ѥƥ��ϥ������ͤ���ʤ롣�����ϰʲ��Υ�
    ��ܥ�Τ����줫�Ǥ��롣

    @c Mfoundry, @c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
    @c Madstyle, @c Mregistry, @c Msize, @c Mresolution

    �ե���ȥץ�ѥƥ��Υ����� @c Msize ���뤤�� @c Mresolution �ξ�硢
    �ͤ������ͤǤ��ꡢ����������ʳ��ξ�硢�ͤϥ���ܥ�Ǥ��롣�֥ե�
    ��� F �Υե���ȥץ�ѥƥ��Τ��������� @c Mxxx �Ǥ����ΡפΤ���
    ���ñ�ˡ�F �� xxx �ץ�ѥƥ��פȸƤ֤��Ȥ����롣

    foundry �ץ�ѥƥ����ͤϡ�adobe, misc ���Υե���Ȥγ�ȯ�������
    ������ܥ�Ǥ��롣

    family �ץ�ѥƥ����ͤϡ�times, helvetica ���Υե���ȥե��ߥ꡼��
    ��������ܥ�Ǥ��롣

    weight �ץ�ѥƥ����ͤϡ�normal, bold ���������˴ؤ������򼨤���
    ��ܥ�Ǥ��롣

    style �ץ�ѥƥ����ͤϡ�normal, italic ���Υ�������˴ؤ�������
    ��������ܥ�Ǥ��롣

    stretch �ץ�ѥƥ����ͤϡ�normal, semicondensed ����ʸ�����˴ؤ���
    ����򼨤�����ܥ�Ǥ��롣

    adstyle �ץ�ѥƥ����ͤϡ�serif, sans-serif �������Ū�ʥե���ȥե�
    �ߥ꡼�˴ؤ������򼨤�����ܥ�Ǥ��롣

    registry �ץ�ѥƥ����ͤϡ�iso10646, iso8895-1 �Υ쥸���ȥ�����
    ��������ܥ�Ǥ��롣

    size �ץ�ѥƥ����ͤϡ��ե���ȤΥǥ����󥵥�����ɽ�魯�����ͤǤ�
    �ꡢñ�̤� 1/10 �ݥ���ȤǤ��롣

    resolution �ץ�ѥƥ����ͤϡ����ꤵ��Ƥ���ǥХ����β����٤�ɽ��
    �������ͤǤ��ꡢñ�̤� dots per inch (dpi) �Ǥ��롣  

    m17n �饤�֥��ϥե���ȥ��֥������Ȥ򣲤Ĥ���Ū���Ѥ��Ƥ��롣��
    �ץꥱ�������ץ���फ��ե���Ȥλ������������Ū�ȡ����ץ�
    ���������ץ��������Ѳ�ǽ�ʥե���Ȥ��󼨤�����Ū�Ǥ��롣����
    �ꥱ�������ץ������Ф����󼨤�Ԥ��ݤˤϡ��ե���ȥץ�ѥƥ�
    �Ϥ��٤ƶ���Ū���ͤ���ġ�

    m17n �饤�֥��� Window �����ƥ�ե���ȡ�FreeType�ե���ȡ�
    OpenType�ե���ȤΣ�����򥵥ݡ��Ȥ��Ƥ��롣

    <ul>

    <li> Window �����ƥ�ե����

    m17n X �饤�֥��ϡ�X �����Ф� X �ե���ȥ����Ф���갷�����ƤΥե�
    ��Ȥ򥵥ݡ��Ȥ��롣XLFD �γƥե�����ɤȥե���ȥץ�ѥƥ����б�
    �ϰʲ����̤ꡣ����ɽ�ˤʤ��ե�����ɤ�̵�뤵��롣

@verbatim
    XLFD �ե������                             �ץ�ѥƥ�
    ---------------                             --------
    FOUNDRY                                     foundry
    FAMILY_NAME                                 family
    WEIGHT_NAME                                 weight
    SLANT                                       style
    SETWIDTH_NAME                               stretch
    ADD_STYLE_NAME                              adstyle
    POINT_SIZE                                  size
    RESOLUTION_Y                                resolution
    CHARSET_REGISTRY-CHARSET_ENCODING           registry
@endverbatim

    <li> FreeType fonts

    m17n �饤�֥��ϡ�FreeType �饤�֥���Ȥ��褦�����ꤵ�줿����
    �ϡ�FreeType ���������٤ƤΥե���Ȥ򥵥ݡ��Ȥ��롣�ѿ� 
    #mfont_freetype_path �� m17n �饤�֥�������ȴĶ��ѿ� @c M17NDIR 
    �˱����ƽ��������롣�ܺ٤��ѿ��������򻲾ȤΤ��ȡ�

    �⤷ m17n �饤�֥�꤬ Xft �饤�֥���Ȥ��褦�����ꤵ�줿���ˤϡ�
    #mfont_freetype_path �˲ä��ơ� fontconfig �ǻ��Ѳ�ǽ�ʥե���Ȥ⤹
    �٤ƥ��ݡ��Ȥ���롣

    FreeType �ե���ȤΥե��ߥ�̾�� family �ץ�ѥƥ����б����롣
    FreeType �ե���ȤΥ�������̾�ϡ�����ɽ�Τ褦�� weight, style,
    stretch �ץ�ѥƥ����б����롣

@verbatim
    ��������̾          weight  style   stretch
    ----------          ------  -----   -------
    Regular             medium  r       normal
    Italic              medium  i       normal
    Bold                bold    r       normal
    Bold Italic         bold    i       normal
    Narrow              medium  r       condensed
    Narrow Italic       medium  i       condensed
    Narrow Bold         bold    r       condensed
    Narrow Bold Italic  bold    i       condensed
    Black               black   r       normal
    Black Italic        black   i       normal
    Oblique		medium	o	normal
    BoldOblique		bold	o	normal
@endverbatim

    ���ɽ�˸����ʤ���������̾�� "Regular" �Ȥ��ư����롣

    platform ID �� encoding ID ���Ȥ߹�碌�� registry �ץ�ѥƥ�����
    �����롣���Ȥ��Ф���ե���Ȥ� (1 1) �Ȥ��� ID ���ȹ礻����ƤС�
    registry �ץ�ѥƥ��� 1-1 �Ȥʤ롣���ˤˤ�������ȹ礻�ˤϰʲ���
    �褦������Ѥ� registry �ץ�ѥƥ� ��Ϳ�����Ƥ��롣

@verbatim
    platform ID         encoding ID     registry �ץ�ѥƥ�
    -----------         -----------     -----------------
    0                   3               unicode-bmp
    0                   4               unicode-full
    1                   0               apple-roman
    3                   1               unicode-bmp
    3                   1               unicode-full
@endverbatim

    �������äơ���Ĥ��ȹ礻 (1 0) ��(3 1) ����ĥե���Ȥϡ����줾�� 
    registry �ץ�ѥƥ���1-0, apple-roman, 3-1, unicode-bmp �Ǥ��룴��
    �Υե���ȥ��֥������Ȥ��б����롣

    <li> OpenType �ե����

    m17n �饤�֥��ϡ�FreeType �饤�֥��� OTF �饤�֥�����Ѥ���
    �褦�����ꤹ��С����٤Ƥ� OpenType �ե���Ȥ򥵥ݡ��Ȥ��롣�ºݤ�
    ���ѤǤ���ե���ȤΥꥹ�Ȥ�FreeType �ե���Ȥξ���Ʊ�ͤ˺���
    �롣OpenType �ե���Ȥ� FLT (Font Layout Table) ��ͳ�ǻ��Ѥ���褦
    �ե���ȥ��åȤ˻��ꤵ��Ƥ��ꡢFLT �� OTF ��Ϣ�Υ��ޥ�� (���Ȥ�
    �� otf:deva) ������С�OTF �饤�֥�꤬�ե���Ȥ� OpenType �쥤��
    ���ȥơ��֥�˽��ä�ʸ����򥰥�ե���������Ѵ�����FreeType �饤
    �֥�꤬�ƥ���դΥӥåȥޥåץ��᡼�����󶡤��롣

    </ul>

*/

/*=*/

#if !defined (FOR_DOXYGEN) || defined (DOXYGEN_INTERNAL_MODULE)
/*** @addtogroup m17nInternal
     @{ */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "m17n-gui.h"
#include "m17n-misc.h"
#include "internal.h"
#include "mtext.h"
#include "symbol.h"
#include "plist.h"
#include "charset.h"
#include "internal-gui.h"
#include "font.h"
#include "face.h"

MFontDriver *mfont__driver_list[MFONT_TYPE_MAX];

/** Indices to font properties sorted by their priority.  */
static int font_score_priority[] =
  { MFONT_SIZE,
    MFONT_ADSTYLE,
    MFONT_FAMILY,
    MFONT_WEIGHT,
    MFONT_STYLE,
    MFONT_STRETCH,
    MFONT_FOUNDRY
  };

#define FONT_SCORE_PRIORITY_SIZE	\
  (sizeof font_score_priority / sizeof font_score_priority[0])

/* Indexed by a font property MFONT_XXX, and the value is how many
   bits to shift the difference of property values.  */
static int font_score_shift_bits[MFONT_PROPERTY_MAX];

/** Predefined symbols for each font property.  The order is important
    because the function score_font () decides how well a font matches
    with a spec by checking how close the index is.  */

static char *common_foundry[] =
  { "misc",
    "adobe" };
static char *common_family[] =
  { "fixed",
    "courier",
    "helvetica",
    "times" };
static char *common_weight[] =
  { "ultralight",
    "extralight",
    "light",
    "demilight",
    "book",
    "normal",
    "medium",
    "regular",
    "demibold",
    "bold",
    "extrabold",
    "ultrabold",
    "black" };
static char *common_style[] =
  { "o",
    "i",
    "r",
    "ri",
    "ro" };
static char *common_stretch[] =
  { "condensed",
    "narrow",
    "semicondensed",
    "normal",
    "semiexpanded",
    "expanded" };
static char *common_adstyle[] =
  { "serif",
    "",
    "sans" };
static char *common_registry[] =
  { "iso8859-1" };

/* Table containing all the data above.  */

struct MFontCommonNames
{
  int num;
  char **names;
};

static struct MFontCommonNames font_common_names[] =
  {
    { sizeof (common_foundry) / sizeof (char *), common_foundry},
    { sizeof (common_family) / sizeof (char *), common_family},
    { sizeof (common_weight) / sizeof (char *), common_weight},
    { sizeof (common_style) / sizeof (char *), common_style},
    { sizeof (common_stretch) / sizeof (char *), common_stretch},
    { sizeof (common_adstyle) / sizeof (char *), common_adstyle},
    { sizeof (common_registry) / sizeof (char *), common_registry}
  };


/** Table of available font property names.  */

MFontPropertyTable mfont__property_table[MFONT_REGISTRY + 1];


/** Return the numeric value of SYMBOL as the Nth font property.  */

#define FONT_PROPERTY_NUMERIC(symbol, n)	\
  ((symbol) == Mnil				\
   ? 0						\
   : ((int) msymbol_get ((symbol), mfont__property_table[(n)].property)))


/** Set the numeric value of SYMBOL as the Nth font property to NUMERIC.  */

#define SET_FONT_PROPERTY_NUMERIC(symbol, n, numeric)		\
  msymbol_put((symbol), mfont__property_table[(n)].property,	\
	      (void *) (numeric))

static char *
gen_font_name (char *buf, MFont *font)
{
  char size[16];
  int i;

  buf[0] = '\0';
  for (i = 0; i <= MFONT_REGISTRY; i++)
    if (FONT_PROPERTY (font, i) != Mnil)
      {
	char *name = msymbol_name (FONT_PROPERTY (font, i));

	if (name[0])
	  {
	    if (i > 0)
	      strcat (buf, ",");
	    strcat (buf, name);
	  }
      }
  sprintf (size, ",%d", font->property[MFONT_SIZE] / 10);
  strcat (buf, size);
  return buf;
}



/* Font selector.  */

struct MFontEncoding {
  MFont spec;
  MSymbol encoding_name;
  MCharset *encoding_charset;
  MSymbol repertory_name;
  MCharset *repertory_charset;
};

static MPlist *font_encoding_list;
static MFontEncoding default_encoding;

/** Load font encoding table from the data <font encoding>.
    The data has this form:
	(FONT-SPEC ENCODING) ...
    where FONT-SPEC has this form:
	([FOUNDRY FAMILY [WEIGHT [STYLE [STRETCH [ADSTYLE]]]]] REGISTRY)
    All elements are symbols.  */

static int
load_font_encoding_table ()
{
  MDatabase *mdb;
  MPlist *encoding_list, *plist, *pl, *elt;

  font_encoding_list = pl = mplist ();

  mdb = mdatabase_find (Mfont, msymbol ("encoding"), Mnil, Mnil);
  if (! mdb
      || ! (encoding_list = (MPlist *) mdatabase_load (mdb)))
    MERROR (MERROR_FONT, -1);

  MPLIST_DO (plist, encoding_list)
    {
      MFontEncoding *encoding;
      MSymbol registry;

      MSTRUCT_CALLOC (encoding, MERROR_FONT);

      if (! MPLIST_PLIST_P (plist)
	  || (elt = MPLIST_PLIST (plist), mplist_length (elt) < 2)
	  || ! MPLIST_PLIST_P (elt))
	MWARNING (MERROR_FONT);
      registry = mfont__set_spec_from_plist (&encoding->spec,
					     MPLIST_PLIST (elt));
      elt = MPLIST_NEXT (elt);
      if (! MPLIST_SYMBOL_P (elt))
	MWARNING (MERROR_FONT);
      encoding->encoding_name = MPLIST_SYMBOL (elt);
      elt = MPLIST_NEXT (elt);
      if (MPLIST_TAIL_P (elt))
	encoding->repertory_name = encoding->encoding_name;
      else if (! MPLIST_SYMBOL_P (elt))
	MWARNING (MERROR_FONT);
      else
	encoding->repertory_name = MPLIST_SYMBOL (elt);

      if (registry == Mnil)
	registry = Mt;
      pl = mplist_add (pl, registry, encoding);
      continue;

    warning:
      free (encoding);
    }

  M17N_OBJECT_UNREF (encoding_list);
  return 0;
}

typedef struct {
  MFont spec;
  int resize;
} MFontResize;

static MPlist *font_resize_list;

/** Load font size table from the data <font size>.
    The data has this form:
	(FONT-SPEC RESIZE-FACTOR) ...
    where FONT-SPEC has this form:
	([FOUNDRY FAMILY [WEIGHT [STYLE [STRETCH [ADSTYLE]]]]] REGISTRY)
    All elements of FONT-SPEC are symbols.  */

static int
load_font_resize_table ()
{
  MDatabase *mdb;
  MPlist *size_adjust_list, *plist, *pl, *elt;

  font_resize_list = pl = mplist ();

  mdb = mdatabase_find (Mfont, msymbol ("resize"), Mnil, Mnil);
  if (! mdb)
    return -1;
  if (! (size_adjust_list = (MPlist *) mdatabase_load (mdb)))
    MERROR (MERROR_FONT, -1);

  MPLIST_DO (plist, size_adjust_list)
    {
      MFontResize *resize;
      MSymbol registry;

      MSTRUCT_CALLOC (resize, MERROR_FONT);

      if (! MPLIST_PLIST_P (plist)
	  || (elt = MPLIST_PLIST (plist), mplist_length (elt) != 2)
	  || ! MPLIST_PLIST_P (elt))
	MWARNING (MERROR_FONT);
      registry = mfont__set_spec_from_plist (&resize->spec,
					     MPLIST_PLIST (elt));
      elt = MPLIST_NEXT (elt);
      if (! MPLIST_INTEGER_P (elt))
	MWARNING (MERROR_FONT);
      resize->resize = MPLIST_INTEGER (elt);

      if (registry == Mnil)
	registry = Mt;
      pl = mplist_add (pl, registry, resize);
      continue;

    warning:
      free (resize);
    }

  M17N_OBJECT_UNREF (size_adjust_list);
  return 0;
}

/** Return a font encoding (and repertory) of FONT.  */

static MFontEncoding *
find_encoding (MFont *font)
{
  MSymbol registry = FONT_PROPERTY (font, MFONT_REGISTRY);
  MFontEncoding *encoding = NULL;
  MPlist *plist;

  if (! font_encoding_list)
    load_font_encoding_table ();
  if (! MPLIST_TAIL_P (font_encoding_list))
    while (1)
      {
	plist = font_encoding_list;
	while (registry ? (plist = mplist_find_by_key (plist, registry))
	       : plist)
	  {
	    encoding = (MFontEncoding *) MPLIST_VAL (plist);
	    if (mfont__match_p (font, &encoding->spec, MFONT_ADSTYLE))
	      {
		if (! encoding->encoding_charset)
		  encoding->encoding_charset
		    = MCHARSET (encoding->encoding_name);
		if (! encoding->encoding_charset)
		  {
		    mplist_pop (plist);
		    continue;
		  }
		if (encoding->repertory_name == encoding->encoding_name)
		  encoding->repertory_charset = encoding->encoding_charset;
		else if (encoding->repertory_name != Mnil)
		  {
		    encoding->repertory_charset
		      = MCHARSET (encoding->repertory_name);
		    if (! encoding->repertory_charset)
		      {
			mplist_pop (plist);
			continue;
		      }
		  }
		return encoding;
	      }
	    else
	      plist = MPLIST_NEXT (plist);
	  }
	if (registry == Mnil || registry == Mt)
	  break;
	registry = Mt;
      }
  return &default_encoding;
}



/* Internal API */

int
mfont__init ()
{
  int i, shift;

  Mfoundry = msymbol ("foundry");
  mfont__property_table[MFONT_FOUNDRY].property = Mfoundry;
  Mfamily = msymbol ("family");
  mfont__property_table[MFONT_FAMILY].property = Mfamily;
  Mweight = msymbol ("weight");
  mfont__property_table[MFONT_WEIGHT].property = Mweight;
  Mstyle = msymbol ("style");
  mfont__property_table[MFONT_STYLE].property = Mstyle;
  Mstretch = msymbol ("stretch");
  mfont__property_table[MFONT_STRETCH].property = Mstretch;
  Madstyle = msymbol ("adstyle");
  mfont__property_table[MFONT_ADSTYLE].property = Madstyle;
  Mregistry = msymbol ("registry");
  mfont__property_table[MFONT_REGISTRY].property = Mregistry;

  Msize = msymbol ("size");
  Mresolution = msymbol ("resolution");

  /* The first entry of each mfont__property_table must be Mnil so
     that actual properties get positive numeric numbers.  */
  for (i = 0; i <= MFONT_REGISTRY; i++)
    {
      MLIST_INIT1 (&mfont__property_table[i], names, 8);
      MLIST_APPEND1 (&mfont__property_table[i], names, Mnil, MERROR_FONT);
    }

  /* Register predefined font property names.  */
  for (i = 0; i <= MFONT_REGISTRY; i++)
    {
      int j;

      for (j = 0; j < font_common_names[i].num; j++)
	{
	  MSymbol sym = msymbol (font_common_names[i].names[j]);

	  if (sym == Mnil)
	    return -1;
	  if (msymbol_put(sym, mfont__property_table[i].property,
			  (void *) (j + 1)) < 0)
	    return -1;
	  MLIST_APPEND1 (&mfont__property_table[i], names, sym, MERROR_FONT);
	}
    }

  memset (mfont__driver_list, 0, sizeof mfont__driver_list);

  /* Here, SHIFT starts from 1, not 0.  This is because the lowest bit
     of a score is a flag for a scalable font (see the documentation
     of mfont_score).  */
  i = FONT_SCORE_PRIORITY_SIZE - 1;
  for (shift = 1; i >= 0; i--)
    {
      font_score_shift_bits[font_score_priority[i]] = shift;
      if (font_score_priority[i] == MFONT_SIZE)
	shift += 16;
      else
	shift += 2;
    }

  MFONT_INIT (&default_encoding.spec);
  default_encoding.encoding_name = Mnil;
  default_encoding.encoding_charset = NULL;
  default_encoding.repertory_name = Mnil;
  default_encoding.repertory_charset = NULL;
  {
    char *path, *buf;
    int bufsize;

    mfont_freetype_path = mplist ();
    bufsize = strlen (M17NDIR) + 7;
    buf = alloca (bufsize);
    sprintf (buf, "%s/fonts", M17NDIR);
    mplist_add (mfont_freetype_path, Mstring, strdup (buf));
    path = getenv ("M17NDIR");
    if (path)
      {
	i = strlen (path) + 7;
	if (i > bufsize)
	  buf = alloca (i);
	sprintf (buf, "%s/fonts", path);
	mplist_push (mfont_freetype_path, Mstring, strdup (buf));
      }
  }

#ifdef HAVE_FREETYPE
  if (mfont__ft_init () < 0)
    return -1;
#endif /* HAVE_FREETYPE */
  if (mfont__flt_init () < 0)
    return -1;

  return 0;
}

void
mfont__fini ()
{
  MPlist *plist;
  int i;

  mfont__flt_fini ();
#ifdef HAVE_FREETYPE
  mfont__ft_fini ();
#endif /* HAVE_FREETYPE */

  MPLIST_DO (plist, mfont_freetype_path)
    free (MPLIST_VAL (plist));
  M17N_OBJECT_UNREF (mfont_freetype_path);

  if (font_resize_list)
    {
      MPLIST_DO (plist, font_resize_list)
	free (MPLIST_VAL (plist));
      M17N_OBJECT_UNREF (font_resize_list);
      font_resize_list = NULL;
    }
  if (font_encoding_list)
    {
      MPLIST_DO (plist, font_encoding_list)
	free (MPLIST_VAL (plist));
      M17N_OBJECT_UNREF (font_encoding_list);
      font_encoding_list = NULL;
    }
  for (i = 0; i <= MFONT_REGISTRY; i++)
    MLIST_FREE1 (&mfont__property_table[i], names);
}

void
mfont__free_realized (MRealizedFont *rfont)
{
  if (rfont->info)
    M17N_OBJECT_UNREF (rfont->info);
  free (rfont);
}


/* Compare FONT with REQUEST and return how much they differs.  If
   FONT does not match with SPEC, return -1.  */

int
mfont__score (MFont *font, MFont *spec, MFont *request, int limited_size)
{
  int score = 0;
  int i = FONT_SCORE_PRIORITY_SIZE;

  while (--i >= 0)
    {
      enum MFontProperty prop = font_score_priority[i];

      if (request->property[prop] != 0)
	{
	  int val = 0;

	  if (spec->property[prop] && font->property[prop]
	      && font->property[prop] != spec->property[prop])
	    return -1;
	  if (font->property[prop])
	    val = abs (font->property[prop] - request->property[prop]);
	  if (prop == MFONT_SIZE)
	    {
	      if (font->property[MFONT_RESY] == 0)
		/* This is a scalable font.  We prefer a bitmap font
		   if the size matches exactly.  */
		score |= 1;
	      else
		score |= (val << font_score_shift_bits[MFONT_SIZE]
			  | ((limited_size && val > 0) ? 0x400000 : 0));
	    }
	  else
	    score |= (val > 3 ? 3 : val) << font_score_shift_bits[prop];
	}
    }
  return score;
}


/** Return 1 iff FONT matches SPEC.  */

int
mfont__match_p (MFont *font, MFont *spec, int prop)
{
  for (; prop >= 0; prop--)
    if (spec->property[prop] && font->property[prop]
	&& font->property[prop] != spec->property[prop])
      return 0;
  return 1;
}


void
mfont__set_spec_from_face (MFont *spec, MFace *face)
{
  int i;

  for (i = 0; i <= MFONT_ADSTYLE; i++)
    mfont__set_property (spec, i, face->property[i]);
  /* The value 1 is "iso8859-1".  */
  spec->property[MFONT_REGISTRY] = 1;
  spec->property[MFONT_SIZE] = (int) (face->property[MFACE_SIZE]);
  spec->property[MFONT_RESY] = 0;
  spec->property[MFONT_TYPE] = 0;
}


extern MSymbol
mfont__set_spec_from_plist (MFont *spec, MPlist *plist)
{
  int i;
  MSymbol spec_list[MFONT_REGISTRY + 1];
  MSymbol registry;

  MFONT_INIT (spec);
  memset (spec_list, 0, sizeof spec_list);
  for (i = 0; ! MPLIST_TAIL_P (plist); i++, plist = MPLIST_NEXT (plist))
    {
      if (! MPLIST_SYMBOL_P (plist))
	MERROR (MERROR_FONT, Mnil);	
      spec_list[i] = MPLIST_SYMBOL (plist);
    }
  registry = spec_list[i - 1];
  mfont__set_property (spec, MFONT_REGISTRY, registry);
  for (i -= 2; i >= 0; i--)
    mfont__set_property (spec, i, spec_list[i]);
  return registry;
}

MRealizedFont *
mfont__select (MFrame *frame, MFont *spec, MFont *request, int limited_size,
	       MSymbol layouter)
{
  MSymbol registry = FONT_PROPERTY (spec, MFONT_REGISTRY);
  MPlist *realized_font_list;
  MRealizedFont *best_font[MFONT_TYPE_MAX], *best;
  int best_index;
  int i;
  int mdebug_mask = MDEBUG_FONT;

  if (registry == Mnil)
    registry = Mt;

  MPLIST_DO (realized_font_list, frame->realized_font_list)
    {
      best = MPLIST_VAL (realized_font_list);
      if (MPLIST_KEY (realized_font_list) == registry
	  && ! memcmp (&best->spec, spec, sizeof (MFont))
	  && ! memcmp (&best->request, request, sizeof (MFont)))
	{
	  if (best->layouter != layouter)
	    {
	      MRealizedFont *copy;

	      MSTRUCT_MALLOC (copy, MERROR_FONT);
	      *copy = *best;
	      copy->layouter = layouter;
	      if (copy->info)
		M17N_OBJECT_REF (copy->info);
	      mplist_add (frame->realized_font_list, registry, copy);
	      best = copy;
	    }
	  return best;
	}
    }

  MDEBUG_PUSH_TIME ();
  best = NULL;
  best_index = -1;
  for (i = 0; i < MFONT_TYPE_MAX; i++)
    {
      MFontDriver *driver = mfont__driver_list[i];

      best_font[i] = (driver
		      ? (driver->select) (frame, spec, request, limited_size)
		      : NULL);
      if (best_font[i]
	  && (best_index < 0
	      || best_font[best_index]->score < best_font[i]->score))
	best_index = i;
    }
  for (i = 0; i < MFONT_TYPE_MAX; i++)
    {
      if (i == best_index)
	best = best_font[i];
      else if (best_font[i])
	free (best_font[i]);
    }

  if (mdebug__flag & mdebug_mask)
    {
      char buf1[256], buf2[256];
      MFont font = *spec;

      for (i = 0; i < MFONT_PROPERTY_MAX; i++)
	if (! font.property[i])
	  font.property[i] = request->property[i];
      gen_font_name (buf2, &font);

      if (best)
	MDEBUG_PRINT_TIME ("FONT", 
			   (stderr, " to select <%s> from <%s>.",
			    gen_font_name (buf1, &best->font),
			    buf2));
      else
	MDEBUG_PRINT_TIME ("FONT", (stderr, " to fail to find <%s>.", buf2));
      MDEBUG_POP_TIME ();
    }

  if (! best)
    return NULL;
  best->layouter = layouter;
  mplist_add (frame->realized_font_list, registry, best);
  return best;
}


/** Open a font specified in RFONT.  Return 0 if successfully
    opened, otherwise return -1.  */

int
mfont__open (MRealizedFont *rfont)
{
  MPlist *realized_font_list;
  MSymbol registry = FONT_PROPERTY (&rfont->font, MFONT_REGISTRY);

  if (rfont->status)
    mdebug_hook ();

  MPLIST_DO (realized_font_list, rfont->frame->realized_font_list)
    {
      MRealizedFont *this_rfont = MPLIST_VAL (realized_font_list);

      if (this_rfont->status != 0
	  && MPLIST_KEY (realized_font_list) == registry
	  && ! memcmp (&this_rfont->font, &rfont->font, sizeof (MFont)))
	{
	  if (rfont->info)
	    M17N_OBJECT_UNREF (rfont->info);
	  rfont->info = this_rfont->info;
	  M17N_OBJECT_REF (this_rfont->info);
	  rfont->status = this_rfont->status;
	  return (this_rfont->status > 0 ? 0 : -1);
	}
    }

  return (rfont->driver->open) (rfont);
}

void
mfont__close (MRealizedFont *rfont)
{
  (rfont->driver->close) (rfont);
}

void
mfont__resize (MFont *spec, MFont *request)
{
  MSymbol registry = FONT_PROPERTY (spec, MFONT_REGISTRY);
  MFontResize *resize;
  MPlist *plist;

  if (! font_resize_list)
    load_font_resize_table ();
  if (! MPLIST_TAIL_P (font_resize_list))
    while (1)
      {
	plist = font_resize_list;
	while (registry ? (plist = mplist_find_by_key (plist, registry))
	       : plist)
	  {
	    resize = (MFontResize *) MPLIST_VAL (plist);
	    if (mfont__match_p (spec, &resize->spec, MFONT_ADSTYLE))
	      {
		request->property[MFONT_SIZE]
		  = request->property[MFONT_SIZE] * resize->resize / 100;
		return;
	      }
	    plist = MPLIST_NEXT (plist);
	  }
	if (registry == Mt)
	  break;
	registry = Mt;
      }
}

/* Return 1 if C is encodable, 0, if C is not encodable, -1 if it
   can't be decided now.  */

int
mfont__encodable_p (MRealizedFont *rfont, MSymbol layouter_name, int c)
{
  MFontEncoding *encoding;

  if (layouter_name != Mnil)
    return (mfont__flt_encode_char (layouter_name, c)
	    != MCHAR_INVALID_CODE);
  if (! rfont->encoding)
    rfont->encoding = find_encoding (&rfont->spec);
  encoding = rfont->encoding;
  if (! encoding->repertory_charset)
    return -1;
  return (ENCODE_CHAR (encoding->repertory_charset, c) != MCHAR_INVALID_CODE);
}

unsigned
mfont__encode_char (MRealizedFont *rfont, int c)
{
  MFontEncoding *encoding;
  unsigned code;

  if (rfont->layouter != Mnil)
    return mfont__flt_encode_char (rfont->layouter, c);
  if (! rfont->encoding)
    rfont->encoding = find_encoding (&rfont->font);
  encoding = rfont->encoding;
  if (! encoding->encoding_charset)
    return MCHAR_INVALID_CODE;
  code = ENCODE_CHAR (encoding->encoding_charset, c);
  if (code == MCHAR_INVALID_CODE)
    return MCHAR_INVALID_CODE;
  if (! encoding->repertory_charset)
    return (rfont->driver->encode_char) (rfont, c, code);
  if (ENCODE_CHAR (encoding->repertory_charset, c) == MCHAR_INVALID_CODE)
    return MCHAR_INVALID_CODE;
  return code;
}

void
mfont__get_metric (MGlyphString *gstring, int from, int to)
{
  MGlyph *from_g = MGLYPH (from), *to_g = MGLYPH (to), *g;
  MRealizedFont *rfont = from_g->rface->rfont;

  for (g = from_g; g != to_g; g++)
    if (g->rface->rfont != rfont)
      {
	int idx = GLYPH_INDEX (g);

	(rfont->driver->find_metric) (rfont, gstring, from, idx);
	from_g = g;
	rfont = g->rface->rfont;
	from = idx;
      }
  (rfont->driver->find_metric) (rfont, gstring, from, GLYPH_INDEX (g));
}


void
mfont__set_property (MFont *font, enum MFontProperty key, MSymbol val)
{
  int numeric;

  if (val == Mnil)
    numeric = 0;
  else
    {
      numeric = FONT_PROPERTY_NUMERIC (val, key);
      if (! numeric)
	{
	  numeric = mfont__property_table[key].used;
	  MLIST_APPEND1 (mfont__property_table + key, names, val, MERROR_FONT);
	  SET_FONT_PROPERTY_NUMERIC (val, key, numeric);
	}
    }
  font->property[key] = numeric;
}

void
mfont__set_spec (MFont *font, MSymbol *attrs,
		 unsigned short size, unsigned short resy)
{
  int i;

  for (i = 0; i <= MFONT_REGISTRY; i++)
    mfont__set_property (font, i, attrs[i]);
  font->property[MFONT_SIZE] = size;
  font->property[MFONT_RESY] = resy;
}

/*** @} */
#endif /* !FOR_DOXYGEN || DOXYGEN_INTERNAL_MODULE */



/* External API */

/*** @addtogroup m17nFont */
/*** @{ */
/*=*/

/***en @name Variables: Keys of font property.  */
/***ja @name �ѿ�: �ե���ȥץ�ѥƥ�����ꤹ������Ѥߥ���ܥ� */
/*** @{ */
/*=*/

/***en
    @brief Key of font property specifying foundry.

    The variable #Mfoundry is a symbol of name <tt>"foundry"</tt> and
    is used as a key of font property and face property.  The property
    value must be a symbol whose name is a foundry name of a font.  */
/***ja
    @brief ��ȯ������ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Mfoundry �� <tt>"fonudry"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ�
    �ꡢ�ե���ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣
    �ͤϡ��ե���Ȥγ�ȯ��̾��̾���Ȥ��ƻ��ĥ���ܥ�Ǥʤ��ƤϤʤ�ʤ���
    */

MSymbol Mfoundry;

/***en
    @brief Key of font property specifying family.

    The variable #Mfamily is a symbol of name <tt>"family"</tt> and is
    used as a key of font property and face property.  The property
    value must be a symbol whose name is a family name of a font.  */ 
/***ja
    @brief �ե��ߥ����ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Mfamily �� <tt>"family"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ��ꡢ
    �ե���ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤϡ�
    �ե���ȤΥե��ߥ�̾��̾���Ȥ��ƻ��ĥ���ܥ�Ǥʤ��ƤϤʤ�ʤ���
    */

MSymbol Mfamily;

/***en
    @brief Key of font property specifying weight.

    The variable #Mweight is a symbol of name <tt>"weight"</tt> and is
    used as a key of font property and face property.  The property
    value must be a symbol whose name is a weight name of a font (e.g
    "medium", "bold").  */ 
/***ja
    @brief ��������ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Mweight �� <tt>"weight"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ��ꡢ
    �ե���ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤϡ�
    �ե���Ȥ�����̾ ( "medium", "bold" ��) ��̾���Ȥ��ƻ��ĥ���ܥ��
    �ʤ��ƤϤʤ�ʤ���
    */

MSymbol Mweight;

/***en
    @brief Key of font property specifying style.

    The variable #Mstyle is a symbol of name <tt>"style"</tt> and is
    used as a key of font property and face property.  The property
    value must be a symbol whose name is a style name of a font (e.g
    "r", "i", "o").  */ 
/***ja
    @brief �����������ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Mstyle �� <tt>"style"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ��ꡢ
    �ե���ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤϡ�
    �ե���ȤΥ�������̾ ("r", "i", "o" ��)��̾���Ȥ��ƻ��ĥ���ܥ��
    �ʤ��ƤϤʤ�ʤ���
    */

MSymbol Mstyle;

/***en
    @brief Key of font property specifying stretch.

    The variable #Mstretch is a symbol of name <tt>"stretch"</tt> and
    is used as a key of font property and face property.  The property
    value must be a symbol whose name is a stretch name of a font (e.g
    "normal", "condensed").  */ 
/***ja
    @brief ������ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Mstretch �� <tt>"stretch"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ�
    �ꡢ�ե���ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣
    �ͤϡ��ե���Ȥ�ʸ����̾ ( "normal", "condensed" ��)��̾���Ȥ��ƻ�
    �ĥ���ܥ�Ǥʤ��ƤϤʤ�ʤ���
    */

MSymbol Mstretch;

/***en
    @brief Key of font property specifying additional style.

    The variable #Madstyle is a symbol of name <tt>"adstyle"</tt> and
    is used as a key of font property and face property.  The property
    value must be a symbol whose name is an additional style name of a
    font (e.g "serif", "", "sans").  */ 
/***ja
    @brief adstyle ����ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Madstyle �� <tt>"adstyle"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ�
    �ꡢ�ե���ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣
    �ͤϡ��ե���Ȥ� adstyle ̾("serif", "", "sans" ��)��̾���Ȥ��ƻ�
    �ĥ���ܥ�Ǥʤ��ƤϤʤ�ʤ���
    */

MSymbol Madstyle;

/***en
    @brief Key of font property specifying registry.

    The variable #Mregistry is a symbol of name <tt>"registry"</tt>
    and is used as a key of font property.  The property value must be
    a symbol whose name is a registry name a font registry
    (e.g. "iso8859-1", "jisx0208.1983-0").  */ 
/***ja
    @brief �쥸���ȥ����ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Mregistry �� <tt>"registry"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ��ꡢ�ե���
    �ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤϡ��ե�
    ��ȤΥ쥸���ȥ�̾ ( "iso8859-1", "jisx0208.1983-0" ��) ��̾���Ȥ�
    �ƻ��ĥ���ܥ�Ǥʤ��ƤϤʤ�ʤ���
    */

MSymbol Mregistry;

/***en
    @brief Key of font property specifying size.

    The variable #Msize is a symbol of name <tt>"size"</tt> and is
    used as a key of font property and face property.  The property
    value must be an integer specifying a font design size in the unit
    of 1/10 point (on 100 dpi display).  */ 
/***ja
    @brief ����������ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Msize �� <tt>"size"</tt> �Ȥ���̾������ĥ���ܥ�Ǥ��ꡢ�ե�
    ��ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ����롣�ͤϡ�
    100 dpi �Υǥ����ץ쥤��ǤΥե���ȤΥǥ����󥵥����� 1/10 �ݥ���
    ��ñ�̤Ǽ��������ͤǤʤ��ƤϤʤ�ʤ���
    */

MSymbol Msize;

/***en
    @brief Key of font property specifying resolution.

    The variable #Mresolution is a symbol of name <tt>"resolution"</tt> and
    is used as a key of font property and face property.  The property
    value must be an integer to specifying a font resolution in the
    unit of dots per inch (dpi).  */ 
/***ja
    @brief �����٤���ꤹ��ե���ȥץ�ѥƥ��Υ���.
    
    �ѿ� #Mresolution �� <tt>"resolution"</tt> �Ȥ���̾������ĥ����
    ��Ǥ��ꡢ�ե���ȥץ�ѥƥ��ȥե������ץ�ѥƥ��Υ����Ȥ����Ѥ���
    ��롣�ͤϡ��ե���Ȥβ����٤� dots per inch (dpi) ñ�̤Ǽ�������
    �ͤǤʤ��ƤϤʤ�ʤ���
    */

MSymbol Mresolution;

/*=*/
/*** @} */
/*=*/

/***en
    @brief List of font files and directories that contain font files.

    The variable @c mfont_freetype_path is a plist of FreeType font
    files and directories that contain FreeType font files.  Key of
    the element is @c Mstring, and the value is a string that
    represents a font file or a directory.

    The macro M17N_INIT () sets up this variable to contain the
    sub-directory "fonts" of the m17n database and the environment
    variable "M17NDIR".  The first call of mframe () creates the
    internal list of the actually available fonts from this variable.
    Thus, an application program, if necessary, must modify the
    variable before calling mframe ().  If it is going to add a new
    element, value must be a string that can be safely freed.

    If the m17n library is not configured to use the FreeType library,
    this variable is not used.  */
/***ja
    @brief �ե���ȥե�����ȥե���ȥե������ޤ�ǥ��쥯�ȥ�Υꥹ��.

    �ѿ� @c mfont_freetype_path �ϡ��ե���ȥե�����ȥե���ȥե�����
    ��ޤ�ǥ��쥯�ȥ�� plist �Ǥ��롣�����ǤΥ����� @c Mstring �Ǥ�
    �ꡢ�ͤϥե���ȥե����뤫�ǥ��쥯�ȥ�򼨤�ʸ����Ǥ��롣

    �ޥ��� M17N_INIT () �ˤ�äơ������ѿ��� m17n �ǡ����١����ȴĶ���
     �� "M17NDIR" �����Υ��֥ǥ��쥯�ȥ� "fonts" ��ޤ�褦�����ꤵ��롣
     mframe () �κǽ�θƤӽФ��κݤˡ������ѿ�����ºݤ˻��ѤǤ���ե�
     ��Ȥ������ꥹ�Ȥ�����롣�����ǥ��ץꥱ�������ץ����ϡ�
     mframe () ��Ƥ����ˡ�ɬ�פʤ�Сˤ����ѿ����ѹ����ʤ��ƤϤʤ��
     �������������Ǥ��ɲä�����ˤϡ������ͤϰ����˳����Ǥ���ʸ����
     �Ǥʤ��ƤϤʤ�ʤ���

    m17n �饤�֥�꤬ FreeType �饤�֥���Ȥ��褦�����ꤵ��Ƥʤ���
    ��ˤϡ������ѿ����Ѥ����ʤ��� */

MPlist *mfont_freetype_path;

/*=*/

/***en
    @brief Create a new font.

    The mfont () function creates a new font object that has no
    property.

    @return
    This function returns a pointer to the created font object.  */
/***ja
    @brief �������ե���Ȥ���.

    �ؿ� mfont () �ϥץ�ѥƥ�����ڻ����ʤ��������ե���Ȥ򥪥֥�����
    �Ȥ��롣

    @return
    ���δؿ��Ϻ�ä��ե���ȥ��֥������ȤؤΥݥ��󥿤��֤���  */

MFont *
mfont ()
{
  MFont *font;

  MSTRUCT_CALLOC (font, MERROR_FONT);
  return font;
}

/*=*/

/***en
    @brief Create a new font from fontname.

    The mfont_from_name () function creates a new font object.  The
    properties are extracted fontname $NAME.

    How to extract properties is window system dependent.  The m17n-X
    library parses $NAME as XLFD (X Logical Font Description).

    @return
    If the operation was successful, this function returns a pointer
    to the created font.  Otherwise it returns @c NULL.  */

/***ja
    @brief �ե����̾����ե���Ȥ���.

    �ؿ� mfont_from_name () �ϡ��ե����̾ $NAME ������Ф��줿�ץ��
    �ƥ�����ġ��������ե���ȥ��֥������Ȥ��롣

    �ɤΤ褦�˥ץ�ѥƥ�����Ф����ϥ�����ɥ������ƥ�˰�¸���롣
    m17n-X �饤�֥��ξ��� XLFD (X Logical Font Description) �˽��ä�
    $NAME ����Ϥ��롣

    @return
    ��������������� mfont_from_name () �Ͽ��������줿�ե���Ȥؤ�
    �ݥ��󥿤��֤��������Ǥʤ���� @c NULL ���֤���  */

MFont *
mfont_from_name (char *name)
{
  MFont template, *font;

  if (mwin__parse_font_name (name, &template) < 0)
    return NULL;
  MSTRUCT_CALLOC (font, MERROR_FONT);
  *font = template;
  return font;
}

/*=*/

/***en
    @brief Make a copy of a font.

    The mfont_copy () function returns a new copy of font $FONT.  */
/***ja
    @brief �ե���ȤΥ��ԡ�����.

    �ؿ� Mfont_copy () �ϥե���� $FONT �Υ��ԡ����ꡢ������֤��� */

MFont *
mfont_copy (MFont *font)
{
  MFont *copy;

  MSTRUCT_MALLOC (copy, MERROR_FONT);
  *copy = *font;
  return copy;
}

/*=*/

/***en
    @brief Create a fontname from a font.

    The mfont_name () function creates a fontname string created from
    font $FONT.

    The syntax of fontname is window system dependent.  The m17n-X
    library returns a fontname conforming to XLFD (X Logical Font
    Description).

    @return
    This function returns the created fontname string, which is not freed
    unless the user explicitly does so by free ().  */
/***ja
    @brief �ե����̾����ե���Ȥ���.

    �ؿ� mfont_name () �ϥե����̾��ʸ�����ե����
    $FONT �򸵤˺�롣

    �ե����̾��ʸˡ�ϥ�����ɥ������ƥ�˰�¸���롣m17n-X �饤�֥��
    �� XLFD (X Logical Font Description) �˽����ե����̾���֤���

    @return 
    ���δؿ��Ϻ�ä��ե����̾��ʸ������֤���ʸ����ϡ��桼��
    �� free () �ˤ�ä�����Ū�˲������ʤ��¤��������ʤ���  */

char *
mfont_name (MFont *font)
{
  return mwin__build_font_name (font);
}

/*=*/

/***en
    @brief Get a property value of a font.

    The mfont_get_prop () function gets the value of $KEY property of
    font $FONT.  $KEY must be one of the following symbols:

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Mregistry, @c Msize, @c Mresolution.

    @return
    If $KEY is @c Mfamily, @c Mweight, @c Mstyle, @c Mstretch, @c
    Madstyle, or @c Mregistry, this function returns the
    corresponding value as a symbol.  If the font does not have $KEY
    property, it returns @c Mnil.
    If $KEY is @c Msize or @c Mresolution, this function returns the
    corresponding value as an integer.  If the font does not have $KEY
    property, it returns 0.
    If $KEY is something else, it returns @c NULL and assigns an error
    code to the external variable #merror_code.  */

/***ja
    @brief �ե���ȤΥץ�ѥƥ����ͤ�����.

    �ؿ� mfont_get_prop () �ϥե���� $FONT �Υץ�ѥƥ��Τ����������� 
    $KEY �Ǥ����Τ��ͤ��֤���$KEY �ϰʲ��Υ���ܥ�Τ����줫�Ǥʤ���
    �Фʤ�ʤ���

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Mregistry, @c Msize, @c Mresolution.

    @return 
    $KEY �� @c Mfamily, @c Mweight, @c Mstyle, @c Mstretch, @c
    Madstyle, @c Mregistry �Τ����줫�Ǥ���С����������ͤ򥷥�ܥ��
    �����֤����ե���Ȥ����Υץ�ѥƥ�������ʤ����ˤ� @c Mnil ���֤���
    $KEY �� @c Msize ���뤤�� @c Mresolution �ξ��ˤϡ����������ͤ�
    �������ͤȤ����֤����ե���Ȥ����Υץ�ѥƥ�������ʤ����ˤ� 0 ��
    �֤���
    $KEY ������ʳ��Τ�ΤǤ���С�@c NULL ���֤��������ѿ� 
    #merror_code �˥��顼�����ɤ����ꤹ�롣  */

void *
mfont_get_prop (MFont *font, MSymbol key)
{
  if (key == Mfoundry)
    return (void *) FONT_PROPERTY (font, MFONT_FOUNDRY);
  if (key == Mfamily)
    return (void *) FONT_PROPERTY (font, MFONT_FAMILY);
  if (key == Mweight)
    return (void *) FONT_PROPERTY (font, MFONT_WEIGHT);
  if (key == Mstyle)
    return (void *) FONT_PROPERTY (font, MFONT_STYLE);
  if (key == Mstretch)
    return (void *) FONT_PROPERTY (font, MFONT_STRETCH);
  if (key == Madstyle)
    return (void *) FONT_PROPERTY (font, MFONT_ADSTYLE);
  if (key == Mregistry)
    return (void *) FONT_PROPERTY (font, MFONT_REGISTRY);
  if (key == Msize)
    {
      int size = font->property[MFONT_SIZE];
      return (void *) size;
    }
  if (key == Mresolution)
    {
      int resy = font->property[MFONT_RESY];
      return (void *) resy;
    }

  MERROR (MERROR_FONT, NULL);
}


/*=*/
/***en
    @brief Put a property value to a font.

    The mfont_put_prop () function puts a font property whose key is
    $KEY and value is $VAL to font $FONT.  $KEY must be one of the
    following symbols:

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Mregistry, @c Msize, @c Mresolution.

    If $KEY is @c Msize or @c Mresolution, $VAL must be an integer.
    Otherwise, $VAL must be a symbol.  */
/***ja
    @brief �ե���ȤΥץ�ѥƥ����ͤ����ꤹ��.

    �ؿ� mfont_put_prop () �ϡ��ե���� $FONT �Υ�����$KEY �Ǥ���ץ�
    �ѥƥ����ͤ� $VAL �����ꤹ�롣$KEY �ϰʲ��Υ���ܥ�Τ����줫�Ǥ�
    �롣

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Mregistry, @c Msize, @c Mresolution.

    $KEY �� @c Msize �� @c Mresolution �Ǥ���� $VAL �������ͤǤʤ���
    �Ϥ�ʤ�������ʳ��ξ�硢$VAL �ϥ���ܥ�Ǥʤ��ƤϤʤ�ʤ���*/

int
mfont_put_prop (MFont *font, MSymbol key, void *val)
{
  if (key == Mfoundry)
    mfont__set_property (font, MFONT_FOUNDRY, (MSymbol) val);
  else if (key == Mfamily)
    mfont__set_property (font, MFONT_FAMILY, (MSymbol) val);
  else if (key == Mweight)
    mfont__set_property (font, MFONT_WEIGHT, (MSymbol) val);
  else if (key == Mstyle)
    mfont__set_property (font, MFONT_STYLE, (MSymbol) val);
  else if (key == Mstretch)
    mfont__set_property (font, MFONT_STRETCH, (MSymbol) val);
  else if (key == Madstyle)
    mfont__set_property (font, MFONT_ADSTYLE, (MSymbol) val);
  else if (key == Mregistry)
    mfont__set_property (font, MFONT_REGISTRY, (MSymbol) val);
  else if (key == Msize)
    {
      unsigned size = (unsigned) val;
      font->property[MFONT_SIZE] = size;
    }
  else if (key == Mresolution)
    {
      unsigned resy = (unsigned) val;
      font->property[MFONT_RESY] =  resy;
    }
  else
    MERROR (MERROR_FONT, -1);
  return 0;
}

/*=*/

/***en
    @brief Return the font selection priority.

    The mfont_selection_priority () function returns a newly created
    array of six symbols.  The elements are the following
    keys of font properties ordered by priority.

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Msize.

   The m17n library selects the best matching font according to the
   order of this array.  A font that has a different value for a
   property of lower priority is preferred to a font that has a
   different value for a property of higher priority.  */
/***ja
    @brief �ե��������ͥ���٤��֤�.

    �ؿ� mfont_selection_priority () ��6�ĤΥ���ܥ뤫��ʤ��������
    ���֤�����������Ǥϡ��ʲ��Υե���ȥץ�ѥƥ��Υ�����ͥ���ٽ����
    �٤���ΤǤ��롣

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Msize.

   m17n �饤�֥��Ϥ�������˽��äơ��Ǥ���פ���ե���Ȥ����򤹤롣
   ͥ���٤��㤤�ץ�ѥƥ����ͤ��㤦�ե���Ȥ�ͥ���٤ι⤤�ץ�ѥƥ���
   �ͤ��㤦�ե���Ȥ������硢���Ԥ����򤵤�롣
   */

MSymbol *
mfont_selection_priority ()
{
  MSymbol *keys;
  int i;

  MTABLE_MALLOC (keys, FONT_SCORE_PRIORITY_SIZE, MERROR_FONT);
  for (i = 0; i < FONT_SCORE_PRIORITY_SIZE; i++)
    {
      enum MFontProperty prop = font_score_priority[i];

      if (prop == MFONT_SIZE)
	keys[i] = Msize;
      else if (prop == MFONT_ADSTYLE)
	keys[i] = Madstyle;
      else if (prop == MFONT_FAMILY)
	keys[i] = Mfamily;
      else if (prop == MFONT_WEIGHT)
	keys[i] = Mweight;
      else if (prop == MFONT_STYLE)
	keys[i] = Mstyle;
      else if (prop == MFONT_STRETCH)
	keys[i] = Mstretch;
      else
	keys[i] = Mfoundry;
    }
  return keys;
}

/*=*/

/***en
    @brief Set the font selection priority.

    The mfont_set_selection_priority () function sets font selection
    priority according to $KEYS, which is an array of six symbols.
    Each element must be one of the below.  No two elements must be
    the same.

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Msize.

    See the documentation of the function mfont_selection_priority ()
    for details.  */
/***ja
    @brief �ե��������ͥ���٤����ꤹ��.

    �ؿ� mfont_set_selection_priority () �ϡ�6�ĤΥ���ܥ������ $KEYS 
    �ˤ������äƥե��������ͥ���٤����ꤹ�롣�����Ǥϰʲ��Τ����Τɤ�
    ���Ǥ��ꡢ���ưۤʤäƤ��ʤ��ƤϤʤ�ʤ���

	@c Mfamily, @c Mweight, @c Mstyle, @c Mstretch,
	@c Madstyle, @c Msize.

    �ܺ٤ϴؿ� mfont_selection_priority () �������򻲾ȤΤ��ȡ�
     */

int
mfont_set_selection_priority (MSymbol *keys)
{
  int priority[FONT_SCORE_PRIORITY_SIZE];
  int i, j;

  for (i = 0; i < FONT_SCORE_PRIORITY_SIZE; i++, keys++)
    {
      enum MFontProperty prop;

      if (*keys == Msize)
	prop = MFONT_SIZE;
      else if (*keys == Madstyle)
	prop = MFONT_ADSTYLE;
      else if (*keys == Mfamily)
	prop = MFONT_FAMILY;
      else if (*keys == Mweight)
	prop = MFONT_WEIGHT;
      else if (*keys == Mstyle)
	prop = MFONT_STYLE;
      else if (*keys == Mstretch)
	prop = MFONT_STRETCH;
      else if (*keys == Mfoundry)
	prop = MFONT_FOUNDRY;
      else
	/* Invalid element.  */
	return -1;
      for (j = 0; j < i; j++)
	if (priority[j] == prop)
	  /* Duplicated element.  */
	  return -1;
      priority[i] = prop;
    }
  for (i = 0; i < FONT_SCORE_PRIORITY_SIZE; i++)
    font_score_priority[i] = priority[i];
  return 0;
}

/*=*/

/***en
    @brief Find a font.

    The mfont_find () function returns a pointer to the available font
    that matches best the specification $SPEC on frame $FRAME.

    $SCORE, if not NULL, must point to a place to store the score
    value that indicates how well the found font matches to $SPEC.  The
    smaller score means a better match.  */
/***ja
    @brief �ե���Ȥ�õ��.

    �ؿ� mfont_find () �ϡ��ե졼�� $FRAME ��ǥե������� $SPEC �ˤ��
    �Ȥ���פ������Ѳ�ǽ�ʥե���ȤؤΥݥ��󥿤��֤���  

    $SCORE �� NULL �Ǥ��뤫�����Ĥ��ä��ե���Ȥ� $SPEC �ˤɤ�ۤɹ��
    �Ƥ��뤫�򼨤�����������¸������ؤΥݥ��󥿤Ǥ��롣������������
    ���ۤ��ɤ���äƤ��뤳�Ȥ��̣���롣
    */

MFont *
mfont_find (MFrame *frame, MFont *spec, int *score, int limited_size)
{
  MFont spec_copy;
  MRealizedFont *rfont;

  MFONT_INIT (&spec_copy);
  spec_copy.property[MFONT_REGISTRY] = spec->property[MFONT_REGISTRY];

  rfont = mfont__select (frame, &spec_copy, spec, limited_size, Mnil);
  if (!rfont)
    return NULL;
  if (score)
    *score = rfont->score;
  return &rfont->font;
}

/*=*/
/***en
    @brief Set encoding of a font.

    The mfont_set_encoding () function sets the encoding information
    of font $FONT.

    $ENCODING_NAME is a symbol representing a charset that has the
    same encoding as the font.

    $REPERTORY_NAME is @c Mnil or a symbol representing a charset that
    has the same repertory as the font.  If it is @c Mnil, whether a
    specific character is supported by the font is asked to each font
    driver.

    @return
    If the operation was successful, this function returns 0.
    Otherwise it returns -1 and assigns an error code to the external
    variable #merror_code.  */
/***ja
    @brief �ե���ȤΥ��󥳡��ǥ��󥰤����ꤹ��.

    �ؿ� mfont_set_encoding () �ϥե���� $FONT �Υ��󥳡��ǥ��󥰾���
    �����ꤹ�롣

    $ENCODING_NAME �ϥե���Ȥ�Ʊ�����󥳡��ǥ��󥰤����ʸ�����åȤ�
    ��������ܥ�Ǥ��롣

    $REPERTORY_NAME �� @c Mnil �Ǥ��뤫���ե���Ȥ�Ʊ�����󥳡��ǥ���
    �������ʸ�����åȤ򼨤�����ܥ�Ǥ��롣@c Mnil �Ǥ���С��ġ���ʸ
    �������Υե���Ȥǥ��ݡ��Ȥ���Ƥ��뤫�ɤ����ϡ��ե���ȥɥ饤�Ф�
    �䤤��碌�롣

    @return
    ��������������Ф��δؿ��� 0 ���֤��������Ǥʤ���� -1 ���֤�����
    ���ѿ� #merror_code �˥��顼�����ɤ����ꤹ�롣  */


int
mfont_set_encoding (MFont *font, MSymbol encoding_name, MSymbol repertory_name)
{
  MCharset *encoding_charset = MCHARSET (encoding_name);
  MCharset *repertory_charset;
  MSymbol registry;
  MFontEncoding *encoding;
  MPlist *plist;

  if (! encoding_charset)
    MERROR (MERROR_FONT, -1);
  if (repertory_name != Mnil)
    {
      repertory_charset = MCHARSET (repertory_name);
      if (! repertory_charset)
	MERROR (MERROR_FONT, -1);
    }
  else
    repertory_charset = NULL;

  MSTRUCT_CALLOC (encoding, MERROR_FONT);
  encoding->spec = *font;
  encoding->encoding_name = encoding_name;
  encoding->encoding_charset = encoding_charset;
  encoding->repertory_name = repertory_name;
  encoding->repertory_charset = repertory_charset;
  registry = FONT_PROPERTY (font, MFONT_REGISTRY);
  if (registry == Mnil)
    registry = Mt;
  if (! font_encoding_list)
    load_font_encoding_table ();
  mplist_push (font_encoding_list, registry, encoding);
  MPLIST_DO (plist, MPLIST_NEXT (font_encoding_list))
    if (! memcmp (font, &((MFontEncoding *) MPLIST_VAL (plist))->spec,
		  sizeof (MFont)))
      {
	mplist_pop (plist);
	break;
      }
  return 0;
}

/*** @} */

/*** @addtogroup m17nDebug */
/*=*/
/*** @{ */

/***en
    @brief Dump a font.

    The mdebug_dump_font () function prints font $FONT in a human readable
    way to the stderr.

    @return
    This function returns $FONT.  */
/***ja
    @brief �ե���Ȥ����פ���.

    �ؿ� mdebug_dump_font () �ϥե���� $FONT �� stderr �˿ʹ֤˲��ɤ�
    ���ǰ������롣

    @return
    ���δؿ��� $FONT ���֤���  */

MFont *
mdebug_dump_font (MFont *font)
{
  char *name = mwin__build_font_name (font);

  fprintf (stderr, "%s", name);
  free (name);
  return font;
}

/*** @} */

/*
  Local Variables:
  coding: euc-japan
  End:
*/
