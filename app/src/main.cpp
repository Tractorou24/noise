#include "noise/noise.h"
#include "noise/perlin.h"

#include "cxxopts.hpp"
#include "libassert/assert.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <exception>
#include <format>
#include <memory>
#include <print>
#include <random>
#include <string_view>

/**
 * \brief Checks if a cli option was specified, and if not, exits with the error and the help text.
 * \param option The option to check.
 * \param msg The error message to show if not present.
 */
#define ASSERT_REQUIRED_OPTION_PRESENT(option, msg) \
        if (args.count(option) == 0) \
        { \
            std::println("{}\n", msg); \
            std::println("{}", options.help()); \
            return 1; \
        }

int main(int argc, char* argv[])
{
    // Parse argument
    cxxopts::Options options("noise-generator", "Generate different noises with different options.");
    options.add_options() //
            ("t,type", "The type of noise to generate.", cxxopts::value<std::string>()) //
            ("s,seed", "Seed used by the generator", cxxopts::value<std::uint32_t>()) //
            ("w,width", "The width of the noise.", cxxopts::value<std::uint32_t>()) //
            ("h,height", "The height of the noise.", cxxopts::value<std::uint32_t>()) //
            ("g,grid", "The grid size of the noise.", cxxopts::value<std::uint64_t>()->default_value("400")) //
            ("p,passes", "The number of passes used to generate the noise.", cxxopts::value<std::uint32_t>()->default_value("3")) //
            ("o,output", "The output file.", cxxopts::value<std::string>());

    cxxopts::ParseResult args;
    try
    {
        args = options.parse(argc, argv);
    } catch (const std::exception& e)
    {
        std::println("{}\n", e.what());
        std::println("{}", options.help());
        return 1;
    }

    // Get the data
    std::uint32_t seed = 0;
    if (args.count("seed") == 0)
        seed = std::random_device()();
    else
        seed = args["seed"].as<std::uint32_t>();

    // Create the noise
    constexpr std::array<std::string_view, 1> available_noise_types = {"perlin"};

    ASSERT_REQUIRED_OPTION_PRESENT("type", "No noise type specified.")
    const auto type = args["type"].as<std::string>();
    ASSERT(std::ranges::find(available_noise_types, type) != available_noise_types.end(), "Invalid noise type.", type, available_noise_types);

    std::unique_ptr<noise::noise> noise = nullptr;
    if (type == "perlin")
        noise = std::make_unique<noise::perlin::perlin>(seed);
    ASSERT(noise != nullptr, "Failed to create noise generator.", type);

    // Generate the noise
    ASSERT_REQUIRED_OPTION_PRESENT("width", "No width specified.")
    ASSERT_REQUIRED_OPTION_PRESENT("height", "No height specified.")
    const std::uint32_t width = args["width"].as<std::uint32_t>();
    const std::uint32_t height = args["height"].as<std::uint32_t>();

    const std::uint64_t grid_size = args["grid"].as<std::uint64_t>();
    const std::uint32_t passes = args["passes"].as<std::uint32_t>();

    std::println("Generating perlin noise with width: {} and height: {}", width, height);
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    noise->generate(width, height, passes, grid_size);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::println("Noise generated in {}ms.", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

    // Save to output file
    auto output_path = std::format("noise_s{}_{}x{}_p{}_g{}.png", seed, width, height, passes, grid_size);
    if (args.count("output") > 0)
        output_path = args["output"].as<std::string>();

    try
    {
        std::println("Saving perlin noise to file: {}", output_path);
        start = std::chrono::high_resolution_clock::now();
        noise->save(output_path);
        end = std::chrono::high_resolution_clock::now();
        std::println("Noise saved in {}ms.", std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());
    } catch (std::exception& ex)
    {
        std::println("Failed to save noise to file: {}", ex.what());
        return 1;
    }

    return 0;
}
