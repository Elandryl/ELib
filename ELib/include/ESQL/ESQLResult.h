//#pragma once
//
//#include "ESQL/ESQLRow.h"
//
//namespace                   ELib
//{
//
//  class                     ESQLResult
//  {
//  public:
//    ESQLResult(void *sqlres);
//    virtual ~ESQLResult();
//    virtual ESQLRow         *at(uint32 row);
//    virtual ESQLRow         *operator[](uint32 row);
//    virtual ESQLField       *at(uint32 row, uint32 column);
//    virtual void            print() const;
//  
//  protected:
//    std::vector<ESQLRow*>   m_rows;
//    void                    *m_sqlres;
//  };
//
//  class                     ESQLIndexedResult : public ESQLResult
//  {
//  public:
//    ESQLIndexedResult(void *sqlres);
//    ~ESQLIndexedResult();
//    template <typename T>
//    EErrorCode              rowAsObject(uint32 row, T *objectAddr)
//    {
//      if (alignof(T) != 1)
//        EReturn(EERROR_WRONG_PADDING);
//      rowAsPaddedObject(row, reinterpret_cast<char*>(objectAddr));
//      return (EERROR_NONE);
//    }
//    using ESQLResult::at;
//    virtual ESQLField       *at(uint32 row, const char *columnName);
//    void                    print() const;
//  
//  private:
//    void                    rowAsPaddedObject(uint32 row, char *objectAddr);
//    template<typename T>
//    void                    assignDataToObject(char *&objectAddr, ESQLField *field)
//    {
//      *reinterpret_cast<T*>(objectAddr) = *field;
//      objectAddr += sizeof(T);
//    }
//
//    struct                  FieldInfo
//    {
//      char                  *name;
//      uint8                 type;
//      bool                  sign;
//    };
//
//    std::vector<FieldInfo>  m_columnInfos;
//  };
//
//}
