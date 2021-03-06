/* -*- C++ -*- */
// DynArray_i.h,v 1.7 2001/06/12 23:07:33 parsons Exp

// ===================================================================
//
// = LIBRARY
//    TAO_DynamicAny
//
// = FILENAME
//    DynArray_i.h
//
// = AUTHOR
//    Jeff Parsons <parsons@cs.wustl.edu>
//
// ===================================================================

#ifndef TAO_DYNARRAY_I_H
#define TAO_DYNARRAY_I_H
#include "ace/pre.h"

#include "DynamicAny.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "DynCommon.h"
#include "ace/Containers.h"

#if defined (_MSC_VER)
# if (_MSC_VER >= 1200)
#  pragma warning(push)
# endif /* _MSC_VER >= 1200 */
# pragma warning (disable:4250)
#endif /* _MSC_VER */

class TAO_DynamicAny_Export TAO_DynArray_i 
  : public virtual DynamicAny::DynArray,
    public virtual TAO_DynCommon,
    public virtual TAO_Local_RefCounted_Object
{
  // = TITLE
  //    TAO_DynArray_i
  //
  // = DESCRIPTION
  //    Implementation of Dynamic Any type for arrays
  //
public:
  TAO_DynArray_i (void);
  // Constructor.

  ~TAO_DynArray_i (void);
  // Destructor.

  void init (CORBA_TypeCode_ptr tc,
             CORBA::Environment &ACE_TRY_ENV);
  // Initialize using just a TypeCode.

  void init (const CORBA_Any& any,
             CORBA::Environment &ACE_TRY_ENV);
  // Initialize using an Any.

  // = LocalObject methods
  static TAO_DynArray_i *_narrow (
      CORBA::Object_ptr obj,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    );
  virtual void *_tao_QueryInterface (ptr_arith_t type);

  // = Functions specific to DynArray.

  virtual DynamicAny::AnySeq * get_elements (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException
      ));

  virtual void set_elements (
      const DynamicAny::AnySeq & value,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException,
        DynamicAny::DynAny::TypeMismatch,
        DynamicAny::DynAny::InvalidValue
      ));

  virtual DynamicAny::DynAnySeq * get_elements_as_dyn_any (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException
      ));

  virtual void set_elements_as_dyn_any (
      const DynamicAny::DynAnySeq & value,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException,
        DynamicAny::DynAny::TypeMismatch,
        DynamicAny::DynAny::InvalidValue
      ));

  // = DynAny common functions not implemented in class TAO_DynCommon.

  virtual void from_any (
      const CORBA::Any & value,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException,
        DynamicAny::DynAny::TypeMismatch,
        DynamicAny::DynAny::InvalidValue
    ));

  virtual CORBA::Any * to_any (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException
      ));

  virtual CORBA::Boolean equal (
      DynamicAny::DynAny_ptr dyn_any,
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException
      ));

  virtual void destroy (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException
      ));

  virtual DynamicAny::DynAny_ptr current_component (
      CORBA::Environment &ACE_TRY_ENV =
        TAO_default_environment ()
    )
    ACE_THROW_SPEC ((
        CORBA::SystemException,
        DynamicAny::DynAny::TypeMismatch
      ));

private:
  // Returns the type of elements contained in the array.
  CORBA::TypeCode_ptr get_element_type (CORBA::Environment &ACE_TRY_ENV =
                                          TAO_default_environment ());

  // Gets the length of the array from the typecode.
  CORBA::ULong get_tc_length (CORBA::TypeCode_ptr tc,
                              CORBA::Environment &ACE_TRY_ENV =
                                TAO_default_environment ());

  // Called by both versions of init().
  void init_common (void);

  // Use copy() or assign() instead of these.
  TAO_DynArray_i (const TAO_DynArray_i &src);
  TAO_DynArray_i &operator= (const TAO_DynArray_i &src);

private:
  ACE_Array_Base<DynamicAny::DynAny_var> da_members_;
  // Each component is also a DynAny.
};

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma warning(pop)
#endif /* _MSC_VER */

#include "ace/post.h"
#endif /* TAO_DYNARRAY_I_H */
