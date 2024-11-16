#include "noise/voronoi.h"
#include "noise/noise.h"

#include "glm/geometric.hpp"
#include "glm/vec2.hpp"

#pragma warning(push)
#pragma warning(disable: 4100 4267 4611)
#include "png++/image.hpp"
#include "png++/rgb_pixel.hpp"
#pragma warning(pop)

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <ranges>
#include <stdexcept>

namespace noise::voronoi
{
    voronoi::voronoi(const std::uint32_t seed)
        : noise(seed), m_size(0, 0) {}

    void voronoi::generate(std::uint32_t width, std::uint32_t height, std::uint32_t, const std::uint64_t grid_size)
    {
        m_data.clear();
        m_data.resize(static_cast<std::size_t>(width) * height);
        m_size = {width, height};

        // Generate feature points for the Voronoi diagram
        for (std::uint32_t i = 0; i < grid_size; ++i)
            m_feature_points.emplace_back(random<float>() * static_cast<float>(m_size.x), random<float>() * static_cast<float>(m_size.y));

        // Calculate distances for each point in the grid
#pragma omp parallel for default(none) shared(width, height)
        for (std::int64_t y = 0; y < height; ++y)
        {
            for (std::int64_t x = 0; x < width; ++x)
            {
                const glm::vec2 point(static_cast<float>(x), static_cast<float>(y));
                m_data[y * width + x] = std::ranges::min(m_feature_points | std::views::transform([&point](const glm::vec2& feature_point)
                {
                    return glm::length(point - feature_point);
                }));
            }
        }
    }

    void voronoi::save(const std::filesystem::path& path)
    {
        if (path.extension() != ".png")
            throw std::runtime_error("Invalid file extension. Only PNG files are supported.");

        png::image<png::rgb_pixel> image(m_size.x, m_size.y);
        for (std::int64_t x = 0; x < m_size.x; ++x)
        {
            for (std::int64_t y = 0; y < m_size.y; ++y)
            {
                const float value = m_data[y * m_size.x + x];
                const std::uint8_t color = static_cast<std::uint8_t>(std::clamp(value, 0.f, 255.f));
                image[y][x] = png::rgb_pixel(color, color, color);
            }
        }

        image.write(path.generic_string());
    }
}
