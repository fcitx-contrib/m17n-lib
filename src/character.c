/* character.c -- character module.
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
    @addtogroup m17nCharacter
    @brief Character objects and API for them.

    The m17n library represents a @e character by a character code (an
    integer).  The minimum character code is @c 0.  The maximum
    character code is defined by the macro #MCHAR_MAX.  It is
    assured that #MCHAR_MAX is not smaller than @c 0x3FFFFF (22
    bits).

    Characters @c 0 to @c 0x10FFFF are equivalent to the Unicode
    characters of the same code values.

    A character can have zero or more properties called @e character
    @e properties.  A character property consists of a @e key and a
    @e value, where key is a symbol and value is anything that can be
    cast to <tt>(void *)</tt>.  "The character property that belongs
    to character C and whose key is K" may be shortened to "the K
    property of C".  */

/***ja
    @addtogroup m17nCharacter
    @brief ʸ�����֥������ȤȤ���˴ؤ��� API.

    m17n �饤�֥��� @e ʸ�� ��ʸ�������ɡ������ˤ�ɽ�����롣�Ǿ���ʸ
    �������ɤ� @c 0 �ǡ������ʸ�������ɤϥޥ��� #MCHAR_MAX �ˤ�ä�
    �������Ƥ��롣#MCHAR_MAX �� @c 0x3FFFFF��22�ӥåȡ� �ʾ�Ǥ���
    ���Ȥ��ݾڤ���Ƥ��롣

    @c 0 ���� @c 0x10FFFF �ޤǤ�ʸ���ϡ������Ʊ���ͤ���� Unicode ��
    ʸ���˳�����Ƥ��Ƥ��롣

    ��ʸ���� @e ʸ���ץ�ѥƥ� �ȸƤ֥ץ�ѥƥ��� 0 �İʾ���Ĥ��Ȥ���
    ���롣ʸ���ץ�ѥƥ��� @e ���� �� @e �� ����ʤ롣�����ϥ���ܥ��
    ���ꡢ�ͤ� <tt>(void *)</tt> ���˥��㥹�ȤǤ����Τʤ鲿�Ǥ�褤�� 
    ��ʸ�� C ��ʸ���ץ�ѥƥ��Τ��������� K �Ǥ����Ρפ��ñ��
    ��ʸ�� C �� K �ץ�ѥƥ��פȸƤ֤��Ȥ����롣  */
/*=*/

#if !defined (FOR_DOXYGEN) || defined (DOXYGEN_INTERNAL_MODULE)
/*** @addtogroup m17nInternal
     @{ */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>

#include "m17n-core.h"
#include "m17n-misc.h"
#include "internal.h"

typedef struct
{
  MSymbol type;
  void *mdb;
  MCharTable *table;
} MCharPropRecord;

static MPlist *char_prop_list;

static void
free_string (int from, int to, void *str, void *arg)
{
  free (str);
}


/* Internal API */

int
mchar__init ()
{
  char_prop_list = mplist ();

  Mname
    = mchar_define_property ("name", Mstring);
  Mcategory
    = mchar_define_property ("category", Msymbol);
  Mcombining_class
    = mchar_define_property ("combining-class", Minteger);
  Mbidi_category
    = mchar_define_property ("bidirectional-category", Msymbol);
  Msimple_case_folding
    = mchar_define_property ("simple-case-folding", Minteger);
  Mcomplicated_case_folding
    = mchar_define_property ("complicated-case-folding", Mtext);
  Mscript
    = mchar_define_property ("script", Msymbol);

  return 0;
}

void
mchar__fini (void)
{
  MPlist *p;

  for (p = char_prop_list; mplist_key (p) != Mnil; p = mplist_next (p))
    {
      MCharPropRecord *record = mplist_value (p);

      if (record->table)
	{
	  if (record->type == Mstring)
	    mchartable_map (record->table, NULL, free_string, NULL);
	  M17N_OBJECT_UNREF (record->table);
	}
      free (record);
    }
  M17N_OBJECT_UNREF (char_prop_list);
}

/*** @} */
#endif /* !FOR_DOXYGEN || DOXYGEN_INTERNAL_MODULE */

/* External API */

/*** @addtogroup m17nCharacter */
/*** @{ */
/*=*/

