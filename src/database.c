/* database.c -- database module.
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
   Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   02111-1307, USA.  */

/***en
    @addtogroup m17nDatabase
    @brief The m17n database and API for it.

    The m17n library acquires various kinds of information
    from data in the <i> m17n database</i> on demand.  Application
    programs can also add/load their original data to/from the m17n
    database by setting the variable #mdatabase_dir to an
    application-specific directory and storing data in it.  Users can
    overwrite those data by storing preferable data in the directory
    specified by the environment variable "M17NDIR", or if it is not
    set, in the directory "~/.m17n.d".

    The m17n database contains multiple heterogeneous data, and each
    data is identified by four tags; TAG0, TAG1, TAG2, TAG3.  Each tag
    must be a symbol.

    TAG0 specifies the type of data stored in the database as below.

    @li
    If TAG0 is #Mchar_table, the data is of the @e chartable @e
    type and provides information about each character.  In this case,
    TAG1 specifies the type of the information and must be #Msymbol,
    #Minteger, #Mstring, #Mtext, or #Mplist.  TAG2 and TAG3 can be any
    symbols.

    @li
    If TAG0 is #Mcharset, the data is of the @e charset @e type
    and provides a decode/encode mapping table for a charset.  In this
    case, TAG1 must be a symbol representing a charset.  TAG2 and TAG3
    can be any symbols.

    @li 
    If TAG0 is neither #Mchar_table nor #Mcharset, the data is of
    the @e plist @e type.  See the documentation of the 
    mdatabase_load () function for the details.  
    In this case, TAG1, TAG2, and TAG3 can be any symbols.

    The notation \<TAG0, TAG1, TAG2, TAG3\> means a data with those
    tags.

    Application programs first calls the mdatabase_find () function to
    get a pointer to an object of the type #MDatabase.  That object
    holds information about the specified data.  When it is
    successfully returned, the mdatabase_load () function loads the
    data.  The implementation of the structure #MDatabase is
    concealed from application programs.
*/

/***ja
    @addtogroup m17nDatabase
    @brief m17n �ǡ����١����ˤȤ���˴ؤ��� API.

    m17n �饤�֥���ɬ�פ˱�����ưŪ�� @e m17n @e �ǡ����١��� 
    ��������������롣�ޤ����ץꥱ�������ץ����⡢�ȼ��Υǡ����� 
    m17n �ǡ����١������ɲä��������ưŪ�˼������뤳�Ȥ��Ǥ��롣
    ���ץꥱ�������ץ���ब�ȼ��Υǡ������ɲá���������ˤϡ��ѿ� 
    #mdatabase_dir �ˤ��Υ��ץꥱ��������ͭ�Υǥ��쥯�ȥ�򥻥åȤ���
    ������˥ǡ������Ǽ���롣�桼�������Υǡ����򥪡��С��饤�Ȥ�����
    �Ȥ��ϡ��Ķ��ѿ� "M17NDIR" �ǻ��ꤵ���ǥ��쥯�ȥ�ʻ��ꤵ��Ƥ���
    ���Ȥ��� "~/.m17n.d" �Ȥ����ǥ��쥯�ȥ�ˤ��̤Υǡ������֤���

    m17n 
    �ǡ����١����ˤ�ʣ����¿�ͤʥǡ������ޤޤ�Ƥ��ꡢ�ƥǡ�����
    TAG0, TAG1, TAG2, TAG3�ʤ��٤ƥ���ܥ�ˤΣ��ĤΥ����ˤ�äƼ��̤���롣

    TAG0 �ˤ�äơ��ǡ����١�����Υǡ����Υ����פϼ��Τ褦�˻��ꤵ��롣

    @li 
    TAG0 �� #Mchar_table �Ǥ���ǡ����� @e chartable������ 
    �ȸƤФ졢��ʸ���˴ؤ��������󶡤��롣���ξ��
    TAG1 �Ͼ���μ������ꤹ�륷��ܥ�Ǥ��ꡢ#Msymbol, #Minteger, #Mstring,
    #Mtext, #Mplist �Τ����줫�Ǥ��롣TAG2 �� TAG3 ��Ǥ�դΥ���ܥ�Ǥ褤��

    @li 
    TAG0 �� #Mcharset �Ǥ���ǡ����� @e charset������ 
    �ȸƤФ졢ʸ�����å��ѤΥǥ����ɡ����󥳡��ɥޥåפ��󶡤��롣���ξ�� TAG1
    ��ʸ�����åȤΥ���ܥ�Ǥʤ���Фʤ�ʤ���TAG2 �� TAG3
    ��Ǥ�դΥ���ܥ�Ǥ褤��

    @li
    TAG0 �� #Mchar_table �Ǥ� #Mcharset �Ǥ�ʤ���硢���Υǡ����� @e
    plist������ �Ǥ��롣�ܺ٤˴ؤ��Ƥϴؿ� mdatabase_load () 
    �������򻲾ȤΤ��ȡ����ξ�� TAG1��TAG2��TAG3 ��Ǥ�դΥ���ܥ�Ǥ褤��

    ����Υ�������ĥǡ����١����� \<TAG0, TAG1, TAG2, TAG3\> 
    �Ȥ���������ɽ����

    ���ץꥱ�������ץ����ϡ��ޤ��ؿ� mdatabase_find () 
    ��Ȥäƥǡ����١����˴ؤ��������ݻ����륪�֥������ȡ�#MDatabase
    ���ˤؤΥݥ��󥿤����롣��������������顢 mdatabase_load () 
    �ˤ�äƼºݤ˥ǡ����١�������ɤ��롣��¤�� #MDatabase 
    ���Ȥ��ɤ���������Ƥ��뤫�ϡ����ץꥱ�������ץ���फ��ϸ����ʤ���

    @latexonly \IPAlabel{database} @endlatexonly
*/

/*=*/

