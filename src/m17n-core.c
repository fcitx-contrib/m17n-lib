/* m17n-core.c -- body of the CORE API.
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
    @addtogroup m17nIntro
    @brief Introduction to the m17n library.

    <em>API LEVELS</em>

    The API of the m17n library is divided into these four.

    <ol>
    <li> CORE API

    It provides basic modules to handle M-texts.  They do not require
    the m17n database.  To use this API, an application program must
    include <m17n-core.h> and be linked by -lm17n-core.

    <li> SHELL API

    It provides modules that utilize the m17n database (code
    conversion, character property, etc).  They load various kinds of
    data from the database on demand.  To use this API, an application
    program must include <m17n.h> and be linked by -lm17n-core -lm17n.

    When you use this API, CORE API is also available.

    <li> GUI API

    It provides GUI modules such as drawing and inputting M-texts on a
    graphic device.  The API itself is independent on a graphic device
    but most functions require an argument MFrame which is created for
    a specific type of graphic device.  Currently, a null device, X
    Window System, and an image date (gdImagePtr) of GD library are
    supported as a graphic device.

    On a frame of the null device, you can't draw text nor use an
    input method (but, for instance, the funciton mdraw_glyph_list ()
    is available).

    On a frame of the X Window System, you can use the whole GUI API.

    On a frame of GD library, you can use all drawing API but can't
    use input method.

    To use this device, an application program must include
    <m17n-gui.h> and be linked by -lm17n-core -lm17n -lm17n-gui.

    When you use this API, CORE and SHELL API are also available.

    <li> MISC API

    It provides miscellaneous functions to support error handling and
    debugging.  This API cannot be used by itself, but with one or
    more APIs listed above.  To use the API, an application program
    must include <m17n-misc.h> in addition to one of a header file
    described above.

    </ol>

    See also the section @ref m17n-config "m17n-config(1)".

    <em>ENVIRONMENT VARIABLE</em>

    The m17n library pays attention to these environment variables.

    <ul>
    <li> @c M17NDIR

    Name of a directory that contains data of the m17n database.  See
    @ref m17nDatabase for more details.

    <li> @c MDEBUG_XXXX

    Environment variables whose name start by "MDEBUG_" controls
    printing of debug information.  See @ref m17nDebug for more
    details.

    </ul>

    <em>API NAMING CONVENTION</em>

    The library exports functions, variables, macros, and types.  All
    of them start by the letter 'm' or 'M' followed by an object name
    (e.g. "symbol" and "plist", but "mtext" object is given the name
    "text" to avoid double 'm' at the head) or a module name
    (e.g. draw, input).
    
    <ul>

    <li> functions -- mobject () or mobject_xxx ()

    They start with 'm' followed by lower case object name.  For
    example, msymbol (), mtext_ref_char (), mdraw_text ().

    <li> non-symbol variables -- mobject, or mobject_xxx
    
    The naming convention is the same as functions (e.g. mface_large).

    <li> symbol variables -- Mname

    Variables of type MSymbol start with 'M' followed by their names
    (e.g. Mlanguage (name is "language"), Miso_2022 (name is
    "iso-2022").

    <li> macros -- MOBJECT_XXX

    They start by 'M' followed by upper case object names.

    <li> types -- MObject or MObjectXxx

    They start by 'M' followed by capitalized object names (e.g.
    MConverter, MInputDriver).

    </ul>

  */

