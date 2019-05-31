/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EError handling tools.
*/

#pragma once

#include <Windows.h>
#include <string>

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief List every EErrorCode from ELib components.
  */
  enum                  EErrorCode
  {
    EERROR_NONE = 0,

    // GENERICS
    EERROR_OOM,

    // NETWORK
    EERROR_WSA_STARTUP,

    EERROR_SOCKET_STATE,
    EERROR_SOCKET_PROTOCOL,
    EERROR_SOCKET_INVALID,
    EERROR_SOCKET_BIND,
    EERROR_SOCKET_LISTEN,
    EERROR_SOCKET_ACCEPT,
    EERROR_SOCKET_CONNECT,
    EERROR_SOCKET_RECV,
    EERROR_SOCKET_RECVFROM,
    EERROR_SOCKET_SEND,
    EERROR_SOCKET_SENDTO,
    EERROR_SOCKET_SHUTDOWN,
    EERROR_SOCKET_CLOSE,

    EERROR_PACKET_INVALID,
    EERROR_PACKET_RECV,
    EERROR_PACKET_SEND,
    EERROR_PACKET_TRUNCATED,
    EERROR_PACKET_RESERVED,

    EERROR_SELECTOR_RUNNING,
    EERROR_SELECTOR_EMPTY,
    EERROR_SELECTOR_SELECT,
    EERROR_SELECTOR_RECV,

    EERROR_SERVER_INIT,
    EERROR_SERVER_START,
    EERROR_SERVER_STOP,
    EERROR_SERVER_ADD,
    EERROR_SERVER_RUNNING,

    EERROR_CLIENT_INIT,
    EERROR_CLIENT_STOP,
    EERROR_CLIENT_RUNNING,

    // SQL
    EERROR_WRONG_PADDING,
    EERROR_OUT_OF_FIELD,
    EERROR_MYSQL_INIT,
    EERROR_SQL_CONNECT,
    EERROR_SQL_QUERY,
    EERROR_SQL_RESULT,

    // GRAPHICS
    EERROR_2D_CREATE,
    EERROR_2D_DRAWABLE_OUT,
    EERROR_IMAGE_LOAD,
    EERROR_FONT_LOAD,

    // END
    EERROR_COUNT
  };

  /**
    @brief ELib structure containing error informations.
    @details toString() is automatically formated in multiline display.
  */
  struct                EError
  {
    EError();                               /**< Empty constructor used for initialing globals. */
    EErrorCode          m_errorCode;        /**< Code from EErrorCode enum. */
    std::string         m_functionName;     /**< Function where the error occured. */
    std::string         m_fileName;         /**< Filename where the function is located. */
    int                 m_lineNumber;       /**< Line number where the function is located. */
    std::string         m_additionnalInfos; /**< Informations such as parent EError or WSA error. */
    void                formatInfos();
    const std::string   toString() const;
    void                print() const;
  };

  std::string           getWSAErrString();

  /**
    @brief ELib object for exception handling.
    @details Automatically retrieve the last error at initialization.
    @details Inherit from std::exception and implements what().
  */
  class                 EException : std::exception
  {
  public:
    EException();
    virtual ~EException();
    const std::string   toString() const;
    void                print() const;
    void                printBox() const;
  };

}

/**
  @brief Global variable for error tracking.
  @details Easy access maccro is defined as mEERROR_G.
  @details Easy modifier maccro are defined as mEERROR_S and mEERROR_SA.
  @details This is used in EException. User can throw exception by calling mETHROW or mETHROWA.
  @details Functions implementing this should first reset it with mEERROR_R.
*/
extern ELib::EError     __gELastEError;

#define mEERROR_G       (__gELastEError)

#define mEERROR_R()                                   \
{                                                     \
  memset(&__gELastEError, 0, sizeof(EError));         \
}

#define mEERROR_S(p_errorCode)                        \
{                                                     \
  mEERROR_G.m_errorCode = p_errorCode;                \
  mEERROR_G.m_functionName = __FUNCTION__;            \
  mEERROR_G.m_fileName = __FILE__;                    \
  mEERROR_G.m_lineNumber = __LINE__;                  \
  /*mEERROR_G.print();*/                                  \
}

#define mEERROR_SA(p_errorCode, p_additionnalInfos)   \
{                                                     \
  mEERROR_G.m_additionnalInfos = p_additionnalInfos;  \
  mEERROR_G.formatInfos();                            \
  mEERROR_S(p_errorCode);                             \
}

#define mETHROW()                                     \
{                                                     \
  throw (ELib::EException());                         \
}

#define mETHROW_S(p_errorCode)                        \
{                                                     \
  mEERROR_S(p_errorCode);                             \
  throw (ELib::EException());                         \
}

#define mETHROW_SA(p_errorCode)                       \
{                                                     \
  mEERROR_SA(p_errorCode, p_additionnalInfos);        \
  throw (ELib::EException());                         \
}