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
    @brief ELib object for SQL row container.
    @details ESQLRow is a list of ESQLFields for storing a SQL row result.
  */
  class                       ESQLRow
  {
  public:
    ESQLRow();
    ~ESQLRow();
    void                      push(char *p_datas);
    void                      push(ESQLField *p_field);
    ESQLField                 *at(uint32 p_field);
    ESQLField                 *operator[](uint32 p_field);
    uint32                    getSize() const;
    char                      **toCharArray();
  
  private:
    std::vector<ESQLField*>   m_fields; /**< ESQLRow contents as ESQLFields. */
  };

}