/***ja
    @addtogroup m17nIntro
    @brief m17n �饤�֥�� ����ȥ���������.

    @em API�Υ�٥�

    m17n �饤�֥��� API �ϰʲ��Σ����ʬ�व��Ƥ��롣

    <ol>
    <li> ���� API

    M-text �򰷤�����δ���Ū�ʥ⥸�塼����󶡤��롣����ʬ��� API ��
    m17n �ǡ����١�����ɬ�פȤ��ʤ������Ѥ��뤿��ˤϡ����ץꥱ������
    ��ץ�����<m17n-core.h> �� include ���� -lm17n-core �ǥ�󥯤�
    ��ʤ��ƤϤʤ�ʤ���

    <li> ������ API

    m17n �ǡ����١��������Ѥ���⥸�塼��ʥ������Ѵ���ʸ���ץ�ѥƥ�
    ���ˤ��󶡤��롣�⥸�塼��ϥǡ����١�������ɬ�פ˱�����¿�ͤʥǡ�
    ������ɤ��롣���Ѥ��뤿��ˤϡ����ץꥱ�������ץ�����
    <m17n.h> �� include ���� -lm17n-core -lm17n �ǥ�󥯤���ʤ��ƤϤ�
    ��ʤ���

    ���� API ����Ѥ���ݤˤϥ��� API ����ѤǤ��롣

    <li> GUI API

    M-texts �򥰥�ե��å��ǥХ������ɽ�����������Ϥ����ꤹ��Ȥ��ä���
    ����ե��å��桼�����󥿥ե������⥸�塼����󶡤��롣API ���Τϥ�
    ��ե��å��ǥХ����Ȥ���Ω�Ǥ��뤬��¿���δؿ�������Υ���ե��å�
    �ǥХ�����˺������줿 MFrame ������˼�롣�������Ǥϡ��̥�ǥХ�
    ����X ������ɥ������ƥࡢGD �饤�֥��Υ��᡼����gdImagePtr�ˡ���
    ����ե��å��ǥХ����Ȥ��ƥ��ݡ��Ȥ���Ƥ��롣

    �̥�ǥХ�����Ǥ�ɽ�������Ϥ�Ǥ��ʤ��ʤ��������Ȥ���
    mdraw_glyph_list () �ʤɤδؿ��ϻ��Ѳ�ǽ�ˡ�

    X ������ɥ������ƥ��ǤϤ��٤Ƥ� GUI API ���Ȥ��롣

    GD �饤�֥��Υ��᡼����Ǥϡ������Ѥ� API �Ϥ��٤ƻȤ��뤬���Ϥ�
    �Ǥ��ʤ���

    ���ΥǥХ�������Ѥ��뤿��ˤϡ����ץꥱ�������ץ�����
    <m17n-gui.h> �� include ����-lm17n-core -lm17n -lm17n-gui �ǥ��
    ����ʤ��ƤϤʤ�ʤ���

    ���� API ����Ѥ���ݤˤϡ����� API �ȥ����� API ����ѤǤ��롣

    <li> ����¾�� API

    ���顼�������ǥХå��ѤΤ���¾�δؿ����󶡤��롣���� API �Ϥ������
    �Ǥϻ��ѤǤ������嵭��¾�Τ�Τȶ��˻Ȥ������Ѥ��뤿��ˤϡ��嵭��
    �����줫�� include �ե�����˲ä��ơ� <m17n-misc.h> �� include ����
    ���ƤϤʤ�ʤ���

    </ol>

    @ref m17n-config "m17n-config(1)" ��⻲�ȡ�

    @em �Ķ��ѿ�

    m17n �饤�֥��ϼ��δĶ��ѿ����θ���롣

    <ul>
    <li> @c M17NDIR

    m17n �ǡ����١����Υǡ�����ޤ�ǥ��쥯�ȥ��̾�����ܺ٤�@ref
    m17nDatabase ���ȡ�

    <li> @c MDEBUG_XXXX

    "MDEBUG_" �ǻϤޤ�̾������ĴĶ��ѿ��ϥǥХå���������椹�롣�ܺ�
    ��@ref m17nDebug ���ȡ�

    </ul>

    @em API @em ��̿̾��§ ���ޥ��

    �饤�֥��ϡ��ؿ����ѿ����ޥ������� export ���롣������'m' ��
    ���� 'M' �Τ��Ȥ˥��֥�������̾("symbol" �� "plist" �ʤɡ������� 
    "mtext" ���֥������ȤϤϤ���� 'm' �ν�ʣ���򤱤뤿�� "text" ����
    ���롣) �ޤ��ϥ⥸�塼��̾(draw, input �ʤ�) ��³������ΤǤ��롣
    
    <ul>

    <li> �ؿ� -- mobject () �ޤ��� mobject_xxx ()

    'm' �Τ��Ȥ˾�ʸ���ǥ��֥�������̾��³�������Ȥ��С�msymbol (),
     mtext_ref_char (), mdraw_text () �ʤɡ�

    <li> ����ܥ�Ǥʤ��ѿ� -- mobject,  �ޤ��� mobject_xxx
    
    �ؿ���Ʊ��̿̾��§�˽�����(���Ȥ���  mface_large)

    <li> ����ܥ��ѿ� -- Mname

    MSymbol ���ѿ��ϡ�'M' �θ��̾����³�������Ȥ��� Mlanguage (̾���� 
    "language"), Miso_2022 (̾����"iso-2022")�ʤɡ�

    <li> �ޥ��� -- MOBJECT_XXX

    'M' �θ����ʸ���ǥ��֥�������̾��³����

    <li> ������ -- MObject �ޤ��� MObjectXxx

    'M' �θ����ʸ���ǻϤ�ƥ��֥�������̾��³���� (���Ȥ���
    MConverter, MInputDriver)

    </ul>
    
    */
