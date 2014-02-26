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
#include "sginterpreter.hpp"
#include "phone.hpp"
#include "game.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <boost/lexical_cast.hpp>

enum PhoneState
{
    PHONE_CLOSING = 0,
    PHONE_OPENING = 1,
    PHONE_OPENING_DONE, // Switch from last opening animation frame to open phone frame
    PHONE_CLOSING_DONE, // Same as above, except that phone needs to be removed
    PHONE_OPEN,
    PHONE_CLOSED
};

const int16_t PHONE_ANIM_SPEED = 40; // TODO: guess
const int8_t PHONE_ANIM_ROW_MAX = 1;
const int8_t PHONE_ANIM_COLUMN_MAX = 4;
const int8_t PHONE_ANIM_UNDERFLOW = -1;
const int16_t PHONE_WIDTH = 313;
const int16_t PHONE_HEIGHT = 576;
const int16_t PHONE_POS_X = 678;
const int16_t PHONE_POS_Y = 8;

// cg/sys/phone/phone_01.png
const int16_t PHONE_TEX_X = 95; // TODO: guess
const int16_t PHONE_TEX_Y = 0; // TODO: guess
const int16_t PHONE_HEADER_TEX_X = 670;
const int16_t PHONE_HEADER_TEX_Y = 384;
const int16_t PHONE_HEADER_WIDTH = 220;
const int16_t PHONE_HEADER_HEIGHT = 24;
const int16_t PHONE_NEW_MAIL_TEX_X = 302;
const int16_t PHONE_NEW_MAIL_TEX_Y = 576;
const int16_t PHONE_NEW_MAIL_WIDTH = 220;
const int16_t PHONE_NEW_MAIL_HEIGHT = 130;

const int16_t PHONE_HEADER_POS_X = PHONE_POS_X + 49; // 727
const int16_t PHONE_HEADER_POS_Y = PHONE_POS_Y + 89; // 97
const int16_t PHONE_WALLPAPER_X = PHONE_HEADER_POS_X;
const int16_t PHONE_WALLPAPER_Y = PHONE_HEADER_POS_Y + PHONE_HEADER_HEIGHT; // TODO: guess
const int16_t PHONE_OVERLAY_POS_X = PHONE_WALLPAPER_X;
const int16_t PHONE_OVERLAY_POS_Y = 180;

const int16_t PHONE_SD_POS_X = 20;
const int16_t PHONE_SD_POS_Y = 20;
const int16_t PHONE_SD_TEX_X = 794;
const int16_t PHONE_SD_TEX_Y = 42;
const int16_t PHONE_SD_WIDTH = 200;
const int16_t PHONE_SD_HEIGHT = 50;

const int16_t PHONE_SD_BLUE_TEX_X = 782;
const int16_t PHONE_SD_BLUE_TEX_Y = 124;
const int16_t PHONE_SD_BLUE_WIDTH = PHONE_SD_WIDTH + (PHONE_SD_TEX_X - PHONE_SD_BLUE_TEX_X) * 2;
const int16_t PHONE_SD_BLUE_HEIGHT = PHONE_SD_HEIGHT + (136 - PHONE_SD_BLUE_TEX_Y) * 2;

// First digit in SD
const int16_t PHONE_SD_DIGIT_POS_X = 36;
const int16_t PHONE_SD_DIGIT_POS_Y = 36;
const int16_t PHONE_SD_DIGIT_OFFSET_X = 7;
const int16_t PHONE_SD_DAY_POS_Y = 47;

// 0-9
const int16_t PHONE_DIGIT_A_TEX_X[10] =
{
    334, 24, 59, 93, 127, 159, 192, 226, 260, 292
};
const int16_t PHONE_DIGIT_A_TEX_Y = 83;
const int16_t PHONE_DIGIT_A_WIDTH = 16;
const int16_t PHONE_DIGIT_A_HEIGHT = 22;

