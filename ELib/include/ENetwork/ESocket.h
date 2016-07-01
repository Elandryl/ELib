#pragma once
#pragma once

#include <Windows.h>
#include "ScaledTypes.h"

namespace ELib
{
	class						ESocket
	{
	public:
		enum					SocketStatus
		{
			STATUS_NONE			= 0,
			STATUS_CREATED		= 1,
			STATUS_CONNECTED	= 2,
			STATUS_BINDED		= 3,
			STATUS_ERROR		= 4
		};

		ESocket();
		ESocket(SOCKET&, SOCKADDR_IN&);
		~ESocket();
		SOCKET					getSocket() const;
		SocketStatus			getStatus() const;
		bool					socket();
		bool					connect(const char *hostname, uint16);
		bool					bind(const char *hostname, uint16 port);
		bool					listen();
		ESocket					*accept();
		bool					send(char *data, uint16 length, uint8 flags = 0);
		int32					recv(char *data, uint16 length, uint8 flags = 0);
		bool					close();

	private:
		ESocket(SocketStatus);
		SOCKET					m_socket;
		SOCKADDR_IN				m_info;
		SocketStatus			m_status;
	};
}