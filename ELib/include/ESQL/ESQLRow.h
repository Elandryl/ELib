/**
  @author Elandryl (Christophe.M).
  @date 2019.
  @brief Header for ESQLRow.
*/

#pragma once

#include <vector>
#include "EGlobals/EGlobal.h"
#include "ESQL/ESQLField.h"

/**
  @brief General scope for ELib components.
*/
namespace                     ELib
{

  /**
    @brief ELib object for SQL row handling.
    @details Container for ESQLFields.
  */
  class                       ESQLRow
  {
  public:
    ESQLRow();                                              /**< /!\ .... */
    ~ESQLRow();                                             /**< /!\ .... */
    void                      push(ESQLField *p_field);     /**< /!\ ..E. */
    void                      push(char *p_datas);          /**< /!\ ..E. */
    ESQLField                 *at(uint32 p_field);          /**< /!\ ..E. */
    ESQLField                 *operator[](uint32 p_field);  /**< /!\ ..E. */
    uint32                    getSize() const;              /**< /!\ .... */
    char                      **toCharArray();              /**< /!\ ..E. */
  
  private:
    std::vector<ESQLField*>   m_fields; /**< ESQLFields list. */
  };

}