#if !defined (FOR_DOXYGEN) || defined (DOXYGEN_INTERNAL_MODULE)
/*** @addtogroup m17nInternal
     @{ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <glob.h>
#include <time.h>
#include <libgen.h>

#include "m17n.h"
#include "m17n-misc.h"
#include "internal.h"
#include "mtext.h"
#include "character.h"
#include "charset.h"
#include "database.h"
#include "coding.h"
#include "plist.h"

/** The file containing a list of databases.  */
#define MDB_DIR "mdb.dir"
/** Length of MDB_DIR.  */
#define MDB_DIR_LEN 7

#define MAX_TIME(TIME1, TIME2) ((TIME1) >= (TIME2) ? (TIME1) : (TIME2))

#define GEN_PATH(path, dir, dir_len, file, file_len)	\
  (dir_len + file_len > PATH_MAX ? 0			\
   : (memcpy (path, dir, dir_len),			\
      memcpy (path + dir_len, file, file_len),		\
      path[dir_len + file_len] = '\0', 1))

static MSymbol Masterisk;

/** Structure for a data in the m17n database.  */

struct MDatabase
{
  /** Tags to identify the data.  <tag>[0] specifies the type of
      database.  If it is #Mchar_table, the type is @e chartable, if
      it is #Mcharset, the type is @e charset, otherwise the type is
      @e plist.  */
  MSymbol tag[4];

  void *(*loader) (MSymbol *tags, void *extra_info);

  /** The meaning of the value is dependent on <loader>.  If <loader>
      is load_database (), the value is a string of the file name that
      contains the data.  */
  void *extra_info;
};

static MPlist *mdatabase__list;

static int
read_number (char *buf, int *i)
{
  int idx = *i;
  int c = buf[idx++];
  int n;

  if (!c)
    return -1;

  while (c && isspace (c)) c = buf[idx++];

  if (c == '0')
    {
      if (buf[idx] == 'x')
	{
	  for (idx++, c = 0; (n = hex_mnemonic[(unsigned) buf[idx]]) < 16;
	       idx++)
	    c  = (c << 4) | n;
	  *i = idx;
	  return c;
	}
      c = 0;
    }
  else if (c == '\'')
    {
      c = buf[idx++];
      if (c == '\\')
	{
	  c = buf[idx++];
	  n = escape_mnemonic[c];
	  if (n != 255)
	    c = n;
	}
      while (buf[idx] && buf[idx++] != '\'');
      *i = idx;
      return c;
    }
  else if (hex_mnemonic[c] < 10)
    c -= '0';
  else
    return -1;

  while ((n = hex_mnemonic[(unsigned) buf[idx]]) < 10)
    c = (c * 10) + n, idx++;
  *i = idx;
  return c;
}


/** Load a data of type @c chartable from the file FD, and return the
    newly created chartable.  */

static void *
load_chartable (FILE *fp, MSymbol type)
{
  int c, from, to;
  char buf[1024];
  void *val;
  MCharTable *table;

  if (! fp)
    MERROR (MERROR_DB, NULL);

  table = mchartable (type, (type == Msymbol ? (void *) Mnil
			     : type == Minteger ? (void *) -1
			     : NULL));

  while (! feof (fp))
    {
      int i, len;

      for (len = 0; len < 1023 && (c = getc (fp)) != EOF && c != '\n'; len++)
	buf[len] = c;
      buf[len] = '\0';	  
      if (hex_mnemonic[(unsigned) buf[0]] >= 10)
	/* skip comment/invalid line */
	continue;
      i = 0;
      from = read_number (buf, &i);
      if (buf[i] == '-')
	i++, to = read_number (buf, &i);
      else
	to = from;
      if (from < 0 || to < from)
	continue;

      while (buf[i] && isspace ((unsigned) buf[i])) i++;
      c = buf[i];
      if (!c)
	continue;

      if (type == Mstring)
	{
	  /* VAL is a C-string.  */
	  if (! (val = strdup (buf + i)))
	    MEMORY_FULL (MERROR_DB);
	}
      else if (type == Minteger)
	{
	  /* VAL is an integer.  */
	  int positive = 1;
	  int n;

	  if (c == '-')
	    i++, positive = -1;
	  n = read_number (buf, &i);
	  if (n < 0)
	    goto label_error;
	  val = (void *) (n * positive);
	}
      else if (type == Mtext)
	{
	  /* VAL is an M-text.  */
	  MText *mt;
	  if (c == '"')
	    mt = mconv_decode_buffer (Mcoding_utf_8,
				      (unsigned char *) (buf + i),
				      len - i - 1);
	  else
	    {
	      mt = mtext ();
	      while ((c = read_number (buf, &i)) >= 0)
		mt = mtext_cat_char (mt, c);
	    }
	  val = (void *) mt;
	}
      else if (type == Msymbol)
	{
	  char *p = buf + i;

	  while (*p && ! isspace (*p)) 
	    {
	      if (*p == '\\' && p[1] != '\0')
		{
		  memmove (p, p + 1, buf + len - (p + 1));
		  len--;
		}
	      p++;
	    }
	  *p = '\0';
	  if (! strcmp (buf + i, "nil"))
	    val = (void *) Mnil;
	  else
	    val = (void *) msymbol (buf + i);
	}
      else if (type == Mplist)
	{
	  val = (void *) mplist__from_string ((unsigned char *) buf + i,
					      strlen (buf + i));
	}
      else
	val = NULL;

      if (from == to)
	mchartable_set (table, from, val);
      else
	mchartable_set_range (table, from, to, val);
    }
  return table;

 label_error:
  M17N_OBJECT_UNREF (table);
  MERROR (MERROR_DB, NULL);
}


/** Load a data of type @c charset from the file FD.  */

