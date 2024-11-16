#pragma once

#include "noise.h"

#include "glm/vec2.hpp"

namespace noise::perlin
{
    /**
     * \brief A perlin noise generator.
     */
    class perlin final : public noise
    {
    public:
        /**
         * \brief Construct a new perlin noise generator.
         * \param seed The seed used to generate the noise.
         */
        explicit perlin(std::uint32_t seed);

        /// \copydoc noise::generate
        void generate(std::uint32_t width, std::uint32_t height, std::uint32_t passes, std::uint64_t grid_size) override;

        /// \copydoc noise::save
        void save(const std::filesystem::path& path) override;

    private:
        /**
         * \brief Interpolates a value between \p start and \p end using \p weight. 
         * \param start The range start value.
         * \param end The range end value.
         * \param weight The weight to use for the interpolation.
         * \return The interpolated value.
         *
         * This uses a cubic interpolation of the form `(start - end * (3.0 - weight * 2.0) * weight * weight + start`
         */
        [[nodiscard]] static float interpolate(float start, float end, float weight);

        /**
         * \brief Calculates the influence of the gradient at \p lhs on the point \p rhs.
         * \param lhs The gradient to use.
         * \param rhs The point to calculate the influence for.
         * \return The influence of the gradient at \p lhs on the point \p rhs.
         */
        [[nodiscard]] float calculate_influence(const glm::vec2& lhs, const glm::vec2& rhs) const;

        /**
         * \brief Computes a noise value for a given \p current point.
         * \param current The point to compute the noise value for.
         * \return The noise value at \p current. (in the range `[-1, 1]`)
         */
        [[nodiscard]] float compute_element(const glm::vec2& current) const;

    private:
        glm::vec<2, std::uint32_t> m_size;
        std::vector<glm::vec2> m_gradients;
        std::vector<float> m_data;
    };
}
