#include <iostream>
#include <ENetwork\EClient.h>

ELib::EError                  __gELastEError;

DWORD WINAPI                  recvPackets(LPVOID p_param)
{
  ELib::EClient               *l_client = static_cast<ELib::EClient*>(p_param);

  while (true == l_client->isRunning())
  {
    ELib::EPacket             *l_packet = nullptr;

    l_packet = l_client->getPacketHandler().getPacket();
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
      delete (l_packet);
    }
  }
  std::cout << "EServer disconnected" << std::endl;
  return (0);
}

int                           main()
{
  try
  {
    ELib::EClient             l_client;

    l_client.init("192.168.1.50", 2222);
    if (ELib::EERROR_NONE == mEERROR_G.m_errorCode)
    {
      l_client.start();
      CreateThread(nullptr, 0, recvPackets, &l_client, 0, nullptr);

      ELib::EPacketRawDatas   *l_packet = new ELib::EPacketRawDatas();
      ELib::EPacket           *l_disconnect = new ELib::EPacketDisconnect();

      l_packet->setDatas("Ceci est un test\0");
      l_packet->setLength(17);
      while (true == l_client.isRunning())
      {
        system("pause");
        l_client.sendPacket(l_packet);
      }
      l_client.sendPacket(l_disconnect);
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