#ifdef FOR_DOXYGEN
/***en
    @brief Maximum character code.

    The macro #MCHAR_MAX gives the maximum character code.  */

/***ja
    @brief ʸ�������ɤκ�����.

    �ޥ��� #MCHAR_MAX ��ʸ�������ɤκ����ͤ�Ϳ���롣  */

#define MCHAR_MAX
/*=*/
#endif /* FOR_DOXYGEN */

/***en
    @ingroup m17nCharacter
    @name Variables: Keys of character properties

    These symbols are used as keys of character properties.  */

/***ja
     @name �ѿ�: ʸ���ץ�ѥƥ��Υ���

     �����Υ���ܥ��ʸ���ץ�ѥƥ��Υ����Ȥ��ƻȤ��롣*/
/*=*/
/*** @{ */

/***en
    @brief Key for script.

    The symbol #Mscript has the name <tt>"script"</tt> and is used as the key
    of a character property.  The value of such a property is a symbol
    representing the script to which the character belongs.

    Each symbol that represents a script has one of the names listed in
    the <em>Unicode Technical Report #24</em>.  */

/***ja
    @brief ������ץȤ�ɽ�魯����.

    ����ܥ� #Mscript �� <tt>"script"</tt> �Ȥ���̾���������ʸ����
    ��ѥƥ��Υ����Ȥ��ƻȤ��롣���Υץ�ѥƥ����ͤϡ�����ʸ����°��
    �륹����ץȤ�ɽ�魯����ܥ�Ǥ��롣

    ������ץȤ�ɽ�魯����ܥ��̾���ϡ�<em>Unicode Technical Report
    #24</em> �˥ꥹ�Ȥ���Ƥ����ΤΤ����줫�Ǥ��롣  */

MSymbol Mscript;

/*=*/

/***en
    @brief Key for character name.

    The symbol #Mname has the name <tt>"name"</tt> and is used as
    the key of a character property.  The value of such a property is a
    C-string representing the name of the character.  */

/***ja
    @brief ̾����ɽ�魯����.

    ����ܥ� #Mname �� <tt>"name"</tt> �Ȥ���̾���������ʸ���ץ��
    �ƥ��Υ����Ȥ��ƻȤ��롣���Υץ�ѥƥ����ͤ� C-string �Ǥ��ꡢ��
    ��ʸ����̾����ɽ�魯��  */

MSymbol Mname;

/*=*/

/***en
    @brief Key for general category.

    The symbol #Mcategory has the name <tt>"category"</tt> and is
    used as the key of a character property.  The value of such a
    property is a symbol representing the <em>general category</em> of
    the character.

    Each symbol that represents a general category has one of the
    names listed as abbreviations for <em>General Category</em> in
    Unicode.  */

/***ja
    @brief ���̥��ƥ����ɽ�魯����.

    ����ܥ� #Mcategory �� <tt>"category"</tt> �Ȥ���̾���������ʸ
    ���ץ�ѥƥ��Υ����Ȥ��ƻȤ��롣���Υץ�ѥƥ����ͤϡ��б�����
    <em>���̥��ƥ���</em> ��ɽ�魯����ܥ�Ǥ��롣

    ���̥��ƥ����ɽ�魯����ܥ��̾���ϡ�<em>General Category</em>��
    ��ά���Ȥ��� Unicode ���������Ƥ����ΤǤ��롣  */

MSymbol Mcategory;

/*=*/

/***en
    @brief Key for canonical combining class.

    The symbol #Mcombining_class has the name
    <tt>"combining-class"</tt> and is used as the key of a character
    property.  The value of such a property is an integer that
    represents the <em>canonical combining class</em> of the character.

    The meaning of each integer that represents a canonical combining
    class is identical to the one defined in Unicode.  */

/***ja
    @brief ɸ���祯�饹��ɽ�魯����.

    ����ܥ� #Mcombining_class �� <tt>"combining-class"</tt> �Ȥ���
    ̾���������ʸ���ץ�ѥƥ��Υ����Ȥ��ƻȤ��롣���Υץ�ѥƥ�����
    �ϡ��б����� @e ɸ���祯�饹 ��ɽ�魯�����Ǥ��롣

    ɸ���祯�饹��ɽ�魯�����ΰ�̣�ϡ�Unicode ���������Ƥ�����
    ��Ʊ���Ǥ��롣  */

