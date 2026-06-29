#include "SituationAnalyzer.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace mahjong::situation {

namespace {

constexpr std::uint8_t kTileTypeCount = 34;
constexpr std::int8_t kMaxShanten = 8;

struct MeldSearchResult {
    std::uint8_t melds {0};
    std::uint8_t taatsu {0};
};

bool isSuitStart(std::size_t index)
{
    return index < 27 && index % 9 <= 6;
}

void incrementVisibleCount(Situation::VisibleCounts& counts, world::TileType type)
{
    const auto index = world::toIndex(type);
    if (counts[index] < 4) {
        ++counts[index];
    }
}

// 面子と搭子の最大数を探索する。精密な評価の土台なので、貪欲ではなく分岐探索にする。
void searchMeldsAndTaatsu(world::Hand::Counts& counts, std::uint8_t melds, std::uint8_t taatsu, MeldSearchResult& best)
{
    std::size_t index = 0;
    while (index < counts.size() && counts[index] == 0) {
        ++index;
    }

    if (index == counts.size()) {
        if (melds + taatsu > best.melds + best.taatsu || (melds + taatsu == best.melds + best.taatsu && melds > best.melds)) {
            best.melds = melds;
            best.taatsu = taatsu;
        }
        return;
    }

    if (counts[index] >= 3) {
        counts[index] = static_cast<std::uint8_t>(counts[index] - 3);
        searchMeldsAndTaatsu(counts, static_cast<std::uint8_t>(melds + 1), taatsu, best);
        counts[index] = static_cast<std::uint8_t>(counts[index] + 3);
    }

    if (isSuitStart(index) && counts[index + 1] > 0 && counts[index + 2] > 0) {
        --counts[index];
        --counts[index + 1];
        --counts[index + 2];
        searchMeldsAndTaatsu(counts, static_cast<std::uint8_t>(melds + 1), taatsu, best);
        ++counts[index];
        ++counts[index + 1];
        ++counts[index + 2];
    }

    if (counts[index] >= 2) {
        counts[index] = static_cast<std::uint8_t>(counts[index] - 2);
        searchMeldsAndTaatsu(counts, melds, static_cast<std::uint8_t>(taatsu + 1), best);
        counts[index] = static_cast<std::uint8_t>(counts[index] + 2);
    }

    if (index < 27 && index % 9 <= 7 && counts[index + 1] > 0) {
        --counts[index];
        --counts[index + 1];
        searchMeldsAndTaatsu(counts, melds, static_cast<std::uint8_t>(taatsu + 1), best);
        ++counts[index];
        ++counts[index + 1];
    }

    if (isSuitStart(index) && counts[index + 2] > 0) {
        --counts[index];
        --counts[index + 2];
        searchMeldsAndTaatsu(counts, melds, static_cast<std::uint8_t>(taatsu + 1), best);
        ++counts[index];
        ++counts[index + 2];
    }

    --counts[index];
    searchMeldsAndTaatsu(counts, melds, taatsu, best);
    ++counts[index];
}

} // namespace

Situation SituationAnalyzer::analyze(const world::World& world, std::uint8_t selfPlayerIndex) const
{
    if (selfPlayerIndex >= world::World::kPlayerCount) {
        throw std::out_of_range("self player index must be in [0, 3]");
    }

    const world::Player& self = world.player(selfPlayerIndex);
    const std::int8_t shanten = calculateShanten(self);

    std::vector<std::uint8_t> opponentReachPlayers;
    for (std::uint8_t playerIndex = 0; playerIndex < world::World::kPlayerCount; ++playerIndex) {
        if (playerIndex != selfPlayerIndex && world.player(playerIndex).isReach()) {
            opponentReachPlayers.push_back(playerIndex);
        }
    }

    Situation situation;
    situation.setSelfPlayerIndex(selfPlayerIndex);
    situation.setShanten(shanten);
    situation.setHandSpeed(classifyHandSpeed(shanten));
    situation.setGamePhase(classifyGamePhase(world.remainingTileCount()));
    situation.setDealer(world.dealer() == selfPlayerIndex);
    situation.setSelfReach(self.isReach());
    situation.setOpponentReachPlayers(opponentReachPlayers);
    situation.setRemainingTileCount(world.remainingTileCount());
    situation.setDoraCount(countDora(self, world));
    situation.setPairCount(countPairs(self.hand().counts()));
    situation.setOpenMeldCount(static_cast<std::uint8_t>(self.openMelds().size()));
    situation.setVisibleTileCounts(buildVisibleTileCounts(world, selfPlayerIndex));

    return situation;
}

std::int8_t SituationAnalyzer::calculateShanten(const world::Player& player) const
{
    const auto& counts = player.hand().counts();
    const auto openMeldCount = static_cast<std::uint8_t>(player.openMelds().size());
    const std::int8_t standard = calculateStandardShanten(counts, openMeldCount);

    // 七対子は副露している場合は成立しないため、門前のときだけ候補に入れる。
    if (openMeldCount == 0) {
        return std::min(standard, calculateSevenPairsShanten(counts));
    }

    return standard;
}