static void *
load_charset (FILE *fp, MSymbol charset_name)
{
  MCharset *charset = MCHARSET (charset_name);
  int *decoder;
  MCharTable *encoder;
  int size;
  int i, c;
  int found = 0;
  MPlist *plist;

  if (! charset)
    MERROR (MERROR_DB, NULL);
  size = (charset->code_range[15]
	  - (charset->min_code - charset->code_range_min_code));
  MTABLE_MALLOC (decoder, size, MERROR_DB);
  for (i = 0; i < size; i++)
    decoder[i] = -1;
  encoder = mchartable (Minteger, (void *) MCHAR_INVALID_CODE);

  while ((c = getc (fp)) != EOF)
    {
      unsigned code1, code2, c1, c2;
      int idx1, idx2;
      char buf[256];

      ungetc (c, fp);
      fgets (buf, 256, fp);
      if (c != '#')
	{
	  if (sscanf (buf, "0x%x-0x%x 0x%x", &code1, &code2, &c1) == 3)
	    {
	      idx1 = CODE_POINT_TO_INDEX (charset, code1);
	      if (idx1 >= size)
		continue;
	      idx2 = CODE_POINT_TO_INDEX (charset, code2);
	      if (idx2 >= size)
		idx2 = size - 1;
	      c2 = c1 + (idx2 - idx1);
	    }
	  else if (sscanf (buf, "0x%x 0x%x", &code1, &c1) == 2)
	    {
	      idx1 = idx2 = CODE_POINT_TO_INDEX (charset, code1);
	      if (idx1 >= size)
		continue;
	      c2 = c1;
	    }
	  else
	    continue;
	  if (idx1 >= 0 && idx2 >= 0)
	    {
	      decoder[idx1] = c1;
	      mchartable_set (encoder, c1, (void *) code1);
	      for (idx1++, c1++; idx1 <= idx2; idx1++, c1++)
		{
		  code1 = INDEX_TO_CODE_POINT (charset, idx1);
		  decoder[idx1] = c1;
		  mchartable_set (encoder, c1, (void *) code1);
		}
	      found++;
	    }
	}
    }

  if (! found)
    {
      free (decoder);
      M17N_OBJECT_UNREF (encoder);
      return NULL;
    }
  plist = mplist ();
  mplist_add (plist, Mt, decoder);
  mplist_add (plist, Mt, encoder);
  return plist;
}

static char *
gen_database_name (char *buf, MSymbol *tags)
{
  int i;

  strcpy (buf, msymbol_name (tags[0]));
  for (i = 1; i < 4; i++)
    {
      strcat (buf, ",");
      strcat (buf, msymbol_name (tags[i]));
    }
  return buf;
}

char *
find_file (MDatabaseInfo *db_info, struct stat *buf)
{
  MPlist *plist;
  char path[PATH_MAX + 1];

  MPLIST_DO (plist, mdatabase__dir_list)
    {
      MDatabaseInfo *dir_info = MPLIST_VAL (plist);

      if (dir_info->status != MDB_STATUS_DISABLED
	  && GEN_PATH (path, dir_info->filename, dir_info->len,
		       db_info->filename, db_info->len)
	  && stat (path, buf) == 0)
	return strdup (path);
    }
  return NULL;
}


/* Return the absolute file name for DB_INFO->filename.  If BUF is
   non-NULL, store the result of `stat' call in it.  It returns NULL
   if no absolute file name was found.  */

char *
get_database_file (MDatabaseInfo *db_info, struct stat *buf)
{
  if (db_info->status == MDB_STATUS_DISABLED)
    return NULL;
  if (db_info->absolute_filename)
    {
      if (buf)
	stat (db_info->absolute_filename, buf);
    }
  else
    {
      struct stat stat_buf;
      struct stat *statbuf = buf ? buf : &stat_buf;

      db_info->absolute_filename = find_file (db_info, statbuf);
    }

  return db_info->absolute_filename;
}

static void *
load_database (MSymbol *tags, void *extra_info)
{
  MDatabaseInfo *db_info = extra_info;
  void *value;
  char *filename = get_database_file (db_info, NULL);
  FILE *fp;
  int mdebug_mask = MDEBUG_DATABASE;
  char buf[256];

  MDEBUG_PRINT1 (" [DB] <%s>", gen_database_name (buf, tags));
  if (! filename || ! (fp = fopen (filename, "r")))
    {
      if (filename)
	MDEBUG_PRINT1 (" open fail: %s\n", filename);
      else
	MDEBUG_PRINT1 (" not found: %s\n", db_info->filename);
      MERROR (MERROR_DB, NULL);
    }

  MDEBUG_PRINT1 (" from %s\n", filename);

  if (tags[0] == Mchar_table)
    value = load_chartable (fp, tags[1]);
  else if (tags[0] == Mcharset)
    value = load_charset (fp, tags[1]);
  else
    value = mplist__from_file (fp, NULL);
  fclose (fp);

  if (! value)
    MERROR (MERROR_DB, NULL);
  db_info->time = time (NULL);
  return value;
}


/** Return a newly allocated MDatabaseInfo for DIRNAME.  */

static MDatabaseInfo *
get_dir_info (char *dirname)
{
  MDatabaseInfo *dir_info;

  MSTRUCT_CALLOC (dir_info, MERROR_DB);
  if (dirname)
    {
      int len = strlen (dirname);

      if (len + MDB_DIR_LEN < PATH_MAX)
	{
	  MTABLE_MALLOC (dir_info->filename, len + 2, MERROR_DB);
	  memcpy (dir_info->filename, dirname, len + 1);
	  /* Append PATH_SEPARATOR if DIRNAME doesn't end with it.  */
	  if (dir_info->filename[len - 1] != PATH_SEPARATOR)
	    {
	      dir_info->filename[len] = PATH_SEPARATOR;
	      dir_info->filename[++len] = '\0';
	    }
	  dir_info->len = len;
	  dir_info->status = MDB_STATUS_AUTO;
	}
      else
	dir_info->status = MDB_STATUS_DISABLED;	
    }
  else
    dir_info->status = MDB_STATUS_DISABLED;
  return dir_info;
}

static MDatabase *
find_database (MSymbol tags[4])
{
  MPlist *plist;
  int i;
  
  if (! mdatabase__list)
    return NULL;
  for (i = 0, plist = mdatabase__list; i < 4; i++)
    {
      plist = mplist__assq (plist, tags[i]);
      if (! plist)
	return NULL;
      plist = MPLIST_PLIST (plist);
      plist = MPLIST_NEXT (plist);
    }
  return MPLIST_VAL (plist);
}

