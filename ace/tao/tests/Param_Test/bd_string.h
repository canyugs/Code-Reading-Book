// bd_string.h,v 1.9 2000/03/21 20:04:47 coryan Exp

// ============================================================================
//
// = LIBRARY
//    TAO/tests/Param_Test
//
// = FILENAME
//    bd_string.h
//
// = DESCRIPTION
//     Tests bounded string
//
// = AUTHORS
//      Carlos O'Ryan
//
// ============================================================================

#ifndef PARAM_TEST_BOUNDED_STRING_H
#define PARAM_TEST_BOUNDED_STRING_H

#include "param_testCli.h"

// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
//                        tests bounded strings
// =*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*
class Test_Bounded_String
{
public:
  Test_Bounded_String (void);
  // ctor

  ~Test_Bounded_String (void);
  // dtor

  int run_sii_test (Param_Test_ptr objref,
                    CORBA::Environment &env);
  // run the SII test

  const char *opname (void) const;
  // return operation name

  int init_parameters (Param_Test_ptr objref,
                        CORBA::Environment &env);
  // set values for parameters

  int reset_parameters (void);
  // reset values for CORBA

  CORBA::Boolean check_validity (void);
  // check if results are valid

  CORBA::Boolean check_validity (CORBA::Request_ptr req);
  // check if results are valid. This is used for DII results

  void print_values (void);
  // print all the values

  void dii_req_invoke (CORBA::Request *, CORBA::Environment &);
  // invoke DII request with appropriate exception handling.

private:
  char *opname_;
  // operation name

  char *in_;
  // in parameter

  char *inout_;
  // inout parameter

  char *out_;
  // out parameter

  char *ret_;
  // return value
};

#endif /* PARAM_TEST_BOUNDED_STRING_H */
