#pragma once
#include <SFML/Network.hpp>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "Global.h"

#include "Data.h"

/*
������:
����aiʱ֪ͨ���пͻ� �������ɵ�
������Դʱ֪ͨ���пͻ� �������ɵ�
��Դ/���/ai����ɱʱ֪ͨ���пͻ�
ÿ10�������пͻ�ͬ��������Դai����ҵ�λ�� - ��Ų���Ҫͬ����Դ

�ͻ���:
����������ai
������������Դ
���������ƶ�,��ʵ��������Ϊ׼
��������ײ���
ÿ0.5s֪ͨ���������Ϳͻ� �Լ���λ��
ÿ�ο���֪ͨ���������Ϳͻ� �ӵ�����ķ���
*/
enum class PacketType;
class NetworkManager {
public:
	NetworkManager() {
		if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
			std::cout << "Error binding socket to any port!" << std::endl;
		}
		socket.setBlocking(false);
		std::cout << "Local IP address: " << sf::IpAddress::getLocalAddress() << ":" << socket.getLocalPort() << std::endl;
	}

	void initObject(Object* object, PacketType type, sf::IpAddress sender, unsigned short port);
	void update();
	void sendToAllPlayers(sf::Packet packet);
	void sendPlayerPositionUpdate();
	void sendPlayerFireEvent(point dir);
	void sendAIStatus(AIPlayer* object);
	void sendObjectDie(Object* object);
	void sendPlayerResource(point pos);

	bool tryConnect(std::string typedText); // �������ӵ�ĳһip
	bool addConnection(const sf::IpAddress& ipAddress, unsigned short port); // �ͻ���������, ����������һ����Ҫ����ָ��, �ȴ����������ĳ�ʼ���ź�
	bool isHost() { return localIsHost; }

private:
	bool localIsHost = true;
	float syncSinceLastSend = 0.0f;
	sf::UdpSocket socket;
	std::vector<std::pair<sf::IpAddress, unsigned short>> connectedPeers;
};