static void
free_db_info (MDatabaseInfo *db_info)
{
  free (db_info->filename);
  if (db_info->absolute_filename
      && db_info->filename != db_info->absolute_filename)
    free (db_info->absolute_filename);
  free (db_info);
}

static MDatabase *
register_database (MSymbol tags[4], void *(*loader) (MSymbol *, void *),
		   void *extra_info, enum MDatabaseStatus status)
{
  MDatabase *mdb = find_database (tags);
  MDatabaseInfo *db_info = NULL;

  if (mdb)
    {
      if (loader == load_database)
	db_info = mdb->extra_info;
    }
  else
    {
      int i;
      MPlist *plist;

      MSTRUCT_MALLOC (mdb, MERROR_DB);
      for (i = 0; i < 4; i++)
	mdb->tag[i] = tags[i];
      mdb->loader = loader;
      if (loader == load_database)
	{
	  MSTRUCT_CALLOC (db_info, MERROR_DB);
	  mdb->extra_info = db_info;
	}
      else
	mdb->extra_info = extra_info;
      if (! mdatabase__list)
	mdatabase__list = mplist ();
      for (i = 0, plist = mdatabase__list; i < 4; i++)
	{
	  MPlist *pl = mplist__assq (plist, tags[i]);

	  if (pl)
	    pl = MPLIST_PLIST (pl);
	  else
	    {
	      pl = mplist ();
	      mplist_add (pl, Msymbol, tags[i]);
	      mplist_push (plist, Mplist, pl);
	      M17N_OBJECT_UNREF (pl);
	    }
	  plist = MPLIST_NEXT (pl);
	}
      mplist_push (plist, Mt, mdb);
    }

  if (db_info)
    {
      db_info->status = status;
      if (! db_info->filename
	  || strcmp (db_info->filename, (char *) extra_info) != 0)
	{
	  if (db_info->filename)
	    free (db_info->filename);
	  if (db_info->absolute_filename
	      && db_info->filename != db_info->absolute_filename)
	    free (db_info->absolute_filename);
	  db_info->filename = strdup ((char *) extra_info);
	  db_info->len = strlen ((char *) extra_info);
	  db_info->time = 0;
	}
      if (db_info->filename[0] == PATH_SEPARATOR)
	db_info->absolute_filename = db_info->filename;
      else
	db_info->absolute_filename = NULL;
    }

  if (mdb->tag[0] == Mchar_table
      && mdb->tag[2] != Mnil
      && (mdb->tag[1] == Mstring || mdb->tag[1] == Mtext
	  || mdb->tag[1] == Msymbol || mdb->tag[1] == Minteger
	  || mdb->tag[1] == Mplist))
    mchar__define_prop (mdb->tag[2], mdb->tag[1], mdb);
  return mdb;
}

static void
register_databases_in_files (MSymbol tags[4], glob_t *globbuf, int headlen)
{
  int i, j;
  MPlist *load_key = mplist ();
  FILE *fp;
  MPlist *plist;

  for (i = 0; i < globbuf->gl_pathc; i++)
    {
      if (! (fp = fopen (globbuf->gl_pathv[i], "r")))
	continue;
      plist = mplist__from_file (fp, load_key);
      fclose (fp);
      if (! plist)
	continue;
      if (MPLIST_PLIST_P (plist))
	{
	  MPlist *pl;
	  MSymbol tags2[4];

	  for (j = 0, pl = MPLIST_PLIST (plist); j < 4 && MPLIST_SYMBOL_P (pl);
	       j++, pl = MPLIST_NEXT (pl))
	    tags2[j] = MPLIST_SYMBOL (pl);
	  for (; j < 4; j++)
	    tags2[j] = Mnil;
	  for (j = 0; j < 4; j++)
	    if (tags[j] == Masterisk ? tags2[j] == Mnil
		: (tags[j] != Mnil && tags[j] != tags2[j]))
	      break;
	  if (j == 4)
	    register_database (tags2, load_database,
			       globbuf->gl_pathv[i] + headlen,
			       MDB_STATUS_AUTO);
	}
      M17N_OBJECT_UNREF (plist);
    }
  M17N_OBJECT_UNREF (load_key);
}


/* Internal API */

/** List of database directories.  */ 
MPlist *mdatabase__dir_list;

int
mdatabase__init ()
{
  MDatabaseInfo *dir_info;
  char *path;

  Mchar_table = msymbol ("char-table");
  Masterisk = msymbol ("*");

  mdatabase__dir_list = mplist ();
  /** The macro M17NDIR specifies a directory where the system-wide
    MDB_DIR file exists.  */
  mplist_set (mdatabase__dir_list, Mt, get_dir_info (M17NDIR));

  /* The variable mdatabase_dir specifies a directory where an
     application program specific MDB_DIR file exists.  */
  if (mdatabase_dir && strlen (mdatabase_dir) > 0)
    mplist_push (mdatabase__dir_list, Mt, get_dir_info (mdatabase_dir));

  /* The environment variable M17NDIR specifies a directory where a
     user specific MDB_DIR file exists.  */
  path = getenv ("M17NDIR");
  if (path && strlen (path) > 0)
    mplist_push (mdatabase__dir_list, Mt, get_dir_info (path));
  else
    {
      /* If the env var M17NDIR is not set, check "~/.m17n.d".  */
      char *home = getenv ("HOME");
      int len;

      if (home
	  && (len = strlen (home))
	  && (path = alloca (len + 9)))
	{
	  strcpy (path, home);
	  if (path[len - 1] != PATH_SEPARATOR)
	    path[len++] = PATH_SEPARATOR;
	  strcpy (path + len, ".m17n.d");
	  dir_info = get_dir_info (path);
	  mplist_push (mdatabase__dir_list, Mt, dir_info);
	}
      else
	mplist_push (mdatabase__dir_list, Mt, get_dir_info (NULL));
    }

  mdatabase__finder = ((void *(*) (MSymbol, MSymbol, MSymbol, MSymbol))
		       mdatabase_find);
  mdatabase__loader = (void *(*) (void *)) mdatabase_load;

  mdatabase__list = mplist ();
  mdatabase__update ();
  return 0;
}

