#include "Situation.h"

namespace mahjong::situation {

std::uint8_t Situation::selfPlayerIndex() const
{
    return selfPlayerIndex_;
}

std::int8_t Situation::shanten() const
{
    return shanten_;
}

HandSpeed Situation::handSpeed() const
{
    return handSpeed_;
}

GamePhase Situation::gamePhase() const
{
    return gamePhase_;
}

bool Situation::isDealer() const
{
    return isDealer_;
}

bool Situation::selfReach() const
{
    return selfReach_;
}

bool Situation::anyOpponentReach() const
{
    return !opponentReachPlayers_.empty();
}

std::uint8_t Situation::opponentReachCount() const
{
    return static_cast<std::uint8_t>(opponentReachPlayers_.size());
}

const std::vector<std::uint8_t>& Situation::opponentReachPlayers() const
{
    return opponentReachPlayers_;
}

std::uint8_t Situation::remainingTileCount() const
{
    return remainingTileCount_;
}

std::uint8_t Situation::doraCount() const
{
    return doraCount_;
}

std::uint8_t Situation::pairCount() const
{
    return pairCount_;
}

std::uint8_t Situation::openMeldCount() const
{
    return openMeldCount_;
}

const Situation::VisibleCounts& Situation::visibleTileCounts() const
{
    return visibleTileCounts_;
}

void Situation::setSelfPlayerIndex(std::uint8_t selfPlayerIndex)
{
    selfPlayerIndex_ = selfPlayerIndex;
}

void Situation::setShanten(std::int8_t shanten)
{
    shanten_ = shanten;
}

void Situation::setHandSpeed(HandSpeed handSpeed)
{
    handSpeed_ = handSpeed;
}

void Situation::setGamePhase(GamePhase gamePhase)
{
    gamePhase_ = gamePhase;
}

void Situation::setDealer(bool isDealer)
{
    isDealer_ = isDealer;
}

void Situation::setSelfReach(bool selfReach)
{
    selfReach_ = selfReach;
}

void Situation::setOpponentReachPlayers(const std::vector<std::uint8_t>& opponentReachPlayers)
{
    opponentReachPlayers_ = opponentReachPlayers;
}

void Situation::setRemainingTileCount(std::uint8_t remainingTileCount)
{
    remainingTileCount_ = remainingTileCount;
}

void Situation::setDoraCount(std::uint8_t doraCount)
{
    doraCount_ = doraCount;
}

void Situation::setPairCount(std::uint8_t pairCount)
{
    pairCount_ = pairCount;
}

void Situation::setOpenMeldCount(std::uint8_t openMeldCount)
{
    openMeldCount_ = openMeldCount;
}

void Situation::setVisibleTileCounts(const VisibleCounts& visibleTileCounts)
{
    visibleTileCounts_ = visibleTileCounts;
}

} // namespace mahjong::situation