MSymbol Mcombining_class;
/*=*/

/***en
    @brief Key for bidi category.

    The symbol #Mbidi_category has the name <tt>"bidi-category"</tt>
    and is used as the key of a character property.  The value of such
    a property is a symbol that represents the <em>bidirectional
    category</em> of the character.

    Each symbol that represents a bidirectional category has one of
    the names listed as types of <em>Bidirectional Category</em> in
    Unicode.  */

/***ja
    @brief ���������ƥ����ɽ�魯����.

    ����ܥ� #Mbidi_category �� <tt>"bidi-category"</tt> �Ȥ���̾��
    �������ʸ���ץ�ѥƥ��Υ����Ȥ��ƻȤ��롣���Υץ�ѥƥ����ͤϡ�
    �б����� @e ���������ƥ��� ��ɽ�魯����ܥ�Ǥ��롣

    ���������ƥ����ɽ�魯����ܥ��̾���ϡ�<em>Bidirectional
    Category</em> �η��Ȥ��� Unicode ���������Ƥ����ΤǤ��롣  */

MSymbol Mbidi_category;
/*=*/

/***en
    @brief Key for corresponding single lowercase character.

    The symbol #Msimple_case_folding has the name
    <tt>"simple-case-folding"</tt> and is used as the key of a
    character property.  The value of such a property is the
    corresponding single lowercase character that is used when
    comparing M-texts ignoring cases.

    If a character requires a complicated comparison (i.e. cannot be
    compared by simply mapping to another single character), the value
    of such a property is @c 0xFFFF.  In this case, the character has
    another property whose key is #Mcomplicated_case_folding.  */

/***ja
    @brief �б����뾮ʸ����ʸ����ɽ�魯����.

    ����ܥ� #Msimple_case_folding �� <tt>"simple-case-folding"</tt> 
    �Ȥ���̾���������ʸ���ץ�ѥƥ��Υ����Ȥ��ƻȤ��롣���Υץ�ѥƥ�
    ���ͤϡ��б����뾮ʸ����ʸ���Ǥ��ꡢ��ʸ������ʸ���ζ��̤�̵�뤷��
    ʸ������Ӥκݤ˻Ȥ��롣

    ʣ���������ˡ��ɬ�פȤ���ʸ���Ǥ��ä������̤ΰ�ʸ�����б��դ���
    ���Ȥˤ�ä���ӤǤ��ʤ����ˡ����Υץ�ѥƥ����ͤ� @c 0xFFFF ��
    �ʤ롣���ξ�礽��ʸ���ϡ�#Mcomplicated_case_folding �Ȥ���������
    ʸ���ץ�ѥƥ�����ġ�  */

MSymbol Msimple_case_folding;
/***en
    @brief Key for corresponding multiple lowercase characters.

    The symbol #Mcomplicated_case_folding has the name
    <tt>"complicated-case-folding"</tt> and is used as the key of a
    character property.  The value of such a property is the
    corresponding M-text that contains a sequence of lowercase
    characters to be used for comparing M-texts ignoring case.  */

/***ja
    @brief �б����뾮ʸ�������ɽ�魯����.

    ����ܥ� #Mcomplicated_case_folding �� 
    <tt>"complicated-case-folding"</tt> �Ȥ���̾���������ʸ���ץ�ѥƥ�
    �Υ����Ȥ��ƻȤ��롣���Υץ�ѥƥ����ͤϡ��б����뾮ʸ���󤫤��
    �� M-text �Ǥ��ꡢ��ʸ������ʸ���ζ��̤�̵�뤷��ʸ������Ӥκݤ˻�
    ���롣
      */

MSymbol Mcomplicated_case_folding;
/*=*/
/*** @} */
/*=*/

/***en
    @brief Define a character property.

    The mchar_define_property () function searches the m17n database
    for a data whose tags are \<#Mchar_table, $TYPE, $SYM \>.
    Here, $SYM is a symbol whose name is $NAME.  $TYPE must be
    #Mstring, #Mtext, #Msymbol, #Minteger, or #Mplist.

    @return
    If the operation was successful, mchar_define_property () returns
    $SYM.  Otherwise it returns #Mnil.  */

