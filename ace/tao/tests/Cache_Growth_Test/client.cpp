// client.cpp,v 1.1 2001/09/13 18:24:53 bala Exp

#include "TestC.h"
#include "ace/Get_Opt.h"

ACE_RCSID(Hello, client, "client.cpp,v 1.1 2001/09/13 18:24:53 bala Exp")

const char *ior = "file://test.ior";
int do_shutdown = 0;

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "k:x");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'k':
        ior = get_opts.optarg;
        break;

      case 'x':
        do_shutdown = 1;
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-k <ior> "
                           "-x <do_shutdown>"
                           "\n",
                           argv [0]),
                          -1);
      }
  // Indicates sucessful parsing of the command line
  return 0;
}

int
main (int argc, char *argv[])
{
  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc, argv, "", ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (parse_args (argc, argv) != 0)
        return 1;

      CORBA::Object_var tmp =
        orb->string_to_object(ior, ACE_TRY_ENV);
      ACE_TRY_CHECK;

      Test::Hello_var hello =
        Test::Hello::_narrow(tmp.in (), ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (CORBA::is_nil (hello.in ()))
        {
          ACE_ERROR_RETURN ((LM_DEBUG,
                             "Nil Test::Hello reference <%s>\n",
                             ior),
                            1);
        }

      CORBA::String_var the_string =
        hello->get_string (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      ACE_DEBUG ((LM_DEBUG, "(%P|%t) - string returned <%s>\n",
                  the_string.in ()));

      if (do_shutdown)
        {
          hello->shutdown (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }

      orb->destroy (ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