/*=*/
/*** @{ */
#ifdef FOR_DOXYGEN
/***en
    The #M17NLIB_MAJOR_VERSION macro gives the major version number
    of the m17n library.  */

/***ja
    �ޥ��� #M17NLIB_MAJOR_VERSION �� m17n �饤�֥��Υ᥸�㡼�С�����
    ���ֹ��Ϳ���롣  */

#define M17NLIB_MAJOR_VERSION

/*=*/

/***en
    The #M17NLIB_MINOR_VERSION macro gives the minor version number
    of the m17n library.  */

/***ja
    �ޥ��� #M17NLIB_MINOR_VERSION �� m17n �饤�֥��Υޥ��ʡ��С�����
    ���ֹ��Ϳ���롣  */

#define M17NLIB_MINOR_VERSION

/*=*/

/***en
    The #M17NLIB_PATCH_LEVEL macro gives the patch level number
    of the m17n library.  */

/***ja
    �ޥ��� #M17NLIB_PATCH_LEVEL �� m17n �饤�֥��Υѥå���٥��ֹ��
    Ϳ���롣  */

#define M17NLIB_PATCH_LEVEL

/*=*/

/***en
    The #M17NLIB_VERSION_NAME macro gives the version name of the
    m17n library as a string.  */

/***ja
    �ޥ��� #M17NLIB_VERSION_NAME �� m17n �饤�֥��ΥС������̾��
    ʸ����Ȥ���Ϳ���롣  */

#define M17NLIB_VERSION_NAME

/*=*/

/***en
    @brief Initialize the m17n library.

    The macro M17N_INIT () initializes the m17n library.  This macro
    must be called before any m17n functions are used.

    If the initialization was successful, the external variable
    #merror_code is set to 0.  Otherwise it is set to -1.  */

/***ja
    @brief m17n �饤�֥�����������.

    �ޥ��� M17N_INIT () �� m17n �饤�֥����������롣m17n �δؿ���
    ���Ѥ������ˡ����Υޥ����ޤ��ƤФʤ��ƤϤʤ�ʤ���
    
    �����ѿ� #merror_code �ϡ����������������� 0 �ˡ������Ǥʤ���� 
    -1 �����ꤵ��롣  */

#define M17N_INIT()

/*=*/

/***en
    @brief Finalize the m17n library.

    The macro M17N_FINI () finalizes the m17n library.  It frees all the
    memory area used by the m17n library.  Once this macro is
    called, no m17n functions should be used until the
    macro M17N_INIT () is called again.  */
/***ja
    @brief m17n �饤�֥���λ����. 

    �ޥ��� M17N_FINI () �� m17n �饤�֥���λ���롣m17n �饤�֥��
    ���Ȥä����ƤΥ����ΰ�ϲ�������롣���٤��Υޥ����ƤФ줿�顢
    �ޥ��� M17N_INIT () �����ٸƤФ��ޤ� m17n �ؿ��ϻȤ��٤��Ǥʤ���
 */

#define M17N_FINI()
#endif /* FOR_DOXYGEN */
/*=*/
/*** @} */ 
/*=*/

#if !defined (FOR_DOXYGEN) || defined (DOXYGEN_INTERNAL_MODULE)
/*** @addtogroup m17nInternal
     @{ */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

#include "m17n-core.h"
#include "m17n-misc.h"
#include "internal.h"

static int core_initialized;

static void
default_error_handler (enum MErrorCode err)
{
  exit (err);
}

static struct timeval time_stack[16];
static int time_stack_index;

static int report_header_printed;


/* Internal API */

void
mdebug__report_object (char *name, M17NObjectArray *array)
{
  if (! (mdebug__flag & MDEBUG_FINI))
    return;
  if (! report_header_printed)
    {
      fprintf (stderr, "%16s %7s %7s %7s\n",
	       "object", "created", "freed", "alive");
      fprintf (stderr, "%16s %7s %7s %7s\n",
	       "------", "-------", "-----", "-----");
      report_header_printed = 1;
    }
  fprintf (stderr, "%16s %7d %7d %7d\n", name,
	   array->used, array->used - array->count, array->count);
  if (array->used > 0)
    {
      free (array->objects);
      array->count = array->used = 0;
    }
}


void *(*mdatabase__finder) (MSymbol tag1, MSymbol tag2,
			   MSymbol tag3, MSymbol tag4);
void *(*mdatabase__loader) (void *);

int mdebug__flag;

void
mdebug__push_time ()
{
  struct timezone tz;

  gettimeofday (time_stack + time_stack_index++, &tz);
}

