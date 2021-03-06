/* locale.inl,v 1.5 2000/07/14 21:09:12 joeh Exp -*- C -*-

 * ============================================================================
 *
 * = LIBRARY
 *    pace
 *
 * = FILENAME
 *    pace/win32/locale.inl
 *
 * = AUTHOR
 *    Joe Hoffert
 *
 * =========================================================================== */

#include <locale.h>

#if (PACE_HAS_POSIX_NONUOF_FUNCS)
PACE_INLINE
pace_lconv *
pace_localeconv (void)
{
  return localeconv ();
}
#endif /* PACE_HAS_POSIX_NONUOF_FUNCS */

#if (PACE_HAS_POSIX_MP_UOF)
PACE_INLINE
char *
pace_setlocale (int category, const char * locale)
{
  return setlocale (category, locale);
}
#endif /* PACE_HAS_POSIX_MP_UOF */
