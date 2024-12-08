#include "terrain/TerrainApp.h"

#include "GLFW/glfw3.h"
#include "litefx/backends/dx12.hpp"
#include "litefx/backends/vulkan.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

int main(const int /*argc*/, const char* /*argv*/[])
{
    if (::glfwInit() == false)
        throw std::runtime_error("Unable to initialize glfw.");

    ::glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    ::glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    auto window = terrain::WindowPtr(::glfwCreateWindow(800u, 600u, "Procedural Terrain", nullptr, nullptr), ::glfwDestroyWindow);

    // Get the required Vulkan extensions from glfw.
    uint32_t extensions = 0;
    const char** extension_names = ::glfwGetRequiredInstanceExtensions(&extensions);
    auto required_extensions = std::vector<std::string>(extension_names, extension_names + extensions);

    // Create the app.
    try
    {
        UniquePtr<App> app = App::build<terrain::TerrainApp>(std::move(window))
                             .logTo<ConsoleSink>(LogLevel::Trace)
                             .logTo<RollingFileSink>("sample.log", LogLevel::Debug)
                             .useBackend<Backends::VulkanBackend>(required_extensions)
                             .useBackend<Backends::DirectX12Backend>();

        app->run();
    } catch (const LiteFX::Exception& ex)
    {
        std::println(std::cerr, "\033[3;41;37mUnhandled exception: {}", ex.what());
        std::println(std::cerr, "at: {}\033[0m", ex.trace());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
