/* m17n-misc.h -- header file for the MISC API.
   Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012
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
   Boston, MA 02110-1301 USA.  */

#ifndef _M17N_ERR_H_
#define _M17N_ERR_H_

#ifndef _M17N_CORE_H_
#include <m17n-core.h>
#endif

M17N_BEGIN_HEADER

/*** @defgroup m17nMisc MISC API */
/***en @brief Miscellaneous API */
/***ja @brief その他の API */
/*=*/
/*** @ingroup m17nMisc  */
/***en @defgroup m17nError Error Handling */
/***ja @defgroup m17nError エラー処理  */
/*=*/

/*** @ingroup m17nError */
/***en 
    @brief Enumeration for error code of the m17n library.

    Enumeration for error code of the m17n library.

    When a library function is called with an invalid argument, it
    sets the external variable #merror_code to one of these values.
    All the error codes are positive integers.

    When a memory allocation error happens, the function pointed to by
    the external variable #m17n_memory_full_handler is called with one
    of these values as an argument.  */

/***ja 
    @brief m17n ライブラリエラーコードの列挙.

    m17n ライブラリエラーコードの列挙

    ライブラリの関数が妥当でない引数とともに呼ばれた場合には、変数 
    #merror_code をこれらの値のどれかにセットする。すべてのエラーコードは正の整数である。

    メモリ割当てエラーの際には、外部変数 #m17n_memory_full_handler 
    の指す関数が、これらの値のうちのどれかを引数として呼ばれる。
    */

enum MErrorCode
  {
    MERROR_NONE,
    MERROR_OBJECT,
    MERROR_SYMBOL,
    MERROR_MTEXT,
    MERROR_TEXTPROP,
    MERROR_CHAR,
    MERROR_CHARTABLE,
    MERROR_CHARSET,
    MERROR_CODING,
    MERROR_RANGE,
    MERROR_LANGUAGE,
    MERROR_LOCALE,
    MERROR_PLIST,
    MERROR_MISC,
    MERROR_WIN,
    MERROR_X,
    MERROR_FRAME,
    MERROR_FACE,
    MERROR_DRAW,
    MERROR_FLT,
    MERROR_FONT,
    MERROR_FONTSET,
    MERROR_FONT_OTF,
    MERROR_FONT_X,
    MERROR_FONT_FT,
    MERROR_IM,
    MERROR_DB,
    MERROR_IO,
    MERROR_DEBUG,
    MERROR_MEMORY,
    MERROR_GD,
    MERROR_MAX
  };

/*=*/

extern void (*m17n_memory_full_handler) (enum MErrorCode err);

/*=*/
/*** @ingroup m17nMisc  */
/***en @defgroup m17nDebug Debugging */
/***ja @defgroup m17nDebug デバッグサポート */
/*=*/

extern int mdebug_hook (void);

extern MSymbol mdebug_dump_symbol (MSymbol sym, int indent);
extern MSymbol mdebug_dump_all_symbols (int indent);
extern MPlist *mdebug_dump_plist (MPlist *plist, int indent);
extern MText *mdebug_dump_mtext (MText *mt, int fullp, int indent);
extern MCharTable *mdebug_dump_chartab (MCharTable *table, int indent);

#ifdef DOXYGEN_INTERNAL_MODULE
/***en @defgroup m17nInternal Internal */
/***ja @defgroup m17nInternal Internal */
#endif

M17N_END_HEADER

#endif /* _M17N_ERR_H_ */

/*
  Local Variables:
  coding: utf-8
  End:
*/
