/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for ENetPacket Class.
*/

#include "ENetwork/ENetPacket.h"

/**
  @brief General scope for ELib components.
*/
namespace           ELib
{

  /**
    @brief Constructor for ENetPacket.
    @param p_type Type of ENetPacket.
    @param p_src ENetSocket source of ENetPacket.
  */
  ENetPacket::ENetPacket(ENetPacketType p_type, ENetSocket *p_src) :
    m_type(p_type),
    m_src(p_src)
  {
  }

  /**
    @brief Destructor for ENetPacket.
  */
  ENetPacket::~ENetPacket()
  {
  }

  /**
    @brief Default ENetPacket sending. Target depends on protocol. /!\ EError.
    @details Handle the transmission of datas from source.
    @details Target is destination if valid or source for connected protocols.
    @details ENetSocket destination must be valid for connectionless protocols.
    @param p_datas Datas of ENetPacket.
    @param p_len Datas length.
    @param p_dst ENetSocket destination.
  */
  void              ENetPacket::send(const char *p_datas, int32 p_len, ENetSocket *p_dst)
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if ((nullptr == p_dst)
      && (ENETSOCKET_FLAGS_PROTOCOL_UDP == (m_src->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS)))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      char          *l_datas = nullptr;
      int32         l_len = -1;
      int32         l_ret = -1;