void
mdatabase__fini (void)
{
  MPlist *plist, *p0, *p1, *p2, *p3;

  MPLIST_DO (plist, mdatabase__dir_list)
    free_db_info (MPLIST_VAL (plist));
  M17N_OBJECT_UNREF (mdatabase__dir_list);

  /* MDATABASE_LIST ::= ((TAG0 (TAG1 (TAG2 (TAG3 t:MDB) ...) ...) ...) ...) */
  MPLIST_DO (plist, mdatabase__list)
    {
      p0 = MPLIST_PLIST (plist);
      /* P0 ::= (TAG0 (TAG1 (TAG2 (TAG3 t:MDB) ...) ...) ...) */
      MPLIST_DO (p0, MPLIST_NEXT (p0))
	{
	  p1 = MPLIST_PLIST (p0);
	  /* P1 ::= (TAG1 (TAG2 (TAG3 t:MDB) ...) ...) */
	  MPLIST_DO (p1, MPLIST_NEXT (p1))
	    {
	      p2 = MPLIST_PLIST (p1);
	      /* P2 ::= (TAG2 (TAG3 t:MDB) ...) */
	      MPLIST_DO (p2, MPLIST_NEXT (p2))
		{
		  MDatabase *mdb;

		  p3 = MPLIST_PLIST (p2); /* P3 ::= (TAG3 t:MDB) */
		  p3 = MPLIST_NEXT (p3);
		  mdb = MPLIST_VAL (p3);
		  if (mdb->loader == load_database)
		    free_db_info (mdb->extra_info);
		  free (mdb);
		}
	    }
	}
    }
  M17N_OBJECT_UNREF (mdatabase__list);
}

void
mdatabase__update (void)
{
  MPlist *plist, *p0, *p1, *p2, *p3;
  char path[PATH_MAX + 1];
  MDatabaseInfo *dir_info;
  struct stat statbuf;
  int rescan = 0;

  /* Update elements of mdatabase__dir_list.  */
  MPLIST_DO (plist, mdatabase__dir_list)
    {
      dir_info = MPLIST_VAL (plist);
      if (dir_info->filename)
	{
	  enum MDatabaseStatus status;

	  if (stat (dir_info->filename, &statbuf) == 0
	      && (statbuf.st_mode & S_IFDIR))
	    status = ((dir_info->time >= statbuf.st_mtime)
		      ? MDB_STATUS_EXPLICIT : MDB_STATUS_AUTO);
	  else
	    status = MDB_STATUS_DISABLED;

	  if (dir_info->status != status)
	    {
	      dir_info->status = status;
	      rescan = 1;
	    }
	  else if (dir_info->status == MDB_STATUS_AUTO)
	    rescan = 1;
	}
    }

  if (! rescan)
    return;

  /* At first, mark all databases defined automatically from mdb.dir
     file(s) as "disabled".  */
  MPLIST_DO (plist, mdatabase__list)
    {
      p0 = MPLIST_PLIST (plist);
      /* P0 ::= (TAG0 (TAG1 (TAG2 (TAG3 MDB) ...) ...) ...) */
      MPLIST_DO (p0, MPLIST_NEXT (p0))
	{
	  p1 = MPLIST_PLIST (p0);
	  MPLIST_DO (p1, MPLIST_NEXT (p1))
	    {
	      p2 = MPLIST_PLIST (p1);
	      MPLIST_DO (p2, MPLIST_NEXT (p2))
		{
		  MDatabase *mdb;
		  MDatabaseInfo *db_info;

		  p3 = MPLIST_PLIST (p2);
		  p3 = MPLIST_NEXT (p3);
		  mdb = MPLIST_VAL (p3);
		  db_info = mdb->extra_info;
		  if (db_info->status == MDB_STATUS_AUTO)
		    db_info->status = MDB_STATUS_DISABLED;
		}
	    }
	}
    }

  plist = mplist (); 
  MPLIST_DO (p0, mdatabase__dir_list)
    mplist_push (plist, MPLIST_KEY (p0), MPLIST_VAL (p0));

  while (! MPLIST_TAIL_P (plist))
    {
      MDatabaseInfo *dir_info = mplist_pop (plist);
      MPlist *pl, *p;
      int i;
      FILE *fp;

      if (dir_info->status == MDB_STATUS_DISABLED)
	continue;
      if (! GEN_PATH (path, dir_info->filename, dir_info->len,
		      MDB_DIR, MDB_DIR_LEN))
	continue;
      if (stat (path, &statbuf) < 0)
	continue;
      dir_info->time = statbuf.st_mtime;
      if (! (fp = fopen (path, "r")))
	continue;
      pl = mplist__from_file (fp, NULL);
      fclose (fp);
      if (! pl)
	continue;
      MPLIST_DO (p, pl)
	{
	  MSymbol tags[4];
	  MPlist *p1;
	  MText *mt;
	  int nbytes;
	  int with_wildcard = 0;

	  if (! MPLIST_PLIST_P (p))
	    continue;
	  for (i = 0, p1 = MPLIST_PLIST (p); i < 4 && MPLIST_SYMBOL_P (p1);
	       i++, p1 = MPLIST_NEXT (p1))
	    with_wildcard |= ((tags[i] = MPLIST_SYMBOL (p1)) == Masterisk);
	  if (i == 0
	      || tags[0] == Masterisk
	      || ! MPLIST_MTEXT_P (p1))
	    continue;
	  for (; i < 4; i++)
	    tags[i] = Mnil;
	  mt = MPLIST_MTEXT (p1);
	  if (mt->format >= MTEXT_FORMAT_UTF_16LE)
	    mtext__adjust_format (mt, MTEXT_FORMAT_UTF_8);
	  nbytes = mtext_nbytes (mt);
	  if (nbytes > PATH_MAX)
	    continue;
	  memcpy (path, MTEXT_DATA (mt), nbytes);
	  path[nbytes] = '\0';
	  if (with_wildcard)
	    {
	      glob_t globbuf;
	      MPlist *dlist;

	      if (tags[0] == Mchar_table || tags[0] == Mcharset)
		continue;
	      if (path[0] == PATH_SEPARATOR)
		{
		  if (glob (path, GLOB_NOSORT, NULL, &globbuf))
		    continue;
		  register_databases_in_files (tags, &globbuf, 0);
		  globfree (&globbuf);
		}
	      else
		MPLIST_DO (dlist, mdatabase__dir_list)
		  {
		    MDatabaseInfo *d_info = MPLIST_VAL (dlist);

		    if (d_info->status == MDB_STATUS_DISABLED)
		      continue;
		    if (! GEN_PATH (path, d_info->filename, d_info->len,
				    MTEXT_DATA (mt), nbytes))
		      continue;
		    if (glob (path, GLOB_NOSORT, NULL, &globbuf))
		      continue;
		    register_databases_in_files (tags, &globbuf, d_info->len);
		    globfree (&globbuf);
		  }
	    }
	  else
	    {
	      register_database (tags, load_database, path, MDB_STATUS_AUTO);
	    }
	}
      M17N_OBJECT_UNREF (pl);
    }
}

