#pragma once
#include <SFML/Network.hpp>
#include <unordered_map>
#include <iostream>
#include <sstream>
#include "Global.h"

#include "Data.h"

/*
主机端:
生成ai时通知所有客机 在哪生成的
生成资源时通知所有客机 在哪生成的
资源/玩家/ai被击杀时通知所有客机
每10秒与所有客机同步所有资源ai和玩家的位置 - 大概不需要同步资源

客机端:
不负责生成ai
不负责生成资源
本地演算移动,但实际以主机为准
不负责碰撞检测
每0.5s通知所有主机和客机 自己的位置
每次开火通知所有主机和客机 子弹发射的方向
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

	bool tryConnect(std::string typedText); // 尝试连接到某一ip
	bool addConnection(const sf::IpAddress& ipAddress, unsigned short port); // 客机加入主机, 向主机发送一个我要来的指令, 等待接收主机的初始化信号
	bool isHost() { return localIsHost; }

private:
	bool localIsHost = true;
	float syncSinceLastSend = 0.0f;
	sf::UdpSocket socket;
	std::vector<std::pair<sf::IpAddress, unsigned short>> connectedPeers;
};
