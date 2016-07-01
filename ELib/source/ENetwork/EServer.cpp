#include "ENetwork/EServer.h"
#include <iostream>

namespace					ELib
{
	/* FUNCTORS */

	DWORD WINAPI			AcceptFunctor(LPVOID lpParam)
	{
		static_cast<EServer*>(lpParam)->accept();
		return (0);
	}

	DWORD WINAPI			SelectFunctor(LPVOID lpParam)
	{
		static_cast<EServer::Selector*>(lpParam)->select();
		return (0);
	}

	/* SERVER */

	EServer::EServer()
	{
		WSADATA				WSAData;

		if (WSAStartup(MAKEWORD(2, 2), &WSAData))
		{
			perror("error WSAStartup()");
			exit(EXIT_FAILURE);
		}
	}

	EServer::~EServer()
	{
		m_socketServer.close();
		for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end();)
		{
			delete(*it);
			it = m_selectors.erase(it);
		}
		WSACleanup();
	}

	bool					EServer::launch(const char *hostname, uint16 port)
	{
		m_socketServer.bind(hostname, port);
		m_socketServer.listen();
		m_isRunning = true;
		m_threadAccept = CreateThread(NULL, 0, AcceptFunctor, this, 0, NULL);
		std::cout << "Server launched : " << hostname << ":" << port << std::endl;
		return (true);
	}

	bool					EServer::accept()
	{
		ESocket				*client;

		while (m_isRunning)
			if (!(client = m_socketServer.accept())	|| !addSocket(client))
			{
				perror("error Accept()");
				exit(EXIT_FAILURE);
			}
		return (true);
	}

	bool					EServer::addSocket(ESocket *client)
	{
		bool		added = false;

		for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end();)
			if (!(*it)->isRunning())
			{
				delete(*it);
				it = m_selectors.erase(it);
			}
			else
			{
				if (!added && (*it)->addSocket(client))
					added = true;
				++it;
			}
		if (!added)
			m_selectors.push_back(new Selector(client));
		return (true);
	}

	bool					EServer::broadcast(char *data, uint16 length, uint8 flags)
	{
		for (std::vector<Selector*>::iterator it = m_selectors.begin(); it != m_selectors.end(); ++it)
			(*it)->broadcast(data, length, flags);
		return (true);
	}

	/* SELECTOR */

	EServer::Selector::Selector(ESocket *socket) :
		m_socketsClients{ socket },
		m_maxFd(socket->getSocket() + 1),
		m_running(true)
	{
		m_socketControll.socket();
		if (m_socketControll.getSocket() >= m_maxFd)
			m_maxFd = m_socketControll.getSocket() + 1;
		m_threadSelect = CreateThread(NULL, 0, SelectFunctor, this, 0, NULL);
	}

	EServer::Selector::~Selector()
	{
		m_socketControll.close();
		for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end();)
		{
			(*it)->close();
			delete(*it);
			it = m_socketsClients.erase(it);
		}
	}

	bool					EServer::Selector::select()
	{
		while (m_socketsClients.size())
		{
			FD_ZERO(&m_set);	
			FD_SET(m_socketControll.getSocket(), &m_set);
			for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
				FD_SET((*it)->getSocket(), &m_set);
			if (::select(m_maxFd, &m_set, NULL, NULL, NULL) < 0)
			{
				std::cout << "error Select" << WSAGetLastError() << std::endl;
				system("pause");
				exit(EXIT_FAILURE);
			}
			if (FD_ISSET(m_socketControll.getSocket(), &m_set))
				m_socketControll.socket();
			for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end();)
			{
				if (FD_ISSET((*it)->getSocket(), &m_set))
				{
					/* recv data */
					int		ret = 1;
					if (ret > 0)
					{
						/* add data to queue */
						++it;
					}
					else
						if (ret == 0)
						{
							perror("disconnected");
							delete(*it);
							it = m_socketsClients.erase(it);
						}
						else
						{
							std::cout << ret << " error " << WSAGetLastError() << std::endl;
							delete(*it);
							it = m_socketsClients.erase(it);
						}
				}
				else
					++it;
			}
		}
		m_running = false;
		return (false);
	}

	bool					EServer::Selector::broadcast(char *data, uint16 length, uint8 flags)
	{
		for (std::vector<ESocket*>::iterator it = m_socketsClients.begin(); it != m_socketsClients.end(); ++it)
			(*it)->send(data, length, flags);
		return (true);
	}

	bool					EServer::Selector::addSocket(ESocket *socket)
	{
		if (m_socketsClients.size() >= MAX_SOCKETS_PER_SET)
			return (false);
		m_socketsClients.push_back(socket);
		if (socket->getSocket() >= m_maxFd)
			m_maxFd = socket->getSocket() + 1;
		m_socketControll.close();
		return (true);
	}

	bool					EServer::Selector::isRunning() const
	{
		return (m_running);
	}
}