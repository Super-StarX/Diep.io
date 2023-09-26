#pragma once
#include <SFML/Network.hpp>
#include <unordered_map>
#include <iostream>

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

// 位置包
struct PositionData {
    int id;
    float x, y;
    float turretRotation;
};

sf::Packet& operator<<(sf::Packet& packet, const PositionData& positionData) {
    return packet << positionData.id << positionData.x << positionData.y << positionData.turretRotation;
}

sf::Packet& operator>>(sf::Packet& packet, PositionData& positionData) {
    return packet >> positionData.id >> positionData.x >> positionData.y >> positionData.turretRotation;
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
                Player newPlayer(15.f, sf::Color::Blue, sf::Vector2f(400, 300), 1000);
                //newPlayer.randomAddToMap();
                newPlayer.setPosition(450, 350);
                players.push_back(newPlayer);

                sf::Packet replyPosition;
                replyPosition << PacketType::PlayerIDAssign;
                replyPosition << PlayerIDAssignData{ newPlayer.getID(), newPlayer.getPosition().x, newPlayer.getPosition().y, newPlayer.getTurretRotation() };
                socket.send(replyPosition, sender, port);

                std::cout << "Send: Player Position" << std::endl;

                // 同时把所有object的Velocity Acceleration currentHealth
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
                    bullet.isAI(),bullet.getOwner()->getID()};

                    socket.send(packet, sender, port);
                }

                break;
            }
            case PacketType::PlayerIDAssign: {
                std::cout << "Player ID Assign" << std::endl;

                PlayerIDAssignData id;
                packet >> id;

                // 接收服务器分配的id
                player.setID(id.id);
                player.setPosition(id.x, id.y);
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
                        Bullet bullet(&*it, sf::Color::Blue);
                        bullet.setPosition(initData.x, initData.y);
                        bullet.setVelocity(sf::Vector2f(initData.vx, initData.vy));
                        bullet.setAcceleration(sf::Vector2f(initData.ax, initData.ay));
                        bullets.push_back(bullet);
                    }

                }
                else {
                    auto it = std::find_if(players.begin(), players.end(),
                        [=](Player& p) { return p.getID() == initData.id; });

                    if (it != players.end()) {
                        Bullet bullet(&*it, sf::Color::Blue);
                        bullet.setPosition(initData.x, initData.y);
                        bullet.setVelocity(sf::Vector2f(initData.vx, initData.vy));
                        bullet.setAcceleration(sf::Vector2f(initData.ax, initData.ay));
                        bullets.push_back(bullet);
                    }
                }

                break;
            }
            case PacketType::PlayerPositionUpdate: {
                std::cout << "Player Position Update" << std::endl;

                InitSyncData positionData;
                packet >> positionData;

                // 同步该玩家的位置
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

                // 在该玩家处产生炮弹
                // 同步该玩家的位置
                auto it = std::find_if(players.begin(), players.end(),
                    [=](Player& p) { return p.getID() == fireEventData.id; });

                if (it != players.end()) {
                    sf::Vector2f target{ fireEventData.targetX, fireEventData.targetY };
                    it->fire(target, sf::Color::Blue);
                }
                break;
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
        if (syncSinceLastSend < 0.1f)
            return;
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

    void sendPlayerFireEvent(sf::Vector2f dir) {
        sf::Packet packet;
        packet << PacketType::PlayerFireEvent;
        packet << FireEventData{ player.getID(), dir.x, dir.y };
        sendToAllPlayers(packet);

        // std::cout << "Send: Player Fire Event" << std::endl;
    }
    
    void sendPlayerResource(sf::Vector2f pos) {
        sf::Packet packet;
        packet << PacketType::SyncResourceStatus;
        packet << InitSyncData{ player.getID(),
            pos.x,pos.y,0,0,0,0,0 };
        sendToAllPlayers(packet);

        std::cout << "Send: Player Resource Created" << std::endl;
    }

    // 尝试连接到某一ip
    bool tryConnect(std::string typedText) {
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
    bool addConnection(const sf::IpAddress& ipAddress, unsigned short port) {
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

    bool isHost() {
        return localIsHost;
    }
    
private:
    bool localIsHost = true;
    float syncSinceLastSend = 0.0f;
    sf::UdpSocket socket;
    std::vector<std::pair<sf::IpAddress, unsigned short>> connectedPeers;
};
