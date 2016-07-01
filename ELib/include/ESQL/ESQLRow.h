#pragma once

#include <vector>
#include "ESQL/ESQLField.h"

namespace							ELib
{
	class							ESQLRow
	{
	public:
		ESQLRow();
		~ESQLRow();
		void				push(char *data);
		void				push(ESQLField *field);
		ESQLField			*at(uint32 column);
		ESQLField			*operator[](uint32 column);
		char				**toCharArray();
		void				print() const;

	private:
		std::vector<ESQLField*>		m_fields;
	};
}