/* -*- C++ -*- */
// DynArray_i.cpp,v 1.14 2001/06/15 16:21:43 parsons Exp

#include "DynArray_i.h"
#include "DynAnyFactory.h"
#include "tao/Marshal.h"

ACE_RCSID(DynamicAny, DynArray_i, "DynArray_i.cpp,v 1.14 2001/06/15 16:21:43 parsons Exp")

// Constructor and destructor,

TAO_DynArray_i::TAO_DynArray_i (void)
{
}

TAO_DynArray_i::~TAO_DynArray_i (void)
{
}

void
TAO_DynArray_i::init_common (void)
{
  this->ref_to_component_ = 0;
  this->container_is_destroying_ = 0;
  this->has_components_ = 1;
  this->destroyed_ = 0;
  this->current_position_ = 0;
  this->component_count_ = this->da_members_.size ();
}

void
TAO_DynArray_i::init (const CORBA_Any & any,
                      CORBA::Environment &ACE_TRY_ENV)
{
  CORBA::TypeCode_var tc = any.type ();

  CORBA::TCKind kind = TAO_DynAnyFactory::unalias (tc.in (),
                                                   ACE_TRY_ENV);
  ACE_CHECK;

  if (kind != CORBA::tk_array)
    {
      ACE_THROW (DynamicAny::DynAnyFactory::InconsistentTypeCode ());
    }

  this->type_ = tc;

  CORBA::ULong numfields = this->get_tc_length (tc.in (),
                                                ACE_TRY_ENV);
  ACE_CHECK;
  // Resize the array.
  this->da_members_.size (numfields);

  this->init_common ();

  // Get the CDR stream of the argument.
  ACE_Message_Block* mb = any._tao_get_cdr ();

  TAO_InputCDR cdr (mb,
                    any._tao_byte_order ());

  CORBA::TypeCode_var field_tc =
    this->get_element_type (ACE_TRY_ENV);
  ACE_CHECK;

  for (CORBA::ULong i = 0; i < numfields; i++)
    {
      // This Any constructor is a TAO extension.
      CORBA_Any field_any (field_tc.in (),
                           0,
                           cdr.byte_order (),
                           cdr.start ());

      // This recursive step will call the correct constructor
      // based on the type of field_any.
      this->da_members_[i] =
        TAO_DynAnyFactory::make_dyn_any (field_any,
                                         ACE_TRY_ENV);
      ACE_CHECK;

      // Move to the next field in the CDR stream.
      (void) TAO_Marshal_Object::perform_skip (field_tc.in (),
                                               &cdr,
                                               ACE_TRY_ENV);
      ACE_CHECK;
    }
}

void
TAO_DynArray_i::init (CORBA_TypeCode_ptr tc,
                      CORBA::Environment &ACE_TRY_ENV)
{
  CORBA::TCKind kind = TAO_DynAnyFactory::unalias (tc,
                                                   ACE_TRY_ENV);
  ACE_CHECK;

  if (kind != CORBA::tk_array)
    {
      ACE_THROW (DynamicAny::DynAnyFactory::InconsistentTypeCode ());
    }

  this->type_ = CORBA::TypeCode::_duplicate (tc);

  CORBA::ULong numfields = this->get_tc_length (tc,
                                                ACE_TRY_ENV);
  ACE_CHECK;

  // Resize the array.
  this->da_members_.size (numfields);

  this->init_common ();

  CORBA::TypeCode_var elemtype = this->get_element_type (ACE_TRY_ENV);
  ACE_CHECK;

  for (CORBA::ULong i = 0; i < numfields; ++i)
    {
      // Recursively initialize each element.
      this->da_members_[i] = TAO_DynAnyFactory::make_dyn_any (elemtype.in (),
                                                              ACE_TRY_ENV);
      ACE_CHECK;
    }
}