// SUN, MON-SAT
const int16_t PHONE_DAY_TEX_X[7] =
{
    23, 75, 126, 178, 229, 281, 332
};
const int16_t PHONE_DAY_TEX_Y = 128;
const int16_t PHONE_DAY_WIDTH = 39;
const int16_t PHONE_DAY_HEIGHT = 14;

// "/"
const int16_t PHONE_SLASH_TEX_X = 400;
const int16_t PHONE_SLASH_TEX_Y = 164;
const int16_t PHONE_SLASH_WIDTH = 16;
const int16_t PHONE_SLASH_HEIGHT = 24;

//
enum PhoneIcon
{
    PHONE_ICON_UNK = 0,
    PHONE_ICON_MAIL = 1,
    PHONE_ICON_SIGNAL = 2,
    PHONE_ICON_BATTERY = 3,
    PHONE_ICON_MAX = 4
};
const int16_t PHONE_ICON_TEX_X = 626;
const int16_t PHONE_ICON_TEX_Y[PHONE_ICON_MAX] =
{
    30, 56, 79, 106
};
const int16_t PHONE_ICON_POS_X[PHONE_ICON_MAX] =
{
    -1, 147, 187, 166
};
const int16_t PHONE_ICON_POS_Y = 30;
const int16_t PHONE_ICON_WIDTH[PHONE_ICON_MAX] =
{
    23, 16, 17, 19
};
const int16_t PHONE_ICON_HEIGHT = 11;

//
enum PhoneSDText
{
    PHONE_SD_TEXT_NEW_MAIL = 0,
    PHONE_SD_TEXT_UNK1 = 1,
    PHONE_SD_TEXT_UNK2 = 2,
    PHONE_SD_TEXT_MAX = 3
};
const int16_t PHONE_SD_TEXT_TEX_X[PHONE_SD_TEXT_MAX] =
{
    135, 168, 200
};

enum PhoneButton
{
    BUTTON_MAIL = 0,
    BUTTON_CONTACTS,
    BUTTON_WEB,
    BUTTON_SETTINGS,
    BUTTON_MAX
};
const int16_t PHONE_BUTTON_TEX_X = 27;
const int16_t PHONE_BUTTON_TEX_Y[BUTTON_MAX * 2] =
{
    151, 222,
    20, 82,
    291, 363,
    436, 505
};
const int16_t PHONE_BUTTON_POS_X[] =
{
    766,
    848
};
const int16_t PHONE_BUTTON_POS_Y[] =
{
    154,
    223
};
const int16_t PHONE_BUTTON_WIDTH = 50;
const int16_t PHONE_BUTTON_HEIGHT = 50;

// Menu wallpaper
const int16_t PHONE_MENU_TEX_X = 532;
const int16_t PHONE_MENU_TEX_Y = 760;
const int16_t PHONE_MENU_WIDTH = 220;
const int16_t PHONE_MENU_HEIGHT = 254;

const int16_t PHONE_MENU_MAIN_TEX_X = 670;
const int16_t PHONE_MENU_MAIN_TEX_Y = 213;
const int16_t PHONE_MENU_MAIN_WIDTH = 220;
const int16_t PHONE_MENU_MAIN_HEIGHT = 38;

const char* ContactString[] =
{
    "ダル",
    "電話レンジ (仮)",
    "フェイリス",
    "まゆり",
    "ルカ子"
};

const char* HeaderString[] =
{
    "アドレス帳",
    "受信メールBOX",
    "送信メールBOX"
};

enum
{
    BLUE_HEADER_MAIL,
    BLUE_HEADER_CONACTS,
    BLUE_HEADER_SETTINGS
};
const int16_t BLUE_HEADER_TEX_X = 670;
const int16_t BLUE_HEADER_TEX_Y[] =
{
    284, 318, 349
};
const int16_t BLUE_HEADER_POS_X = PHONE_HEADER_POS_X;
const int16_t BLUE_HEADER_POS_Y = PHONE_HEADER_POS_Y + PHONE_HEADER_HEIGHT;
const int16_t BLUE_HEADER_WIDTH = 220;
const int16_t BLUE_HEADER_HEIGHT = 23;