MPlist *
mdatabase__load_for_keys (MDatabase *mdb, MPlist *keys)
{
  int mdebug_mask = MDEBUG_DATABASE;
  MDatabaseInfo *db_info;
  char *filename;
  FILE *fp;
  MPlist *plist;
  char name[256];

  if (mdb->loader != load_database
      || mdb->tag[0] == Mchar_table
      || mdb->tag[0] == Mcharset)
    MERROR (MERROR_DB, NULL);
  MDEBUG_PRINT1 (" [DB]  <%s>.\n",
		 gen_database_name (name, mdb->tag));
  db_info = mdb->extra_info;
  filename = get_database_file (db_info, NULL);
  if (! filename || ! (fp = fopen (filename, "r")))
    MERROR (MERROR_DB, NULL);
  plist = mplist__from_file (fp, keys);
  fclose (fp);
  return plist;
}


/* Check if the database MDB should be reloaded or not.  It returns:

	1: The database has not been updated since it was loaded last
	time.

	0: The database has never been loaded or has been updated
	since it was loaded last time.

	-1: The database is not loadable at the moment.  */

int
mdatabase__check (MDatabase *mdb)
{
  MDatabaseInfo *db_info = (MDatabaseInfo *) mdb->extra_info;
  struct stat buf;

  if (! get_database_file (db_info, &buf))
    return -1;
  if (db_info->time < buf.st_mtime)
    return 0;
  return 1;
}

/* Search directories in mdatabase__dir_list for file FILENAME.  If
   the file exist, return the absolute pathname.  If FILENAME is
   already absolute, return a copy of it.  */

char *
mdatabase__find_file (char *filename)
{
  struct stat buf;
  MDatabaseInfo db_info;

  if (filename[0] == PATH_SEPARATOR)
    return (stat (filename, &buf) == 0 ? filename : NULL);
  db_info.filename = filename;
  db_info.len = strlen (filename);
  db_info.time = 0;
  db_info.absolute_filename = NULL;
  if (! get_database_file (&db_info, &buf)
      || stat (db_info.absolute_filename, &buf) < 0)
    return NULL;
  return db_info.absolute_filename;
}

char *
mdatabase__file (MDatabase *mdb)
{
  MDatabaseInfo *db_info;

  if (mdb->loader != load_database)
    return NULL;
  db_info = mdb->extra_info;
  return get_database_file (db_info, NULL);
}

int
mdatabase__lock (MDatabase *mdb)
{
  MDatabaseInfo *db_info;
  struct stat buf;
  FILE *fp;
  int len;
  char *file;

  if (mdb->loader != load_database)
    return -1;
  db_info = mdb->extra_info;
  if (db_info->lock_file)
    return -1;
  file = get_database_file (db_info, NULL);
  if (! file)
    return -1;
  len = strlen (file);
  db_info->uniq_file = malloc (len + 35);
  if (! db_info->uniq_file)
    return -1;
  db_info->lock_file = malloc (len + 5);
  if (! db_info->lock_file)
    {
      free (db_info->uniq_file);
      return -1;
    }
  sprintf (db_info->uniq_file, "%s.%X.%X", db_info->absolute_filename,
	   (unsigned) time (NULL), (unsigned) getpid ());
  sprintf (db_info->lock_file, "%s.LCK", db_info->absolute_filename);

  fp = fopen (db_info->uniq_file, "w");
  if (! fp)
    {
      char *str = strdup (db_info->uniq_file);
      char *dir = dirname (str);
      
      if (stat (dir, &buf) == 0
	  || mkdir (dir, 0777) < 0
	  || ! (fp = fopen (db_info->uniq_file, "w")))
	{
	  free (db_info->uniq_file);
	  free (db_info->lock_file);
	  db_info->lock_file = NULL;
	  free (str);
	  return -1;
	}
      free (str);
    }
  fclose (fp);
  if (link (db_info->uniq_file, db_info->lock_file) < 0
      && (stat (db_info->uniq_file, &buf) < 0
	  || buf.st_nlink != 2))
    {
      unlink (db_info->uniq_file);
      unlink (db_info->lock_file);
      free (db_info->uniq_file);
      free (db_info->lock_file);
      db_info->lock_file = NULL;
      return 0;
    }
  return 1;
}

int
mdatabase__save (MDatabase *mdb, MPlist *data)
{
  MDatabaseInfo *db_info;
  FILE *fp;
  char *file;
  MText *mt;
  int ret;

  if (mdb->loader != load_database)
    return -1;
  db_info = mdb->extra_info;
  if (! db_info->lock_file)
    return -1;
  file = get_database_file (db_info, NULL);
  if (! file)
    return -1;
  mt = mtext ();
  if (mplist__serialize (mt, data, 1) < 0)
    {
      M17N_OBJECT_UNREF (mt);
      return -1;
    }
  fp = fopen (db_info->uniq_file, "w");
  if (! fp)
    {
      M17N_OBJECT_UNREF (mt);
      return -1;
    }
  mconv_encode_stream (msymbol ("utf-8"), mt, fp);
  M17N_OBJECT_UNREF (mt);
  fclose (fp);
  if ((ret = rename (db_info->uniq_file, file)) < 0)
    unlink (db_info->uniq_file);
  free (db_info->uniq_file);
  db_info->uniq_file = NULL;
  return ret;
}

