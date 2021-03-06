#include <iostream>
#include <ENetwork\EServer.h>

enum                          CustomPacketType
{
  CUSTOM_PACKET_TYPE_CHAT = ELib::EPACKET_TYPE_RESERVED + 1
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

DWORD WINAPI              recvPackets(LPVOID p_param)
{
  ELib::EServer           *l_server = reinterpret_cast<ELib::EServer*>(p_param);

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
          mEPRINT_STD(std::to_string(*l_packet->getSource()) + " disconnected");
        }
          break;
        case ELib::EPACKET_TYPE_CONNECT:
        {
          mEPRINT_STD(std::to_string(*l_packet->getSource()) + " connected");
        }
          break;
        case ELib::EPACKET_TYPE_RAW_DATAS:
        {
          mEPRINT_STD(std::to_string(*l_packet->getSource()) + " " + reinterpret_cast<ELib::EPacketRawDatas*>(l_packet)->getDatas());
        }
          break;
        case CUSTOM_PACKET_TYPE_CHAT:
        {
          mEPRINT_STD(reinterpret_cast<CustomPacketChat*>(l_packet)->getLogin() + ": " + reinterpret_cast<CustomPacketChat*>(l_packet)->getMessage());
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

    mEPRINT_G.start();
    l_server.init("192.168.1.50", 2222);
    if (ELib::EERROR_NONE == mEERROR)
    {
      l_server.getPacketHandler().setGenerator(static_cast<ELib::EPacketType>(CUSTOM_PACKET_TYPE_CHAT), generatePacketChat);
      l_server.start();
      if (ELib::EERROR_NONE == mEERROR)
      {
        for (int i = 0; i < 1; ++i)
        {
          CreateThread(nullptr, 0, recvPackets, &l_server, 0, nullptr);
        }
        while (true == l_server.isRunning())
        {
          Sleep(50);
        }
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