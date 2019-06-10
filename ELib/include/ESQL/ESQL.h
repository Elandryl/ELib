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
    @brief ELib object for MYSQL handling (Singleton).
  */
  class           ESQL
  {
  public:
    ~ESQL();                                                                                        /**< /!\ .... */
    static ESQL   *getInstance();                                                                   /**< /!\ ..E. */
    void          connect(const std::string &p_hostname, uint16 p_port, const std::string &p_user,  /**< /!\ ..E. */
                    const std::string &p_password, const std::string &p_database);
    void          query(const std::string &p_query);                                                /**< /!\ ..E. */
    ESQLResult    *getResult(bool p_indexed = false);                                               /**< /!\ ..E. */
    void          close();                                                                          /**< /!\ .... */
  
  private:
    ESQL();

    void          *m_mysql;       /**< Handle for MYSQL connection handler. */
    bool          m_isConnected;  /**< Indicate if ESQL is connected. */
  };

}
