#pragma once

#include "ESQL/ESQLRow.h"

#define _aligned(value) (value % 4 == 0 ? value : (value/ 4 + 1) * 4)

namespace						ELib
{
	class						ESQLResult
	{
	public:
		ESQLResult(void *sqlres);
		virtual ~ESQLResult();
		virtual ESQLRow			*at(uint32 row);
		virtual ESQLRow			*operator[](uint32 row);
		virtual ESQLField		*at(uint32 row, uint32 column);
		virtual void			print() const;

	protected:
		std::vector<ESQLRow*>	m_rows;
		void					*m_sqlres;
	};

	class						ESQLIndexedResult : public ESQLResult
	{
	public:
		ESQLIndexedResult(void *sqlres);
		~ESQLIndexedResult();
		virtual ESQLField		*at(uint32 row, const char *columnName);
		void					rowAsObject(uint32 row, char *objectAddr);
		void					print() const;

		template<typename T>
		void					assignDataToObject(char *&objectAddr, ESQLField *field)
		{
			if (sizeof(T) == pack)
				count++;
			else
				if (sizeof(T) != _aligned(sizeof(T)) && sizeof(T) != pack)
				{
					pack = sizeof(T);
					count = 1;
				}
				else
				{
					objectAddr += _aligned(pack * count) - pack * count;
					count = pack = 0;
				}
			*reinterpret_cast<T*>(objectAddr) = *field;
			objectAddr += sizeof(T);
		}

	private:
		struct					FieldInfo
		{
			char				*name;
			uint8				type;
			bool				sign;
		};
		
		std::vector<FieldInfo>	m_columnInfos;
		uint8					pack = 0, count = 0;
	};
}