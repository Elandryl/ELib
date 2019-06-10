/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EError handling tools.
*/

#pragma once

#include <Windows.h>
#include <string>

/**
  @brief ELib EErrorCode list.
*/
enum                    EErrorCode
{
  // GLOBALS
  EERROR_NONE           = 0,
  EERROR_MEMORY,
  EERROR_NULL_PTR,
  EERROR_WINDOWS_ERR,
  EERROR_OUT_OF_RANGE,

  // NETWORK
  EERROR_NET_SOCKET_ERR,
  EERROR_NET_SOCKET_STATE,
  EERROR_NET_SOCKET_PROTOCOL,
  EERROR_NET_PACKET_ERR,
  EERROR_NET_PACKET_TRUNCATED,
  EERROR_NET_PACKET_TYPE,
  EERROR_NET_PACKETHANDLER_ERR,
  EERROR_NET_SELECTOR_ERR,
  EERROR_NET_SELECTOR_STATE,
  EERROR_NET_SELECTOR_EMPTY,
  EERROR_NET_SERVER_ERR,
  EERROR_NET_SERVER_STATE,
  EERROR_NET_CLIENT_ERR,
  EERROR_NET_CLIENT_STATE,

  // SQL
  EERROR_SQL_STATE,
  EERROR_SQL_MYSQL_ERR,
  EERROR_SQL_FIELD_ERR,
  EERROR_SQL_ROW_ERR,
  EERROR_SQL_RESULT_ERR,

  // END
  EERROR_COUNT
};

/**
  @brief General scope for ELib components.
*/
namespace               ELib
{

  /**
    @brief ELib object for error tracking.
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

  std::string           WindowsErrString(DWORD p_code);

  /**
    @brief ELib object for exception handling.
    @details Retrieve last EError at initialization.
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
    @details Access maccro is defined as mEERROR_G.
    @details Modifier maccro are defined as mEERROR_S(), mEERROR_SA(additionnal) and mEERROR_SH(history).
    @details Used by EException. User can throw exception by calling mETHROW_S() or mETHROW_SA(additionnal).
    @details Functions using EError should first reset it by calling mEERROR_R().
  */
  extern EError         __gELastEError;

}

#define mEERROR         (ELib::__gELastEError.m_errorCode)
#define mEERROR_G       (ELib::__gELastEError)

#define mEERROR_R()                                     \
{                                                       \
  memset(&__gELastEError, 0, sizeof(EError));           \
}

#define mEERROR_S(p_errorCode)                          \
{                                                       \
  mEERROR = p_errorCode;                                \
  mEERROR_G.m_functionName = __FUNCTION__;              \
  mEERROR_G.m_fileName = __FILE__;                      \
  mEERROR_G.m_lineNumber = __LINE__;                    \
  mEPRINT_ERR(mEERROR_G.toString())                     \
}

#define mEERROR_SA(p_errorCode, p_additionnalInfos)     \
{                                                       \
  mEERROR_G.m_additionnalInfos = p_additionnalInfos;    \
  mEERROR_G.formatInfos();                              \
  mEERROR_S(p_errorCode);                               \
}

#define mEERROR_SH(p_errorCode)                         \
{                                                       \
  mEERROR_G.m_additionnalInfos = mEERROR_G.toString();  \
  mEERROR_G.formatInfos();                              \
  mEERROR_S(p_errorCode);                               \
}

#define mETHROW_S(p_errorCode)                          \
{                                                       \
  mEERROR_S(p_errorCode);                               \
  throw (ELib::EException());                           \
}

#define mETHROW_SA(p_errorCode)                         \
{                                                       \
  mEERROR_SA(p_errorCode, p_additionnalInfos);          \
  mETHROW_S()                                           \
}
