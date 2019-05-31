/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Source for EError handling tools.
*/

#include "EError.h"
#include <iostream>
#include <atlstr.h>

ELib::EError          __gELastEError;

/**
  @brief General scope for ELib components.
*/
namespace             ELib
{

  /**
    @brief EError definitions for EErrorCodes.
  */
  const std::string   EErrorDefinitions[] =
  {
    "EERROR_NONE",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_",
    "EERROR_"
  };

  /**
    @brief Empty constructor used for initialing globals.
    @details This call forces EError.cpp to be call before any EError and allocate its global variable.
  */
  EError::EError()
  {
  }

  /**
    @brief Format AdditionnalInfos from EError for display.
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
    @brief Convert EError to std::string.
    @return String of EError informations.
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
    @brief Retrieve WSA last error and returns its informations.
    @return String containing informations of last WSA error.
  */
  std::string         getWSAErrString()
  {
    int               l_err = 0;
    LPWSTR            l_buf = nullptr;
    std::string       l_str = "";

    return (l_str);
    l_err = WSAGetLastError();
    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, l_err,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)l_buf, 0, NULL);
    //l_str = CW2A(l_buf, CP_UTF8);
    LocalFree(l_buf);
    return (l_str);
  }

  /**
    @brief Initialize the EException with __gELastEError datas.
  */
  EException::EException() :
    std::exception(mEERROR_G.toString().c_str())
  {
  }

  /**
    @brief Do nothing.
  */
  EException::~EException()
  {
  }

  /**
    @brief Convert EException to std::string.
    @return String of Exception informations.
  */
  const std::string   EException::toString() const
  {
    return (what());
  }

  /**
    @brief Print the String of Exception Information through Windows MessageBox.
  */
  void                EException::printBox() const
  {
    std::string       l_title = "";

    l_title = std::string(what(), what() + (strchr(what(), '\n') - what()));
    MessageBoxA(nullptr, what(), l_title.c_str(), MB_ICONERROR | MB_OK);
  }

}