/***ja
    @brief ʸ���ץ�ѥƥ����������.

    �ؿ� mchar_define_property () �ϡ� \<#Mchar_table, $TYPE, $SYM \>
    �Ȥ�����������ä��ǡ����١����� m17n �������١�������õ����  ��
    ���� $SYM �� $NAME �Ȥ���̾���Υ���ܥ�Ǥ��롣$TYPE ��#Mstring,
    #Mtext, #Msymbol, #Minteger, #Mplist �Τ����줫�Ǥʤ���Фʤ�ʤ���

    @return
    ��������������� mchar_define_property () ��$SYM ���֤���
    ���Ԥ������� #Mnil ���֤���  */

/***
    @errors
    @c MERROR_DB

    @seealso
    mchar_get_prop (), mchar_put_prop ()  */

MSymbol
mchar_define_property (const char *name, MSymbol type)
{
  MSymbol key = msymbol (name);
  MCharPropRecord *record;

  record = mplist_get (char_prop_list, key);
  if (record)
    {
      if (record->table)
	M17N_OBJECT_UNREF (record->table);
    }
  else
    {
      MSTRUCT_CALLOC (record, MERROR_CHAR);
      mplist_put (char_prop_list, key, record);
    }

  record->type = type;
  if (mdatabase__finder
      && (record->mdb = (*mdatabase__finder) (Mchar_table, type, key, Mnil)))
    {
      record->table = NULL;
    }
  else
    {
      void *default_value = NULL;

      record->mdb = NULL;
      if (type == Minteger)
	default_value = (void *) -1;
      record->table = mchartable (type, default_value);
    }

  return key;
}

/*=*/

/***en
    @brief Get the value of a character property.

    The mchar_get_prop () function searches character $C for the
    character property whose key is $KEY.

    @return
    If the operation was successful, mchar_get_prop () returns the
    value of the character property.  Otherwise it returns @c
    NULL.  */

/***ja
    @brief ʸ���ץ�ѥƥ����ͤ�����.

    �ؿ� mchar_get_prop () �ϡ�ʸ�� $C ��ʸ���ץ�ѥƥ��Τ��������� 
    $KEY �Ǥ����Τ�õ����

    @return
    ��������������� mchar_get_prop () �ϸ��Ĥ��ä��ץ�ѥƥ����ͤ���
    �������Ԥ������� @c NULL ���֤���

    @latexonly \IPAlabel{mchar_get_prop} @endlatexonly
*/
/***
    @errors
    @c MERROR_SYMBOL, @c MERROR_DB

    @seealso
    mchar_define_property (), mchar_put_prop ()  */

void *
mchar_get_prop (int c, MSymbol key)
{
  MCharPropRecord *record;

  record = mplist_get (char_prop_list, key);
  if (! record)
    return NULL;
  if (record->mdb)
    {
      record->table = (*mdatabase__loader) (record->mdb);
      if (! record->table)
	MERROR (MERROR_DB, NULL);
      record->mdb = NULL;
    }
  return mchartable_lookup (record->table, c);
}

/*=*/

/***en
    @brief Set the value of a character property.

    The mchar_put_prop () function searches character $C for the
    character property whose key is $KEY and assigns $VAL to the value
    of the found property.

    @return
    If the operation was successful, mchar_put_prop () returns 0.
    Otherwise, it returns -1.  */
/***ja
    @brief ʸ���ץ�ѥƥ����ͤ����ꤹ��.

    �ؿ� mchar_put_prop () �ϡ�ʸ�� $C ��ʸ���ץ�ѥƥ��Τ��������� $KEY ��
    �����Τ�õ���������ͤȤ��� $VAL �����ꤹ�롣

    @return
    ��������������� mchar_put_prop () ��0���֤������Ԥ�������-1����
    ����  */
/***
    @errors
    @c MERROR_SYMBOL, @c MERROR_DB

    @seealso
    mchar_define_property (), mchar_get_prop ()   */

int
mchar_put_prop (int c, MSymbol key, void *val)
{
  MCharPropRecord *record;

  record = mplist_get (char_prop_list, key);
  if (! record)
    return -1;
  if (record->mdb)
    {
      record->table = (*mdatabase__loader) (record->mdb);
      if (! record->table)
	MERROR (MERROR_DB, -1);
      record->mdb = NULL;
    }
  return mchartable_set (record->table, c, val);
}

/*** @} */

/*
  Local Variables:
  coding: euc-japan
  End:
*/
