// ping.cpp,v 1.14 2001/07/11 21:43:49 oci Exp

#include "ping.h"
#include "orbsvcs/AV/Protocol_Factory.h"
#include "tao/corba.h"
#include "tao/Strategies/advanced_resource.h"
#include "ace/Get_Opt.h"
#include "ace/High_Res_Timer.h"
#include "ace/Stats.h"

ACE_RCSID(Latency, ping, "ping.cpp,v 1.14 2001/07/11 21:43:49 oci Exp")

const char *ior_output_file = "ping.ior";
const char *protocol = "RTP/UDP";
int milliseconds = 100;
int respond = 1;
AVStreams::protocolSpec ping_protocols;
AVStreams::protocolSpec pong_protocols;

Pong_Send_Callback pong_callback;

ACE_hrtime_t recv_base = 0;
ACE_Throughput_Stats recv_latency;


int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "xo:s:r:t:");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'o':
        ior_output_file = get_opts.optarg;
        break;

      case 'r':
        {
          CORBA::ULong l = ping_protocols.length ();
          ping_protocols.length (l + 1);
          ping_protocols[l] = CORBA::string_dup (get_opts.optarg);
        }
        break;

      case 's':
        {
          CORBA::ULong l = pong_protocols.length ();
          pong_protocols.length (l + 1);
          pong_protocols[l] = CORBA::string_dup (get_opts.optarg);
        }
        break;

      case 't':
        milliseconds = ACE_OS::atoi (get_opts.optarg);
        break;

      case 'x':
        respond = 0;
        break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
                           "-o <iorfile> "
                           "-r <protocol=addr> "
                           "-s <protocol=addr> "
                           "-t <milliseconds> "
                           "\n",
                           argv [0]),
                          -1);
      }


  // If no protocols are specified use the default...
  if (ping_protocols.length () == 0)
    {
      ping_protocols.length (1);
      ping_protocols[0] = CORBA::string_dup ("UDP=224.9.9.2:12345");
    }

  if (pong_protocols.length () == 0)
    {
      pong_protocols.length (1);
      pong_protocols[0] = CORBA::string_dup ("UDP=224.9.9.2:23456");
    }

  // Indicates sucessful parsing of the command line
  return 0;
}

