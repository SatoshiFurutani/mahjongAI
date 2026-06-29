#include "TileSafetyEvaluator.h"

#include <algorithm>
#include <stdexcept>

namespace mahjong::safety {

namespace {

constexpr std::uint8_t kTileTypeCount = 34;
constexpr std::uint8_t kCopiesPerTileType = 4;
constexpr std::uint8_t kMaxHeadWeight = 4;
constexpr std::uint8_t kMaxTripletWeight = 6;
constexpr std::uint8_t kMaxSequenceWeight = 16;

bool isNumberTileIndex(std::size_t index)
{
    return index < 27;
}

bool isValidSequenceStart(std::size_t startIndex)
{
    return isNumberTileIndex(startIndex) && startIndex % 9 <= 6;
}

bool isSameSuitSequence(std::size_t startIndex, std::size_t tileIndex)
{
    return isValidSequenceStart(startIndex)
        && startIndex <= tileIndex
        && tileIndex <= startIndex + 2
        && startIndex / 9 == tileIndex / 9;
}

} // namespace

TileSafety TileSafetyEvaluator::evaluate(
    const situation::Situation::VisibleCounts& visibleCounts,
    world::TileType tileType) const
{
    return evaluateWithRiverConstraints(visibleCounts, tileType, nullptr);
}

TileSafety TileSafetyEvaluator::evaluateWithRiverConstraints(
    const situation::Situation::VisibleCounts& visibleCounts,
    world::TileType tileType,
    const world::River* river) const
{
    const std::size_t tileIndex = world::toIndex(tileType);

    TileSafety result;
    result.tileType = tileType;
    result.visibleCount = countVisible(visibleCounts, tileType);
    result.remainingCount = countRemaining(visibleCounts, tileType);
    result.maxMentsuPatternCount = maxPatternCount(tileType);
    result.maxPatternWeight = maxPatternWeight(tileType);

    if (river != nullptr && isGenbutsu(*river, tileType)) {
        result.structuralDangerScore = 0.0;
        result.dangerScore = 0.0;
        result.safetyScore = 1.0;
        result.level = classify(result.dangerScore);
        result.safetyReasons.push_back("現物のため全パターンを除外");
        return result;
    }

    // 雀頭: 単騎待ちを想定し、放銃牌と同種牌が1枚残り得るなら可能形とする。
    if (result.remainingCount >= 1) {
        ++result.possibleHeadPatternCount;
        result.possiblePatternWeight = static_cast<std::uint16_t>(result.possiblePatternWeight + result.remainingCount);
        result.possiblePatterns.push_back(patternNameWithWeight(
            world::toString(tileType) + "-" + world::toString(tileType),
            result.remainingCount));
    }

    // 刻子: 放銃牌を含めて刻子になるには、相手側に同種牌が2枚残り得る必要がある。
    if (result.remainingCount >= 2) {
        const std::uint16_t tripletWeight = static_cast<std::uint16_t>(result.remainingCount * (result.remainingCount - 1) / 2);
        ++result.possibleMentsuPatternCount;
        result.possiblePatternWeight = static_cast<std::uint16_t>(result.possiblePatternWeight + tripletWeight);
        result.possiblePatterns.push_back(patternNameWithWeight(
            world::toString(tileType) + "-" + world::toString(tileType) + "-" + world::toString(tileType),
            tripletWeight));
    }

    // 順子: 残り2種の枚数の積を重みとする。
    // 例: 456で4と6が1枚ずつ残りなら重み1、どちらも4枚残りなら重み16。
    if (world::isNumberTile(tileType)) {
        const std::size_t firstStart = tileIndex >= 2 ? tileIndex - 2 : 0;
        for (std::size_t startIndex = firstStart; startIndex <= tileIndex; ++startIndex) {
            if (!isSameSuitSequence(startIndex, tileIndex)) {
                continue;
            }

            if (river != nullptr && isSequenceBlockedBySuji(*river, tileType, startIndex)) {
                result.safetyReasons.push_back(sequencePatternName(startIndex) + " は筋により除外");
                continue;
            }

            std::uint16_t sequenceWeight = 1;
            for (std::size_t offset = 0; offset < 3; ++offset) {
                const std::size_t currentIndex = startIndex + offset;
                if (currentIndex == tileIndex) {
                    continue;
                }

                const auto requiredType = world::tileTypeFromIndex(static_cast<std::uint8_t>(currentIndex));
                sequenceWeight = static_cast<std::uint16_t>(sequenceWeight * countRemaining(visibleCounts, requiredType));
            }

            if (sequenceWeight > 0) {
                ++result.possibleMentsuPatternCount;
                result.possiblePatternWeight = static_cast<std::uint16_t>(result.possiblePatternWeight + sequenceWeight);
                result.possiblePatterns.push_back(patternNameWithWeight(sequencePatternName(startIndex), sequenceWeight));
            }
        }
    }

    if (result.maxPatternWeight > 0) {
        result.structuralDangerScore = static_cast<double>(result.possiblePatternWeight)
            / static_cast<double>(result.maxPatternWeight);
    }
    result.dangerScore = result.structuralDangerScore;
    result.safetyScore = 1.0 - result.dangerScore;
    result.level = classify(result.dangerScore);

    return result;
}

TileSafety TileSafetyEvaluator::evaluate(const situation::Situation& situation, world::TileType tileType) const
{
    TileSafety result = evaluate(situation.visibleTileCounts(), tileType);
    result.opponentReachCount = situation.opponentReachCount();
    result.underReachPressure = result.opponentReachCount > 0;

    // 状況補正はリーチのみ。複数リーチ時は構造的危険度を少し強める。
    if (result.underReachPressure) {
        const double reachMultiplier = 1.0 + 0.25 * static_cast<double>(result.opponentReachCount - 1);
        result.dangerScore = std::min(1.0, result.dangerScore * reachMultiplier);
        result.safetyScore = 1.0 - result.dangerScore;
        result.level = classify(result.dangerScore);
    }

    return result;
}

TileSafety TileSafetyEvaluator::evaluate(
    const situation::Situation& situation,
    const world::World& world,
    std::uint8_t selfPlayerIndex,
    world::TileType tileType) const
{
    if (selfPlayerIndex >= world::World::kPlayerCount) {
        throw std::out_of_range("self player index must be in [0, 3]");
    }

    TileSafety result = evaluate(situation, tileType);

    double dangerScoreSum = 0.0;
    std::uint8_t opponentCount = 0;
    for (std::uint8_t playerIndex = 0; playerIndex < world::World::kPlayerCount; ++playerIndex) {
        if (playerIndex == selfPlayerIndex) {
            continue;
        }

        const world::River& river = world.player(playerIndex).river();
        TileSafety opponentResult = evaluateWithRiverConstraints(situation.visibleTileCounts(), tileType, &river);
        dangerScoreSum += opponentResult.structuralDangerScore;
        ++opponentCount;

        for (const std::string& reason : opponentResult.safetyReasons) {
            result.safetyReasons.push_back("P" + std::to_string(playerIndex) + ": " + reason);
        }
    }

    if (opponentCount > 0) {
        result.dangerScore = dangerScoreSum / static_cast<double>(opponentCount);
        result.structuralDangerScore = result.dangerScore;

        if (result.underReachPressure) {
            const double reachMultiplier = 1.0 + 0.25 * static_cast<double>(result.opponentReachCount - 1);
            result.dangerScore = std::min(1.0, result.dangerScore * reachMultiplier);
        }

        result.safetyScore = 1.0 - result.dangerScore;
        result.level = classify(result.dangerScore);
    }

    return result;
}

TileSafetyEvaluator::SafetyTable TileSafetyEvaluator::evaluateAll(const situation::Situation& situation) const
{
    SafetyTable table {};
    for (std::uint8_t index = 0; index < kTileTypeCount; ++index) {
        const world::TileType tileType = world::tileTypeFromIndex(index);
        table[index] = evaluate(situation, tileType);
    }
    return table;
}

TileSafetyEvaluator::SafetyTable TileSafetyEvaluator::evaluateAll(
    const situation::Situation& situation,
    const world::World& world,
    std::uint8_t selfPlayerIndex) const
{
    SafetyTable table {};
    for (std::uint8_t index = 0; index < kTileTypeCount; ++index) {
        const world::TileType tileType = world::tileTypeFromIndex(index);
        table[index] = evaluate(situation, world, selfPlayerIndex, tileType);
    }
    return table;
}

TileSafetyEvaluator::OpponentSafetyTable TileSafetyEvaluator::evaluateAgainstOpponents(
    const world::World& world,
    std::uint8_t selfPlayerIndex,
    world::TileType tileType) const
{
    if (selfPlayerIndex >= world::World::kPlayerCount) {
        throw std::out_of_range("self player index must be in [0, 3]");
    }

    OpponentSafetyTable table {};
    for (std::uint8_t playerIndex = 0; playerIndex < world::World::kPlayerCount; ++playerIndex) {
        OpponentTileSafety safety;
        safety.playerIndex = playerIndex;

        if (playerIndex == selfPlayerIndex) {
            safety.dangerMultiplier = 0.0;
            table[playerIndex] = safety;
            continue;
        }

        const world::River& river = world.player(playerIndex).river();
        safety.genbutsu = isGenbutsu(river, tileType);
        safety.suji = !safety.genbutsu && isSuji(river, tileType);

        if (safety.genbutsu) {
            safety.dangerMultiplier = 0.0;
            safety.reasons.push_back("現物");
        } else if (safety.suji) {
            safety.dangerMultiplier = 1.0;
            safety.reasons.push_back("筋により一部の順子パターンを除外");
        }

        table[playerIndex] = safety;
    }

    return table;
}

std::uint8_t TileSafetyEvaluator::countVisible(
    const situation::Situation::VisibleCounts& visibleCounts,
    world::TileType tileType) const
{
    return std::min(visibleCounts.at(world::toIndex(tileType)), kCopiesPerTileType);
}

std::uint8_t TileSafetyEvaluator::countRemaining(
    const situation::Situation::VisibleCounts& visibleCounts,
    world::TileType tileType) const
{
    return static_cast<std::uint8_t>(kCopiesPerTileType - countVisible(visibleCounts, tileType));
}

std::uint8_t TileSafetyEvaluator::maxPatternCount(world::TileType tileType) const
{
    constexpr std::uint8_t kTripletPatternCount = 1;

    if (!world::isNumberTile(tileType)) {
        return kTripletPatternCount;
    }

    const std::size_t tileIndex = world::toIndex(tileType);
    std::uint8_t sequenceCount = 0;
    const std::size_t firstStart = tileIndex >= 2 ? tileIndex - 2 : 0;
    for (std::size_t startIndex = firstStart; startIndex <= tileIndex; ++startIndex) {
        if (isSameSuitSequence(startIndex, tileIndex)) {
            ++sequenceCount;
        }
    }

    return static_cast<std::uint8_t>(kTripletPatternCount + sequenceCount);
}

std::uint16_t TileSafetyEvaluator::maxPatternWeight(world::TileType tileType) const
{
    if (!world::isNumberTile(tileType)) {
        return kMaxHeadWeight + kMaxTripletWeight;
    }

    const std::size_t tileIndex = world::toIndex(tileType);
    std::uint8_t sequenceCount = 0;
    const std::size_t firstStart = tileIndex >= 2 ? tileIndex - 2 : 0;
    for (std::size_t startIndex = firstStart; startIndex <= tileIndex; ++startIndex) {
        if (isSameSuitSequence(startIndex, tileIndex)) {
            ++sequenceCount;
        }
    }

    return static_cast<std::uint16_t>(
        kMaxHeadWeight + kMaxTripletWeight + sequenceCount * kMaxSequenceWeight);
}

bool TileSafetyEvaluator::isGenbutsu(const world::River& river, world::TileType tileType) const
{
    return hasDiscard(river, tileType);
}

bool TileSafetyEvaluator::isSuji(const world::River& river, world::TileType tileType) const
{
    if (!world::isNumberTile(tileType)) {
        return false;
    }

    const std::uint8_t tileIndex = world::toIndex(tileType);
    const std::uint8_t number = static_cast<std::uint8_t>(tileIndex % 9 + 1);

    if (number <= 6) {
        const auto upperSuji = world::tileTypeFromIndex(static_cast<std::uint8_t>(tileIndex + 3));
        if (hasDiscard(river, upperSuji)) {
            return true;
        }
    }

    if (number >= 4) {
        const auto lowerSuji = world::tileTypeFromIndex(static_cast<std::uint8_t>(tileIndex - 3));
        if (hasDiscard(river, lowerSuji)) {
            return true;
        }
    }

    return false;
}

bool TileSafetyEvaluator::isSequenceBlockedBySuji(
    const world::River& river,
    world::TileType tileType,
    std::size_t sequenceStartIndex) const
{
    if (!world::isNumberTile(tileType)) {
        return false;
    }

    const std::size_t tileIndex = world::toIndex(tileType);
    if (!isSameSuitSequence(sequenceStartIndex, tileIndex)) {
        return false;
    }

    const std::size_t offset = tileIndex - sequenceStartIndex;
    if (offset == 0) {
        const std::size_t sujiIndex = tileIndex + 3;
        if (sujiIndex < 27 && sujiIndex / 9 == tileIndex / 9) {
            return hasDiscard(river, world::tileTypeFromIndex(static_cast<std::uint8_t>(sujiIndex)));
        }
    }

    if (offset == 2) {
        if (tileIndex >= 3) {
            const std::size_t sujiIndex = tileIndex - 3;
            if (sujiIndex / 9 == tileIndex / 9) {
                return hasDiscard(river, world::tileTypeFromIndex(static_cast<std::uint8_t>(sujiIndex)));
            }
        }
    }

    return false;
}

bool TileSafetyEvaluator::hasDiscard(const world::River& river, world::TileType tileType) const
{
    return std::any_of(river.discards().begin(), river.discards().end(), [tileType](const world::RiverTile& discard) {
        return discard.tile.type() == tileType;
    });
}

SafetyLevel TileSafetyEvaluator::classify(double dangerScore) const
{
    if (dangerScore <= 0.0) {
        return SafetyLevel::VerySafe;
    }
    if (dangerScore <= 0.25) {
        return SafetyLevel::Safe;
    }
    if (dangerScore <= 0.50) {
        return SafetyLevel::Neutral;
    }
    if (dangerScore <= 0.75) {
        return SafetyLevel::Dangerous;
    }
    return SafetyLevel::VeryDangerous;
}

std::string TileSafetyEvaluator::sequencePatternName(std::size_t startIndex) const
{
    const auto first = world::tileTypeFromIndex(static_cast<std::uint8_t>(startIndex));
    const auto second = world::tileTypeFromIndex(static_cast<std::uint8_t>(startIndex + 1));
    const auto third = world::tileTypeFromIndex(static_cast<std::uint8_t>(startIndex + 2));
    return world::toString(first) + "-" + world::toString(second) + "-" + world::toString(third);
}

std::string TileSafetyEvaluator::patternNameWithWeight(const std::string& patternName, std::uint16_t weight) const
{
    return patternName + " x" + std::to_string(weight);
}

const char* toString(SafetyLevel level)
{
    switch (level) {
    case SafetyLevel::VerySafe:
        return "VerySafe";
    case SafetyLevel::Safe:
        return "Safe";
    case SafetyLevel::Neutral:
        return "Neutral";
    case SafetyLevel::Dangerous:
        return "Dangerous";
    case SafetyLevel::VeryDangerous:
        return "VeryDangerous";
    }
    return "Unknown";
}

} // namespace mahjong::safety