      l_len = sizeof(ENetPacketType) + p_len;
      l_datas = new char[l_len];
      if (nullptr != l_datas)
      {
        memcpy(l_datas, &m_type, sizeof(ENetPacketType));
        memcpy(l_datas + sizeof(ENetPacketType), p_datas, p_len);
        switch (m_src->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS)
        {
          case ENETSOCKET_FLAGS_PROTOCOL_TCP:
          {
            if (nullptr != p_dst)
            {
              l_ret = p_dst->send(l_datas, l_len);
            }
            else
            {
              l_ret = m_src->send(l_datas, l_len);
            }
          }
            break;
          case ENETSOCKET_FLAGS_PROTOCOL_UDP:
          {
            l_ret = m_src->sendto(l_datas, l_len, p_dst);
          }
            break;
          default:
            break;
        }
        if (EERROR_NONE == mEERROR)
        {
          if (l_ret < l_len)
          {
            mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
          }
        }
        else
        {
          mEERROR_SH(EERROR_NET_SOCKET_ERR);
        }
        delete (l_datas);
      }
      else
      {
        mEERROR_S(EERROR_MEMORY);
      }
    }
  }

  /**
    @brief Get type of ENetPacket.
    @return Type.
  */
  ENetPacketType    ENetPacket::getType() const
  {
    return (m_type);
  }

  /**
    @brief Get ENetSocket source of ENetPacket.
    @return ENetPacket source.
  */
  const ENetSocket  *ENetPacket::getSource() const
  {
    return (m_src);
  }

  /**
    @brief Set ENetSocket source of ENetPacket.
    @param p_src ENetPacket source.
  */
  void              ENetPacket::setSource(ENetSocket *p_src)
  {
    m_src = p_src;
  }

  /**
    @brief Constructor for ENetPacketDisconnect.
    @param p_src ENetPacket source.
  */
  ENetPacketDisconnect::ENetPacketDisconnect(ENetSocket *p_src) :
    ENetPacket(ENETPACKET_TYPE_DISCONNECT, p_src)
  {
  }

  /**
    @brief Destructor for ENetPacketDisconnect.
  */
  ENetPacketDisconnect::~ENetPacketDisconnect()
  {
  }

  /**
    @brief Receive ENetPacketDisconnect from ENetSocket source. Used for connected protocols.
    @details Handle the reception of ENetPacketDisconnect from ENetSocket source.
  */
  void              ENetPacketDisconnect::recv()
  {
  }

  /**
    @brief Read ENetPacketDisconnect from datas in parameters. Used for connectionless protocols.
    @details Handle the reading of ENetPacketDisconnect from datas in parameters.
    @param p_datas Datas of ENetPacketDisconnect.
    @param p_len Datas length.
  */
  void              ENetPacketDisconnect::read(const char *p_datas, int32 p_len)
  {
  }

  /**
    @brief Send ENetPacketDisconnect. Destination depends on protocol. /!\ EError.
    @details Handle the transmission of ENetPacketDisconnect from source.
    @details Use default send() with copy of members.
    @param p_dst ENetSocket destination.
  */
  void              ENetPacketDisconnect::send(ENetSocket *p_dst)
  {
    mEERROR_R();
    ENetPacket::send(nullptr, 0, p_dst);
    if (EERROR_NONE != mEERROR)
    {
      mEERROR_SH(EERROR_NET_PACKET_ERR);
    }
  }

  /**
    @brief Constructor for ENetPacketConnect.
    @param p_src ENetPacket source.
  */
  ENetPacketConnect::ENetPacketConnect(ENetSocket *p_src) :
    ENetPacket(ENETPACKET_TYPE_CONNECT, p_src)
  {
  }

  /**
    @brief Destructor for ENetPacketConnect.
  */
  ENetPacketConnect::~ENetPacketConnect()
  {
  }

  /**
    @brief Receive ENetPacketConnect from ENetSocket source. Used for connected protocols.
    @details Handle the reception of a ENetPacketConnect from ENetSocket source.
  */
  void              ENetPacketConnect::recv()
  {
  }

  /**
    @brief Read ENetPacketConnect from datas in parameters. Used for connectionless protocols.
    @details Handle the reading of ENetPacketConnect from datas in parameters.
    @param p_datas Datas of ENetPacketConnect.
    @param p_len Datas length.
  */
  void              ENetPacketConnect::read(const char *p_datas, int32 p_len)
  {
  }

  /**
    @brief Send ENetPacketConnect. Destination depends on protocol. /!\ EError.
    @details Handle the transmission of ENetPacketConnect from source.
    @details Use default send() with copy of members.
    @param p_dst ENetSocket destination.
  */
  void              ENetPacketConnect::send(ENetSocket *p_dst)
  {
    mEERROR_R();
    ENetPacket::send(nullptr, 0, p_dst);
    if (EERROR_NONE != mEERROR)
    {
      mEERROR_SH(EERROR_NET_PACKET_ERR);
    }
  }

  /**
    @brief Constructor for ENetPacketRawDatas.
    @param p_src ENetPacket source.
  */
  ENetPacketRawDatas::ENetPacketRawDatas(ENetSocket *p_src) :
    ENetPacket(ENETPACKET_TYPE_RAW_DATAS, p_src),
    m_len(0),
    m_datas(nullptr)
  {
  }

  /**
    @brief Destructor for ENetPacketRawDatas.
  */
  ENetPacketRawDatas::~ENetPacketRawDatas()
  {
    if (nullptr != m_datas)
    {
      delete (m_datas);
    }
  }

  /**
    @brief Receive ENetPacketRawDatas from ENetSocket source. Used for connected protocols. /!\ Blocking. /!\ EError.
    @details Handle the reception of ENetPacketRawDatas from ENetSocket source.
    @details Can be blocking on bad behavior, waiting for datas that were not properly sent.
  */
  void              ENetPacketRawDatas::recv()
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if (ENETSOCKET_FLAGS_PROTOCOL_UDP == (m_src->getFlags() & ENETSOCKET_FLAGS_PROTOCOLS))
    {
      mEERROR_S(EERROR_NET_SOCKET_PROTOCOL);
    }
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      int32         l_len = -1;

      l_len = m_src->recv(reinterpret_cast<char*>(&m_len), sizeof(int32));
      if (EERROR_NONE == mEERROR)
      {
        if (sizeof(int32) == l_len)
        {
          m_datas = new char[m_len];
          if (nullptr != m_datas)
          {
            l_len = m_src->recv(m_datas, m_len);
            if (EERROR_NONE == mEERROR)
            {
              if (l_len < m_len)
              {
                mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
              }
            }
            else
            {
              mEERROR_SH(EERROR_NET_SOCKET_ERR);
            }
            if (EERROR_NONE != mEERROR)
            {
              m_len = 0;
              delete (m_datas);
              m_datas = nullptr;
            }
          }
          else
          {
            mEERROR_S(EERROR_MEMORY);
          }
        }
        else
        {
          mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
        }
      }
      else
      {
        mEERROR_SH(EERROR_NET_SOCKET_ERR);
      }
    }
  }

  /**
    @brief Read ENetPacketRawDatas from datas in parameters. Used for connectionless protocols. /!\ EError.
    @details Handle the reading of ENetPacketRawDatas from datas in parameters.
    @param p_datas Datas of ENetPacketRawDatas.
    @param p_len Datas length.
  */
  void              ENetPacketRawDatas::read(const char *p_datas, int32 p_len)
  {
    mEERROR_R();
    if (nullptr == p_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      if (sizeof(int32) <= p_len)
      {
        int32       l_len = -1;

        l_len = *reinterpret_cast<const int32*>(p_datas);
        if (l_len == (p_len - sizeof(int32)))
        {
          m_datas = new char[l_len];
          if (nullptr != m_datas)
          {
            m_len = l_len;
            memcpy(m_datas, p_datas + sizeof(int32), m_len);
          }
          else
          {
            mEERROR_S(EERROR_MEMORY);
          }
        }
        else
        {
          mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
        }
      }
      else
      {
        mEERROR_S(EERROR_NET_PACKET_TRUNCATED);
      }
    }
  }

  /**
    @brief Send ENetPacketRawDatas. Destination depends on protocol. /!\ EError.
    @details Handle the transmission of ENetPacketRawDatas from source.
    @details Use default send() with copy of members.
    @param p_dst ENetSocket destination.
  */
  void              ENetPacketRawDatas::send(ENetSocket *p_dst)
  {
    mEERROR_R();
    if (nullptr == m_src)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }
    if (nullptr == m_datas)
    {
      mEERROR_S(EERROR_NULL_PTR);
    }

    if (EERROR_NONE == mEERROR)
    {
      char          *l_datas = nullptr;
      int32         l_len = -1;

      l_len = sizeof(int32) + m_len;
      l_datas = new char[l_len];
      if (nullptr != l_datas)
      {
        memcpy(l_datas, &l_len, sizeof(int32));
        memcpy(l_datas + sizeof(int32), m_datas, l_len);
        ENetPacket::send(l_datas, l_len, p_dst);
        if (EERROR_NONE != mEERROR)
        {
          mEERROR_SH(EERROR_NET_PACKET_ERR);
        }
        delete (l_datas);
      }
      else
      {
        mEERROR_S(EERROR_MEMORY);
      }
    }
  }

  /**
    @brief Get datas length of ENetPacketRawDatas.
    @return Datas length.
  */
  int32             ENetPacketRawDatas::getLength() const
  {
    return (m_len);
  }

  /**
    @brief Get datas of ENetPacketRawDatas.
    @return Datas.
  */
  const char        *ENetPacketRawDatas::getDatas() const
  {
    return (m_datas);
  }

  /**
    @brief Set datas of ENetPacketRawDatas.
    @param p_datas Datas of ENetPacketRawDatas.
    @param p_len Datas length.
  */
  void              ENetPacketRawDatas::setDatas(char *p_datas, int32 p_len)
  {
    m_len = p_len;
    m_datas = p_datas;
  }

}
