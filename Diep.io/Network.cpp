#include "Network.h"
#include "Object.h"
#include "Bullet.h"
#include "AIPlayer.h"
#include "Packet.h"

template<class T>
static auto find_if_id(std::vector<T>& vec, int id) {
	return std::find_if(vec.begin(), vec.end(),
		[=](T p) { return p->getID() == id; });
}

void NetworkManager::initObject(Object* object, PacketType type, sf::IpAddress sender, unsigned short port) {
	sf::Packet packet;
	packet << type;
	packet << InitSyncData{ object->getID(),
		object->getPosition().x,object->getPosition().y, object->getTurretRotation(),
		object->getVelocity().x,object->getVelocity().y,
		object->getAcceleration().x,object->getAcceleration().y };

	socket.send(packet, sender, port);
}

void receivedObjDie(ObjectType objType, int id) {
	switch (objType) {
	case ObjectType::Object: {
		auto it = find_if_id(objects, id);
		if (it != objects.end())
			objects.erase(it);
		break;
	}
	case ObjectType::Bullet: {
		auto it = find_if_id(bullets, id);
		if (it != bullets.end())
			bullets.erase(it);
		break;
	}
	case ObjectType::Player: {
		auto it = find_if_id(players, id);
		if (it != players.end())
			players.erase(it);
		break;
	}
	case ObjectType::AIPlayer: {
		auto it = find_if_id(enemies, id);
		if (it != enemies.end())
			enemies.erase(it);
		break;
	}
	default:
		break;
	}
}

