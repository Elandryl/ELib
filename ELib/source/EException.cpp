#include "EException.h"
#include <iostream>
#include <Windows.h>

const std::string   EErrorDefinitions[] =
{
  "EERROR_NONE",
  "EERROR_WRONG_PADDING",
  "EERROR_OUT_OF_FIELD",
  "EERROR_MYSQL_INIT",
  "EERROR_SQL_CONNECT",
  "EERROR_SQL_QUERY",
  "EERROR_SQL_RESULT",
  "EERROR_INVALID_SOCKET",
  "EERROR_REACH_HOSTNAME",
  "EERROR_SOCKET_CONNECT",
  "EERROR_SOCKET_BIND",
  "EERROR_SOCKET_LISTEN",
  "EERROR_SOCKET_ACCEPT",
  "EERROR_SOCKET_SEND",
  "EERROR_CLIENT_RECEIVE",
  "EERROR_SERVER_SELECT",
  "EERROR_INVALID_PACKET",
  "EERROR_SOCKET_CLOSE",
  "EERROR_SOCKET_SHUTDOWN",
  "EERROR_WSA_STARTUP",
  "EERROR_PACKET_GENERATOR",
  "EERROR_SERVER_ADD",
  "EERROR_SELECTOR_LAUNCH",
  "Undefined"
};

EException::EException(EErrorCode errorCode, const std::string &functionName, const std::string &fileName, int lineNumber) :
  m_errorCode(errorCode),
  m_functionName(functionName),
  m_fileName(fileName),
  m_lineNumber(lineNumber)
{
}

std::string	    EException::toString() const
{
  return ("Error : " + EErrorDefinitions[m_errorCode < EERROR_COUNT ? m_errorCode : EERROR_COUNT] + " (" + std::to_string(m_errorCode) +
	  ")\nFunction : " + m_functionName +
	  "\nFile : " + m_fileName +
	  "\nLine : " + std::to_string(m_lineNumber));
}

void		    EException::print() const
{
  std::cout << toString() << std::endl;
}

void		    EException::printBox() const
{
  MessageBoxA(nullptr, toString().c_str(), ("Error " + std::to_string(m_errorCode)).c_str(), MB_ICONERROR | MB_OK);
}