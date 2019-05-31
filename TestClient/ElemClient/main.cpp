#include <iostream>
#include <ENetwork\EClient.h>

enum                          CustomPacketType
{
  CUSTOM_PACKET_TYPE_CHAT     = ELib::EPACKET_TYPE_RESERVED + 1
};

class                         CustomPacketChat : public ELib::EPacket
{
public:
  CustomPacketChat(ELib::ESocket *p_src = nullptr) :
    ELib::EPacket(static_cast<ELib::EPacketType>(CUSTOM_PACKET_TYPE_CHAT), p_src),
    m_loginLen(0),
    m_messageLen(0),
    m_login(nullptr),
    m_message(nullptr)
  {
  }

  void                        recv()
  {
    m_src->recv(reinterpret_cast<char*>(&m_loginLen), sizeof(int32));
    m_src->recv(reinterpret_cast<char*>(&m_messageLen), sizeof(int32));
    m_login = new char[m_loginLen + 1]();
    m_src->recv(m_login, m_loginLen);
    m_message = new char[m_messageLen + 1]();
    m_src->recv(m_message, m_messageLen);
  }

  void                        generate(const char *p_datas, int p_len)
  {
    m_loginLen = *reinterpret_cast<const int32*>(p_datas);
    m_messageLen = *reinterpret_cast<const int32*>(p_datas + sizeof(int32));
    m_login = new char[m_loginLen + 1]();
    memcpy(m_login, p_datas + sizeof(int32) + sizeof(int32), m_loginLen);
    m_message = new char[m_messageLen + 1]();
    memcpy(m_message, p_datas + sizeof(int32) + sizeof(int32) + m_loginLen, m_messageLen);
  }

  void                        send(ELib::ESocket *p_dst)
  {
    char                      *l_datas = nullptr;
    int32                     l_len = -1;

    l_len = sizeof(int32) + sizeof(int32) + m_loginLen + m_messageLen;
    l_datas = new char[l_len];
    memcpy(l_datas, &m_loginLen, sizeof(int32));
    memcpy(l_datas + sizeof(int32), &m_messageLen, sizeof(int32));
    memcpy(l_datas + sizeof(int32) + sizeof(int32), m_login, m_loginLen);
    memcpy(l_datas + sizeof(int32) + sizeof(int32) + m_loginLen, m_message, m_messageLen);
    EPacket::send(l_datas, l_len, p_dst);
  }

  const std::string           getLogin() const
  {
    return (m_login);
  }

  void                        setLogin(const std::string &p_login)
  {
    m_loginLen = static_cast<int32>(p_login.size());
    m_login = new char[m_loginLen + 1]();
    memcpy(m_login, p_login.c_str(), m_loginLen);
  }

  const std::string           getMessage() const
  {
    return (m_message);
  }

  void                        setMessage(const std::string &p_message)
  {
    m_messageLen = static_cast<int32>(p_message.size());
    m_message = new char[m_messageLen + 1]();
    memcpy(m_message, p_message.c_str(), m_messageLen);
  }

private:
  int32                       m_loginLen;
  int32                       m_messageLen;
  char                        *m_login;
  char                        *m_message;
};

ELib::EPacket                 *generatePacketChat(ELib::ESocket *p_src)
{
  return (new CustomPacketChat(p_src));
}

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
          l_client->getPrinter().print(ELib::EPRINT_TYPE_PRIORITY_ERROR, std::to_string(*l_packet->getSource()) + " disconnected");
        }
          break;
        case ELib::EPACKET_TYPE_CONNECT:
        {
          l_client->getPrinter().print(ELib::EPRINT_TYPE_PRIORITY_ERROR, std::to_string(*l_packet->getSource()) + " connected");
        }
          break;
        case ELib::EPACKET_TYPE_RAW_DATAS:
        {
          l_client->getPrinter().print(ELib::EPRINT_TYPE_PRIORITY_ERROR, std::to_string(*l_packet->getSource()) + " " + reinterpret_cast<ELib::EPacketRawDatas*>(l_packet)->getDatas());
        }
          break;
        case CUSTOM_PACKET_TYPE_CHAT:
        {
          l_client->getPrinter().print(ELib::EPRINT_TYPE_PRIORITY_STD, reinterpret_cast<CustomPacketChat*>(l_packet)->getLogin() + ": " + reinterpret_cast<CustomPacketChat*>(l_packet)->getMessage());
        }
          break;
        default:
          break;
      }
      delete (l_packet);
    }
  }
  l_client->getPrinter().print(ELib::EPRINT_TYPE_PRIORITY_ERROR, "EServer disconnected");
  return (0);
}

int                           main()
{
  try
  {
    ELib::EClient             l_client;

    l_client.getPrinter().start();
    l_client.init("192.168.1.50", 2222);
    l_client.getPacketHandler().setGenerator(static_cast<ELib::EPacketType>(CUSTOM_PACKET_TYPE_CHAT), generatePacketChat);
    if (ELib::EERROR_NONE == mEERROR_G.m_errorCode)
    {
      l_client.start();
      CreateThread(nullptr, 0, recvPackets, &l_client, 0, nullptr);

      CustomPacketChat        *l_packet = new CustomPacketChat();
      ELib::EPacket           *l_disconnect = new ELib::EPacketDisconnect();
      std::string             l_in;

      l_client.getPrinter().print(ELib::EPRINT_TYPE_SPECIAL_ACTIVE, "Enter your login: ");
      l_in = l_client.getPrinter().getLine();
      l_packet->setLogin(l_in);
      while (true == l_client.isRunning())
      {
        l_client.getPrinter().print(ELib::EPRINT_TYPE_SPECIAL_ACTIVE, "Message: ");
        l_in = l_client.getPrinter().getLine();
        l_packet->setMessage(l_in);
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