const int16_t WALLPAPER_WIDTH = 220;
const int16_t WALLPAPER_HEIGHT = 244;

const int16_t MASK_TEX_X = 425;
const int16_t MASK_TEX_Y = 21;
const int16_t MASK_WIDTH = 220;
const int16_t MASK_HEIGHT = 253;

const int16_t OVERLAY_RED_TEX_X = 302;
const int16_t OVERLAY_RED_TEX_Y = 722;
const int16_t OVERLAY_RED_WIDTH = 220;
const int16_t OVERLAY_RED_HEIGHT = 119;
const int16_t OVERLAY_RED_POS_X = 728;
const int16_t OVERLAY_RED_POS_Y = 159;

const int16_t MAIL_MENU_TEXT_POS_X = 750;

const string PhoneModeString[] =
{
    "PhoneMode_AddressBook",
    "PhoneMode_AddressConfirmDial",
    "PhoneMode_AddressConfirmMail",
    "PhoneMode_Calling",
    "PhoneMode_CompleteReceiveMail",
    "PhoneMode_CompleteSendMail",
    "PhoneMode_Default",
    "PhoneMode_DefaultOperatable",
    "PhoneMode_DialogSendMailEdit",
    "PhoneMode_EngageNormal",
    "PhoneMode_EngageVisual",
    "PhoneMode_MailMenu",
    "PhoneMode_MailSubMenu",
    "PhoneMode_PowerOff",
    "PhoneMode_ReceiveBox",
    "PhoneMode_ReceivedMail",
    "PhoneMode_ReceivingMail",
    "PhoneMode_SendBox",
    "PhoneMode_Sending",
    "PhoneMode_SendMailEdit"
};

int DateToWeekDay(string Date)
{
    string Month(Date, 0, 2);
    string Day(Date, 2, 2);

    std::tm time_in = { 0, 0, 0,
                        boost::lexical_cast<int>(Day),
                        boost::lexical_cast<int>(Month) - 1,
                        2010 - 1900 };

    std::time_t time = std::mktime(&time_in);
    return std::localtime(&time)->tm_wday;
}

