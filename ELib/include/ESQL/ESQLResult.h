#pragma once

#include "ESQL/ESQLRow.h"

namespace		    ELib
{
  /* ESQLResult */

  class			    ESQLResult
  {
  public:
    ESQLResult(void *sqlres);
    virtual ~ESQLResult();
    virtual ESQLRow	    *at(uint32 row);
    virtual ESQLRow	    *operator[](uint32 row);
    virtual ESQLField	    *at(uint32 row, uint32 column);
    virtual void	    print() const;
  
  protected:
    std::vector<ESQLRow*>   m_rows;
    void		    *m_sqlres;
  };

  /* ESQLIndexedResult */

  class			    ESQLIndexedResult : public ESQLResult
  {
  public:
    ESQLIndexedResult(void *sqlres);
    ~ESQLIndexedResult();
    using ESQLResult::at;
    virtual ESQLField	    *at(uint32 row, const char *columnName);
    void		    rowAsObject(uint32 row, char *objectAddr);

    template <typename T>
    bool		    rowAsObject(uint32 row, T *objectAddr)
    {
      if (alignof(T) != 1)
	return (false);
      rowAsObject(row, reinterpret_cast<char*>(objectAddr));
      return (true);
    }

    template<typename T>
    void		    assignDataToObject(char *&objectAddr, ESQLField *field)
    {
      *reinterpret_cast<T*>(objectAddr) = *field;
      objectAddr += sizeof(T);
    }

    void		    print() const;
  
  private:
    struct		    FieldInfo
    {
      char		    *name;
      uint8		    type;
      bool		    sign;
    };

    std::vector<FieldInfo>	m_columnInfos;
    uint8			pack = 0, count = 0;
  };
}