void NetworkManager::update() {
	syncSinceLastSend += Global::deltaTime;
	sf::IpAddress sender;
	unsigned short port;
	sf::Packet packet;

	while (socket.receive(packet, sender, port) == sf::Socket::Done) {
		// 处理接收到的数据
		// 假设服务器发送了一个包含其他玩家位置的sf::Packet
		PacketType packetType;
		packet >> packetType;
		std::cout << "Message received: " << static_cast<char>(packetType) << ": ";
		switch (packetType) {
		case PacketType::PlayerJoined: {
			for (auto& [addr, p] : connectedPeers) {
				if (addr == sender && p == port) {
					std::cout << "Player Has Joined, Don't join again" << std::endl;
					break;
				}
			}
			std::cout << "Player Joined: " << sender << ":" << port << std::endl;

			connectedPeers.emplace_back(sender, port);
			// 有人加入, 给他分配id
			auto newPlayer = new Player(15.f, sf::Color::Blue, point(450, 350), 1000, 1);
			players.push_back(newPlayer);

			sf::Packet replyPosition;
			replyPosition << PacketType::PlayerIDAssign;
			replyPosition << PlayerIDAssignData{ newPlayer->getID(), newPlayer->getPosition().x, newPlayer->getPosition().y, newPlayer->getTurretRotation() };
			socket.send(replyPosition, sender, port);

			std::cout << "Send: Player Position" << std::endl;

			// 同时把所有object的Velocity Acceleration currentHealth
			initObject(player, PacketType::SyncPlayerStatus, sender, port);
			for (auto resource : objects)
				initObject(resource, PacketType::SyncResourceStatus, sender, port);
			for (auto enemy : enemies)
				initObject(enemy, PacketType::SyncEnemyStatus, sender, port);
			for (auto enemy : players)
				initObject(enemy, PacketType::SyncPlayerStatus, sender, port);
			for (auto bullet : bullets) {
				sf::Packet packet;
				packet << PacketType::SyncBulletStatus;
				packet << InitBulletSyncData{ bullet->getID(),
					bullet->getPosition().x,bullet->getPosition().y,
					bullet->getVelocity().x,bullet->getVelocity().y,
					bullet->getAcceleration().x,bullet->getAcceleration().y,
				bullet->isAI(),bullet->getOwner()->getID() };

				socket.send(packet, sender, port);
			}

			break;
		}
		case PacketType::PlayerIDAssign: {
			std::cout << "Player ID Assign" << std::endl;

			PlayerIDAssignData id;
			packet >> id;

			// 接收服务器分配的id
			player->setID(id.id);
			player->setPosition(id.x, id.y);
			player->setTurretRotation(id.turretRotation);
			connectedPeers.emplace_back(sender, port);
			localIsHost = false;

			objects.clear();
			bullets.clear();
			enemies.clear();

			break;
		}
		case PacketType::SyncResourceStatus: {
			std::cout << "Sync Resource Status" << std::endl;

			InitSyncData initData;
			packet >> initData;

			auto resource = new Object(10.f, sf::Color::Green, point(initData.x, initData.y), 100);
			resource->setID(initData.id);
			resource->setVelocity(point(initData.vx, initData.vy));
			resource->setAcceleration(point(initData.ax, initData.ay));
			objects.push_back(resource);

			break;
		}
		case PacketType::SyncPlayerStatus: {
			std::cout << "Sync Player Status" << std::endl;

			InitSyncData initData;
			packet >> initData;

			auto enemy = new Player(15.f, blueTeamColor, point(initData.x, initData.y), 1000, 1);
			enemy->setID(initData.id);
			enemy->setVelocity(point(initData.vx, initData.vy));
			enemy->setAcceleration(point(initData.ax, initData.ay));
			enemy->setTurretRotation(initData.turretRotation);
			players.push_back(enemy);

			break;
		}
		case PacketType::SyncEnemyStatus: {
			std::cout << "Sync Enemy Status" << std::endl;

			InitSyncData initData;
			packet >> initData;

			auto enemy = new AIPlayer(15.f, redTeamColor, point(initData.x, initData.y), 100, 1);
			enemy->setID(initData.id);
			enemy->setVelocity(point(initData.vx, initData.vy));
			enemy->setAcceleration(point(initData.ax, initData.ay));
			enemy->setTurretRotation(initData.turretRotation);
			enemies.push_back(enemy);

			break;
		}
		case PacketType::SyncBulletStatus: {
			std::cout << "Sync Bullet Status" << std::endl;

			InitBulletSyncData initData;
			packet >> initData;

			if (initData.isAI) {
				auto it = find_if_id(enemies, initData.id);
				if (it != enemies.end()) {
					auto bullet = new Bullet(*it, point(initData.x, initData.y));
					bullet->setVelocity(point(initData.vx, initData.vy));
					bullet->setAcceleration(point(initData.ax, initData.ay));
					bullets.push_back(bullet);
				}
			}
			else {
				auto it = find_if_id(players, initData.id);
				if (it != players.end()) {
					auto bullet = new Bullet(*it, point(initData.x, initData.y));
					bullet->setVelocity(point(initData.vx, initData.vy));
					bullet->setAcceleration(point(initData.ax, initData.ay));
					bullets.push_back(bullet);
				}
			}

			break;
		}
		case PacketType::PlayerPositionUpdate: {
			// std::cout << "Player Position Update" << std::endl;

			InitSyncData positionData;
			packet >> positionData;

			// 同步该玩家的位置
			auto it = find_if_id(players, positionData.id);
			if (it != players.end()) {
				(*it)->setPosition(positionData.x, positionData.y);
				(*it)->setVelocity(point(positionData.vx, positionData.vy));
				(*it)->setAcceleration(point(positionData.ax, positionData.ay));
				(*it)->setTurretRotation(positionData.turretRotation);
			}
			break;
		}
		case PacketType::PlayerFireEvent: {
			FireEventData fireEventData;
			packet >> fireEventData;

			// 在该玩家处产生炮弹
			// 同步该玩家的位置
			auto it = find_if_id(players, fireEventData.id);
			if (it != players.end()) {
				point target{ fireEventData.targetX, fireEventData.targetY };
				(*it)->fire(target);
			}
			break;
		}
		case PacketType::EnemyPositionUpdate: {
			// std::cout << "Player Position Update" << std::endl;

			InitSyncData positionData;
			packet >> positionData;

			// 同步该玩家的位置
			auto it = find_if_id(enemies, positionData.id);
			if (it != enemies.end()) {
				(*it)->setPosition(positionData.x, positionData.y);
				(*it)->setVelocity(point(positionData.vx, positionData.vy));
				(*it)->setAcceleration(point(positionData.ax, positionData.ay));
				(*it)->setTurretRotation(positionData.turretRotation);
			}
			break;
		}
		case PacketType::ObjectKilled: {
			ObjectDieData objectDieData;
			packet >> objectDieData;
			receivedObjDie(objectDieData.objType, objectDieData.id);
			break;
		}
		default:
			std::cout << "Unknown packet type received" << std::endl;
			break;
		}
	}
}

