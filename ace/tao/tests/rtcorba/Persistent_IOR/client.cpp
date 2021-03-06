// client.cpp,v 1.4 2001/09/18 00:06:04 irfan Exp

#include "ace/Get_Opt.h"
#include "testC.h"

static int iterations = 5;
static int shutdown_server = 0;

static const char *ior = 0;

static int
parse_args (int argc, char **argv)
{
  ACE_Get_Opt get_opts (argc, argv, "k:i:x");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'k':
        ior = get_opts.optarg;
        break;

      case 'i':
        iterations = ::atoi (get_opts.optarg);
        break;

      case 'x':
        shutdown_server = 1;
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-k ior "
                           "-i iterations "
                           "-x shutdown server "
                           "\n",
                           argv [0]),
                          -1);
      }

  if (ior == 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Error: No IOR specified\n"),
                      -1);

  return 0;
}

int
main (int argc, char **argv)
{
  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc,
                         argv,
                         0,
                         ACE_TRY_ENV);
      ACE_TRY_CHECK;

      int result =
        parse_args (argc, argv);
      if (result != 0)
        return result;

      CORBA::Object_var object =
        orb->string_to_object (ior,
                               ACE_TRY_ENV);
      ACE_TRY_CHECK;

      test_var test =
        test::_narrow (object.in (),
                       ACE_TRY_ENV);
      ACE_TRY_CHECK;

      for (int i = 0; i < iterations; i++)
        {
          test->method (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }

      if (shutdown_server)
        {
          test->shutdown (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION, "Unexpected exception!");
      return -1;
    }
  ACE_ENDTRY;

  return 0;
}
