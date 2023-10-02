#pragma once
#include <SFML/Network.hpp>

// 数据类型
enum class PacketType {
	PlayerJoined,
	PlayerIDAssign,
	SyncBulletStatus,
	SyncResourceStatus,
	SyncPlayerStatus,
	SyncEnemyStatus,
	PlayerPositionUpdate,
	PlayerFireEvent,
	EnemyPositionUpdate,
	ObjectKilled,
};

// 打包数据包的类型
sf::Packet& operator<<(sf::Packet& packet, const PacketType& type) {
	return packet << static_cast<unsigned char>(type);
}

sf::Packet& operator>>(sf::Packet& packet, PacketType& type) {
	unsigned char temp;
	packet >> temp;
	type = static_cast<PacketType>(temp);
	return packet;
}

// 加入玩家的回执包
struct PlayerIDAssignData {
	int id;
	float x, y;
	float turretRotation;
};

sf::Packet& operator<<(sf::Packet& packet, const PlayerIDAssignData& initSyncData) {
	return packet << initSyncData.id << initSyncData.x << initSyncData.y << initSyncData.turretRotation;
}

sf::Packet& operator>>(sf::Packet& packet, PlayerIDAssignData& initSyncData) {
	return packet >> initSyncData.id >> initSyncData.x >> initSyncData.y >> initSyncData.turretRotation;
}

// 初始同步包 Position Velocity Acceleration currentHealth
struct InitSyncData {
	int id;
	float x, y;
	float turretRotation;
	float vx, vy;
	float ax, ay;
};

sf::Packet& operator<<(sf::Packet& packet, const InitSyncData& initSyncData) {
	return packet << initSyncData.id << initSyncData.x << initSyncData.y << initSyncData.turretRotation << initSyncData.vx << initSyncData.vy << initSyncData.ax << initSyncData.ay;
}

sf::Packet& operator>>(sf::Packet& packet, InitSyncData& initSyncData) {
	return packet >> initSyncData.id >> initSyncData.x >> initSyncData.y >> initSyncData.turretRotation >> initSyncData.vx >> initSyncData.vy >> initSyncData.ax >> initSyncData.ay;
}

// 初始子弹同步包 Position Velocity Acceleration currentHealth
struct InitBulletSyncData {
	int id;
	float x, y;
	float vx, vy;
	float ax, ay;
	bool isAI;
	int ownerID;
};

sf::Packet& operator<<(sf::Packet& packet, const InitBulletSyncData& initSyncData) {
	return packet << initSyncData.id << initSyncData.x << initSyncData.y << initSyncData.vx << initSyncData.vy << initSyncData.ax << initSyncData.ay << initSyncData.isAI << initSyncData.ownerID;
}

sf::Packet& operator>>(sf::Packet& packet, InitBulletSyncData& initSyncData) {
	return packet >> initSyncData.id >> initSyncData.x >> initSyncData.y >> initSyncData.vx >> initSyncData.vy >> initSyncData.ax >> initSyncData.ay >> initSyncData.isAI >> initSyncData.ownerID;
}

// 开火事件包
struct FireEventData {
	int id;
	float targetX, targetY;
};

sf::Packet& operator<<(sf::Packet& packet, const FireEventData& fireEventData) {
	return packet << fireEventData.id << fireEventData.targetX << fireEventData.targetY;
}

sf::Packet& operator>>(sf::Packet& packet, FireEventData& fireEventData) {
	return packet >> fireEventData.id >> fireEventData.targetX >> fireEventData.targetY;
}

// 有人死了包
struct ObjectDieData {
	int id;
	ObjectType type;
};

sf::Packet& operator<<(sf::Packet& packet, const ObjectDieData& fireEventData) {
	return packet << fireEventData.id << static_cast<unsigned char>(fireEventData.type);
}

sf::Packet& operator>>(sf::Packet& packet, ObjectDieData& fireEventData) {
	packet >> fireEventData.id;
	unsigned char type;
	packet >> type;
	fireEventData.type = static_cast<ObjectType>(type);
	return  packet;
}