int
mdatabase__unlock (MDatabase *mdb)
{
  MDatabaseInfo *db_info;

  if (mdb->loader != load_database)
    return -1;
  db_info = mdb->extra_info;
  if (! db_info->lock_file)
    return -1;
  unlink (db_info->lock_file);
  free (db_info->lock_file);
  db_info->lock_file = NULL;
  if (db_info->uniq_file)
    {
      unlink (db_info->uniq_file);
      free (db_info->uniq_file);
    }
  return 0;
}

/*** @} */
#endif /* !FOR_DOXYGEN || DOXYGEN_INTERNAL_MODULE */


/* External API */

/*** @addtogroup m17nDatabase */
/*** @{ */

/*=*/
/***en
    @brief Directory for application specific data.

    If an application program wants to provide a data specific to the
    program or a data overriding what supplied by the m17n database,
    it must set this variable to a name of directory that contains the
    data files before it calls the macro M17N_INIT ().  The directory
    may contain a file "mdb.dir" which contains a list of data
    definitions in the format described in @ref mdbDir "mdbDir(5)".

    The default value is NULL.  */
/***ja
    @brief ���ץꥱ��������ͭ�Υǡ����ѥǥ��쥯�ȥ�.

    ���ץꥱ�������ץ���ब�����Υץ�����ͭ�Υǡ����� m17n 
    �ǡ����١������񤭤���ǡ������󶡤�����ˤϡ��ޥ��� M17N_INIT () 
    ��Ƥ����ˤ����ѿ���ǡ����ե������ޤ�ǥ��쥯�ȥ�̾�˥��åȤ��ʤ��ƤϤʤ�ʤ����ǥ��쥯�ȥ�ˤ�
    "mdb.dir" �ե�����򤪤����Ȥ��Ǥ��롣����"mdb.dir"�ե�����ˤϡ� 
    @ref mdbDir "mdbDir(5)" ����������Ƥ���ե����ޥåȤǥǡ�������Υꥹ�Ȥ򵭽Ҥ��롣

    �ǥե���Ȥ��ͤ� NULL �Ǥ��롣  */

char *mdatabase_dir;

/*=*/
/***en
    @brief Look for a data in the database.

    The mdatabase_find () function searches the m17n database for a
    data who has tags $TAG0 through $TAG3, and returns a pointer to
    the data.  If such a data is not found, it returns @c NULL.  */

/***ja
    @brief �ǡ����١�����Υǡ�����õ��.

    �ؿ� mdatabase_find () �ϡ� m17n �������١������ $TAG0 ���� 
    $TAG3 �ޤǤΥ�������ĥǡ�����õ��������ؤΥݥ��󥿤��֤������Τ褦�ʥǡ������ʤ����
    @c NULL ���֤���

    @latexonly \IPAlabel{mdatabase_find} @endlatexonly  */

MDatabase *
mdatabase_find (MSymbol tag0, MSymbol tag1, MSymbol tag2, MSymbol tag3)
{
  MSymbol tags[4];

  mdatabase__update ();
  tags[0] = tag0, tags[1] = tag1, tags[2] = tag2, tags[3] = tag3;
  return find_database (tags);
}

/*=*/
/***en
    @brief Return a data list of the m17n database.

    The mdatabase_list () function searches the m17n database for data
    who have tags $TAG0 through $TAG3, and returns their list by a
    plist.  The value #Mnil in $TAGn means a wild card that matches
    any tag.  Each element of the plist has key #Mt and value a
    pointer to type #MDatabase.  */
/***ja
    @brief m17n �ǡ����١����Υǡ����ꥹ�Ȥ��֤�.

    �ؿ� mdatabase_list () �� m17n �ǡ����١����椫�� $TAG0 ����$TAG3 
    �ޤǤΥ�������ĥǡ�����õ�������Υꥹ�Ȥ�plist �Ȥ����֤��� $TAGn �� #Mnil
    �Ǥ��ä����ˤϡ�Ǥ�դΥ����˥ޥå�����磻��ɥ����ɤȤ��Ƽ�갷���롣�֤����
    plist �γ����Ǥϥ��� �Ȥ��� #Mt ���ͤȤ��� #MDatabase ���ؤΥݥ��󥿤���ġ�  */

MPlist *
mdatabase_list (MSymbol tag0, MSymbol tag1, MSymbol tag2, MSymbol tag3)
{
  MPlist *plist = mplist (), *pl = plist;
  MPlist *p, *p0, *p1, *p2, *p3;

  mdatabase__update ();

  MPLIST_DO (p, mdatabase__list)
    {
      p0 = MPLIST_PLIST (p);
      /* P0 ::= (TAG0 (TAG1 (TAG2 (TAG3 MDB) ...) ...) ...) */
      if (tag0 != Mnil && MPLIST_SYMBOL (p0) != tag0)
	continue;
      MPLIST_DO (p0, MPLIST_NEXT (p0))
	{
	  p1 = MPLIST_PLIST (p0);
	  if (tag1 != Mnil && MPLIST_SYMBOL (p1) != tag1)
	    continue;
	  MPLIST_DO (p1, MPLIST_NEXT (p1))
	    {
	      p2 = MPLIST_PLIST (p1);
	      if (tag2 != Mnil && MPLIST_SYMBOL (p2) != tag2)
		continue;
	      MPLIST_DO (p2, MPLIST_NEXT (p2))
		{
		  p3 = MPLIST_PLIST (p2);
		  if (tag3 != Mnil && MPLIST_SYMBOL (p3) != tag3)
		    continue;
		  p3 = MPLIST_NEXT (p3);
		  pl = mplist_add (pl, Mt, MPLIST_VAL (p3));
		}
	    }
	}
    }
  if (MPLIST_TAIL_P (plist))
    {
      M17N_OBJECT_UNREF (plist);
      plist = NULL;
    }
  return plist;
}

