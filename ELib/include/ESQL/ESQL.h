//#pragma once
//
//#include "ESQL/ESQLResult.h"
//
//namespace         ELib
//{
//
//  class           ESQL
//  {
//  public:
//    ESQL();
//    ~ESQL();
//    EErrorCode    connect(const char *hostname, const char *user, const char *password, const char *database, uint16 port);
//    EErrorCode    query(const char *query, unsigned long length = 0);
//    ESQLResult    *getResult(bool indexed = false);
//  
//  private:
//    void          *m_mysql;
//  };
//
//}
