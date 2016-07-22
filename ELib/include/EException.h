#pragma once

#include <string>

enum			    EErrorCode
{
  EERROR_NONE,
  EERROR_WRONG_PADDING,
  EERROR_OUT_OF_FIELD,
  EERROR_MYSQL_INIT,
  EERROR_SQL_CONNECT,
  EERROR_SQL_QUERY,
  EERROR_SQL_RESULT,
  EERROR_INVALID_SOCKET,
  EERROR_REACH_HOSTNAME,
  EERROR_SOCKET_CONNECT,
  EERROR_SOCKET_BIND,
  EERROR_SOCKET_LISTEN,
  EERROR_SOCKET_ACCEPT,
  EERROR_SOCKET_SEND,
  EERROR_CLIENT_RECEIVE,
  EERROR_SERVER_SELECT,
  EERROR_INVALID_PACKET,
  EERROR_SOCKET_CLOSE,
  EERROR_SOCKET_SHUTDOWN,
  EERROR_WSA_STARTUP,
  EERROR_PACKET_GENERATOR,
  EERROR_SERVER_ADD,
  EERROR_SELECTOR_LAUNCH,
  EERROR_COUNT
};

static EErrorCode	    _lastError = EERROR_NONE;

#define EReturn(errorCode)									\
{												\
  _lastError = errorCode;									\
  return (errorCode);										\
}

#define EReturnValue(errorCode, value)								\
{												\
  _lastError = errorCode;									\
  return (value);										\
}

#define EThrowIfFailed(errorCode)								\
{												\
  if (errorCode)										\
    throw (EException(static_cast<EErrorCode>(errorCode), __FUNCTION__, __FILE__, __LINE__));	\
}

class			    EException
{
public:
  EException() = default;
  EException(EErrorCode errorCode, const std::string &functionName = "Undefined", const std::string &fileName = "Undefined", int lineNumber = 0);
  std::string		    toString() const;
  void			    print() const;
  void			    printBox() const;

private:
  EErrorCode		    m_errorCode;
  std::string		    m_functionName, m_fileName, m_additionnalInfos;
  int			    m_lineNumber;
};