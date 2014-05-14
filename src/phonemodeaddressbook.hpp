/* 
 * steins-gate: Open source implementation of Steins;Gate Visual Novel
 * Copyright (C) 2014 Mislav Blažević <krofnica996@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * */
#ifndef PHONE_MODE_ADDRESS_BOOK_HPP
#define PHONE_MODE_ADDRESS_BOOK_HPP

#include "phonemode.hpp"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <list>

class Phone;

struct Contact
{
    sf::Text Text;
    uint8_t Index;
};

class PhoneModeAddressBook : PhoneMode
{
    friend class Phone;
protected:
    PhoneModeAddressBook(Phone* pPhone);
    ~PhoneModeAddressBook();

    virtual void OnOpen(uint8_t OldMode);
    virtual void Draw(sf::RenderWindow* pWindow);
    virtual void MouseMoved(sf::Vector2i Pos);
    virtual uint8_t LeftMouseClicked();
    virtual uint8_t RightMouseClicked();

    void SetHighlight(int16_t Highlight);
    void AddContact(uint8_t Index);
    void DelContact(uint8_t Index);
private:
    int16_t ContactHighlight;
    uint16_t CallAllowedMask, AddressMask;
    sf::Texture* pWhite;
    sf::Texture* pHighlight;
    sf::Sprite Highlight;
    sf::Sprite Mask;
    sf::Text HeaderText;
    std::list<Contact> Contacts;
};

#endif
