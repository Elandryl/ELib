/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ESQL, ELib SQL handling object.
*/

#pragma once

#include "EGlobals/EGlobal.h"
#include "ESQL/ESQLResult.h"

/**
  @brief General scope for ELib components.
*/
namespace         ELib
{

  /**
    @brief Instantiate a ESQL object for ELib SQL handling.
    @details ESQL object use mysql library.
  */
  class           ESQL
  {
  public:
    ESQL();
    ~ESQL();
    void          connect(const std::string &p_hostname, uint16 p_port, const std::string &p_user,
                    const std::string &p_password, const std::string &p_database);
    void          query(const std::string &p_query);
    ESQLResult    *getResult(bool p_indexed = false);
  
  private:
    void          *m_mysql;       /**< Handle for mysql connection. */
    bool          m_isConnected;  /**< Indicate if ESQL is connected. */
  };

}
