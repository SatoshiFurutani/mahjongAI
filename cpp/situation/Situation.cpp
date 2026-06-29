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

std::uint8_t Situation::openMeldCount() const
{
    return openMeldCount_;
}

const Situation::Scores& Situation::scores() const
{
    return scores_;
}

int Situation::selfScore() const
{
    return scores_.at(selfPlayerIndex_);
}

std::uint8_t Situation::selfRank() const
{
    return selfRank_;
}

int Situation::pointGapToNextRank() const
{
    return pointGapToNextRank_;
}

int Situation::pointGapFromLowerRank() const
{
    return pointGapFromLowerRank_;
}

std::uint8_t Situation::roundNumber() const
{
    return roundNumber_;
}

std::uint8_t Situation::honba() const
{
    return honba_;
}

std::uint8_t Situation::reachSticks() const
{
    return reachSticks_;
}

bool Situation::isAllLast() const
{
    return isAllLast_;
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

void Situation::setOpenMeldCount(std::uint8_t openMeldCount)
{
    openMeldCount_ = openMeldCount;
}

void Situation::setScores(const Scores& scores)
{
    scores_ = scores;
}

void Situation::setSelfRank(std::uint8_t selfRank)
{
    selfRank_ = selfRank;
}

void Situation::setPointGapToNextRank(int pointGapToNextRank)
{
    pointGapToNextRank_ = pointGapToNextRank;
}

void Situation::setPointGapFromLowerRank(int pointGapFromLowerRank)
{
    pointGapFromLowerRank_ = pointGapFromLowerRank;
}

void Situation::setRoundNumber(std::uint8_t roundNumber)
{
    roundNumber_ = roundNumber;
}

void Situation::setHonba(std::uint8_t honba)
{
    honba_ = honba;
}

void Situation::setReachSticks(std::uint8_t reachSticks)
{
    reachSticks_ = reachSticks;
}

void Situation::setAllLast(bool isAllLast)
{
    isAllLast_ = isAllLast;
}

void Situation::setVisibleTileCounts(const VisibleCounts& visibleTileCounts)
{
    visibleTileCounts_ = visibleTileCounts;
}

} // namespace mahjong::situation
