#include "World.h"

#include <stdexcept>

namespace mahjong::world {

namespace {

void validatePlayerIndex(std::uint8_t playerIndex)
{
    if (playerIndex >= World::kPlayerCount) {
        throw std::out_of_range("player index must be in [0, 3]");
    }
}

} // namespace

const World::Players& World::players() const
{
    return players_;
}

World::Players& World::players()
{
    return players_;
}

const Player& World::player(std::uint8_t playerIndex) const
{
    validatePlayerIndex(playerIndex);
    return players_[playerIndex];
}

Player& World::player(std::uint8_t playerIndex)
{
    validatePlayerIndex(playerIndex);
    return players_[playerIndex];
}

const std::vector<Tile>& World::doraTiles() const
{
    return doraTiles_;
}

std::uint8_t World::roundNumber() const
{
    return roundNumber_;
}

std::uint8_t World::honba() const
{
    return honba_;
}

std::uint8_t World::reachSticks() const
{
    return reachSticks_;
}

std::uint8_t World::dealer() const
{
    return dealer_;
}

std::uint8_t World::turn() const
{
    return turn_;
}

std::uint8_t World::remainingTileCount() const
{
    return remainingTileCount_;
}

void World::setPlayers(const Players& players)
{
    players_ = players;
}

void World::setPlayer(std::uint8_t playerIndex, const Player& player)
{
    validatePlayerIndex(playerIndex);
    players_[playerIndex] = player;
}

void World::setDoraTiles(const std::vector<Tile>& doraTiles)
{
    doraTiles_ = doraTiles;
}

void World::addDoraTile(const Tile& tile)
{
    doraTiles_.push_back(tile);
}

void World::clearDoraTiles()
{
    doraTiles_.clear();
}

void World::setRoundNumber(std::uint8_t roundNumber)
{
    roundNumber_ = roundNumber;
}

void World::setHonba(std::uint8_t honba)
{
    honba_ = honba;
}

void World::setReachSticks(std::uint8_t reachSticks)
{
    reachSticks_ = reachSticks;
}

void World::setDealer(std::uint8_t dealer)
{
    validatePlayerIndex(dealer);
    dealer_ = dealer;
}

void World::setTurn(std::uint8_t turn)
{
    turn_ = turn;
}

void World::setRemainingTileCount(std::uint8_t remainingTileCount)
{
    remainingTileCount_ = remainingTileCount;
}

} // namespace mahjong::world