std::int8_t SituationAnalyzer::calculateStandardShanten(const world::Hand::Counts& counts, std::uint8_t openMeldCount) const
{
    std::int8_t best = kMaxShanten;

    for (std::size_t pairIndex = 0; pairIndex <= kTileTypeCount; ++pairIndex) {
        world::Hand::Counts work = counts;
        const bool hasPair = pairIndex < kTileTypeCount && work[pairIndex] >= 2;
        if (hasPair) {
            work[pairIndex] = static_cast<std::uint8_t>(work[pairIndex] - 2);
        }

        MeldSearchResult result;
        searchMeldsAndTaatsu(work, 0, 0, result);

        std::uint8_t melds = static_cast<std::uint8_t>(result.melds + openMeldCount);
        std::uint8_t taatsu = result.taatsu;
        if (melds > 4) {
            melds = 4;
        }
        if (melds + taatsu > 4) {
            taatsu = static_cast<std::uint8_t>(4 - melds);
        }

        const std::int8_t pairValue = hasPair ? 1 : 0;
        const std::int8_t shanten = static_cast<std::int8_t>(8 - 2 * melds - taatsu - pairValue);
        best = std::min(best, shanten);
    }

    return best;
}

std::int8_t SituationAnalyzer::calculateSevenPairsShanten(const world::Hand::Counts& counts) const
{
    std::uint8_t pairs = 0;
    std::uint8_t uniqueTiles = 0;

    for (const auto count : counts) {
        if (count >= 2) {
            ++pairs;
        }
        if (count > 0) {
            ++uniqueTiles;
        }
    }

    const std::uint8_t missingUniqueTiles = uniqueTiles < 7 ? static_cast<std::uint8_t>(7 - uniqueTiles) : 0;
    return static_cast<std::int8_t>(6 - pairs + missingUniqueTiles);
}

std::uint8_t SituationAnalyzer::countPairs(const world::Hand::Counts& counts) const
{
    return static_cast<std::uint8_t>(std::count_if(counts.begin(), counts.end(), [](std::uint8_t count) {
        return count >= 2;
    }));
}

std::uint8_t SituationAnalyzer::countDora(const world::Player& player, const world::World& world) const
{
    std::uint8_t doraCount = 0;

    for (const world::Tile& dora : world.doraTiles()) {
        doraCount = static_cast<std::uint8_t>(doraCount + player.hand().count(dora.type()));
    }

    for (const world::OpenMeld& openMeld : player.openMelds()) {
        for (const world::Tile& tile : openMeld.tiles) {
            for (const world::Tile& dora : world.doraTiles()) {
                if (tile.type() == dora.type()) {
                    ++doraCount;
                }
            }
        }
    }

    return doraCount;
}

Situation::VisibleCounts SituationAnalyzer::buildVisibleTileCounts(const world::World& world, std::uint8_t selfPlayerIndex) const
{
    Situation::VisibleCounts visibleCounts {};

    const auto& selfCounts = world.player(selfPlayerIndex).hand().counts();
    for (std::size_t index = 0; index < selfCounts.size(); ++index) {
        visibleCounts[index] = selfCounts[index];
    }

    for (const world::Tile& dora : world.doraTiles()) {
        incrementVisibleCount(visibleCounts, dora.type());
    }

    for (std::uint8_t playerIndex = 0; playerIndex < world::World::kPlayerCount; ++playerIndex) {
        const world::Player& player = world.player(playerIndex);

        for (const world::RiverTile& discard : player.river().discards()) {
            incrementVisibleCount(visibleCounts, discard.tile.type());
        }

        for (const world::OpenMeld& openMeld : player.openMelds()) {
            for (const world::Tile& tile : openMeld.tiles) {
                incrementVisibleCount(visibleCounts, tile.type());
            }
        }
    }

    return visibleCounts;
}

HandSpeed SituationAnalyzer::classifyHandSpeed(std::int8_t shanten) const
{
    if (shanten <= 0) {
        return HandSpeed::Tenpai;
    }
    if (shanten == 1) {
        return HandSpeed::Fast;
    }
    if (shanten == 2) {
        return HandSpeed::Medium;
    }
    return HandSpeed::Slow;
}

GamePhase SituationAnalyzer::classifyGamePhase(std::uint8_t remainingTileCount) const
{
    if (remainingTileCount >= 50) {
        return GamePhase::Early;
    }
    if (remainingTileCount >= 30) {
        return GamePhase::Middle;
    }
    if (remainingTileCount >= 12) {
        return GamePhase::Late;
    }
    return GamePhase::Endgame;
}

} // namespace mahjong::situation