CORBA::TypeCode_ptr
TAO_DynArray_i::get_element_type (CORBA::Environment& ACE_TRY_ENV)
{
  CORBA::TypeCode_var element_type =
    CORBA_TypeCode::_duplicate (this->type_.in ());

  // Strip away aliases (if any) on top of the outer type
  CORBA::TCKind kind = element_type->kind (ACE_TRY_ENV);
  ACE_CHECK_RETURN (CORBA_TypeCode::_nil ());

  while (kind != CORBA::tk_array)
    {
      element_type = element_type->content_type (ACE_TRY_ENV);
      ACE_CHECK_RETURN (CORBA_TypeCode::_nil ());

      kind = element_type->kind (ACE_TRY_ENV);
      ACE_CHECK_RETURN (CORBA_TypeCode::_nil ());
    }

  // Return the content type.
  CORBA_TypeCode_ptr retval = element_type->content_type (ACE_TRY_ENV);
  ACE_CHECK_RETURN (CORBA_TypeCode::_nil ());

  return retval;
}

// Get the length from the (possibly aliased) typecode.
CORBA::ULong
TAO_DynArray_i::get_tc_length (CORBA::TypeCode_ptr tc,
                               CORBA::Environment &ACE_TRY_ENV)
{
  CORBA::TypeCode_var tctmp = CORBA::TypeCode::_duplicate (tc);
  CORBA::TCKind kind = tctmp->kind (ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  while (kind == CORBA::tk_alias)
    {
      tctmp = tctmp->content_type (ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);
      kind = tctmp->kind (ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);
    }

  CORBA::ULong retval = tctmp->length (ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  return retval;
}

// ****************************************************************

TAO_DynArray_i *
TAO_DynArray_i::_narrow (CORBA::Object_ptr obj,
                         CORBA::Environment &)
{
  if (CORBA::is_nil (obj))
    {
      return 0;
    }

  return ACE_reinterpret_cast (
             TAO_DynArray_i*,
             obj->_tao_QueryInterface (
                      ACE_reinterpret_cast (
                          ptr_arith_t,
                          &TAO_DynArray_i::_narrow
                        )
                    )
           );
}

void*
TAO_DynArray_i::_tao_QueryInterface (ptr_arith_t type)
{
  ptr_arith_t mytype =
    ACE_reinterpret_cast (ptr_arith_t,
                          &TAO_DynArray_i::_narrow);
  if (type == mytype)
    {
      this->_add_ref ();
      return this;
    }

  return 
    this->ACE_NESTED_CLASS (DynamicAny, DynArray::_tao_QueryInterface) (type);
}

// ****************************************************************

DynamicAny::AnySeq *
TAO_DynArray_i::get_elements (CORBA::Environment& ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW_RETURN (CORBA::OBJECT_NOT_EXIST (),
                        0);
    }

  CORBA::ULong length = this->da_members_.size ();

  DynamicAny::AnySeq *elements = 0;
  ACE_NEW_THROW_EX (elements,
                    DynamicAny::AnySeq (length),
                    CORBA::NO_MEMORY ());
  ACE_CHECK_RETURN (0);

  elements->length (length);
  DynamicAny::AnySeq_var safe_retval (elements);

  CORBA::Any_var tmp;

  // Initialize each Any.
  for (CORBA::ULong i = 0; i < length; i++)
    {
      tmp = this->da_members_[i]->to_any (ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      safe_retval[i] = tmp.in ();
    }

  return safe_retval._retn ();
}

void
TAO_DynArray_i::set_elements (const DynamicAny::AnySeq & value,
                              CORBA::Environment& ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException,
      DynamicAny::DynAny::TypeMismatch,
      DynamicAny::DynAny::InvalidValue
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW (CORBA::OBJECT_NOT_EXIST ());
    }

  CORBA::ULong length = value.length ();

  if (length != this->da_members_.size ())
    {
      ACE_THROW (DynamicAny::DynAny::InvalidValue ());
    }

  CORBA::TypeCode_var value_tc;
  CORBA::TypeCode_var element_type = this->get_element_type (ACE_TRY_ENV);
  ACE_CHECK;

  for (CORBA::ULong i = 0; i < length; i++)
    {
      // Check each arg element for type match.
      value_tc = value[i].type ();
      CORBA::Boolean equivalent = value_tc->equivalent (element_type.in (),
                                                        ACE_TRY_ENV);
      ACE_CHECK;

      if (equivalent)
        {
          this->da_members_[i]->destroy (ACE_TRY_ENV);
          ACE_CHECK;

          this->da_members_[i] =
            TAO_DynAnyFactory::make_dyn_any (value[i],
                                             ACE_TRY_ENV);
          ACE_CHECK;
        }
      else
        {
          ACE_THROW (DynamicAny::DynAny::TypeMismatch ());
        }
    }
}

DynamicAny::DynAnySeq *
TAO_DynArray_i::get_elements_as_dyn_any (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW_RETURN (CORBA::OBJECT_NOT_EXIST (),
                        0);
    }

  DynamicAny::DynAnySeq *retval = 0;
  ACE_NEW_THROW_EX (retval,
                    DynamicAny::DynAnySeq (this->component_count_),
                    CORBA::NO_MEMORY ());
  ACE_CHECK_RETURN (0);

  retval->length (this->component_count_);
  DynamicAny::DynAnySeq_var safe_retval (retval);

  for (CORBA::ULong i = 0; i < this->component_count_; ++i)
    {
      // A deep copy is made only by copy() (CORBA 2.4.2 section 9.2.3.6).
      // Set the flag so the caller can't destroy.
      this->set_flag (this->da_members_[i].in (),
                      0,
                      ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      safe_retval[i] = 
        DynamicAny::DynAny::_duplicate (this->da_members_[i].in ());
      ACE_CHECK_RETURN (0);
    }

  return safe_retval._retn ();
}

void
TAO_DynArray_i::set_elements_as_dyn_any (
    const DynamicAny::DynAnySeq & values,
    CORBA::Environment &ACE_TRY_ENV
  )
  ACE_THROW_SPEC ((
      CORBA::SystemException,
      DynamicAny::DynAny::TypeMismatch,
      DynamicAny::DynAny::InvalidValue
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW (CORBA::OBJECT_NOT_EXIST ());
    }

  CORBA::ULong length = this->da_members_.size ();

  if (values.length () != length)
    {
      ACE_THROW (DynamicAny::DynAny::InvalidValue ());
    }

  CORBA::TypeCode_var element_type = this->get_element_type (ACE_TRY_ENV);
  ACE_CHECK;

  CORBA::TypeCode_var val_type;
  CORBA::Boolean equivalent;

  for (CORBA::ULong i = 0; i < length; ++i)
    {
      val_type = values[i]->type (ACE_TRY_ENV);
      ACE_CHECK;

      equivalent = val_type->equivalent (element_type.in (),
                                         ACE_TRY_ENV);
      
      ACE_CHECK;

      if (equivalent)
        {
          this->da_members_[i] = values[i]->copy (ACE_TRY_ENV);
          ACE_CHECK;
        }
      else
        {
          ACE_THROW (DynamicAny::DynAny::TypeMismatch ());
        }
    }
}

// ****************************************************************

void
TAO_DynArray_i::from_any (const CORBA_Any& any,
                          CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException,
      DynamicAny::DynAny::TypeMismatch,
      DynamicAny::DynAny::InvalidValue
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW (CORBA::OBJECT_NOT_EXIST ());
    }

  CORBA::TypeCode_var tc = any.type ();
  CORBA::Boolean equivalent = this->type_.in ()->equivalent (tc.in (),
                                                             ACE_TRY_ENV);
  ACE_CHECK;

  if (equivalent)
    {
      // Get the CDR stream of the argument.
      ACE_Message_Block* mb = any._tao_get_cdr ();
      TAO_InputCDR cdr (mb,
                        any._tao_byte_order ());

      CORBA::ULong length = this->da_members_.size ();
      CORBA::ULong arg_length = this->get_tc_length (tc.in (),
                                                     ACE_TRY_ENV);
      ACE_CHECK;

      if (length != arg_length)
        {
          ACE_THROW (DynamicAny::DynAny::TypeMismatch ());
        }

      CORBA::TypeCode_var field_tc = this->get_element_type (ACE_TRY_ENV);
      ACE_CHECK;

      for (CORBA::ULong i = 0; i < arg_length; ++i)
        {
          // This Any constructor is a TAO extension.
          CORBA_Any field_any (field_tc.in (),
                               0,
                               cdr.byte_order (),
                               cdr.start ());

          this->da_members_[i]->destroy (ACE_TRY_ENV);
          ACE_CHECK;

          this->da_members_[i] =
            TAO_DynAnyFactory::make_dyn_any (field_any,
                                          ACE_TRY_ENV);
          ACE_CHECK;

          // Move to the next field in the CDR stream.
          (void) TAO_Marshal_Object::perform_skip (field_tc.in (),
                                                   &cdr,
                                                   ACE_TRY_ENV);
          ACE_CHECK;
        }

      this->current_position_ = arg_length ? 0 : -1;
    }
  else
    {
      ACE_THROW (DynamicAny::DynAny::TypeMismatch ());
    }
}