Phone::Phone(sf::Drawable* pDrawable, sf::Window* pWindow) :
DrawableBase(pDrawable, -1, DRAWABLE_TEXTURE),
ShowSD(false),
ShowOverlay(false),
Mode(MODE_POWER_OFF),
ButtonHighlightX(-1),
ButtonHighlightY(-1),
MailMenuHighlight(0),
pWindow(pWindow)
{
    pHighlight = LoadTextureFromColor("#ffc896", 220, 20);
    pWallpaper = LoadTextureFromFile("cg/sys/phone/pwcg101.png", sf::IntRect());
    pPhoneTex = LoadTextureFromFile("cg/sys/phone/phone_01.png", sf::IntRect());
    pPhoneOpenTex = LoadTextureFromFile("cg/sys/phone/phone_open_anim.png", sf::IntRect());
    pSDTex = LoadTextureFromFile("cg/sys/phone/phone_sd.png", sf::IntRect());
    pWhite = LoadTextureFromColor("white", MASK_WIDTH, MASK_HEIGHT);

    Highlight.setTexture(*pHighlight);
    OverlayRed.setTexture(*pPhoneTex);
    OverlayRed.setPosition(OVERLAY_RED_POS_X, OVERLAY_RED_POS_Y);
    OverlayRed.setTextureRect(sf::IntRect(OVERLAY_RED_TEX_X, OVERLAY_RED_TEX_Y, OVERLAY_RED_WIDTH, OVERLAY_RED_HEIGHT));
    MenuOverlay.setTexture(*pPhoneTex);
    MenuOverlay.setPosition(PHONE_WALLPAPER_X, PHONE_WALLPAPER_Y);
    Mask.setTexture(*pPhoneTex);
    Mask.setPosition(PHONE_WALLPAPER_X, PHONE_WALLPAPER_Y);
    BlueHeader.setTexture(*pPhoneTex);
    BlueHeader.setPosition(BLUE_HEADER_POS_X, BLUE_HEADER_POS_Y);
    SD.setTexture(*pSDTex);
    SD.setPosition(PHONE_SD_POS_X, PHONE_SD_POS_Y);
    SD.setTextureRect(sf::IntRect(PHONE_SD_TEX_X, PHONE_SD_TEX_Y, PHONE_SD_WIDTH, PHONE_SD_HEIGHT));
    Overlay.setTexture(*pPhoneTex);
    Overlay.setPosition(PHONE_OVERLAY_POS_X, PHONE_OVERLAY_POS_Y);
    Overlay.setTextureRect(sf::IntRect(PHONE_NEW_MAIL_TEX_X, PHONE_NEW_MAIL_TEX_Y, PHONE_NEW_MAIL_WIDTH, PHONE_NEW_MAIL_HEIGHT));
    Header.setTexture(*pPhoneTex);
    Header.setPosition(PHONE_HEADER_POS_X, PHONE_HEADER_POS_Y);
    Header.setTextureRect(sf::IntRect(PHONE_HEADER_TEX_X, PHONE_HEADER_TEX_Y, PHONE_HEADER_WIDTH, PHONE_HEADER_HEIGHT));

    Wallpaper.setPosition(PHONE_WALLPAPER_X, PHONE_WALLPAPER_Y);
    ToSprite()->setPosition(PHONE_POS_X, PHONE_POS_Y);

    for (int y = 0; y < 2; ++y)
    {
        for (int x = 0; x < 2; ++x)
        {
            sf::IntRect ClipArea(PHONE_BUTTON_TEX_X, PHONE_BUTTON_TEX_Y[(y * 2 + x) * 2 + 1], PHONE_BUTTON_WIDTH, PHONE_BUTTON_HEIGHT);
            Button[y][x].setTexture(*pPhoneTex);
            Button[y][x].setTextureRect(ClipArea);
            Button[y][x].setPosition(PHONE_BUTTON_POS_X[x], PHONE_BUTTON_POS_Y[y]);
        }
    }

    for (int i = 0; i < 5; ++i)
    {
        Contacts[i].setString(sf::String::fromUtf8(ContactString[i], ContactString[i] + strlen(ContactString[i])));
        Contacts[i].setFont(Text::Font);
        Contacts[i].setPosition(PHONE_WALLPAPER_X, BLUE_HEADER_POS_Y + BLUE_HEADER_HEIGHT + i * 20);
        Contacts[i].setCharacterSize(20);
        Contacts[i].setColor(sf::Color::Black);
    }

    for (int i = 0; i < 2; ++i)
    {
        MailMenuText[i].setString(sf::String::fromUtf8(HeaderString[i + 1], HeaderString[i + 1] + strlen(HeaderString[i + 1])));
        MailMenuText[i].setFont(Text::Font);
        MailMenuText[i].setPosition(MAIL_MENU_TEXT_POS_X, 170 + i * 20);
        MailMenuText[i].setCharacterSize(20);
        MailMenuText[i].setColor(sf::Color::Black);
    }

    HeaderText.setFont(Text::Font);
    HeaderText.setPosition(BLUE_HEADER_POS_X + 24, BLUE_HEADER_POS_Y);
    HeaderText.setCharacterSize(20);
}

Phone::~Phone()
{
    delete pHighlight;
    delete pWhite;
    delete pSDTex;
    delete pPhoneTex;
    delete pPhoneOpenTex;
    delete pWallpaper;
}

