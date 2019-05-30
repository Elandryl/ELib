#include <iostream>
#include <ENetwork\EServer.h>

ELib::EError              __gELastEError;

DWORD WINAPI              recvPackets(LPVOID p_param)
{
  ELib::EServer           *l_server = static_cast<ELib::EServer*>(p_param);

  while (true == l_server->isRunning())
  {
    ELib::EPacket         *l_packet = nullptr;

    l_packet = l_server->getPacketHandler().getPacket();
    if (nullptr != l_packet)
    {
      switch (l_packet->getType())
      {
      case ELib::EPACKET_TYPE_DISCONNECT:
      {
        std::cout << *l_packet->getSource() << " disconnected" << std::endl;
      }
      break;
      case ELib::EPACKET_TYPE_CONNECT:
      {
        std::cout << *l_packet->getSource() << " connected" << std::endl;
      }
      break;
      case ELib::EPACKET_TYPE_RAW_DATAS:
      {
        std::cout << *l_packet->getSource() << " " << reinterpret_cast<ELib::EPacketRawDatas*>(l_packet)->getDatas() << std::endl;
      }
      break;
      default:
        break;
      }
      l_server->broadcast(l_packet);
      delete (l_packet);
    }
  }

  return (0);
}

int	                      main()
{
  try
  {
    ELib::EServer         l_server;

    l_server.init("192.168.1.50", 2222);
    if (ELib::EERROR_NONE == mEERROR_G.m_errorCode)
    {
      l_server.start();
      if (ELib::EERROR_NONE == mEERROR_G.m_errorCode)
      {
        for (int i = 0; i < 1; ++i)
        {
          CreateThread(nullptr, 0, recvPackets, &l_server, 0, nullptr);
        }
        system("pause");
      }
      else
      {
        mETHROW();
      }
    }
    else
    {
      mETHROW();
    }
  }
  catch (ELib::EException e)
  {
    e.printBox();
  }
  return (0);
}