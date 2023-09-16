#pragma once
#include <SFML/Graphics.hpp>

#include <vector>

#include "Global.h"
#include "Network.h"

class Menu {
public:
    Menu() {
        float x = window.getSize().x / 2.f;
        float y = window.getSize().y / 2.f;

        // ���� titleText
        titleText.setFont(Global::font);
        titleText.setString("diep.io!");
        titleText.setCharacterSize(55);
        titleText.setFillColor(sf::Color::White);
        sf::FloatRect titleTextBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleTextBounds.width / 2.0f, titleTextBounds.height / 2.0f);
        titleText.setPosition(x - 85, y - 80);

        // ���� startGameText
        startGameText.setFont(Global::font);
        startGameText.setString("Start Game");
        startGameText.setCharacterSize(32);
        startGameText.setFillColor(sf::Color::White);
        sf::FloatRect startGameTextBounds = startGameText.getGlobalBounds();
        startGameText.setOrigin(startGameTextBounds.width / 2.0f, startGameTextBounds.height / 2.0f);
        startGameText.setPosition(x - 85, y + 20);

        // ���� connectToIPAddressText
        connectToIPAddressText.setFont(Global::font);
        connectToIPAddressText.setString("Connect to IP Address");
        connectToIPAddressText.setCharacterSize(32);
        connectToIPAddressText.setFillColor(sf::Color::White);
        sf::FloatRect connectToIPAddressTextBounds = connectToIPAddressText.getGlobalBounds();
        connectToIPAddressText.setOrigin(connectToIPAddressTextBounds.width / 2.0f, connectToIPAddressTextBounds.height / 2.0f);
        connectToIPAddressText.setPosition(x - 120, y + 60);

        // ����ip
        inputText.setString("input ip address...");
        inputText.setFont(Global::font);
        inputText.setCharacterSize(32);
        inputText.setFillColor(sf::Color::White);
        inputText.setPosition(280, y + 90); // ����Ϊ�����λ��
    }

    bool inMenu() {
        return isInMenu;
    }

    void updateMenu(sf::Event event) {
        if (inMenu()) {
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    if (clickStartGameButton(mousePosition)) {
                        // �㿪ʼ��Ϸ
                        isInMenu = false;
                    }
                    else if (clickConnectToIPAddressButton(mousePosition)) {
                        // �����ӵ�ip
                        isTyping = true;
                    }
                }
            }

            if (isTyping) {
                if (event.type == sf::Event::KeyPressed) { // ����ճ��
                    if (event.key.control && event.key.code == sf::Keyboard::V) {
                        typedText += sf::Clipboard::getString().toAnsiString(); // ���������ı���ӵ� typedText
                        inputText.setString(typedText);
                    }
                }
                else if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == '\b') { // �����˸�ɾ��
                        if (!typedText.empty()) {
                            typedText.pop_back();
                        }
                    }
                    else if (event.text.unicode == '\r') { // �����»س���
                        if (networkManager.tryConnect(typedText)) {
                            isInMenu = false;
                            isTyping = false;
                        }
                        else if (!typedText.empty()) {
                            typedText.pop_back();
                        }
                    }
                    else {
                        typedText += event.text.unicode;
                    }
                    inputText.setString(typedText);
                }
            }
        }
    }

    void draw() const {
        window.draw(titleText);
        window.draw(startGameText);
        window.draw(connectToIPAddressText);
        if (isTyping) {
            window.draw(inputText);
        }
    }

    bool clickStartGameButton(const sf::Vector2i& mousePosition) const {
        const auto& buttonGlobalBounds = startGameText.getGlobalBounds();
        return buttonGlobalBounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
    }

    bool clickConnectToIPAddressButton(const sf::Vector2i& mousePosition) const {
        const auto& buttonGlobalBounds = connectToIPAddressText.getGlobalBounds();
        return buttonGlobalBounds.contains(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));
    }


public:
    bool isInMenu = true;
    bool isTyping = false;
    sf::Text titleText;
    sf::Text startGameText;
    sf::Text connectToIPAddressText;
    std::string typedText;
    sf::Text inputText;
};
