
//=============================================================================
/**
 *  @file   Init_ACE.h
 *
 *  Init_ACE.h,v 1.2 2000/11/01 22:16:39 coryan Exp
 *
 * This class consolidates the operations on the Handles.
 *
 *
 *  @author Priyanka Gontla <pgontla@ece.uci.edu>
 */
//=============================================================================


#ifndef ACE_INIT_ACE_H
#define ACE_INIT_ACE_H
#include "ace/pre.h"

#include "ace/OS.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/**
 * @class ACE_Init_ACE
 *
 * @brief Initialize ACE library services.  Can be called only once
 *   per program invocation.
 *
 */
class ACE_Export ACE_Init_ACE
{
public:
  /// Returns 0 on success, -1 on failure, and 1 if it had already been called.
  /**
   * This class implements the fucntions for the initialization and
   * shutting down ACE.  These functions are called only once per ACE
   * invokation.
   */
  static int init (void);

  /// Returns 0 on success, -1 on failure, and 1 if it had already been called.
  /**
   * Shut down ACE library services.  Can be called only once per
   * program invocation.
   */
  static int fini (void);

private:
  /**
   * Counter to match <init>/<fini> calls.  <init> must increment it;
   * <fini> must decrement it.  <fini> then does nothing until it
   * reaches 0.
   */
  static u_int init_fini_count_;
};

#if !defined (ACE_LACKS_INLINE_FUNCTIONS)
#include "ace/Init_ACE.i"
#endif /* ACE_LACKS_INLINE_FUNCTIONS */

#include "ace/post.h"
#endif  /* ACE_INIT_ACE_H */
