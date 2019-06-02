/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ENetPacket Class.
*/

#pragma once

#include "EGlobals/EGlobal.h"
#include "ENetwork/ENetSocket.h"

/**
  @brief General scope for ELib components.
*/
namespace             ELib
{

  /**
    @brief ELib reserved ENetPacketType.
    @brief User can define custom type with value over ENETPACKET_TYPE_RESERVED.
  */
  enum                ENetPacketType
  {
    ENETPACKET_TYPE_DISCONNECT  = 0x00,
    ENETPACKET_TYPE_CONNECT     = 0x01,
    ENETPACKET_TYPE_RAW_DATAS   = 0x02,
    ENETPACKET_TYPE_RESERVED    = 0x0F  /**< Range of ELib PacketTypes. */
  };

  /**
    @brief Parent object for ENetPacket generation.
    @details A class need to inherit from this class to be used in the ENetPacketHandler automation.
    @details recv, generate and send must be implemented in child class.
  */
  class               ENetPacket
  {
  public:
    ENetPacket(ENetPacketType p_type, ENetSocket *p_src = nullptr);
    virtual ~ENetPacket();
    virtual void      recv() = 0;
    virtual void      generate(const char *p_datas, int32 p_len) = 0;
    virtual void      send(ENetSocket *p_dst = nullptr) = 0;
    virtual void      send(const char *p_datas, int32 p_len, ENetSocket *p_dst = nullptr);
    ENetPacketType    getType() const;
    const ENetSocket  *getSource() const;
    void              setSource(ENetSocket *p_src);

  protected:
    ENetPacketType    m_type; /**< Type of the ENetPacket. */
    ENetSocket        *m_src; /** Pointer to Source of the ENetPacket. */
  };

  /**
    @brief ENetPacket indicating a disconnection.
    @details This ENetPacket is auto-generated from the ENetPacketHandler.
    @details It is use to indicating the User of the disconnection of a ENetSocket.
  */
  class               ENetPacketDisconnect : public ENetPacket
  {
  public:
    ENetPacketDisconnect(ENetSocket *p_src = nullptr);
    ~ENetPacketDisconnect();
    void              recv();
    void              generate(const char *p_datas, int32 p_len);
    void              send(ENetSocket *p_dst = nullptr);
  };

  /**
    @brief ENetPacket indicating a connection.
    @details This ENetPacket is auto-generated from the ENetPacketHandler.
    @details It is use to indicating the User of the connection of a ENetSocket.
  */
  class             ENetPacketConnect : public ENetPacket
  {
  public:
    ENetPacketConnect(ENetSocket *p_src = nullptr);
    ~ENetPacketConnect();
    void              recv();
    void              generate(const char *p_datas, int32 p_len);
    void              send(ENetSocket *p_dst = nullptr);
  };

  /**
    @brief Buffer of datas based ENetPacket.
    @details This ENetPacket handle the transmission of a buffer of datas preceded by its length.
  */
  class               ENetPacketRawDatas : public ENetPacket
  {
  public:
    ENetPacketRawDatas(ENetSocket *p_src = nullptr);
    ~ENetPacketRawDatas();
    void              recv();
    void              generate(const char *p_datas, int32 p_len);
    void              send(ENetSocket *p_dst = nullptr);
    int32             getLength() const;
    void              setLength(int32 p_len);
    const char        *getDatas() const;
    void              setDatas(char *p_datas);

  private:
    int32             m_len;    /**< Length of the buffer m_datas. */
    char              *m_datas; /**< Buffer of datas. */
  };

}
