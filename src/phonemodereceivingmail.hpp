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
#ifndef PHONE_MODE_RECEIVING_MAIL_HPP
#define PHONE_MODE_RECEIVING_MAIL_HPP

#include "phonemode.hpp"

class Phone;

class PhoneModeReceivingMail : PhoneMode
{
    friend class Phone;
protected:
    PhoneModeReceivingMail(Phone* pPhone);
    ~PhoneModeReceivingMail();

    virtual void OnOpen(uint8_t OldMode);
    virtual void Draw(sf::RenderWindow* pWindow);
    virtual void MouseMoved(sf::Vector2i Pos);

private:
    void UpdateAnim();

    sf::Sprite HDots[4];
    sf::Sprite VDots[2];
    sf::Text RecMailText;
    sf::Clock AnimClock;
    uint8_t AnimStep;
};

#endif
