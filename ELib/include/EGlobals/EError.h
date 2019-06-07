/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EError handling tools.
*/

#pragma once

#include <Windows.h>
#include <string>

/**
  @brief List every EErrorCode from ELib components.
*/
enum                    EErrorCode
{
  // GLOBALS
  EERROR_NONE           = 0,
  EERROR_OOM,
  EERROR_NULL_PTR,

  // NETWORK
  EERROR_NET_WSA_STARTUP,
  EERROR_NET_SOCKET_STATE,
  EERROR_NET_SOCKET_PROTOCOL,
  EERROR_NET_SOCKET_INVALID,
  EERROR_NET_SOCKET_BIND,
  EERROR_NET_SOCKET_LISTEN,
  EERROR_NET_SOCKET_ACCEPT,
  EERROR_NET_SOCKET_CONNECT,
  EERROR_NET_SOCKET_RECV,
  EERROR_NET_SOCKET_RECVFROM,
  EERROR_NET_SOCKET_SEND,
  EERROR_NET_SOCKET_SENDTO,
  EERROR_NET_SOCKET_SHUTDOWN,
  EERROR_NET_SOCKET_CLOSE,
  EERROR_NET_PACKET_INVALID,
  EERROR_NET_PACKET_RECV,
  EERROR_NET_PACKET_SEND,
  EERROR_NET_PACKET_TRUNCATED,
  EERROR_NET_PACKET_RESERVED,
  EERROR_NET_SELECTOR_RUNNING,
  EERROR_NET_SELECTOR_EMPTY,
  EERROR_NET_SELECTOR_SELECT,
  EERROR_NET_SELECTOR_RECV,
  EERROR_NET_SELECTOR_SEND,
  EERROR_NET_SERVER_INIT,
  EERROR_NET_SERVER_START,
  EERROR_NET_SERVER_STOP,
  EERROR_NET_SERVER_ADD,
  EERROR_NET_SERVER_RUNNING,
  EERROR_NET_CLIENT_INIT,
  EERROR_NET_CLIENT_STOP,
  EERROR_NET_CLIENT_RUNNING,

  // SQL
  EERROR_SQL_MYSQL_ERROR,
  EERROR_SQL_NOT_CONNECTED,
  EERROR_SQL_FIELD_INVALID,
  EERROR_SQL_OUT_OF_RANGE,
  EERROR_SQL_WRONG_PADDING,
  EERROR_SQL_RESULT_FAILURE,

  // END
  EERROR_COUNT
};

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

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
  };

  /**
    @brief ELib function to retrieve Windows error informations.
    @return Windows error informations as a string.
  */
  std::string           getWSAErrString();

  /**
    @brief ELib object for exception handling.
    @details Automatically retrieve the last error at initialization.
    @details Inherit from std::exception.
  */
  class                 EException : std::exception
  {
  public:
    EException();
    ~EException();
    const std::string   toString() const;
    void                printBox() const;
  };

  /**
    @brief Global variable for error tracking.
    @details Easy access maccro is defined as mEERROR_G.
    @details Easy modifier maccro are defined as mEERROR_S and mEERROR_SA.
    @details This is used in EException. User can throw exception by calling mETHROW or mETHROWA.
    @details Functions implementing this should first reset it with mEERROR_R.
  */
  extern EError         __gELastEError;

}

#define mEERROR         (ELib::__gELastEError.m_errorCode)
#define mEERROR_G       (ELib::__gELastEError)

#define mEERROR_R()                                   \
{                                                     \
  memset(&__gELastEError, 0, sizeof(EError));         \
}

#define mEERROR_S(p_errorCode)                        \
{                                                     \
  mEERROR = p_errorCode;                \
  mEERROR_G.m_functionName = __FUNCTION__;            \
  mEERROR_G.m_fileName = __FILE__;                    \
  mEERROR_G.m_lineNumber = __LINE__;                  \
  mEPRINT_ERR(mEERROR_G.toString())                   \
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