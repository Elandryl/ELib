/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EError handling tools.
*/

#include <atlstr.h>
#include <iostream>
#include "EGlobals/EGlobal.h"
#include "EGlobals/EError.h"

/**
  @brief General scope for ELib components.
*/
namespace             ELib
{

  /**
    @brief Global __gELastEError definition.
  */
  ELib::EError        __gELastEError;

  /**
    @brief EError definitions.
  */
  const std::string   EErrorDefinitions[] =
  {
    // GLOBALS
    "EERROR_NONE",
    "EERROR_MEMORY",
    "EERROR_NULL_PTR",
    "EERROR_WINDOWS_ERR",

    // NETWORK
    "EERROR_NET_SOCKET_ERR",
    "EERROR_NET_SOCKET_STATE",
    "EERROR_NET_SOCKET_PROTOCOL",
    "EERROR_NET_PACKET_ERR",
    "EERROR_NET_PACKET_TRUNCATED",
    "EERROR_NET_PACKET_TYPE",
    "EERROR_NET_PACKETHANDLER_ERR",
    "EERROR_NET_SELECTOR_ERR",
    "EERROR_NET_SELECTOR_STATE",
    "EERROR_NET_SELECTOR_EMPTY",
    "EERROR_NET_SERVER_ERR",
    "EERROR_NET_SERVER_STATE",
    "EERROR_NET_CLIENT_ERR",
    "EERROR_NET_CLIENT_STATE",

    // SQL
    "EERROR_SQL_MYSQL_ERROR",
    "EERROR_SQL_NOT_CONNECTED",
    "EERROR_SQL_FIELD_INVALID",
    "EERROR_SQL_OUT_OF_RANGE",
    "EERROR_SQL_WRONG_PADDING",
    "EERROR_SQL_RESULT_FAILURE",

    // END
    "EERROR_COUNT"
  };

  /**
    @brief Constructor for EError.
    @details Force EError.cpp to be call before any EError to allocate its global variable.
  */
  EError::EError()
  {
  }

  /**
    @brief Format AdditionnalInfos.
  */
  void                EError::formatInfos()
  {
    size_t            l_pos = 0;

    l_pos = m_additionnalInfos.find_first_of("\n", l_pos);
    while (std::string::npos != l_pos)
    {
      m_additionnalInfos.replace(l_pos, 1, "\n\t");
      l_pos = m_additionnalInfos.find_first_of("\n", ++l_pos);
    }
  }

  /**
    @brief Get EError informations as string.
    @return Informations of EError.
  */
  const std::string   EError::toString() const
  {
    std::string       l_str = "";

    l_str = "EError ";
    l_str += std::to_string(m_errorCode);
    l_str += " (" + EErrorDefinitions[m_errorCode < EERROR_COUNT ? m_errorCode : EERROR_COUNT] + ")";
    l_str += "\nFunction : " + m_functionName;
    l_str += "\nFile : " + m_fileName;
    l_str += "\nLine : " + std::to_string(m_lineNumber);
    if (false == m_additionnalInfos.empty())
    {
      l_str += "\n\t" + m_additionnalInfos;
    }
    l_str += "\n";

    return (l_str);
  }

  /**
    @brief Get string from Windows error code.
    @return Windows error informations.
  */
  std::string         WindowsErrString(DWORD l_code)
  {
    LPWSTR            l_buf = nullptr;
    std::string       l_str = "";

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, l_code,
      NULL, reinterpret_cast<LPWSTR>(&l_buf), 0, NULL);
    l_str = CW2A(l_buf, CP_UTF8);
    LocalFree(l_buf);
    return (l_str);
  }

  /**
    @brief Constructor for EException.
    @details Retrieve __gELastEError as content.
  */
  EException::EException() :
    std::exception(mEERROR_G.toString().c_str())
  {
  }

  /**
    @brief Destructor for EException.
  */
  EException::~EException()
  {
  }

  /**
    @brief Get EException informations as string.
    @return Informations of EException.
  */
  const std::string   EException::toString() const
  {
    return (what());
  }

  /**
    @brief Print Exception Information through Windows MessageBox.
  */
  void                EException::printBox() const
  {
    std::string       l_title = "";

    l_title = std::string(what(), what() + (strchr(what(), '\n') - what()));
    MessageBoxA(nullptr, what(), l_title.c_str(), MB_ICONERROR | MB_OK);
  }

}
