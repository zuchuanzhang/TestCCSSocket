#ifndef __SOCKET_SERVER_H__
#define __SOCKET_SERVER_H__

#include "SocketBase.h"

//�������ݽṹ��װ
struct RecvData
{
	//�ͻ��˵��׽���
	HSocket socketClient;
	//���ݳ���
	int dataLen;
	//����Buffer
	char data[1024];
};

class SocketServer : public SocketBase
{
public:
	//����������
	static SocketServer* getInstance();
	//����ʵ��
	void destroyInstance();
	//�����������ķ���
	bool startServer(unsigned short port);
	//������Ϣ�ķ���
	void sendMessage(HSocket socket, const char* data, int count);
	//������Ϣ�ķ���
	void sendMessage(const char* data, int count);
	//ѭ�����÷���
	void update(float dt);

	//�����ص�����
	//��ʼ�Ļص�
	std::function<void(const char* ip)> onStart;
	//�����ӵĻص�
	std::function<void(HSocket socket)> onNewConnection;
	//�յ����ݵĻص�
	std::function<void(HSocket socket, const char* data, int count)> onRecv;
	//�Ͽ����ӵĻص�
	std::function<void(HSocket socket)> onDisconnect;

CC_CONSTRUCTOR_ACCESS:
	SocketServer();
	~SocketServer();

private:
	//������ݷ���
	void clear();
	//��ʼ������������
	bool initServer(unsigned short port);
	//���ܿͻ��˵ķ���
	void acceptClient();
	//���ܷ���
	void acceptFunc();
	//�¿ͻ������ӵķ���
	void newClientConnected(HSocket socket);
	//������Ϣ�ķ���
	void recvMessage(HSocket socket);
	
private:
	//����������ʵ��
	static SocketServer* s_server;
	//�������׽���
	HSocket _socketServer;
	//�������󶨵Ķ˿ں�
	unsigned short _serverPort;

private:
	//�ͻ��б�
	std::list<HSocket> _clientSockets;
	//��Ϣ����
	std::list<SocketMessage*> _UIMessageQueue;
	//��Ϣ���л�����
	std::mutex   _UIMessageQueueMutex;

};

#endif