void NetworkManager::sendToAllPlayers(sf::Packet packet) {
	for (auto& [ip, port] : connectedPeers) {
		socket.send(packet, ip, port);
	}
}

void NetworkManager::sendPlayerPositionUpdate() {
	if (syncSinceLastSend > 0.1f) {
		syncSinceLastSend = 0.f;

		sf::Packet packet;
		packet << PacketType::PlayerPositionUpdate;
		packet << InitSyncData{ player->getID(),
			player->getPosition().x,player->getPosition().y,player->getTurretRotation(),
			player->getVelocity().x,player->getVelocity().y,
			player->getAcceleration().x,player->getAcceleration().y };
		this->sendToAllPlayers(packet);

		// std::cout << "Send: Player Position" << std::endl;
	}
}

void NetworkManager::sendPlayerFireEvent(point dir) {
	sf::Packet packet;
	packet << PacketType::PlayerFireEvent;
	packet << FireEventData{ player->getID(), dir.x, dir.y };
	this->sendToAllPlayers(packet);

	// std::cout << "Send: Player Fire Event" << std::endl;
}

void NetworkManager::sendAIStatus(AIPlayer* object) {
	sf::Packet packet;
	packet << PacketType::EnemyPositionUpdate;
	packet << InitSyncData{ object->getID(),
		object->getPosition().x,object->getPosition().y, object->getTurretRotation(),
		object->getVelocity().x,object->getVelocity().y,
		object->getAcceleration().x,object->getAcceleration().y };
	this->sendToAllPlayers(packet);
}

void NetworkManager::sendObjectDie(Object* object) {
	sf::Packet packet;
	packet << PacketType::ObjectKilled;
	packet << ObjectDieData{ object->getID(),object->WhatAmI() };
	this->sendToAllPlayers(packet);
}

void NetworkManager::sendPlayerResource(point pos) {
	sf::Packet packet;
	packet << PacketType::SyncResourceStatus;
	packet << InitSyncData{ player->getID(),
		pos.x,pos.y,0,0,0,0,0 };
	this->sendToAllPlayers(packet);

	// std::cout << "Send: Player Resource Created" << std::endl;
}

// 尝试连接到某一ip
bool NetworkManager::tryConnect(std::string typedText) {
	std::string ip;
	std::string port;

	// 解析 IP 地址和端口号
	std::stringstream ss(typedText);
	std::getline(ss, ip, ':');
	std::getline(ss, port, ':');

	sf::IpAddress ipAddress(ip);

	std::cout << "Try connect to ip:" << ip << std::endl;

	bool success = false;
	short parsedPort = port.empty() ? 0 : static_cast<unsigned short>(std::stoi(port));
	return addConnection(ipAddress, parsedPort);
}

// 客机加入主机, 向主机发送一个我要来的指令, 等待接收主机的初始化信号
bool NetworkManager::addConnection(const sf::IpAddress& ipAddress, unsigned short port) {
	for (auto& [addr, p] : connectedPeers) {
		if (addr == ipAddress && p == port)
			return false;
	}

	sf::Packet packet;
	packet << PacketType::PlayerJoined;
	sf::Socket::Status status = socket.send(packet, ipAddress, port);

	// 检查发送状态
	bool success = status == sf::Socket::Done;
	if (success)
		std::cout << "Connection successful with " << ipAddress << ":" << port << std::endl;
	else
		std::cout << "Error sending packet to " << ipAddress << ":" << port << std::endl;

	return success;
}
