#pragma once

#include "../situation/Situation.h"
#include "../world/Tile.h"
#include "../world/World.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace mahjong::safety {

// 牌単体の安全度。
// 状況補正はリーチのみを使い、点数、手出し/自摸切り、聴牌気配は見ない。
enum class SafetyLevel : std::uint8_t {
    VerySafe,
    Safe,
    Neutral,
    Dangerous,
    VeryDangerous
};

struct TileSafety {
    world::TileType tileType {world::TileType::M1};
    std::uint8_t visibleCount {0};
    std::uint8_t remainingCount {4};
    std::uint8_t possibleMentsuPatternCount {0};
    std::uint8_t maxMentsuPatternCount {0};
    std::uint8_t possibleHeadPatternCount {0};
    std::uint8_t maxHeadPatternCount {1};
    std::uint16_t possiblePatternWeight {0};
    std::uint16_t maxPatternWeight {0};
    std::uint8_t opponentReachCount {0};
    bool underReachPressure {false};
    double structuralDangerScore {0.0};
    double dangerScore {0.0};
    double safetyScore {1.0};
    SafetyLevel level {SafetyLevel::VerySafe};
    std::vector<std::string> possiblePatterns {};
    std::vector<std::string> safetyReasons {};
};

struct OpponentTileSafety {
    std::uint8_t playerIndex {0};
    bool genbutsu {false};
    bool suji {false};
    double dangerMultiplier {1.0};
    std::vector<std::string> reasons {};
};

class TileSafetyEvaluator {
public:
    using SafetyTable = std::array<TileSafety, 34>;
    using OpponentSafetyTable = std::array<OpponentTileSafety, world::World::kPlayerCount>;

    [[nodiscard]] TileSafety evaluate(
        const situation::Situation::VisibleCounts& visibleCounts,
        world::TileType tileType) const;
    [[nodiscard]] TileSafety evaluate(const situation::Situation& situation, world::TileType tileType) const;
    [[nodiscard]] TileSafety evaluate(
        const situation::Situation& situation,
        const world::World& world,
        std::uint8_t selfPlayerIndex,
        world::TileType tileType) const;
    [[nodiscard]] SafetyTable evaluateAll(const situation::Situation& situation) const;
    [[nodiscard]] SafetyTable evaluateAll(
        const situation::Situation& situation,
        const world::World& world,
        std::uint8_t selfPlayerIndex) const;
    [[nodiscard]] OpponentSafetyTable evaluateAgainstOpponents(
        const world::World& world,
        std::uint8_t selfPlayerIndex,
        world::TileType tileType) const;

private:
    [[nodiscard]] TileSafety evaluateWithRiverConstraints(
        const situation::Situation::VisibleCounts& visibleCounts,
        world::TileType tileType,
        const world::River* river) const;
    [[nodiscard]] std::uint8_t countVisible(
        const situation::Situation::VisibleCounts& visibleCounts,
        world::TileType tileType) const;
    [[nodiscard]] std::uint8_t countRemaining(
        const situation::Situation::VisibleCounts& visibleCounts,
        world::TileType tileType) const;
    [[nodiscard]] std::uint8_t maxPatternCount(world::TileType tileType) const;
    [[nodiscard]] std::uint16_t maxPatternWeight(world::TileType tileType) const;
    [[nodiscard]] bool isGenbutsu(const world::River& river, world::TileType tileType) const;
    [[nodiscard]] bool isSuji(const world::River& river, world::TileType tileType) const;
    [[nodiscard]] bool isSequenceBlockedBySuji(
        const world::River& river,
        world::TileType tileType,
        std::size_t sequenceStartIndex) const;
    [[nodiscard]] bool hasDiscard(const world::River& river, world::TileType tileType) const;
    [[nodiscard]] SafetyLevel classify(double dangerScore) const;
    [[nodiscard]] std::string sequencePatternName(std::size_t startIndex) const;
    [[nodiscard]] std::string patternNameWithWeight(const std::string& patternName, std::uint16_t weight) const;
};

[[nodiscard]] const char* toString(SafetyLevel level);

} // namespace mahjong::safety
