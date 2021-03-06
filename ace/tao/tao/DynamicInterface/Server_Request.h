// This may look like C, but it's really -*- C++ -*-
// Server_Request.h,v 1.8 2001/07/09 18:08:01 othman Exp

// ============================================================================
//
// = LIBRARY
//    TAO
//
// = FILENAME
//    Server_Request.h
//
// = DESCRIPTION
//    Header file for CORBA's Dynamic Server Skeleton Interface's
//    "Server Request" type.
//
// = AUTHOR
//     Copyright 1994-1995 by Sun Microsystems, Inc. and Chris Cleeland.
//     Modifications by Aniruddha Gokhale based on CORBAv2.2 Feb 98
// ============================================================================

#ifndef TAO_CORBA_SERVER_REQUEST_H
#define TAO_CORBA_SERVER_REQUEST_H
#include "ace/pre.h"

#include "tao/corbafwd.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// To force execution of the static constructor
// that registers the dynamic service object.
#include "Dynamic_Adapter_Impl.h"

#include "Context.h"
#include "tao/TAO_Server_Request.h"

class TAO_DynamicInterface_Export CORBA_ServerRequest
{
  // = TITLE
  //    Class representing the CORBA ServerRequest pseudo-object.
  //
  // = DESCRIPTION
  //    Instantiated by the POA for DSI requests and passed up
  //    to the application. Contains a reference to the instance
  //    of TAO_ServerRequest that is passed up to the POA from
  //    the ORB.
public:
  CORBA_ServerRequest (TAO_ServerRequest &orb_server_request);
  // Constructor.

  ~CORBA_ServerRequest (void);
  // Destructor.

  void arguments (CORBA::NVList_ptr &list,
                  CORBA_Environment &ACE_TRY_ENV =
                    TAO_default_environment ());
  // Implementation uses this to provide the ORB with the operation's
  // parameter list ... on return, their values are available; the
  // list fed in has typecodes and (perhap) memory assigned.

  void set_result (const CORBA::Any &value,
                   CORBA_Environment &ACE_TRY_ENV =
                     TAO_default_environment ());
  // Implementation uses this to provide the operation result
  // ... illegal if exception() was called or params() was not called.
  //
  // XXX Implementation should be able to assume response has been
  // sent when this returns, and reclaim memory it allocated.

  void set_exception (const CORBA::Any &value,
                      CORBA_Environment &ACE_TRY_ENV =
                        TAO_default_environment ());
  // Implementation uses this to provide the exception value which is
  // the only result of this particular invocation.
  //
  // XXX Implementation should be able to assume response has been
  // sent when this returns, and reclaim memory it allocated.

  // = Get various universal call attributes.

  // e.g., who made the call, the target of the call, what ORB and OA
  // that target object uses.
  //
  // NOTE: none of these report exceptions; unavailability of any of
  // this stuff is a catastrophic error since this is all part of the
  // basic CORBA Object Model.

  void dsi_marshal (CORBA_Environment &ACE_TRY_ENV =
                      TAO_default_environment ());
  // Marshal outgoing parameters.

  CORBA::Context_ptr ctx (void) const;
  // Accessor for the Context member.

  void ctx (CORBA::Context_ptr);
  // Mutator for the Context member.

  const char *operation (void) const;
  // Get the operation name.

  // Pseudo object methods.
  static CORBA_ServerRequest_ptr _duplicate (CORBA_ServerRequest_ptr);
  static CORBA_ServerRequest_ptr _nil (void);

  // = Reference counting.
  CORBA::ULong _incr_refcnt (void);
  CORBA::ULong _decr_refcnt (void);

  void _tao_lazy_evaluation (int lazy_evaluation);
  // Set the lazy evaluation flag.

  int _tao_incoming_byte_order (void) const;
  // Get the byte order of the incoming CDR stream.

  void _tao_reply_byte_order (int byte_order);
  // Set the byte order of the outgoing CDR stream.

  /// Return a reference to the underlying TAO_ServerRequest object.
  TAO_ServerRequest & _tao_server_request (void);

  void gateway_exception_reply (ACE_CString &raw_exception);
  // Returns a user exception through a TAO gateway without
  // knowing its type.

#if !defined(__GNUC__) || __GNUC__ > 2 || __GNUC_MINOR__ >= 8
  typedef CORBA::ServerRequest_ptr _ptr_type;
#endif /* __GNUC__ */
  // Useful for template programming.

private:
  int lazy_evaluation_;
  // If zero then the NVList is evaluated ASAP.

  CORBA::Context_ptr ctx_;
  // Context associated with this request.

  CORBA::NVList_ptr params_;
  // Incoming parameters.

  CORBA::Any_ptr retval_;
  // Return value.

  CORBA::Any_ptr exception_;
  // Any exception which might be raised.

  CORBA::ULong refcount_;
  // Reference counting.

  TAO_SYNCH_MUTEX lock_;
  // Protect the refcount_ and response_receieved_.

  TAO_ServerRequest &orb_server_request_;
  // Request from the ORB.

  int sent_gateway_exception_;
  // Have we sent a user exception obtained from a gateway?
};

#if defined (__ACE_INLINE__)
# include "Server_Request.inl"
#endif /* __ACE_INLINE__ */

#include "ace/post.h"
#endif /* TAO_CORBA_SERVER_REQUEST_H */
