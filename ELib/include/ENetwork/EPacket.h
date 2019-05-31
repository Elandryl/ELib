/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for EPacket Class.
*/

#pragma once

#include "ESocket.h"

/**
  @brief General scope for ELib components.
*/
namespace           ELib
{

  /**
    @brief ELib reserved EPacketType.
    @brief User can define custom type with value over EPACKET_TYPE_RESERVED.
  */
  enum              EPacketType
  {
    EPACKET_TYPE_DISCONNECT         = 0x00,
    EPACKET_TYPE_CONNECT            = 0x01,
    EPACKET_TYPE_RAW_DATAS          = 0x02,
    EPACKET_TYPE_RESERVED           = 0x0F  /**< Range of ELib PacketTypes. */
  };

  /**
    @brief Parent object for EPacket generation.
    @details A class need to inherit from this class to be used in the EPacketHandler automation.
    @details recv, generate and send are virtual pure functions and must be implemented in child class.
  */
  class             EPacket
  {
  public:
    EPacket(EPacketType p_type, ESocket *p_src = nullptr);
    virtual ~EPacket();
    virtual void    recv() = 0;
    virtual void    generate(const char *p_datas, int32 p_len) = 0;
    virtual void    send(ESocket *p_dst = nullptr) = 0;
    virtual void    send(const char *p_datas, int32 p_len, ESocket *p_dst = nullptr);
    EPacketType     getType() const;
    const ESocket   *getSource() const;
    void            setSource(ESocket *p_src);

  protected:
    EPacketType     m_type; /**< Type of the EPacket. */
    ESocket         *m_src; /** Pointer to Source of the EPacket. */
  };

  /**
    @brief EPacket indicating a disconnection.
    @details This EPacket is auto-generated from the EPacketHandler.
    @details It is use to indicating the User of the disconnection of a ESocket.
  */
  class             EPacketDisconnect : public EPacket
  {
  public:
    EPacketDisconnect(ESocket *p_src = nullptr);
    ~EPacketDisconnect();
    void            recv();
    void            generate(const char *p_datas, int32 p_len);
    void            send(ESocket *p_dst = nullptr);
  };

  /**
    @brief EPacket indicating a connection.
    @details This EPacket is auto-generated from the EPacketHandler.
    @details It is use to indicating the User of the connection of a ESocket.
  */
  class             EPacketConnect : public EPacket
  {
  public:
    EPacketConnect(ESocket *p_src = nullptr);
    ~EPacketConnect();
    void            recv();
    void            generate(const char *p_datas, int32 p_len);
    void            send(ESocket *p_dst = nullptr);
  };

  /**
    @brief Vector of datas based EPacket.
    @details This EPacket handle the transmission of a buffer of datas preceded by its length.
  */
  class             EPacketRawDatas : public EPacket
  {
  public:
    EPacketRawDatas(ESocket *p_src = nullptr);
    ~EPacketRawDatas();
    void            recv();
    void            generate(const char *p_datas, int32 p_len);
    void            send(ESocket *p_dst = nullptr);
    int32           getLength() const;
    void            setLength(int32 p_len);
    const char      *getDatas() const;
    void            setDatas(char *p_datas);

  private:
    int32           m_len;    /**< Length of the buffer m_datas. */
    char            *m_datas; /**< Vector of datas. */
  };

}