CORBA::Any_ptr
TAO_DynArray_i::to_any (CORBA::Environment& ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW_RETURN (CORBA::OBJECT_NOT_EXIST (),
                        0);
    }

  CORBA_TypeCode_var field_tc = this->get_element_type (ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  TAO_OutputCDR out_cdr;
  CORBA::Any_var field_any;
  CORBA::ULong length = this->da_members_.size ();

  for (CORBA::ULong i = 0; i < length; ++i)
    {
      // Recursive step.
      field_any = this->da_members_[i]->to_any (ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      ACE_Message_Block* field_mb = field_any->_tao_get_cdr ();

      TAO_InputCDR field_cdr (field_mb,
                              field_any->_tao_byte_order ());

      (void) TAO_Marshal_Object::perform_append (field_tc.in (),
                                                 &field_cdr,
                                                 &out_cdr,
                                                 ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);
    }

  TAO_InputCDR in_cdr (out_cdr);

  CORBA_Any_ptr retval = 0;
  ACE_NEW_THROW_EX (retval,
                    CORBA_Any (this->type_.in (),
                               0,
                               in_cdr.byte_order (),
                               in_cdr.start ()),
                    CORBA::NO_MEMORY ());
  ACE_CHECK_RETURN (0);

  return retval;
}

CORBA::Boolean
TAO_DynArray_i::equal (DynamicAny::DynAny_ptr rhs,
                       CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW_RETURN (CORBA::OBJECT_NOT_EXIST (),
                        0);
    }

  CORBA::TypeCode_var tc = rhs->type (ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  CORBA::Boolean equivalent = tc->equivalent (this->type_.in (),
                                              ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  if (!equivalent)
    {
      return 0;
    }

  DynamicAny::DynAny_var tmp;
  CORBA::Boolean member_equal;

  for (CORBA::ULong i = 0; i < this->component_count_; ++i)
    {
      rhs->seek (ACE_static_cast (CORBA::Long, 
                                  i),
                 ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      tmp = rhs->current_component (ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      // Recursive step.
      member_equal = tmp->equal (this->da_members_[i].in (),
                                 ACE_TRY_ENV);
      ACE_CHECK_RETURN (0);

      if (!member_equal)
        {
          return 0;
        }
    }

  return 1;
}

void
TAO_DynArray_i::destroy (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW (CORBA::OBJECT_NOT_EXIST ());
    }

  if (!this->ref_to_component_ || this->container_is_destroying_)
    {
      // Do a deep destroy.
      for (CORBA::ULong i = 0; i < this->component_count_; ++i)
        {
          this->set_flag (da_members_[i].in (),
                          1,
                          ACE_TRY_ENV);
          ACE_CHECK;

          this->da_members_[i]->destroy (ACE_TRY_ENV);
          ACE_CHECK;
        }

      this->destroyed_ = 1;
    }
}

DynamicAny::DynAny_ptr
TAO_DynArray_i::current_component (CORBA::Environment &ACE_TRY_ENV)
  ACE_THROW_SPEC ((
      CORBA::SystemException,
      DynamicAny::DynAny::TypeMismatch
    ))
{
  if (this->destroyed_)
    {
      ACE_THROW_RETURN (CORBA::OBJECT_NOT_EXIST (),
                        DynamicAny::DynAny::_nil ());
    }

  if (this->current_position_ == -1)
    {
      return DynamicAny::DynAny::_nil ();
    }

  CORBA::ULong index = ACE_static_cast (CORBA::ULong, 
                                        this->current_position_);

  this->set_flag (this->da_members_[index].in (),
                  0,
                  ACE_TRY_ENV);
  ACE_CHECK_RETURN (DynamicAny::DynAny::_nil ());

  return DynamicAny::DynAny::_duplicate (
            this->da_members_[index].in ()
          );
}