void Phone::UpdateOpenMode(int32_t OpenMode)
{
    // TODO: Don't "jump" to end of animation if it didn't finish
    ToSprite()->setTexture(*pPhoneOpenTex);
    State = OpenMode;
    switch (State)
    {
        case PHONE_OPENING: AnimRow = PHONE_ANIM_ROW_MAX; AnimColumn = PHONE_ANIM_COLUMN_MAX; break;
        case PHONE_CLOSING: AnimRow = 0; AnimColumn = 0; break;
    }
    UpdateAnim();
    AnimClock.restart();
}

void Phone::Draw(sf::RenderWindow* pWindow)
{
    DrawableBase::Draw(pWindow);
    if (State == PHONE_OPEN)
    {
        if (Mode != MODE_POWER_OFF)
        {
            pWindow->draw(Wallpaper);
            pWindow->draw(Header);
            if (ShowOverlay)
                pWindow->draw(Overlay);
        }
        if (Mode == MODE_DEFAULT_OPERATABLE)
        {
            pWindow->draw(MenuOverlay);
            for (int y = 0; y < 2; ++y)
                for (int x = 0; x < 2; ++x)
                    pWindow->draw(Button[y][x]);
        }
        if (Mode == MODE_ADDRESS_BOOK)
        {
            pWindow->draw(Mask);
            pWindow->draw(BlueHeader);
            pWindow->draw(HeaderText);
            pWindow->draw(Highlight);
            for (int i = 0; i < 5; ++i)
                pWindow->draw(Contacts[i]);
        }
        if (Mode == MODE_MAIL_MENU)
        {
            pWindow->draw(OverlayRed);
            for (int i = 0; i < 2; ++i)
                pWindow->draw(MailMenuText[i]);
        }
    }
    if (ShowSD)
    {
        pWindow->draw(SD);
        for (int i = 0; i < 6; ++i)
            pWindow->draw(SDDate[i]);
        for (int i = PHONE_ICON_SIGNAL; i <= PHONE_ICON_BATTERY; ++i)
            pWindow->draw(SDIcon[i]);
    }
}

void Phone::Update()
{
    if (AnimClock.getElapsedTime().asMilliseconds() < PHONE_ANIM_SPEED
        || State == PHONE_OPEN || State == PHONE_CLOSED)
        return;

    UpdateAnim();
    AnimClock.restart();
}

void Phone::UpdateAnim()
{
    //
    // See: cg/sys/phone/phone_open_anim.png
    //

    // Animation finished in last call
    if (State == PHONE_OPENING_DONE)
    {
        sf::IntRect ClipArea(PHONE_TEX_X, PHONE_TEX_Y, PHONE_WIDTH, PHONE_HEIGHT);
        ToSprite()->setTexture(*pPhoneTex);
        ToSprite()->setTextureRect(ClipArea);
        State = PHONE_OPEN;
        return;
    }
    else if (State == PHONE_CLOSING_DONE)
    {
        delete pDrawable;
        pDrawable = new sf::Sprite;
        ToSprite()->setPosition(PHONE_POS_X, PHONE_POS_Y);
        State = PHONE_CLOSED;
        return;
    }

    // Animation is not done: set next frame
    sf::IntRect ClipArea(AnimColumn * PHONE_WIDTH, AnimRow * PHONE_HEIGHT, PHONE_WIDTH, PHONE_HEIGHT);
    ToSprite()->setTextureRect(ClipArea);

    // Check if animation is done
    switch (State)
    {
        case PHONE_OPENING:
            if (AnimColumn == 0 && AnimRow == 0)
                State = PHONE_OPENING_DONE;
            break;
        case PHONE_CLOSING:
            if (AnimColumn == PHONE_ANIM_COLUMN_MAX && AnimRow == PHONE_ANIM_ROW_MAX)
                State = PHONE_CLOSING_DONE;
            break;
    }

    // Advance animation progress
    switch (State)
    {
        case PHONE_OPENING: --AnimColumn; break;
        case PHONE_CLOSING: ++AnimColumn; break;
        case PHONE_OPENING_DONE: case PHONE_CLOSING_DONE: return;
    }

    // Go to previous row
    if (AnimColumn == PHONE_ANIM_UNDERFLOW)
    {
        AnimColumn = PHONE_ANIM_COLUMN_MAX;
        --AnimRow;
    }
    // Go to next row
    else if (AnimColumn > PHONE_ANIM_COLUMN_MAX)
    {
        AnimColumn = 0;
        ++AnimRow;
    }
}

