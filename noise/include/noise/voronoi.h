#pragma once

#include "noise.h"

#include "glm/vec2.hpp"

#include <filesystem>
#include <vector>

namespace noise::voronoi
{
    /**
     * \brief A voronoi noise generator.
     */
    class voronoi final : public noise
    {
    public:
        /**
         * \brief Constructs a voronoi noise generator.
         * \param seed The seed used to generate the noise.
         */
        explicit voronoi(std::uint32_t seed);

        /// \copydoc noise::generate
        void generate(std::uint32_t width, std::uint32_t height, std::uint32_t, std::uint64_t grid_size) override;

        /// \copydoc noise::save
        void save(const std::filesystem::path& path) override;

    private:
        glm::vec<2, std::uint32_t> m_size;
        std::vector<glm::vec2> m_feature_points;
        std::vector<float> m_data;
    };
}
