// -*- C++ -*-
// test_i.h,v 1.2 2001/03/30 16:35:09 parsons Exp

#include "testS.h"

// Must include this header file and link to TAO_IFR_Client.lib
// to dynamically load this necessary library.
#include "tao/IFR_Client/IFR_Client_Adapter_Impl.h"

class inventory_i : public POA_warehouse::inventory
{
public:
  CORBA::Boolean getCDinfo (const char * artist,
                            char *& title,
                            CORBA::Float_out price,
                            CORBA::Environment &ACE_TRY_ENV = 
                              TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException));
};

