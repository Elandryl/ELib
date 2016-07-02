#pragma once

#include "ESQL/ESQLResult.h"

namespace	  ELib
{
  class		  ESQL
  {
  public:
    ESQL();
    ~ESQL();
    bool	  connect(const char *hostname, const char *user, const char *password, const char *database, uint16 port);
    bool	  query(const char *query, unsigned long length = 0); //Indicate length for better performences or binary query
    ESQLResult	  *getResult(bool indexed = false);
  
  private:
    void	  *m_mysql;
  };
}