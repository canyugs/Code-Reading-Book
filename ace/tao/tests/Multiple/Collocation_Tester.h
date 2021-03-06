//Collocation_Tester.h,v 1.1 2000/11/21 19:25:00 corsaro Exp
//
// ============================================================================
//
// = LIBRARY
//    TAO/tests/Multiple
//
// = FILENAME
//    Collocation_Tester.h
//
// = DESCRIPTION
//     This file contains the class that tests the TAO's collocation
//     mechanism.
//
// = AUTHOR
//     Angelo Corsaro <corsaro@cs.wustl.edu>
//
// ============================================================================

// -- Custom App. Include --
#include "MultipleC.h"

struct Quote
{
  static const char *top;
  static const char *left;
  static const char *right;
  static const char *bottom;
};


class Collocation_Tester
{
public:

  // -- Constructor/Destructors --
  Collocation_Tester (CORBA::Object_ptr object);

  ~Collocation_Tester ();

  // -- Command --
  void run (CORBA::Environment &ACE_TRY_ENV);
  // Runs the test.


private:

  // -- Helper Methods --

  int  test_top (CORBA::Environment &ACE_TRY_ENV);
  // Tests the method accessible thru the
  // Top interface.

  int test_right (CORBA::Environment &ACE_TRY_ENV);
  // Tests the method accessible thru the
  // Right interface.

  int test_left (CORBA::Environment &ACE_TRY_ENV);
  // Tests the method accessible thru the
  // Left interface.

  int test_bottom (CORBA::Environment &ACE_TRY_ENV);
  // Tests the method accessible thru the
  // Bottom interface.

  int match_answer (const char *actual_answer,
                     const char *right_answer,
                     const char *method_name);
  // This method tests wether the answer obtained
  // is the one expected. As strcmp, it returns zero
  // if a match occurs and a non-zero value if there
  // is no match (actually 1 is returned if there is
  // no match.

  void shutdown (CORBA::Environment &ACE_TRY_ENV);
private:
  CORBA::Object_var object_;
};