void
mdebug__pop_time ()
{
  time_stack_index--;
}

void
mdebug__print_time ()
{
  struct timeval tv;
  struct timezone tz;
  long diff;

  gettimeofday (&tv, &tz);
  diff = ((tv.tv_sec - time_stack[time_stack_index - 1].tv_sec) * 1000000
	  + (tv.tv_usec - time_stack[time_stack_index - 1].tv_usec));
  fprintf (stderr, "%8ld ms.", diff);
  time_stack[time_stack_index - 1] = tv;
}

#define SET_DEBUG_FLAG(env_name, mask)		\
  do {						\
    char *env_value = getenv (env_name);	\
						\
    if (env_value && env_value[0] == '1')	\
      mdebug__flag |= (mask);			\
  } while (0)


void
mdebug__register_object (M17NObjectArray *array, void *object)
{
  if (array->used == 0)
    MLIST_INIT1 (array, objects, 256);
  array->count++;
  MLIST_APPEND1 (array, objects, object, MERROR_OBJECT);
}

void
mdebug__unregister_object (M17NObjectArray *array, void *object)
{
  array->count--;
  if (array->count >= 0)
    {
      int i = 0;

      while (i < array->used && array->objects[i] != object) i++;
      if (i < array->used)
	array->objects[i] = NULL;
      else
	mdebug_hook ();
    }
  else									\
    mdebug_hook ();
}


/* External API */

/* The following two are actually not exposed to a user but concealed
   by the macro M17N_INIT (). */

void
m17n_init_core (void)
{
  int mdebug_mask = MDEBUG_INIT;

  if (core_initialized++)
    return;

  merror_code = MERROR_NONE;
  m17n_memory_full_handler = default_error_handler;

  mdebug__flag = 0;
  SET_DEBUG_FLAG ("MDEBUG_INIT", MDEBUG_INIT);
  SET_DEBUG_FLAG ("MDEBUG_FINI", MDEBUG_FINI);
  SET_DEBUG_FLAG ("MDEBUG_CHARSET", MDEBUG_CHARSET);
  SET_DEBUG_FLAG ("MDEBUG_CODING", MDEBUG_CODING);
  SET_DEBUG_FLAG ("MDEBUG_DATABASE", MDEBUG_DATABASE);
  SET_DEBUG_FLAG ("MDEBUG_FONT", MDEBUG_FONT); 
  SET_DEBUG_FLAG ("MDEBUG_FONT_FLT", MDEBUG_FONT_FLT);
  SET_DEBUG_FLAG ("MDEBUG_FONT_OTF", MDEBUG_FONT_OTF);
  SET_DEBUG_FLAG ("MDEBUG_INPUT", MDEBUG_INPUT);

  MDEBUG_PUSH_TIME ();
  MDEBUG_PUSH_TIME ();
  if (msymbol__init () < 0)
    goto err;
  MDEBUG_PRINT_TIME ("INIT", (stderr, " to initialize symbol module."));
  if  (mplist__init () < 0)
    goto err;
  MDEBUG_PRINT_TIME ("INIT", (stderr, " to initialize plist module."));
  if (mtext__init () < 0)
    goto err;
  if (mtext__prop_init () < 0)
    goto err;
  MDEBUG_PRINT_TIME ("INIT", (stderr, " to initialize mtext module."));
  if  (mchartable__init () < 0)
    goto err;
  MDEBUG_PRINT_TIME ("INIT", (stderr, " to initialize chartable module."));

  mdatabase__finder = NULL;
  mdatabase__loader = NULL;

 err:
  MDEBUG_POP_TIME ();
  MDEBUG_PRINT_TIME ("INIT", (stderr, " to initialize the core modules."));
  MDEBUG_POP_TIME ();
}

void
m17n_fini_core (void)
{
  int mdebug_mask = MDEBUG_FINI;

  if (core_initialized > 1)
    core_initialized--;
  else
    {
      core_initialized = 0;
      MDEBUG_PUSH_TIME ();
      MDEBUG_PUSH_TIME ();
      MDEBUG_PRINT_TIME ("FINI", (stderr, " to finalize chartable module."));
      mchartable__fini ();
      MDEBUG_PRINT_TIME ("FINI", (stderr, " to finalize textprop module."));
      mtext__prop_fini ();
      MDEBUG_PRINT_TIME ("FINI", (stderr, " to finalize mtext module."));
      mtext__fini ();
      MDEBUG_PRINT_TIME ("FINI", (stderr, " to finalize symbol module."));
      msymbol__fini ();
      MDEBUG_PRINT_TIME ("FINI", (stderr, " to finalize plist module."));
      mplist__fini ();
      MDEBUG_POP_TIME ();
      MDEBUG_PRINT_TIME ("FINI", (stderr, " to finalize the core modules."));
      MDEBUG_POP_TIME ();
    }
  report_header_printed = 0;
}

