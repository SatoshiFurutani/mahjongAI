#pragma once

#include "ObservationDto.h"
#include "World.h"

#include <cstdint>
#include <vector>

namespace mahjong::world {

struct WorldConversionResult {
    World world {};
    std::uint8_t selfPlayer {0};
    std::vector<LegalActionDto> legalActions {};
};

// Python側で作ったDTOをC++内部のWorldへ変換する。
// MJXのPython型には依存せず、変換境界をこのクラスに閉じ込める。
class MJXWorldConverter {
public:
    [[nodiscard]] WorldConversionResult convert(const ObservationDto& observation) const;
    [[nodiscard]] World toWorld(const ObservationDto& observation) const;

private:
    [[nodiscard]] Tile toTile(const TileDto& tileDto) const;
    [[nodiscard]] Hand toHand(const std::vector<TileDto>& tileDtos) const;
    [[nodiscard]] River toRiver(const std::vector<RiverTileDto>& riverDtos) const;
    [[nodiscard]] OpenMeld toOpenMeld(const OpenMeldDto& openMeldDto) const;
    [[nodiscard]] OpenType toOpenType(OpenTypeDto openTypeDto) const;

    void validateObservation(const ObservationDto& observation) const;
    void validatePlayerIndex(std::uint8_t playerIndex) const;
};

} // namespace mahjong::world
