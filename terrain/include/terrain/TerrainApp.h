#pragma once

#include <memory>

#include "GLFW/glfw3.h"
#include "litefx/litefx.h"

namespace terrain
{
    using WindowPtr = std::unique_ptr<::GLFWwindow, decltype(&::glfwDestroyWindow)>;

    std::uint32_t find_best_adapter(const LiteFX::Rendering::IRenderBackend& backend);

    class TerrainApp : public LiteFX::App
    {
    public:
        static std::string Name() noexcept { return "Procedural Terrain"; }
        std::string name() const noexcept override { return Name(); }

        static AppVersion Version() noexcept { return AppVersion(1, 0, 0, 0); }
        AppVersion version() const noexcept override { return Version(); }

    private:
        /// <summary>
        /// Stores the GLFW window pointer.
        /// </summary>
        WindowPtr m_window;

        /// <summary>
        /// Stores a reference of the input assembler state.
        /// </summary>
        std::shared_ptr<IInputAssembler> m_inputAssembler;

        /// <summary>
        /// Stores the viewport.
        /// </summary>
        std::shared_ptr<IViewport> m_viewport;

        /// <summary>
        /// Stores the scissor.
        /// </summary>
        std::shared_ptr<IScissor> m_scissor;

        /// <summary>
        /// Stores a pointer to the currently active device.
        /// </summary>
        IGraphicsDevice* m_device;

        /// <summary>
        /// Stores the fence created at application load time.
        /// </summary>
        UInt64 m_transferFence = 0;

    public:
        TerrainApp(WindowPtr&& window) :
            m_window(std::move(window)), m_device(nullptr)
        {
            this->initializing += std::bind(&TerrainApp::onInit, this);
            this->startup += std::bind(&TerrainApp::onStartup, this);
            this->resized += std::bind(&TerrainApp::onResize, this, std::placeholders::_1, std::placeholders::_2);
            this->shutdown += std::bind(&TerrainApp::onShutdown, this);
        }

    private:
        /// <summary>
        /// Initializes the buffers.
        /// </summary>
        /// <param name="backend">The render backend to use.</param>
        void initBuffers(IRenderBackend* backend);

        /// <summary>
        /// Updates the camera buffer. This needs to be done whenever the frame buffer changes, since we need to pass changes in the aspect ratio to the view/projection matrix.
        /// </summary>
        void updateCamera(const ICommandBuffer& commandBuffer, IBuffer& buffer) const;

    private:
        void onInit();
        void onStartup();
        void onShutdown();
        void onResize(const void* sender, ResizeEventArgs e);

    public:
        void keyDown(int key, int scancode, int action, int mods);
        void handleEvents();
        void drawFrame();
        void updateWindowTitle();
    };
}