/*** @} */
#endif /* !FOR_DOXYGEN || DOXYGEN_INTERNAL_MODULE */
/*=*/

/***en
    @addtogroup m17nObject
    @brief Managed objects are objects managed by the reference count.

    There are some types of m17n objects that are managed by their
    reference count.  Those objects are called @e managed @e objects.
    When created, the reference count of a managed object is
    initialized to one.  The m17n_object_ref () function increments
    the reference count of a managed object by one, and the
    m17n_object_unref () function decrements by one.  A managed
    object is automatically freed when its reference count becomes
    zero.

    A property whose key is a managing key can have only a managed
    object as its value.  Such functions as msymbol_put () and
    mplist_put () pay special attention to such a property.

    In addition to the predefined managed object types, users can
    define their own managed object types.  See the documentation of
    the m17n_object () for more details.  */
/***ja
    @addtogroup m17nObject
    @brief ���������֥������ȤȤϻ��ȿ��ˤ�äƴ�������Ƥ��륪�֥������ȤǤ���.

    m17n ���֥������ȤΤ��뷿�Τ�Τϡ����ȿ��ˤ�äƴ�������Ƥ��롣
    �����Υ��֥������Ȥ� @e ���������֥������� �ȸƤФ�롣��������
    �������Ǥλ��ȿ��� 0 �˽��������Ƥ��롣�ؿ� m17n_object_ref () 
    �ϴ��������֥������Ȥλ��ȿ��� 1 ���䤷���ؿ�m17n_object_unref () 
    �� 1 ���餹�����ȿ��� 0 �ˤʤä����������֥������Ȥϼ�ưŪ�˲�����
    ��롣

    ���������������Ǥ���ץ�ѥƥ��ϡ��ͤȤ��ƴ��������֥������Ȥ�����
    ��롣�ؿ� msymbol_put () �� mplist_put () �ʤɤϤ����Υץ�ѥƥ�
    �����̰������롣

    ����Ѥߴ��������֥������ȥ����פ�¾�ˡ��桼����ɬ�פʴ��������֥���
    ���ȥ����פ�ʬ��������뤳�Ȥ��Ǥ��롣�ܺ٤�m17n_object () ����
    ���򻲾ȡ�  */

/*** @{  */
/*=*/
/***en
    @brief Allocate a managed object.

    The m17n_object () function allocates a new managed object of
    $SIZE bytes and sets its reference count to 1.  $FREER is the
    function that is used to free the object when the reference count
    becomes 0.  If $FREER is NULL, the object is freed by the free ()
    function.

    The heading bytes of the allocated object is occupied by
    #M17NObjectHead.  That area is reserved for the m17n library and
    application programs should never touch it.

    @return
    This function returns a newly allocated object.

    @errors
    This function never fails.  */

/***ja
    @brief ���������֥������Ȥ������Ƥ�.

    �ؿ� m17n_object () ��$SIZE �Х��Ȥο��������������֥������Ȥ���
    ���ơ����λ��ȿ��� 1 �Ȥ��롣 $FREER �ϻ��ȿ��� 0 �ˤʤä��ݤˤ���
    ���֥������Ȥ�������뤿����Ѥ�����ؿ��Ǥ��롣$FREER �� NULL��
    ��С����֥������Ȥϴؿ� free () �ˤ�äƲ�������롣

    ������Ƥ�줿���֥���������Ƭ�ΥХ��Ȥϡ�#M17NObjectHead �����롣 
    �����ΰ�� m17n �饤�֥�꤬���Ѥ���Τǡ����ץꥱ�������ץ���
    ��Ͽ���ƤϤʤ�ʤ���

    @return
    ���δؿ��Ͽ�����������Ƥ�줿���֥������Ȥ��֤���

    @errors
    ���δؿ��ϼ��Ԥ��ʤ���    */

#if EXAMPLE_CODE
typedef struct
{
  M17NObjectHead head;
  int mem1;
  char *mem2;
} MYStruct;

void
my_freer (void *obj)
{
  free (((MYStruct *) obj)->mem2);
  free (obj);
}

void
my_func (MText *mt, MSymbol key, int num, char *str)
{
  MYStruct *st = m17n_object (sizeof (MYStruct), my_freer);

  st->mem1 = num;
  st->mem2 = strdup (str);
  /* KEY must be a managing key.   */
  mtext_put_prop (mt, 0, mtext_len (mt), key, st);
  /* This sets the reference count of ST back to 1.  */
  m17n_object_unref (st);
}
#endif