/*=*/
/***en
    @brief Define a data of the m17n database.

    The mdatabase_define () function defines a data that has tags
    $TAG0 through $TAG3 and additional information $EXTRA_INFO.

    $LOADER is a pointer to a function that loads the data from the
    database.  This function is called from the mdatabase_load ()
    function with the two arguments $TAGS and $EXTRA_INFO.  Here,
    $TAGS is the array of $TAG0 through $TAG3.

    If $LOADER is @c NULL, the default loader of the m17n library is
    used.  In this case, $EXTRA_INFO must be a string specifying a
    filename that contains the data.

    @return
    If the operation was successful, mdatabase_define () returns a
    pointer to the defined data, which can be used as an argument to
    mdatabase_load ().  Otherwise, it returns @c NULL.  */

/***ja
    @brief m17n �ǡ����١����Υǡ������������.

    �ؿ� mdatabase_define () �� $TAG0 ���� $TAG3 �ޤǤΥ���������ղþ��� 
    $EXTRA_INFO ����ĥǡ�����������롣

    $LOADER �Ϥ��Υǡ����Υ��ɤ��Ѥ�����ؿ��ؤΥݥ��󥿤Ǥ��롣���δؿ���
    mdatabase_load () ���� $TAGS �� $EXTRA_INFO �Ȥ�����Ĥΰ����դ��ǸƤӽФ���롣������ 
    $TAGS �� $TAG0 ���� $TAG3 �ޤǤ�����Ǥ��롣

    �⤷ $LOADER �� @c NULL �ʤ顢m17n �饤�֥��ɸ��Υ������Ȥ��롣���ξ��ˤ�
    $EXTRA_INFO �ϥǡ�����ޤ�ե�����̾�Ǥʤ��ƤϤʤ�ʤ���

    @return
    ��������������� mdatabase_define () 
    ��������줿�ǡ����١����ؤΥݥ��󥿤��֤������Υݥ��󥿤ϴؿ� mdatabase_load () 
    �ΰ����Ȥ����Ѥ��뤳�Ȥ��Ǥ��롣�����Ǥʤ���� @c NULL ���֤���

    @latexonly \IPAlabel{mdatabase_define} @endlatexonly  */

/***
    @seealso
    mdatabase_load (),  mdatabase_define ()  */

MDatabase *
mdatabase_define (MSymbol tag0, MSymbol tag1, MSymbol tag2, MSymbol tag3,
		  void *(*loader) (MSymbol *, void *),
		  void *extra_info)
{
  MDatabase *mdb;
  MSymbol tags[4];

  tags[0] = tag0, tags[1] = tag1, tags[2] = tag2, tags[3] = tag3;
  if (! loader)
    loader = load_database;
  mdb = register_database (tags, loader, extra_info, MDB_STATUS_EXPLICIT);
  return mdb;
}

/*=*/
/***en
    @brief Load a data from the database.

    The mdatabase_load () function loads a data specified in $MDB and
    returns the contents.  The type of contents depends on the type of
    the data.

    If the data is of the @e plist @e type, this function returns a
    pointer to @e plist.

    If the database is of the @e chartable @e type, it returns a
    chartable.  The default value of the chartable is set according to
    the second tag of the data as below:

    @li If the tag is #Msymbol, the default value is #Mnil.
    @li If the tag is #Minteger, the default value is -1.
    @li Otherwise, the default value is @c NULL.

    If the data is of the @e charset @e type, it returns a plist of length 2
    (keys are both #Mt).  The value of the first element is an array
    of integers that maps code points to the corresponding character
    codes.  The value of the second element is a chartable of integers
    that does the reverse mapping.  The charset must be defined in
    advance.  */


/***ja
    @brief �ǡ����١�������ǡ�������ɤ���.

    �ؿ� mdatabase_load () �� $MDB 
    ���ؤ��ǡ�������ɤ���������Ȥ��֤����֤�����Τϥǡ����Υ����פˤ�äưۤʤ롣

    �ǡ����� @e plist������ �ʤ�С� @e plist �ؤΥݥ��󥿤��֤���

    �ǡ����� @e chartable������ �ʤ��ʸ���ơ��֥���֤���
    ʸ���ơ��֥�Υǥե�����ͤϡ��ǡ�������2�����ˤ�äưʲ��Τ褦�˷�ޤ롣

    @li ������ #Msymbol �ʤ顢�ǥե�����ͤ� #Mnil
    @li ������ #Minteger �ʤ顢�ǥե�����ͤ� -1
    @li ����ʳ��ʤ顢�ǥե�����ͤ� @c NULL

    �ǡ����� @e charset������ �ʤ��Ĺ�� 2 �� plist ���֤��ʥ����϶���#Mt �ˡ�
    �ǽ�����Ǥ��ͤϥ����ɥݥ���Ȥ��б�����ʸ�������ɤ˥ޥåפ�������������Ǥ��롣
    �����ܤ����Ǥ��ͤϵդΥޥåפ򤹤�ʸ���ơ��֥�Ǥ��롣
    ����ʸ�����åȤ�ͽ���������Ƥ��ʤ���Фʤ�ʤ���

    @latexonly \IPAlabel{mdatabase_load} @endlatexonly
  */

/***
    @seealso
    mdatabase_load (),  mdatabase_define ()  */

void *
mdatabase_load (MDatabase *mdb)
{
  return (*mdb->loader) (mdb->tag, mdb->extra_info);
}

/*=*/
/***en
    @brief Get tags of a data.

    The mdatabase_tag () function returns an array of tags (symbols)
    that identify the data in $MDB.  The length of the array is
    four.  */

/***ja
    @brief �ǡ����Υ���������.

    �ؿ� mdatabase_tag () �ϡ��ǡ��� $MDB �Υ����ʥ���ܥ�ˤ�������֤��������Ĺ����
    4 �Ǥ��롣

    @latexonly \IPAlabel{mdatabase_tag} @endlatexonly  */

MSymbol *
mdatabase_tag (MDatabase *mdb)
{
  return mdb->tag;
}

/*** @} */

/*
  Local Variables:
  coding: euc-japan
  End:
*/
