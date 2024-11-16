#include "noise/perlin.h"

#include "glm/geometric.hpp"

#pragma warning(push)
#pragma warning(disable: 4100 4267 4611)
#include "png++/image.hpp"
#include "png++/rgb_pixel.hpp"
#pragma warning(pop)

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <numbers>
#include <stdexcept>
#include <vector>

namespace noise::perlin
{
    perlin::perlin(const std::uint32_t seed)
        : noise(seed), m_size(0, 0) {}

    void perlin::generate(const std::uint32_t width, const std::uint32_t height, const std::uint32_t passes, const std::uint64_t grid_size)
    {
        // Reset state
        m_data.clear();
        m_data.resize(static_cast<std::size_t>(width) * height);
        m_gradients.clear();
        m_gradients.resize(static_cast<std::size_t>(width) * height);
        m_size = {width, height};

        // Generate the random gradients (do not generate in parallel as it will not be reproducible with the same seed)
        {
            for (std::int64_t y = 0; y < height; ++y)
            {
                for (std::int64_t x = 0; x < width; ++x)
                {
                    const float theta = std::acos(2 * random<float>() - 1);
                    const float phi = 2.f * random<float>() * std::numbers::pi_v<float>;
                    m_gradients[y * width + x] = glm::vec2(std::cos(phi) * std::sin(theta), std::sin(phi) * std::sin(theta));
                }
            }

            // Calculate each element in the grid for all octaves
#pragma omp parallel for default(none) shared(width, height, passes, grid_size)
            for (std::int64_t x = 0; x < width; ++x)
            {
                for (std::int64_t y = 0; y < height; ++y)
                {
                    // Calculate for each octave
                    float frequency = 1.f;
                    float amplitude = 1.f;
                    float value = 0.f;
                    for (std::uint64_t i = 0; i < passes; ++i)
                    {
                        const glm::vec2 current(static_cast<float>(x) * frequency / static_cast<float>(grid_size),
                                                static_cast<float>(y) * frequency / static_cast<float>(grid_size));
                        value += compute_element(current) * amplitude;
                        frequency *= 2.f;
                        amplitude /= 2.f;
                    }

                    m_data[y * width + x] = value;
                }
            }
        }
    }

    void perlin::save(const std::filesystem::path& path)
    {
        if (path.extension() != ".png")
            throw std::runtime_error("Invalid file extension. Only PNG files are supported.");

        png::image<png::rgb_pixel> image(m_size.x, m_size.y);
        for (std::int64_t x = 0; x < m_size.x; ++x)
        {
            for (std::int64_t y = 0; y < m_size.y; ++y)
            {
                float value = m_data[y * m_size.x + x];

                value *= 1.2f; // contrast adjustment
                value = value + 1 / 2.f; // convert from [-1;1] to [0;1]
                value *= 255.f; // convert from [0;1] to [0;255]

                const std::uint8_t color = static_cast<std::uint8_t>(std::clamp(value, 0.f, 255.f));
                image[y][x] = png::rgb_pixel(color, color, color);
            }
        }

        image.write(path.generic_string());
    }

    float perlin::interpolate(const float start, const float end, const float weight)
    {
        return (end - start) * (3.f - weight * 2.f) * weight * weight + start;
    }

    float perlin::calculate_influence(const glm::vec2& lhs, const glm::vec2& rhs) const
    {
        const std::size_t idx = static_cast<std::size_t>(lhs.y) * m_size.x + static_cast<std::size_t>(lhs.x);
        if (idx >= m_gradients.size())
            return 0.f;

        const glm::vec2& gradient = m_gradients[idx];
        const glm::vec2 distance = rhs - lhs;
        return glm::dot(gradient, distance);
    }

    float perlin::compute_element(const glm::vec2& current) const
    {
        // Get the corners
        const std::uint64_t left = static_cast<std::uint64_t>(current.x);
        const std::uint64_t right = left + 1;
        const std::uint64_t up = static_cast<std::uint64_t>(current.y);
        const std::uint64_t down = up + 1;

        // Calculate the influence of each corner
        const float left_up = calculate_influence({left, up}, current);
        const float right_up = calculate_influence({right, up}, current);
        const float left_down = calculate_influence({left, down}, current);
        const float right_down = calculate_influence({right, down}, current);

        // Interpolate the influence
        const float first = interpolate(left_up, right_up, current.x - static_cast<float>(left));
        const float second = interpolate(left_down, right_down, current.x - static_cast<float>(left));
        return interpolate(first, second, current.y - static_cast<float>(up));
    }
}
