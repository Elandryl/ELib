/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ESQLResult.
*/

#pragma once

#include "EGlobals/EGlobal.h"
#include "ESQL/ESQLRow.h"

/**
  @brief General scope for ELib components.
*/
namespace                         ELib
{

  /**
    @brief Container for ESQLField information.
  */
  struct                          ESQLFieldInfo
  {
    std::string                   m_name; /**< Name of the ESQLField. */
    uint8                         m_type; /**< Type of the ESQLField. */
    bool                          m_sign; /**< Indicate for numeric type if it is signed. */
  };

  /**
    @brief ELib object for SQL result handling and container.
    @details Handle and contain a SQL result as list of ESQLRow.
  */
  class                           ESQLResult
  {
  public:
    ESQLResult(void *p_mysql, void *p_sqlRes);
    virtual ~ESQLResult();
    ESQLRow                       *at(uint32 p_row);
    ESQLRow                       *operator[](uint32 p_row);
    ESQLField                     *at(uint32 p_row, uint32 p_field);
    uint32                        getSize() const;
  
  protected:
    void                          *m_mysql; /**< Handle for mysql connection. */
    std::vector<ESQLRow*>         m_rows;   /**< List of ESQLRow in the ESQLResult. */
  };

  /**
    @brief ELib object for SQL result handling and container with indexation.
    @details Handle and contain a SQL result as list of ESQLRow with indexation.
    @details Indexation is done with ESQLFieldInfo structure list.
    @details Inherit from ESQLResult.
  */
  class                           ESQLResultIndexed : public ESQLResult
  {
  public:
    ESQLResultIndexed(void *p_mysql, void *p_sqlRes);
    ~ESQLResultIndexed();
    ESQLField                     *at(uint32 p_row, const std::string &p_fieldName);
    void                          rowAsBuffer(uint32 p_row, char *p_buffer);

  private:
    std::vector<ESQLFieldInfo*>   m_fieldInfos;  /**< List of ESQLFieldInfo for indexation. */
  };

}
