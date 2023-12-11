#pragma once
#include <SFML/Network.hpp>
#include <unordered_map>
#include <iostream>
#include "Packet.h"

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

class NetworkManager {
public:
	NetworkManager() {
		if (socket.bind(sf::Socket::AnyPort) != sf::Socket::Done) {
			std::cout << "Error binding socket to any port!" << std::endl;
		}
		socket.setBlocking(false);
		std::cout << "Local IP address: " << sf::IpAddress::getLocalAddress() << ":" << socket.getLocalPort() << std::endl;
	}

	void initObject(Object object, PacketType type, sf::IpAddress sender, unsigned short port) {
		sf::Packet packet;
		packet << type;
		packet << InitSyncData{ object.getID(),
			object.getPosition().x,object.getPosition().y, object.getTurretRotation(),
			object.getVelocity().x,object.getVelocity().y,
			object.getAcceleration().x,object.getAcceleration().y };

		socket.send(packet, sender, port);
	}

	void update() {
		syncSinceLastSend += Global::deltaTime;
		sf::IpAddress sender;
		unsigned short port;
		sf::Packet packet;

		while (socket.receive(packet, sender, port) == sf::Socket::Done) {
			// ������յ�������
			// ���������������һ�������������λ�õ�sf::Packet
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
				// ���˼���, ��������id
				Player newPlayer(15.f, sf::Color::Blue, sf::Vector2f(400, 300), 1000);
				//newPlayer.randomAddToMap();
				newPlayer.setPosition(450, 350);
				players.push_back(newPlayer);

				sf::Packet replyPosition;
				replyPosition << PacketType::PlayerIDAssign;
				replyPosition << PlayerIDAssignData{ newPlayer.getID(), newPlayer.getPosition().x, newPlayer.getPosition().y, newPlayer.getTurretRotation() };
				socket.send(replyPosition, sender, port);

				std::cout << "Send: Player Position" << std::endl;

				// ͬʱ������object��Velocity Acceleration currentHealth
				initObject(player, PacketType::SyncPlayerStatus, sender, port);
				for (auto resource : resources)
					initObject(resource, PacketType::SyncResourceStatus, sender, port);
				for (auto enemy : enemies)
					initObject(enemy, PacketType::SyncEnemyStatus, sender, port);
				for (auto enemy : players)
					initObject(enemy, PacketType::SyncPlayerStatus, sender, port);
				for (auto bullet : bullets) {
					sf::Packet packet;
					packet << PacketType::SyncBulletStatus;
					packet << InitBulletSyncData{ bullet.getID(),
						bullet.getPosition().x,bullet.getPosition().y,
						bullet.getVelocity().x,bullet.getVelocity().y,
						bullet.getAcceleration().x,bullet.getAcceleration().y,
					bullet.isAI(),bullet.getOwner()->getID() };

					socket.send(packet, sender, port);
				}

				break;
			}
			case PacketType::PlayerIDAssign: {
				std::cout << "Player ID Assign" << std::endl;

				PlayerIDAssignData id;
				packet >> id;

				// ���շ����������id
				player.setID(id.id);
				player.setPosition(id.x, id.y);
				player.setTurretRotation(id.turretRotation);
				connectedPeers.emplace_back(sender, port);
				localIsHost = false;

				resources.clear();
				bullets.clear();
				enemies.clear();

				break;
			}
			case PacketType::SyncResourceStatus: {
				std::cout << "Sync Resource Status" << std::endl;

				InitSyncData initData;
				packet >> initData;

				Object resource(10.f, sf::Color::Green, sf::Vector2f(initData.x, initData.y), 100);
				resource.setID(initData.id);
				resource.setVelocity(sf::Vector2f(initData.vx, initData.vy));
				resource.setAcceleration(sf::Vector2f(initData.ax, initData.ay));
				resources.push_back(resource);

				break;
			}
			case PacketType::SyncPlayerStatus: {
				std::cout << "Sync Player Status" << std::endl;

				InitSyncData initData;
				packet >> initData;

				Player enemy(15.f, sf::Color::Blue, sf::Vector2f(initData.x, initData.y), 1000);
				enemy.setID(initData.id);
				enemy.setVelocity(sf::Vector2f(initData.vx, initData.vy));
				enemy.setAcceleration(sf::Vector2f(initData.ax, initData.ay));
				enemy.setTurretRotation(initData.turretRotation);
				players.push_back(enemy);

				break;
			}
			case PacketType::SyncEnemyStatus: {
				std::cout << "Sync Enemy Status" << std::endl;

				InitSyncData initData;
				packet >> initData;

				AIPlayer enemy(15.f, sf::Color::Red, sf::Vector2f(initData.x, initData.y), 100);
				enemy.setID(initData.id);
				enemy.setVelocity(sf::Vector2f(initData.vx, initData.vy));
				enemy.setAcceleration(sf::Vector2f(initData.ax, initData.ay));
				enemy.setTurretRotation(initData.turretRotation);
				enemies.push_back(enemy);

				break;
			}
			case PacketType::SyncBulletStatus: {
				std::cout << "Sync Bullet Status" << std::endl;

				InitBulletSyncData initData;
				packet >> initData;

				if (initData.isAI) {
					auto it = std::find_if(enemies.begin(), enemies.end(),
						[=](AIPlayer& p) { return p.getID() == initData.id; });

					if (it != enemies.end()) {
						Bullet bullet(&*it, sf::Vector2f(initData.x, initData.y));
						bullet.setVelocity(sf::Vector2f(initData.vx, initData.vy));
						bullet.setAcceleration(sf::Vector2f(initData.ax, initData.ay));
						bullets.push_back(bullet);
					}
				}
				else {
					auto it = std::find_if(players.begin(), players.end(),
						[=](Player& p) { return p.getID() == initData.id; });

					if (it != players.end()) {
						Bullet bullet(&*it, sf::Vector2f(initData.x, initData.y));
						bullet.setVelocity(sf::Vector2f(initData.vx, initData.vy));
						bullet.setAcceleration(sf::Vector2f(initData.ax, initData.ay));
						bullets.push_back(bullet);
					}
				}

				break;
			}
			case PacketType::PlayerPositionUpdate: {
				// std::cout << "Player Position Update" << std::endl;

				InitSyncData positionData;
				packet >> positionData;

				// ͬ������ҵ�λ��
				auto it = std::find_if(players.begin(), players.end(),
					[=](Player& p) { return p.getID() == positionData.id; });

				if (it != players.end()) {
					it->setPosition(positionData.x, positionData.y);
					it->setVelocity(sf::Vector2f(positionData.vx, positionData.vy));
					it->setAcceleration(sf::Vector2f(positionData.ax, positionData.ay));
					it->setTurretRotation(positionData.turretRotation);
				}
				break;
			}
			case PacketType::PlayerFireEvent: {
				FireEventData fireEventData;
				packet >> fireEventData;

				// �ڸ���Ҵ������ڵ�
				// ͬ������ҵ�λ��
				auto it = std::find_if(players.begin(), players.end(),
					[=](Player& p) { return p.getID() == fireEventData.id; });

				if (it != players.end()) {
					sf::Vector2f target{ fireEventData.targetX, fireEventData.targetY };
					it->fire(target);
				}
				break;
			}
			case PacketType::EnemyPositionUpdate: {
				// std::cout << "Player Position Update" << std::endl;

				InitSyncData positionData;
				packet >> positionData;

				// ͬ������ҵ�λ��
				auto it = std::find_if(enemies.begin(), enemies.end(),
					[=](AIPlayer& p) { return p.getID() == positionData.id; });

				if (it != enemies.end()) {
					it->setPosition(positionData.x, positionData.y);
					it->setVelocity(sf::Vector2f(positionData.vx, positionData.vy));
					it->setAcceleration(sf::Vector2f(positionData.ax, positionData.ay));
					it->setTurretRotation(positionData.turretRotation);
				}
				break;
			}
			case PacketType::ObjectKilled: {
				ObjectDieData objectDieData;
				packet >> objectDieData;
				switch (objectDieData.type)
				{
				case ObjectType::Bullet: {
					auto it = std::find_if(bullets.begin(), bullets.end(),
						[=](Bullet& p) { return p.getID() == objectDieData.id; });

					if (it != bullets.end())
						bullets.erase(it);
					break;
				}
				case ObjectType::Player: {
					auto it = std::find_if(players.begin(), players.end(),
						[=](Player& p) { return p.getID() == objectDieData.id; });

					if (it != players.end())
						players.erase(it);
					break;
				}
				case ObjectType::AIPlayer: {
					auto it = std::find_if(enemies.begin(), enemies.end(),
						[=](AIPlayer& p) { return p.getID() == objectDieData.id; });

					if (it != enemies.end())
						enemies.erase(it);
					break;
				}
				default: {
					auto it = std::find_if(resources.begin(), resources.end(),
						[=](Object& p) { return p.getID() == objectDieData.id; });

					if (it != resources.end())
						resources.erase(it);
					break;
				}
				}
			}
			default:
				std::cout << "Unknown packet type received" << std::endl;
				break;
			}
		}
	}

	void sendToAllPlayers(sf::Packet packet) {
		for (auto& [ip, port] : connectedPeers) {
			socket.send(packet, ip, port);
		}
	}

	void sendPlayerPositionUpdate() {
		if (syncSinceLastSend > 0.1f) {
			syncSinceLastSend = 0.f;

			sf::Packet packet;
			packet << PacketType::PlayerPositionUpdate;
			packet << InitSyncData{ player.getID(),
				player.getPosition().x,player.getPosition().y,player.getTurretRotation(),
				player.getVelocity().x,player.getVelocity().y,
				player.getAcceleration().x,player.getAcceleration().y };
			sendToAllPlayers(packet);

			// std::cout << "Send: Player Position" << std::endl;
		}
	}

	void sendPlayerFireEvent(sf::Vector2f dir) {
		sf::Packet packet;
		packet << PacketType::PlayerFireEvent;
		packet << FireEventData{ player.getID(), dir.x, dir.y };
		sendToAllPlayers(packet);

		// std::cout << "Send: Player Fire Event" << std::endl;
	}

	void sendAIStatus(AIPlayer object) {
		sf::Packet packet;
		packet << PacketType::EnemyPositionUpdate;
		packet << InitSyncData{ object.getID(),
			object.getPosition().x,object.getPosition().y, object.getTurretRotation(),
			object.getVelocity().x,object.getVelocity().y,
			object.getAcceleration().x,object.getAcceleration().y };
		sendToAllPlayers(packet);
	}

	void sendObjectDie(Object object) {
		sf::Packet packet;
		packet << PacketType::ObjectKilled;
		packet << ObjectDieData{ object.getID(),object.WhatAmI() };
		sendToAllPlayers(packet);
	}

	void sendPlayerResource(sf::Vector2f pos) {
		sf::Packet packet;
		packet << PacketType::SyncResourceStatus;
		packet << InitSyncData{ player.getID(),
			pos.x,pos.y,0,0,0,0,0 };
		sendToAllPlayers(packet);

		// std::cout << "Send: Player Resource Created" << std::endl;
	}

	// �������ӵ�ĳһip
	bool tryConnect(std::string typedText) {
		std::string ip;
		std::string port;

		// ���� IP ��ַ�Ͷ˿ں�
		std::stringstream ss(typedText);
		std::getline(ss, ip, ':');
		std::getline(ss, port, ':');

		sf::IpAddress ipAddress(ip);

		std::cout << "Try connect to ip:" << ip << std::endl;

		bool success = false;
		short parsedPort = port.empty() ? 0 : static_cast<unsigned short>(std::stoi(port));
		return addConnection(ipAddress, parsedPort);
	}

	// �ͻ���������, ����������һ����Ҫ����ָ��, �ȴ����������ĳ�ʼ���ź�
	bool addConnection(const sf::IpAddress& ipAddress, unsigned short port) {
		for (auto& [addr, p] : connectedPeers) {
			if (addr == ipAddress && p == port)
				return false;
		}

		sf::Packet packet;
		packet << PacketType::PlayerJoined;
		sf::Socket::Status status = socket.send(packet, ipAddress, port);

		// ��鷢��״̬
		bool success = status == sf::Socket::Done;
		if (success)
			std::cout << "Connection successful with " << ipAddress << ":" << port << std::endl;
		else
			std::cout << "Error sending packet to " << ipAddress << ":" << port << std::endl;

		return success;
	}

	bool isHost() {
		return localIsHost;
	}

private:
	bool localIsHost = true;
	float syncSinceLastSend = 0.0f;
	sf::UdpSocket socket;
	std::vector<std::pair<sf::IpAddress, unsigned short>> connectedPeers;
};
