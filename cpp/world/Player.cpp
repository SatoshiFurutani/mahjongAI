#include "Player.h"

namespace mahjong::world {

const Hand& Player::hand() const
{
    return hand_;
}

Hand& Player::hand()
{
    return hand_;
}

const River& Player::river() const
{
    return river_;
}

River& Player::river()
{
    return river_;
}

const std::vector<OpenMeld>& Player::openMelds() const
{
    return openMelds_;
}

int Player::score() const
{
    return score_;
}

bool Player::isReach() const
{
    return reach_;
}

bool Player::isIppatsu() const
{
    return ippatsu_;
}

std::uint8_t Player::drawOrder() const
{
    return drawOrder_;
}

void Player::setHand(const Hand& hand)
{
    hand_ = hand;
}

void Player::setRiver(const River& river)
{
    river_ = river;
}

void Player::setScore(int score)
{
    score_ = score;
}

void Player::setReach(bool reach)
{
    reach_ = reach;
}

void Player::setIppatsu(bool ippatsu)
{
    ippatsu_ = ippatsu;
}

void Player::setDrawOrder(std::uint8_t drawOrder)
{
    drawOrder_ = drawOrder;
}

void Player::addOpenMeld(const OpenMeld& openMeld)
{
    openMelds_.push_back(openMeld);
}

void Player::clearOpenMelds()
{
    openMelds_.clear();
}

void Player::resetForNewRound(int initialScore)
{
    hand_.clear();
    river_.clear();
    openMelds_.clear();
    score_ = initialScore;
    reach_ = false;
    ippatsu_ = false;
    drawOrder_ = 0;
}

} // namespace mahjong::world
