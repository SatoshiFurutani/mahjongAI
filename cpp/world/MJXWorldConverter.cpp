#include "MJXWorldConverter.h"

#include <stdexcept>

namespace mahjong::world {

WorldConversionResult MJXWorldConverter::convert(const ObservationDto& observation) const
{
    validateObservation(observation);

    World world;
    world.setRoundNumber(observation.roundNumber);
    world.setHonba(observation.honba);
    world.setReachSticks(observation.reachSticks);
    world.setDealer(observation.dealer);
    world.setRemainingTileCount(observation.remainingTileCount);

    std::vector<Tile> doraTiles;
    doraTiles.reserve(observation.doraIndicators.size());
    for (const TileDto& dora : observation.doraIndicators) {
        doraTiles.push_back(toTile(dora));
    }
    world.setDoraTiles(doraTiles);

    for (std::uint8_t playerIndex = 0; playerIndex < World::kPlayerCount; ++playerIndex) {
        const PlayerObservationDto& playerDto = observation.players[playerIndex];

        Player player;
        player.setScore(observation.scores[playerIndex]);
        player.setReach(playerDto.reach);
        player.setIppatsu(playerDto.ippatsu);
        player.setDrawOrder(playerDto.drawOrder);
        player.setRiver(toRiver(playerDto.river));

        for (const OpenMeldDto& openMeldDto : playerDto.openMelds) {
            player.addOpenMeld(toOpenMeld(openMeldDto));
        }

        if (playerIndex == observation.selfPlayer) {
            player.setHand(toHand(observation.selfHand));
        }

        world.setPlayer(playerIndex, player);
    }

    return WorldConversionResult {
        world,
        observation.selfPlayer,
        observation.legalActions
    };
}

World MJXWorldConverter::toWorld(const ObservationDto& observation) const
{
    return convert(observation).world;
}

Tile MJXWorldConverter::toTile(const TileDto& tileDto) const
{
    return Tile(tileTypeFromIndex(tileDto.type), tileDto.red);
}

Hand MJXWorldConverter::toHand(const std::vector<TileDto>& tileDtos) const
{
    Hand hand;
    for (const TileDto& tileDto : tileDtos) {
        if (!hand.add(toTile(tileDto))) {
            throw std::invalid_argument("self hand contains more than four copies of one tile type");
        }
    }
    return hand;
}

River MJXWorldConverter::toRiver(const std::vector<RiverTileDto>& riverDtos) const
{
    River river;
    for (const RiverTileDto& riverTileDto : riverDtos) {
        river.addDiscard(toTile(riverTileDto.tile), riverTileDto.afterReach, riverTileDto.tsumogiri);
        if (riverTileDto.called) {
            const std::size_t lastIndex = river.size() - 1;
            river.markCalled(lastIndex);
        }
    }
    return river;
}

OpenMeld MJXWorldConverter::toOpenMeld(const OpenMeldDto& openMeldDto) const
{
    validatePlayerIndex(openMeldDto.fromPlayer);

    OpenMeld openMeld;
    openMeld.type = toOpenType(openMeldDto.type);
    openMeld.fromPlayer = openMeldDto.fromPlayer;
    openMeld.tiles.reserve(openMeldDto.tiles.size());
    for (const TileDto& tileDto : openMeldDto.tiles) {
        openMeld.tiles.push_back(toTile(tileDto));
    }
    return openMeld;
}

OpenType MJXWorldConverter::toOpenType(OpenTypeDto openTypeDto) const
{
    switch (openTypeDto) {
    case OpenTypeDto::Chi:
        return OpenType::Chi;
    case OpenTypeDto::Pon:
        return OpenType::Pon;
    case OpenTypeDto::Daiminkan:
        return OpenType::Daiminkan;
    case OpenTypeDto::Ankan:
        return OpenType::Ankan;
    case OpenTypeDto::Kakan:
        return OpenType::Kakan;
    }

    throw std::invalid_argument("unknown open meld type");
}

void MJXWorldConverter::validateObservation(const ObservationDto& observation) const
{
    validatePlayerIndex(observation.selfPlayer);
    validatePlayerIndex(observation.dealer);

    for (const TileDto& dora : observation.doraIndicators) {
        (void)toTile(dora);
    }

    for (const TileDto& tile : observation.selfHand) {
        (void)toTile(tile);
    }

    for (const LegalActionDto& action : observation.legalActions) {
        if (action.hasTile) {
            (void)toTile(action.tile);
        }
    }

    for (const PlayerObservationDto& player : observation.players) {
        for (const RiverTileDto& riverTile : player.river) {
            (void)toTile(riverTile.tile);
        }
        for (const OpenMeldDto& openMeld : player.openMelds) {
            validatePlayerIndex(openMeld.fromPlayer);
            for (const TileDto& tile : openMeld.tiles) {
                (void)toTile(tile);
            }
        }
    }
}

void MJXWorldConverter::validatePlayerIndex(std::uint8_t playerIndex) const
{
    if (playerIndex >= World::kPlayerCount) {
        throw std::out_of_range("player index must be in [0, 3]");
    }
}

} // namespace mahjong::world