void *
m17n_object (int size, void (*freer) (void *))
{
  M17NObject *obj = malloc (size);

  obj->ref_count = 1;
  obj->u.freer = freer;
  return obj;
}

/*=*/

/***en
    @brief Increment the reference count of a managed object.

    The m17n_object_ref () function increments the reference count of
    the managed object pointed to by $OBJECT.

    @return
    This function returns the resulting reference count if it fits in
    a 16-bit unsigned integer (i.e. less than 0x10000).  Otherwise, it
    return -1.

    @errors
    This function never fails.  */
/***ja
    @brief ���������֥������Ȥλ��ȿ��� 1 ���䤹.

    �ؿ� m17n_object_ref () �� $OBJECT �ǻؤ������������֥������Ȥ�
    ���ȿ��� 1 ���䤹��

    @return 
    ���δؿ��ϡ����䤷�����ȿ��� 16 �ӥåȤ����̵��������(���ʤ�� 
    0x10000 ̤��)�ˤ����ޤ�С�������֤��������Ǥʤ���� -1 ���֤���

    @errors
    ���δؿ��ϼ��Ԥ��ʤ���    */

int
m17n_object_ref (void *object)
{
  M17NObject *obj = (M17NObject *) object;
  M17NObjectRecord *record;
  unsigned *count;

  if (! obj->ref_count_extended)
    {
      if (++obj->ref_count)
	return (int) obj->ref_count;
      MSTRUCT_MALLOC (record, MERROR_OBJECT);
      record->freer = obj->u.freer;
      MLIST_INIT1 (record, counts, 1);
      MLIST_APPEND1 (record, counts, 0, MERROR_OBJECT);
      obj->u.record = record;
      obj->ref_count_extended = 1;
    }
  else
    record = obj->u.record;

  count = record->counts;
  while (*count == 0xFFFFFFFF)
    *(count++) = 0;
  (*count)++;
  if (*count == 0xFFFFFFFF)
    MLIST_APPEND1 (record, counts, 0, MERROR_OBJECT);
  return -1;
}

/*=*/

/***en
    @brief Decrement the reference count of a managed object.

    The m17n_object_unref () function decrements the reference count
    of the managed object pointed to by $OBJECT.  When the reference
    count becomes zero, the object is freed by its freer function.

    @return
    This function returns the resulting reference count if it fits in
    a 16-bit unsigned integer (i.e. less than 0x10000).  Otherwise, it
    returns -1.  Thus, the return value zero means that $OBJECT is
    freed.

    @errors
    This function never fails.  */
/***ja
    @brief ���������֥������Ȥλ��ȿ��� 1 ���餹.

    �ؿ� m17n_object_unref () �� $OBJECT �ǻؤ������������֥�������
    �λ��ȿ��� 1 ���餹�����ȿ��� 0 �ˤʤ�С����֥������Ȥϲ����ؿ���
    ��äƲ�������롣

    @return 
    ���δؿ��ϡ����餷�����ȿ��� 16 �ӥåȤ����̵��������(���ʤ�� 
    0x10000 ̤��)�ˤ����ޤ�С�������֤��������Ǥʤ���� -1 ���֤���
    �Ĥޤꡢ0 ���֤ä��褿����$OBJECT �ϲ�������Ƥ��롣

    @errors
    ���δؿ��ϼ��Ԥ��ʤ���    */
int
m17n_object_unref (void *object)
{
  M17NObject *obj = (M17NObject *) object;
  M17NObjectRecord *record;
  unsigned *count;

  if (! obj->ref_count_extended)
    {
      if (! --obj->ref_count)
	{
	  if (obj->u.freer)
	    (obj->u.freer) (object);
	  else
	    free (object);
	  return 0;
	}
      return (int) obj->ref_count;
    }

  record = obj->u.record;
  count = record->counts;
  while (! *count)
    *(count++) = 0xFFFFFFFF;
  (*count)--;
  if (! record->counts[0])
    {
      obj->ref_count_extended = 0;
      obj->ref_count--;
      obj->u.freer = record->freer;
      MLIST_FREE1 (record, counts);
      free (record);
    }
  return -1;
}

/*=*/

/*** @} */

/***en
    @addtogroup m17nError Error handling
    @brief Error handling of the m17n library.

    There are two types of errors that may happen in a function of
    the m17n library.

    The first type is argument errors.  When a library function is
    called with invalid arguments, it returns a value that indicates
    error and at the same time sets the external variable #merror_code
    to a non-zero integer.

    The second type is memory allocation errors.  When the required
    amount of memory is not available on the system, m17n library
    functions call a function pointed to by the external variable @c
    m17n_memory_full_handler.  The default value of the variable is a
    pointer to the default_error_handle () function, which just calls
    <tt> exit ()</tt>.  */

