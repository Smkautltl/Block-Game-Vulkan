#pragma once
//STD
#include <memory>
#include <vector>

#include "Window.h"
#include "Renderer.h"
#include "Descriptors.h"
#include "Core/Game Components/GameObject.h"
#include "Core/Systems/RenderSystem.h"
#include "Core/Systems/PointLightSystem.h"
#include "Core/Systems/ShadowMapSystem.h"


class CORE
{
public:

    CORE(std::string AppName, FrameInfo& Frameinfo);

    CORE(const CORE&) = delete;
    CORE& operator=(const CORE&) = delete;

    void Setup();
    void Run();

    GLFWwindow* GetCoreWindow() { return window.GetWindow(); }
    Window& GetWindow() { return window; }
    VkDevice GetCoreDevice() { return device.vkDevice(); }
    Device& GetDevice() { return device; }
    Renderer& GetRenderer() { return renderer; }

private:
    static void framebufferResizeCallback(GLFWwindow* window_, int width_, int height_);

    void Update();
    void Draw();
    void setWindowFPS();
    void setWindowMode();
    std::unique_ptr<Model> createCubeModel(glm::vec3 offset);  
    std::unique_ptr<Model> createSkyBoxModel(glm::vec3 offset);

    void loadGameObjects();

public:

private:
    Window window{ WINWIDTH, WINHEIGHT, "Vulkan!" };
    Device device{ window };
    Renderer renderer{ window, device };
    RenderSystem* renderSystem;
    PointLightSystem* pointLightSystem;
    ShadowMapSystem* shadowMapSystem;

    std::string appName;

    std::unique_ptr<DescriptorPool> globalPool{};
    std::unique_ptr<DescriptorSetLayout> globalSetLayout;
    std::vector<VkDescriptorSet> globalDescriptorSets;
    
    std::vector<std::unique_ptr<Buffer>> uboBuffers;
    std::vector<std::unique_ptr<Buffer>> shadowMapBuffers;

    std::chrono::steady_clock::time_point currentTime;
    FrameInfo& frameinfo;
};