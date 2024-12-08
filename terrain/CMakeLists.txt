find_package(litefx QUIET REQUIRED)
find_package(spdlog QUIET REQUIRED)
find_package(glfw3 QUIET REQUIRED)
find_package(directx-dxc QUIET REQUIRED)

add_executable(terrain)
target_sources(terrain
    PUBLIC
        FILE_SET public_headers
        TYPE HEADERS
        FILES
            include/terrain/TerrainApp.h
        BASE_DIRS
            include
    PRIVATE
        src/main.cpp
        src/TerrainApp.cpp
)

target_link_libraries(terrain 
    PUBLIC 
        noise
        LiteFX.AppModel
        LiteFX.Core
        LiteFX.Rendering
        LiteFX.Backends.DirectX12
        LiteFX.Backends.Vulkan
    PRIVATE
        glfw
)

add_shader_module(terrain.vk.shaders.vs SOURCE "shaders/basic_vs.hlsl" LANGUAGE HLSL TYPE VERTEX COMPILE_AS SPIRV SHADER_MODEL 6_8 COMPILER DXC)
add_shader_module(terrain.vk.shaders.fs SOURCE "shaders/basic_fs.hlsl" LANGUAGE HLSL TYPE FRAGMENT COMPILE_AS SPIRV SHADER_MODEL 6_8 COMPILER DXC)
add_shader_module(terrain.dx.shaders.vs SOURCE "shaders/basic_vs.hlsl" LANGUAGE HLSL TYPE VERTEX COMPILE_AS DXIL SHADER_MODEL 6_8 COMPILER DXC)
add_shader_module(terrain.dx.shaders.fs SOURCE "shaders/basic_fs.hlsl" LANGUAGE HLSL TYPE PIXEL COMPILE_AS DXIL SHADER_MODEL 6_8 COMPILER DXC)
target_link_shaders(terrain
    SHADERS
        terrain.vk.shaders.vs
        terrain.vk.shaders.fs
        terrain.dx.shaders.vs
        terrain.dx.shaders.fs
)