int main (int argc, char *argv[])
{
  ACE_TRY_NEW_ENV
    {
      parse_args (argc, argv);

      CORBA::ORB_var orb = CORBA::ORB_init (argc,
                                            argv);

      CORBA::Object_var obj
        = orb->resolve_initial_references ("RootPOA", ACE_TRY_ENV);
      ACE_TRY_CHECK;

      PortableServer::POA_var poa
        = PortableServer::POA::_narrow (obj.in ());

      PortableServer::POAManager_var mgr
        = poa->the_POAManager ();

      mgr->activate ();

      TAO_AV_CORE::instance ()->init (orb.in (),
                                      poa.in (),
                                      ACE_TRY_ENV);
      ACE_TRY_CHECK;

      // Register the video mmdevice object with the ORB
      Reactive_Strategy *reactive_strategy;
      ACE_NEW_RETURN (reactive_strategy,
                      Reactive_Strategy,
                      1);
      reactive_strategy->init (orb.in (), poa.in ());
      TAO_MMDevice *mmdevice_impl;
      ACE_NEW_RETURN (mmdevice_impl,
                      TAO_MMDevice (reactive_strategy),
                      1);

      AVStreams::MMDevice_var mmdevice =
        mmdevice_impl->_this (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      CORBA::String_var ior =
        orb->object_to_string (mmdevice.in (), ACE_TRY_ENV);
      ACE_TRY_CHECK;

      ACE_DEBUG ((LM_DEBUG, "Activated as <%s>\n", ior.in ()));

      // If the ior_output_file exists, output the ior to it
      if (ior_output_file != 0)
        {
          FILE *output_file= ACE_OS::fopen (ior_output_file, "w");
          if (output_file == 0)
            ACE_ERROR_RETURN ((LM_ERROR,
                               "Cannot open output file for writing IOR: %s",
                               ior_output_file),
                              1);
          ACE_OS::fprintf (output_file, "%s", ior.in ());
          ACE_OS::fclose (output_file);
        }

      Ping_Recv_FDev* ping_fdev_impl;
      ACE_NEW_RETURN (ping_fdev_impl,
                      Ping_Recv_FDev ("Ping"),
                      1);
      Pong_Send_FDev* pong_fdev_impl;
      ACE_NEW_RETURN (pong_fdev_impl,
                      Pong_Send_FDev ("Pong"),
                      1);

      AVStreams::FDev_var ping_fdev =
        ping_fdev_impl->_this (ACE_TRY_ENV);
      ACE_TRY_CHECK;
      AVStreams::FDev_var pong_fdev =
        pong_fdev_impl->_this (ACE_TRY_ENV);
      ACE_TRY_CHECK;

      mmdevice->add_fdev (ping_fdev.in (), ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (respond == 1)
        {
          mmdevice->add_fdev (pong_fdev.in (), ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }

      orb->run ();
      ACE_TRY_CHECK;


      ACE_DEBUG ((LM_DEBUG, "Calibrating scale factory . . . "));
      ACE_UINT32 gsf = ACE_High_Res_Timer::global_scale_factor ();
      ACE_DEBUG ((LM_DEBUG, "done %d \n", gsf));

      recv_latency.dump_results ("Receive", gsf);

    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Catched exception:");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}

// ****************************************************************

Ping_Recv::Ping_Recv (void)
  :  TAO_FlowConsumer ("Ping",
                       ping_protocols,
                       "UNS:ping")
{
}

int
Ping_Recv::get_callback (const char *,
                         TAO_AV_Callback *&callback)
{
  ACE_DEBUG ((LM_DEBUG,"Ping_Recv::get_callback\n"));
  callback = &this->callback_;
  return 0;
}

Ping_Recv_Callback::Ping_Recv_Callback (void)
  : count_ (0)
{
}

int
Ping_Recv_Callback::handle_stop (void)
{
  ACE_DEBUG ((LM_DEBUG,"Ping_Recv_Callback::stop"));
  TAO_AV_CORE::instance ()->orb ()->shutdown ();

  return 0;
}

int
Ping_Recv_Callback::receive_frame (ACE_Message_Block *frame,
                                   TAO_AV_frame_info *,
                                   const ACE_Addr &)
{
  this->count_++;

  ACE_DEBUG ((LM_DEBUG,"Ping_Recv_Callback::receive_frame %d\n", this->count_));

  if (this->count_ < 10)
    {
      for (const ACE_Message_Block *i = frame;
           i != 0;
           i = i->cont ())
        {
          ACE_hrtime_t stamp;

          if (i->length () < sizeof(stamp))
            return 0;

          ACE_OS::memcpy (&stamp, i->rd_ptr (), sizeof(stamp));

          ACE_hrtime_t now = ACE_OS::gethrtime ();
          if (recv_base == 0)
            {
              recv_base = now;
            }
          else
            {
          recv_latency.sample (now - recv_base,
                               now - stamp);
            }

          if (respond == 1)
            pong_callback.send_response (stamp);
        }
    }
  else
    TAO_AV_CORE::instance ()->orb ()->shutdown ();
  return 0;
}

int
Ping_Recv_Callback::handle_destroy (void)
{
  ACE_DEBUG ((LM_DEBUG,"Ping_Recv_Callback::destroy\n"));
  return 0;
}

// ****************************************************************

Pong_Send::Pong_Send (void)
  :  TAO_FlowProducer ("Pong",
                       pong_protocols,
                       "UNS:pong")
{
}

int
Pong_Send::get_callback (const char *,
                         TAO_AV_Callback *&callback)
{
  ACE_DEBUG ((LM_DEBUG,"Pong_Send::get_callback\n"));
  callback = &pong_callback;
  return 0;
}

void
Pong_Send_Callback::get_timeout (ACE_Time_Value *&tv,
                                 void *&)
{
  // @@ ACE_NEW (tv, ACE_Time_Value (0, milliseconds * 1000));
  tv = 0;
}

int
Pong_Send_Callback::handle_timeout (void *)
{
  // ACE_DEBUG ((LM_DEBUG, "pong timeout (ignored)\n"));
  return 0;
}

int
Pong_Send_Callback::handle_end_stream (void)
{
  return 0;
}

int
Pong_Send_Callback::send_response (ACE_hrtime_t stamp)
{
  ACE_DEBUG ((LM_DEBUG, "pong send response)\n"));

  ACE_hrtime_t buf[2];

  ACE_Message_Block mb (ACE_reinterpret_cast (char*,buf),
                        sizeof(buf));

  buf[0] = stamp;
  buf[1] = ACE_OS::gethrtime ();
  mb.wr_ptr (sizeof(buf));

  int result = this->protocol_object_->send_frame (&mb);
  if (result < 0)
    ACE_ERROR_RETURN ((LM_ERROR,
                       "Pong_Send_Callback::send - %p\n",
                       ""),
                      -1);

  return 0;
}

// ****************************************************************

// @@ TODO
#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
template class TAO_AV_Endpoint_Reactive_Strategy_B<TAO_StreamEndPoint_B, TAO_VDev, AV_Null_MediaCtrl>;
template class TAO_AV_Endpoint_Reactive_Strategy<TAO_StreamEndPoint_B, TAO_VDev, AV_Null_MediaCtrl>;
template class TAO_FDev<TAO_FlowProducer, Ping_Recv>;
template class TAO_FDev<Pong_Send, TAO_FlowConsumer>;
#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#pragma instantiate TAO_AV_Endpoint_Reactive_Strategy_B<TAO_StreamEndPoint_B, TAO_VDev, AV_Null_MediaCtrl>
#pragma instantiate TAO_AV_Endpoint_Reactive_Strategy<TAO_StreamEndPoint_B, TAO_VDev, AV_Null_MediaCtrl>
#pragma instantiate TAO_FDev<TAO_FlowProducer, Ping_Recv>
#pragma instantiate TAO_FDev<Pong_Send, TAO_FlowConsumer>
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
