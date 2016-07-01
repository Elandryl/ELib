#pragma once

#include <vector>
#include <queue>
#include "ENetwork/ESocket.h"

#define MAX_SOCKETS_PER_SET			63

namespace							ELib
{
	class							EServer
	{
	public:
		class						Selector
		{
		public:
			Selector(ESocket *socket);
			~Selector();
			bool					select();
			bool					broadcast(char *data, uint16 length, uint8 flags = 0);
			bool					addSocket(ESocket *socket);
			bool					isRunning() const;

		private:
			fd_set					m_set;
			ESocket					m_socketControll;
			std::vector<ESocket*>	m_socketsClients;
			unsigned int			m_maxFd;
			HANDLE					m_threadSelect;
			bool					m_running;
		};

		EServer();
		~EServer();
		bool						launch(const char *hostname, uint16 port);
		bool						accept();
		bool						broadcast(char *data, uint16 length, uint8 flags = 0);

	private:
		bool						addSocket(ESocket *client);

		ESocket						m_socketServer;
		HANDLE						m_threadAccept;
		std::vector<Selector*>		m_selectors;
		bool						m_isRunning;
	};
}