void Phone::UpdateMode(uint8_t NewMode)
{
    if (NewMode == Mode)
        return;

    switch (NewMode)
    {
        case MODE_DEFAULT:
            Wallpaper.setTexture(*pWallpaper);
            Wallpaper.setTextureRect(sf::IntRect(0, 0, WALLPAPER_WIDTH, WALLPAPER_HEIGHT));
            break;
        case MODE_DEFAULT_OPERATABLE:
        {
            sf::IntRect WallpaperClipArea(PHONE_MENU_TEX_X, PHONE_MENU_TEX_Y, PHONE_MENU_WIDTH, PHONE_MENU_HEIGHT);
            Wallpaper.setTexture(*pPhoneTex);
            Wallpaper.setTextureRect(WallpaperClipArea);
            sf::IntRect OverlayClipArea(PHONE_MENU_MAIN_TEX_X, PHONE_MENU_MAIN_TEX_Y, PHONE_MENU_MAIN_WIDTH, PHONE_MENU_MAIN_HEIGHT);
            MenuOverlay.setTextureRect(OverlayClipArea);

            // Check old mode
            switch (Mode)
            {
                case MODE_ADDRESS_BOOK:
                    sf::Mouse::setPosition(sf::Vector2i(PHONE_BUTTON_POS_X[1] + PHONE_BUTTON_WIDTH / 2,
                                                        PHONE_BUTTON_POS_Y[0] + PHONE_BUTTON_HEIGHT / 2),
                                           *pWindow);
                    break;
                case MODE_MAIL_MENU:
                    sf::Mouse::setPosition(sf::Vector2i(PHONE_BUTTON_POS_X[0] + PHONE_BUTTON_WIDTH / 2,
                                                        PHONE_BUTTON_POS_Y[0] + PHONE_BUTTON_HEIGHT / 2),
                                           *pWindow);
                    break;
            }
            break;
        }
        case MODE_ADDRESS_BOOK:
        {
            sf::IntRect BlueHeaderClipArea(BLUE_HEADER_TEX_X, BLUE_HEADER_TEX_Y[BLUE_HEADER_CONACTS], BLUE_HEADER_WIDTH, BLUE_HEADER_HEIGHT);
            BlueHeader.setTextureRect(BlueHeaderClipArea);
            Wallpaper.setTexture(*pWhite, true);
            sf::IntRect MaskClipArea(MASK_TEX_X, MASK_TEX_Y, MASK_WIDTH, MASK_HEIGHT);
            Mask.setTextureRect(MaskClipArea);
            HeaderText.setString(sf::String::fromUtf8(HeaderString[0], HeaderString[0] + strlen(HeaderString[0]) - 3));
            break;
        }
        case MODE_MAIL_MENU:
            break;
        case MODE_POWER_OFF:
            break;
    }
    Mode = NewMode;
}

void Phone::MailReceive(int32_t Show)
{
    switch (Show)
    {
        case PHONE_CLOSING:
            ShowOverlay = false;
            break;
        case PHONE_OPENING:
            ShowOverlay = true;
            break;
        default:
            std::cout << "Invalid value " << Show << " passed to MailReceive." << std::endl;
            break;
    }
}

