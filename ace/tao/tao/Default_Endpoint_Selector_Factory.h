// This may look like C, but it's really -*- C++ -*-

//=============================================================================
/**
 *  @file   Default_Endpoint_Selector_Factory.h
 *
 *  Default_Endpoint_Selector_Factory.h,v 1.1 2001/06/12 18:57:58 fhunleth Exp
 *
 * Strategies for selecting profile/endpoint from an IOR for making an
 * invocation.
 *
 *  @author Marina Spivak <marina@cs.wustl.edu>
 *  @author Frank Hunleth <fhunleth@cs.wustl.edu>
 */
//=============================================================================


#ifndef TAO_DEFAULT_ENDPOINT_SELECTOR_FACTORY_H
#define TAO_DEFAULT_ENDPOINT_SELECTOR_FACTORY_H
#include "ace/pre.h"

#include "tao/Endpoint_Selector_Factory.h"
#include "ace/Service_Config.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// Forward references
class TAO_Default_Endpoint_Selector;

// ****************************************************************

/**
 * @class TAO_Default_Endpoint_Selector_Factory
 *
 * @brief Factory for initializing <Endpoint_Selection_State> and
 * obtaining appropriate <Invocation_Endpoint_Selector>.
 *
 * Used by Invocation classes to intialize its endpoint selection
 * strategy and state based on the effective policies.
 * Endpoint selection strategies are stateless objects - all the
 * state they need is contained by Invocation in
 * <Endpoint_Selection_State>.  Thus, rather than allocating an
 * endpoint selection strategy object for each Invocation, the
 * factory simply returns the appropriate one from the
 * set preallocated in the ORB_Core.  One endpoint selection
 * strategy object can be used by many invocations concurrently.
 */
class TAO_Export TAO_Default_Endpoint_Selector_Factory
  : public TAO_Endpoint_Selector_Factory
{
public:
  /// Constructor.
  TAO_Default_Endpoint_Selector_Factory (void);

  /// Destructor.
  virtual ~TAO_Default_Endpoint_Selector_Factory (void);

  /// Get an Invocation's endpoint selection strategy and
  /// initialize the endpoint selection state instance.
  virtual TAO_Invocation_Endpoint_Selector *get_selector (
                             TAO_GIOP_Invocation *invocation,
                             CORBA::Environment &ACE_TRY_ENV);

protected:
  /// The possible endpoint selector strategies that can be
  /// returned by this factory

  TAO_Default_Endpoint_Selector *default_endpoint_selector_;
};

ACE_STATIC_SVC_DECLARE_EXPORT (TAO, TAO_Default_Endpoint_Selector_Factory)
ACE_FACTORY_DECLARE (TAO, TAO_Default_Endpoint_Selector_Factory)

#include "ace/post.h"
#endif  /* TAO_DEFAULT_ENDPOINT_SELECTOR_FACTORY_H */
