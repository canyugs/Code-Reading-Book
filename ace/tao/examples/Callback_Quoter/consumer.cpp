// consumer.cpp,v 1.2 1998/10/25 04:55:46 kirthika Exp

#include "Consumer_Handler.h"

// This function runs the Callback Quoter Consumer application.

int
main (int argc, char **argv)
{
  Consumer_Handler consumer;

  ACE_DEBUG ((LM_DEBUG,
              "\n\t***Consumer***\n\n"));

  if (consumer.init (argc, argv) == -1)
    return -1;
  else
    return consumer.run ();
}
