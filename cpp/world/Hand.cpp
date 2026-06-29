#include "Hand.h"

#include <algorithm>
#include <numeric>
#include <stdexcept>

namespace mahjong::world {

namespace {

constexpr std::uint8_t kMaxCopiesPerTileType = 4;

void validateCount(std::uint8_t count)
{
    if (count > kMaxCopiesPerTileType) {
        throw std::invalid_argument("a hand cannot contain more than four copies of one tile type");
    }
}

} // namespace

Hand::Hand()
{
    clear();
}

std::uint8_t Hand::count(TileType type) const
{
    return counts_.at(toIndex(type));
}

std::uint8_t Hand::count(const Tile& tile) const
{
    return count(tile.type());
}

std::uint8_t Hand::countByIndex(std::size_t index) const
{
    return counts_.at(index);
}

std::uint16_t Hand::totalCount() const
{
    return std::accumulate(counts_.begin(), counts_.end(), std::uint16_t {0});
}

bool Hand::empty() const
{
    return totalCount() == 0;
}

const Hand::Counts& Hand::counts() const
{
    return counts_;
}

std::vector<Tile> Hand::tiles() const
{
    std::vector<Tile> result;
    result.reserve(totalCount());

    for (std::size_t index = 0; index < counts_.size(); ++index) {
        const TileType type = tileTypeFromIndex(static_cast<std::uint8_t>(index));
        for (std::uint8_t copy = 0; copy < counts_[index]; ++copy) {
            result.emplace_back(type);
        }
    }

    return result;
}

void Hand::clear()
{
    counts_.fill(0);
}

void Hand::setCount(TileType type, std::uint8_t count)
{
    validateCount(count);
    counts_.at(toIndex(type)) = count;
}

void Hand::setCounts(const Counts& counts)
{
    for (const std::uint8_t count : counts) {
        validateCount(count);
    }
    counts_ = counts;
}

bool Hand::add(TileType type, std::uint8_t amount)
{
    const std::uint8_t index = toIndex(type);
    if (counts_[index] + amount > kMaxCopiesPerTileType) {
        return false;
    }
    counts_[index] = static_cast<std::uint8_t>(counts_[index] + amount);
    return true;
}

bool Hand::add(const Tile& tile, std::uint8_t amount)
{
    return add(tile.type(), amount);
}

bool Hand::remove(TileType type, std::uint8_t amount)
{
    const std::uint8_t index = toIndex(type);
    if (counts_[index] < amount) {
        return false;
    }
    counts_[index] = static_cast<std::uint8_t>(counts_[index] - amount);
    return true;
}

bool Hand::remove(const Tile& tile, std::uint8_t amount)
{
    return remove(tile.type(), amount);
}

} // namespace mahjong::world
