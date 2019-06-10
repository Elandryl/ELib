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
    @brief ELib struct for ESQLField information.
  */
  struct                          ESQLFieldInfo
  {
    std::string                   m_name; /**< ESQLField name. */
    uint8                         m_type; /**< ESQLField type. */
    bool                          m_sign; /**< Indicate for numeric type if it is signed. */
  };

  /**
    @brief ELib object for SQL result handling.
    @details Handle and store a SQL result as list of ESQLRow.
  */
  class                           ESQLResult
  {
  public:
    ESQLResult(void *p_mysql, void *p_sqlRes);                        /**< /!\ ..E. */
    virtual ~ESQLResult();                                            /**< /!\ .... */
    ESQLRow                       *at(uint32 p_row);                  /**< /!\ ..E. */
    ESQLRow                       *operator[](uint32 p_row);          /**< /!\ ..E. */
    ESQLField                     *at(uint32 p_row, uint32 p_field);  /**< /!\ ..E. */
    uint32                        getSize() const;                    /**< /!\ .... */
    void                          print() const;                      /**< /!\ .... */
  
  protected:
    void                          *m_mysql;   /**< Handle for mysql connection. */
    void                          *m_sqlRes;  /**< ESQLResult as mysql datas. */
    std::vector<ESQLRow*>         m_rows;     /**< List of ESQLRow in the ESQLResult. */
  };

  /**
    @brief ELib object for SQL result handling with indexation.
    @details Handle and store a SQL result as list of ESQLRow with indexation.
    @details Indexation is done with ESQLFieldInfo list.
  */
  class                           ESQLResultIndexed : public ESQLResult
  {
  public:
    ESQLResultIndexed(void *p_mysql, void *p_sqlRes);                                 /**< /!\ ..E. */
    ~ESQLResultIndexed();                                                             /**< /!\ .... */
    ESQLField                     *at(uint32 p_row, const std::string &p_fieldName);  /**< /!\ ..E. */
    void                          rowAsBuffer(uint32 p_row, char *p_buffer);          /**< /!\ ..E. */

  private:
    std::vector<ESQLFieldInfo*>   m_fieldInfos; /**< ESQLFieldInfo list. */
    bool                          m_isValid;    /**< State. */
  };

}