void Phone::SDDisplay(int32_t Show)
{
    switch (Show)
    {
        case PHONE_CLOSING:
            ShowSD = false;
            break;
        case PHONE_OPENING:
            for (int i = PHONE_ICON_SIGNAL; i <= PHONE_ICON_BATTERY; ++i)
            {
                if (!SDIcon[i].getTexture())
                {
                    sf::IntRect ClipArea(PHONE_ICON_TEX_X, PHONE_ICON_TEX_Y[i], PHONE_ICON_WIDTH[i], PHONE_ICON_HEIGHT);
                    SDIcon[i].setTexture(*pSDTex);
                    SDIcon[i].setTextureRect(ClipArea);
                    SDIcon[i].setPosition(PHONE_ICON_POS_X[i], PHONE_ICON_POS_Y);
                }
            }
            ShowSD = true;
            break;
        default:
            std::cout << "Invalid value " << Show << " passed to SDDisplay." << std::endl;
            break;
    }
}

void Phone::SetDate(string Date)
{
    int32_t PosX = PHONE_SD_DIGIT_POS_X;
    int i;

    // Month
    i = (Date[0] == '0' ? 1 : 0); // Alignment : skip preceeding zero
    for (; i < 2; ++i)
    {
        sf::IntRect ClipArea(PHONE_DIGIT_A_TEX_X[Date[i] - '0'], PHONE_DIGIT_A_TEX_Y, PHONE_DIGIT_A_WIDTH, PHONE_DIGIT_A_HEIGHT);
        SDDate[i].setTexture(*pSDTex);
        SDDate[i].setTextureRect(ClipArea);
        SDDate[i].setPosition(PosX, PHONE_SD_DIGIT_POS_Y);
        PosX += PHONE_SD_DIGIT_OFFSET_X + PHONE_DIGIT_A_WIDTH;
    }

    // Slash
    SDDate[2].setTexture(*pSDTex);
    SDDate[2].setTextureRect(sf::IntRect(PHONE_SLASH_TEX_X, PHONE_SLASH_TEX_Y, PHONE_SLASH_WIDTH, PHONE_SLASH_HEIGHT));
    SDDate[2].setPosition(PosX, PHONE_SD_DIGIT_POS_Y);
    PosX += PHONE_SD_DIGIT_OFFSET_X + PHONE_DIGIT_A_WIDTH;

    // Day
    i = (Date[2] == '0' ? 3 : 2);
    for (; i < 4; ++i)
    {
        sf::IntRect ClipArea(PHONE_DIGIT_A_TEX_X[Date[i] - '0'], PHONE_DIGIT_A_TEX_Y, PHONE_DIGIT_A_WIDTH, PHONE_DIGIT_A_HEIGHT);
        SDDate[i + 1].setTexture(*pSDTex);
        SDDate[i + 1].setTextureRect(ClipArea);
        SDDate[i + 1].setPosition(PosX, PHONE_SD_DIGIT_POS_Y);
        PosX += PHONE_SD_DIGIT_OFFSET_X + PHONE_DIGIT_A_WIDTH;
    }

    // Week day
    sf::IntRect ClipArea(PHONE_DAY_TEX_X[DateToWeekDay(Date)], PHONE_DAY_TEX_Y, PHONE_DAY_WIDTH, PHONE_DAY_HEIGHT);
    SDDate[5].setTexture(*pSDTex);
    SDDate[5].setTextureRect(ClipArea);
    SDDate[5].setPosition(PosX, PHONE_SD_DAY_POS_Y);
}

void Phone::SetPriority(int32_t Priority)
{
    this->Priority = Priority;
}

