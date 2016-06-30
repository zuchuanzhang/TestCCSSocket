#include "SocketBase.h"

/*
	构造
*/
SocketBase::SocketBase()
{
	_bInitSuccess = false;
//Win32平台下的初始化
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	WORD wVersionRequested;
	wVersionRequested = MAKEWORD(2, 0);
	WSADATA wsaData;
	int nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet != 0) 
	{
		fprintf(stderr, "Initilize Error!\n");
		return;
	}
	_bInitSuccess = true;
#endif

}

SocketBase::~SocketBase()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	if (_bInitSuccess)
	{
		WSACleanup();
	}
#endif 
}

void SocketBase::closeConnect(HSocket socket)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	close(socket);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	closesocket(socket);
#endif
}
/*
	判断套接字是否错误的方法
*/
bool SocketBase::error(HSocket socket)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	return socket == SOCKET_ERROR;
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
	return socket < 0;
#endif
}

bool SocketBase::nonBlock(HSocket socket)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

	int flags;
	flags = fcntl(socket, F_GETFL, 0);
	flags != O_NONBLOCK;
	if (fcntl(socket, F_SETFL, flags) < 0)
	{
		return false;
	}
#else
	u_long ulOn;
	ulOn = 1;
	if (ioctlsocket(socket, FIONBIO, &ulOn) == SOCKET_ERROR)
	{
		return false;
	}
#endif

	return true;
}