/***ja
    @addtogroup m17nError ���顼����
    @brief m17n �饤�֥��Υ��顼����.

    m17n �饤�֥��δؿ��Ǥϡ����Ĥμ���Υ��顼�����������롣

    ��Ĥϰ����Υ��顼�Ǥ��롣�饤�֥��δؿ��������Ǥʤ������ȤȤ��
    �ƤФ줿��硢���δؿ��ϥ��顼���̣�����ͤ��֤���Ʊ���˳����ѿ� 
    #merror_code �˥���Ǥʤ������򥻥åȤ��롣

    �⤦��Ĥμ���ϥ�������ƥ��顼�Ǥ��롣�����ƥबɬ�פ��̤Υ��
    �������Ƥ뤳�Ȥ��Ǥ��ʤ���硢�饤�֥��ؿ��ϳ����ѿ� @c
    m17n_memory_full_handler ���ؤ��ؿ���Ƥ֡��ǥե���ȤǤϡ��ؿ� 
    default_error_handle () ��ؤ��Ƥ��ꡢ���δؿ���ñ�� <tt>exit
    ()</tt> ��Ƥ֡�
*/

/*** @{ */

/*=*/

/***en 
    @brief External variable to hold error code of the m17n library.

    The external variable #merror_code holds an error code of the
    m17n library.  When a library function is called with an invalid
    argument, it sets this variable to one of @c enum #MErrorCode.

    This variable initially has the value 0.  */

/***ja 
    @brief m17n �饤�֥��Υ��顼�����ɤ��ݻ����볰���ѿ�.

    �����ѿ� #merror_code �ϡ�m17n �饤�֥��Υ��顼�����ɤ��ݻ����롣
    �饤�֥��ؿ��������Ǥʤ������ȤȤ�˸ƤФ줿�ݤˤϡ������ѿ��� 
    @c enum #MErrorCode �ΰ�Ĥ˥��åȤ��롣

    �����ѿ��ν���ͤ� 0 �Ǥ��롣  */

enum MErrorCode merror_code;

/*=*/

/***en 
    @brief Memory allocation error handler.

    The external variable #m17n_memory_full_handler holds a pointer
    to the function to call when a library function failed to allocate
    memory.  $ERR is one of @c enum #MErrorCode indicating in which
    function the error occurred.

    This variable initially points a function that simply calls the
    <tt>exit </tt>() function with $ERR as an argument.

    An application program that needs a different error handling can
    change this variable to point a proper function.  */

/***ja 
    @brief ��������ƥ��顼�ϥ�ɥ�.

    �ѿ� #m17n_memory_full_handler �ϡ��饤�֥��ؿ������������
    �˼��Ԥ����ݤ˸Ƥ֤٤��ؿ��ؤΥݥ��󥿤Ǥ��롣$ERR �� @c enum
    #MErrorCode �Τ����줫�Ǥ��ꡢ�ɤΥ饤�֥��ؿ��ǥ��顼�����ä�
    ���򼨤���

    @anchor test

    �������Ǥϡ������ѿ���ñ�� <tt>exit ()</tt> �� $ERR ������Ȥ���
    �Ƥִؿ���ؤ��Ƥ��롣

    ����Ȥϰۤʤ륨�顼������ɬ�פȤ��륢�ץꥱ�������ϡ������ѿ���
    Ŭ���ʴؿ������ꤹ�뤳�Ȥǡ���Ū��ã���Ǥ��롣  */

void (*m17n_memory_full_handler) (enum MErrorCode err);

/*** @} */

/*=*/