void Phone::MouseMoved(sf::Vector2i Pos)
{
    switch (Mode)
    {
        case MODE_DEFAULT_OPERATABLE:
            for (int y = 0; y < 2; ++y)
                for (int x = 0; x < 2; ++x)
                    if (Pos.x > PHONE_BUTTON_POS_X[x] && Pos.x < PHONE_BUTTON_POS_X[x] + PHONE_BUTTON_WIDTH)
                        if (Pos.y > PHONE_BUTTON_POS_Y[y] && Pos.y < PHONE_BUTTON_POS_Y[y] + PHONE_BUTTON_HEIGHT)
                            HighlightButton(x, y);
            break;
        case MODE_ADDRESS_BOOK:
            if (Pos.x > BLUE_HEADER_POS_X && Pos.x < BLUE_HEADER_POS_X + MASK_WIDTH && Pos.y > BLUE_HEADER_POS_Y + BLUE_HEADER_HEIGHT)
            {
                int i = (Pos.y - (BLUE_HEADER_POS_Y + BLUE_HEADER_HEIGHT)) / 20;
                if (i >= 0 && i < 5)
                    Highlight.setPosition(BLUE_HEADER_POS_X, BLUE_HEADER_POS_Y + BLUE_HEADER_HEIGHT + i * 20);
            }
            break;
        case MODE_MAIL_MENU:
            if (Pos.x > MAIL_MENU_TEXT_POS_X && Pos.x < MAIL_MENU_TEXT_POS_X + MailMenuText[0].getLocalBounds().width)
            {
                int i = (Pos.y - 170) / 20;
                if (i >= 0 && i < 2 && i != MailMenuHighlight)
                {
                    MailMenuText[MailMenuHighlight].setColor(sf::Color::Black);
                    MailMenuText[i].setColor(sf::Color::Red);
                    MailMenuHighlight = i;
                }
            }
            break;
    }
}

void Phone::LeftMouseClicked(sf::Vector2i Pos)
{
    if (!(Pos.x > PHONE_WALLPAPER_X && Pos.x < PHONE_WALLPAPER_X + WALLPAPER_WIDTH &&
          Pos.y > PHONE_WALLPAPER_Y && Pos.y < PHONE_WALLPAPER_Y + WALLPAPER_HEIGHT))
        return;

    if (ButtonHighlightX != -1 && Mode == MODE_DEFAULT_OPERATABLE)
    {
        switch (ButtonHighlightY * 2 + ButtonHighlightX)
        {
            case BUTTON_CONTACTS:
                UpdateMode(MODE_ADDRESS_BOOK);
                break;
            case BUTTON_MAIL:
                UpdateMode(MODE_MAIL_MENU);
                break;
            case BUTTON_WEB:
                if (fork() == 0)
                    execlp("/usr/bin/xdg-open", "/usr/bin/xdg-open", "http://futuregadget-lab.com/", NULL);
                break;
            case BUTTON_SETTINGS:
                break;
        }
    }
}

void Phone::RightMouseClicked(SGInterpreter* pInterpreter)
{
    switch (Mode)
    {
        case MODE_MAIL_MENU:
        case MODE_ADDRESS_BOOK:
            UpdateMode(MODE_DEFAULT_OPERATABLE);
            break;
        case MODE_DEFAULT_OPERATABLE:
            UpdateMode(MODE_DEFAULT);
            break;
        case MODE_DEFAULT:
            pInterpreter->PhoneToggle();
            break;
    }
}

void Phone::HighlightButton(int x, int y)
{
    if (x == ButtonHighlightX && y == ButtonHighlightY)
        return;

    // Remove old highlight
    sf::IntRect OldClipArea(PHONE_BUTTON_TEX_X, PHONE_BUTTON_TEX_Y[(ButtonHighlightY * 2 + ButtonHighlightX) * 2 + 1], PHONE_BUTTON_WIDTH, PHONE_BUTTON_HEIGHT);
    Button[ButtonHighlightY][ButtonHighlightX].setTextureRect(OldClipArea);

    // Add new highlight
    ButtonHighlightX = x;
    ButtonHighlightY = y;
    sf::IntRect ClipArea(PHONE_BUTTON_TEX_X, PHONE_BUTTON_TEX_Y[(y * 2 + x) * 2], PHONE_BUTTON_WIDTH, PHONE_BUTTON_HEIGHT);
    Button[y][x].setTextureRect(ClipArea);

    // Move mouse to center of button
    sf::Mouse::setPosition(sf::Vector2i(PHONE_BUTTON_POS_X[x] + PHONE_BUTTON_WIDTH / 2,
                                        PHONE_BUTTON_POS_Y[y] + PHONE_BUTTON_HEIGHT / 2),
                           *pWindow);
}