/***en
    @addtogroup m17nDebug
    @brief Support for m17n library users to debug their programs.

    The m17n library provides the following facilities to support the
    library users to debug their programs.

    <ul>

    <li> Environment variables to control printing of various
    information.

    <ul>

    <li> MDEBUG_INIT -- If set to 1, print information about the
    library initialization on the call of M17N_INIT ().

    <li> MDEBUG_FINI -- If set to 1, print counts of objects that are
    not yet freed on the call of M17N_FINI ().

    <li> MDEBUG_CHARSET -- If set to 1, print information about
    charsets being loaded from the m17n database.

    <li> MDEBUG_CODING -- If set to 1, print information about coding
    systems being loaded from the m17n database.

    <li> MDEBUG_DATABASE -- If set to 1, print information about
    data being loaded from the m17n database.

    <li> MDEBUG_FONT -- If set to 1, print information about fonts
    being selected and opened.

    <li> MDEBUG_FONT_FLT -- If set to 1, print information about which
    command of Font Layout Table are being executed.

    <li> MDEBUG_FONT_OTF -- If set to 1, print information about which
    feature of OpenType Layout Table are being executed.

    <li> MDEBUG_INPUT -- If set to 1, print information about how an
    input method is running.

    <li> MDEBUG_ALL -- Setting this variable to 1 is equivalent to
    setting all the above variables to 1.

    </ul>

    <li> Functions to print various objects in a human readable way.
    See the documentation of mdebug_dump_XXXX () functions.

    <li> The hook function called on an error.  See the documentation
    of mdebug_hook ().

    </ul>
*/
/***ja
    @addtogroup m17nDebug
    @brief m17n �饤�֥��桼���Τ���Υץ����ǥХå����ݡ���.

    m17n �饤�֥��ϡ����Υ桼������ʬ�Υץ�����ǥХå����뤿��
    �ˡ��ʲ��ε�ǽ�򥵥ݡ��Ȥ��Ƥ��롣

    <ul>

    <li> ���ޤ��ޤʾ���Υץ��Ȥ����椹��Ķ��ѿ���

    <ul>

    <li> MDEBUG_INIT -- 1 �ʤ�С�M17N_INIT () ���ƤФ줿�����ǡ��饤
    �֥��ν�����˴ؤ�������ץ��Ȥ��롣

    <li> MDEBUG_FINI -- 1 �ʤ�С�M17N_FINI () ���ƤФ줿�����ǡ��ޤ�
    ��������Ƥ��ʤ����֥������Ȥλ��ȿ���ץ��Ȥ��롣

    <li> MDEBUG_CHARSET -- 1 �ʤ�С�m17n �ǡ����١���������ɤ��줿
    ʸ�����åȤˤĤ��Ƥξ����ץ��Ȥ��롣

    <li> MDEBUG_CODING --  1 �ʤ�С�m17n �ǡ����١���������ɤ��줿
    �����ɷϤˤĤ��Ƥξ����ץ��Ȥ��롣

    <li> MDEBUG_DATABASE -- 1 �ʤ�С�m17n �ǡ����١���������ɤ���
    ���ǡ����ˤĤ��Ƥξ����ץ��Ȥ��롣

    <li> MDEBUG_FONT -- 1 �ʤ�С����򤵤�ƥ����ץ󤵤줿�ե���Ȥˤ�
    ���Ƥξ����ץ��Ȥ��롣

    <li> MDEBUG_FONT_FLT -- 1 �ʤ�С�Font Layout Table �ΤɤΥ��ޥ��
    ���¹��椫�ˤĤ��ƤΤ�ץ��Ȥ��롣

    <li> MDEBUG_FONT_OTF -- 1 �ʤ�С�OpenType Layout Table �Τɤ�°��
    ���¹��椫�ˤĤ��ƤΤ�ץ��Ȥ��롣

    <li> MDEBUG_INPUT -- 1 �ʤ�С��¹�������ϥ᥽�åɤξ��֤��դ���
    �ξ����ץ��Ȥ��롣

    <li> MDEBUG_ALL -- 1 �ʤ�С��嵭���٤Ƥ��ѿ��� 1 �ˤ����Τ�Ʊ����
    �̤���ġ�

    </ul>

    <li> ��Υ��֥������Ȥ�ʹ֤˲��ɤʷ��ǥץ��Ȥ���ؿ����ܺ٤�
    �ؿ� mdebug_dump_XXXX () ���������ȡ�

    <li> ���顼ȯ�����˸ƤФ��եå��ؿ���mdebug_hook () ���������ȡ�

    </ul>
*/

/*=*/
/*** @{ */
/*=*/

/***en
    @brief Hook function called on an error.

    The mdebug_hook () function is called when an error happens.  It
    returns -1 without doing anything.  It is useful to set a break
    point on this function in a debugger.  */ 
/***ja
    @brief ���顼�κݤ˸ƤФ��եå��ؿ�.

    �ؿ� mdebug_hook () �ϥ��顼�������ä��ݤ˸ƤФ졢���⤻����-1 ��
    �֤����ǥХå���ǥ֥졼���ݥ���Ȥ����ꤹ�뤿����Ѥ��뤳�Ȥ��Ǥ�
    �롣
    */ 

int
mdebug_hook ()
{
  return -1;
}

/*=*/

/*** @} */ 

/*
  Local Variables:
  coding: euc-japan
  End:
*/
