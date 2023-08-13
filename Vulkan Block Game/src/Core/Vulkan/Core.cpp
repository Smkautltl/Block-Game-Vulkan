#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/constants.hpp>

#include "Core.h"

CORE::CORE(std::string AppName, FrameInfo& Frameinfo) : appName(AppName), frameinfo(Frameinfo)
{
    //Setup glfw Settings
    glfwSetWindowUserPointer(window.GetWindow(), this);
    glfwSetFramebufferSizeCallback(window.GetWindow(), framebufferResizeCallback);
    glfwSetWindowTitle(window.GetWindow(), appName.c_str());

    frameinfo.aspect = renderer.GetAspectRatio();
    currentTime = std::chrono::high_resolution_clock::now();
}
void CORE::framebufferResizeCallback(GLFWwindow* window_, int width_, int height_)
{
    auto core = reinterpret_cast<CORE*>(glfwGetWindowUserPointer(window_));
    core->window.frameBufferResized = true;
    core->window.width = width_;
    core->window.height = height_;

    core->renderer.RecreateSwapchain();
    core->frameinfo.aspect = core->renderer.GetAspectRatio();
    core->Draw();
}

//Public
void CORE::Setup()
{
    //Setup Descriptor Pool, Layout and Sets
    globalPool = DescriptorPool::Builder(device)
        .setMaxSets(MAX_FRAMES_IN_FLIGHT + SHADOWMAP_CASCADE_COUNT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, MAX_FRAMES_IN_FLIGHT + 30)
        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SHADOWMAP_CASCADE_COUNT + MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLER, MAX_FRAMES_IN_FLIGHT + 30)
        .addPoolSize(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, MAX_TEXTURES * (MAX_FRAMES_IN_FLIGHT + 30))
        .build();
    globalSetLayout = DescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(2, VK_DESCRIPTOR_TYPE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .addBinding(3, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, VK_SHADER_STAGE_FRAGMENT_BIT, MAX_TEXTURES)
        .build();
    globalDescriptorSets = std::vector<VkDescriptorSet>(MAX_FRAMES_IN_FLIGHT);

    //Setup Uniform Buffers
    uboBuffers = std::vector<std::unique_ptr<Buffer>>(MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++)
    {
        uboBuffers[i] = std::make_unique<Buffer>(
            device,
            sizeof(UniformBufferObject),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            device.Properties.limits.minUniformBufferOffsetAlignment
        );
        uboBuffers[i]->Map();
    }

    shadowMapBuffers = std::vector<std::unique_ptr<Buffer>>(SHADOWMAP_CASCADE_COUNT);
    for (int i = 0; i < shadowMapBuffers.size(); i++)
    {
        shadowMapBuffers[i] = std::make_unique<Buffer>(
            device,
            sizeof(ShadowBufferObject),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            device.Properties.limits.minUniformBufferOffsetAlignment
        );
        shadowMapBuffers[i]->Map();
    }
    
    frameinfo.Sampler = Sampler::CreateSampler(device);
    VkDescriptorImageInfo samplerInfo = {};
    samplerInfo.sampler = frameinfo.Sampler.get()->Get();

    auto& shadowmap = renderer.GetShadowMap();

    VkDescriptorImageInfo depthInfo{};
    depthInfo.sampler = shadowmap->GetSampler();
    depthInfo.imageView = shadowmap->GetView();
    depthInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

    VkDescriptorImageInfo imageInfos[MAX_TEXTURES]{};

    for (auto i = 0; i < MAX_TEXTURES; i++)
    {
        if (i >= frameinfo.Textures.size())
        {
            auto imageinfo = frameinfo.Textures[0].get()->GetImageInfo();
            imageInfos[i].sampler = nullptr;
            imageInfos[i].imageLayout = imageinfo.imageLayout;
            imageInfos[i].imageView = imageinfo.imageView;
            continue;
        }

        auto imageinfo = frameinfo.Textures[i].get()->GetImageInfo();
        imageInfos[i].sampler = nullptr;
        imageInfos[i].imageLayout = imageinfo.imageLayout;
        imageInfos[i].imageView = imageinfo.imageView;
    }

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        auto bufferInfo = uboBuffers[i]->DescriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .writeImage(1, &depthInfo)
            .writeSampler(2, &samplerInfo)
            .writeImages(3, imageInfos)
            .build(globalDescriptorSets[i]);
    } 

    for (uint32_t i = 0; i < SHADOWMAP_CASCADE_COUNT; i++) 
    {
        auto bufferInfo = shadowMapBuffers[i]->DescriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .writeImage(1, &depthInfo)
            .build(shadowmap->GetCascades()[i].DescriptorSet);
    }
}

void CORE::Run()
{ 
    RenderSystem Rendersystem{ device, renderer.GetSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
    renderSystem = &Rendersystem;
    PointLightSystem PointLightsystem{ device, renderer.GetSwapchainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
    pointLightSystem = &PointLightsystem;
    ShadowMapSystem Shadowmapsystem{ device, renderer.GetShadowRenderPass(), globalSetLayout->getDescriptorSetLayout() };
    shadowMapSystem = &Shadowmapsystem;

    loadGameObjects();

    while (!window.ShouldClose())
    {
        auto newTime = std::chrono::high_resolution_clock::now();
        frameinfo.FrameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        glfwPollEvents();

        setWindowMode();
        setWindowFPS();
        Update();
        Draw();
    }
    vkDeviceWaitIdle(device.vkDevice());
}

//Private
void CORE::Update()
{
    renderer.UpdateCascades(frameinfo);

    UniformBufferObject ubo{};
    ubo.Projection = frameinfo.Projection;
    ubo.View = frameinfo.View;
    ubo.inverseView = frameinfo.InverseView;
    pointLightSystem->Update(frameinfo, ubo);

    uboBuffers[frameinfo.FrameIndex]->WriteToBuffer(&ubo);
    uboBuffers[frameinfo.FrameIndex]->Flush();

    auto casecades = renderer.GetShadowMap()->GetCascades();
    ShadowBufferObject sbo{}; 
    for (uint8_t i = 0; i < SHADOWMAP_CASCADE_COUNT; i++)
    {
        sbo.cascadeViewProjMat[i] = casecades[i].ViewProjMatrix;
    }

    shadowMapBuffers[frameinfo.FrameIndex]->WriteToBuffer(&sbo);
    shadowMapBuffers[frameinfo.FrameIndex]->Flush();
}

void CORE::Draw()
{
    if (frameinfo.CommandBuffer = renderer.BeginFrame())
    {
        frameinfo.FrameIndex = renderer.GetFrameIndex();
        frameinfo.GlobalDescriptorSet = globalDescriptorSets[frameinfo.FrameIndex];

        auto casecades = renderer.GetShadowMap()->GetCascades();
        for (uint8_t i = 0; i < SHADOWMAP_CASCADE_COUNT; i++)
        {
            renderer.BeginShadowmapRenderpass(frameinfo.CommandBuffer, i);
            shadowMapSystem->Render(frameinfo, casecades[i].DescriptorSet, i);
            renderer.EndShadowmapRenderpass(frameinfo.CommandBuffer);
        }

        renderer.BeginSwapchainRenderpass(frameinfo.CommandBuffer);

        renderSystem->Render(frameinfo);
        pointLightSystem->Render(frameinfo);

        renderer.EndSwapchainRenderpass(frameinfo.CommandBuffer);
        renderer.EndFrame();
    }
}
void CORE::loadGameObjects()
{
   std::shared_ptr<Model> model = createCubeModel({0, 0, 0});
   auto cube = GameObject::CreateGameObject();
   cube.model = model;
   cube.transform.translation = { 0.f, -1.f, 0.f };
   cube.transform.scale = { .5f, .5f, .5f };
   frameinfo.GameObjects.emplace(cube.GetID(), std::move(cube));
   
   //model = createSkyBoxModel({ 0, 0, 0 });
   //cube = GameObject::CreateGameObject();  
   //cube.model = model;
   //cube.transform.translation = { 0.f, 0.f, 0.f };
   //cube.transform.scale = { 800.0f, 800.0f, 800.0f };
   //frameinfo.GameObjects.emplace(cube.GetID(), std::move(cube));

   model = Model::CreateFromFile(device, "Models/floor.obj");
   cube = GameObject::CreateGameObject();
   cube.model = model;
   cube.transform.translation = { 0.0f, 0.5f, 0.f };
   cube.transform.scale = { 5.f, 5.f, 5.f };
   frameinfo.GameObjects.emplace(cube.GetID(), std::move(cube));

    std::vector<glm::vec3> lightColors
    {
        { 133, 228, 234 },
        { 140,11,44 },
        { 101,93,115 },
        { 71,137,224 },
        { 71,23,47 },
        { 162,27,235 },
        { 177,3,45 },
        { 5,190,207 },
        { 185,228,92 },
        { 40,114,253 }
    };

    int index = 0;
    for (auto lightcolour : lightColors)
    {
        auto pointLight = GameObject::MakePointLight(0.2f);
        pointLight.colour = glm::normalize(glm::vec3{ (float)(rand() % 255) / 255, (float)(rand() % 255) / 255, (float)(rand() % 255) / 255 });

        auto rotateLight = glm::rotate( glm::mat4(1.f), (index * glm::two_pi<float>()) / lightColors.size(), {0.f, -1.f, 0.f});
        pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));

        frameinfo.GameObjects.emplace(pointLight.GetID(), std::move(pointLight));
        index++;
    } 
}

void CORE::setWindowFPS()
{
    if (!device.enableFPS) return;
    static double UpdateTitle = 0;
    static uint32_t frames = 0;   
    float UpdateInterval = 0.2f;

    if (UpdateTitle >= UpdateInterval)
    {
        char title[256];
        title[255] = '\0';

        snprintf(title, 255, "%s %s - [FT: %3.2f, FPS: %i]", appName.c_str(), "v0.1", 1000.f / (float)(frames * (1 / UpdateInterval)), (uint32_t)(frames / UpdateTitle));

        glfwSetWindowTitle(window.GetWindow(), title);

        frames = 0;
        UpdateTitle = 0;
        return;
    }

    frames++;
    UpdateTitle += frameinfo.FrameTime;
}
void CORE::setWindowMode()
{
    if (frameinfo.MakeFullscreen)
    {
        std::vector<int> windowpossize { 0, 0, 0, 0 };
        glfwGetWindowPos(window.GetWindow(), &windowpossize[0], &windowpossize[1]);
        glfwGetWindowSize(window.GetWindow(), &windowpossize[2], &windowpossize[3]);
        window.SaveWindowData(windowpossize);

        auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

        glfwSetWindowMonitor(window.GetWindow(), glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, 0);
        frameinfo.MakeFullscreen = false;
        frameinfo.isinWindowed = false;
    }   
    else if (frameinfo.MakeWindowed)
    {
        std::vector<int> windowpossize = window.GetWindowData();
        glfwSetWindowMonitor(window.GetWindow(), NULL, windowpossize[0], windowpossize[1], windowpossize[2], windowpossize[3], 0);
        frameinfo.MakeWindowed = false;
        frameinfo.isinWindowed = true;
    }

}
std::unique_ptr<Model> CORE::createCubeModel(glm::vec3 offset)
{
    Model::Builder modelBuilder{};

    modelBuilder.vertices =
    {
        //  |         POS         |       COLOUR         |        NORMAL         |        UV      |
        //  |   X      Y      Z   |     R     G     B    |    X      Y      Z    |     U     V    |
            {{ 1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }, {0} },
            {{-1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }, {0} },
            {{ 1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }, {0} },
            {{-1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }, {0} },

            {{-1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }, {0} },
            {{ 1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }, {0} },
            {{-1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }, {0} },
            {{ 1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }, {0} },

            {{ 1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, {0} },
            {{-1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, {0} },
            {{ 1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, {0} },
            {{-1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, {0} },

            {{-1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 0.0f, 0.0f }, {0} },
            {{-1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 1.0f, 1.0f }, {0} },
            {{-1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 1.0f, 0.0f }, {0} },
            {{-1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 0.0f, 1.0f }, {0} },

            {{ 1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, {0} },
            {{ 1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, {0} },
            {{ 1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, {0} },
            {{ 1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, {0} },

            {{-1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f }, {0} },
            {{ 1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f }, {0} },
            {{-1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 0.0f }, {0} },
            {{ 1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f }, {0} }
    };

    modelBuilder.indices = 
    {
        0, 1, 2,    1, 0, 3,    //BOTTOM
        4, 5, 6,    5, 4, 7,    //FRONT
        8, 9, 10,   9, 8, 11,   //BACK
        12,13,14,   13,12,15,   //LEFT
        16,17,18,   17,16,19,   //RIGHT
        20,21,22,   21,20,23    //TOP
    };

    for (auto& v : modelBuilder.vertices)
    {
        v.pos += offset;
    }

    return std::make_unique<Model>(device, modelBuilder);
}

std::unique_ptr<Model> CORE::createSkyBoxModel(glm::vec3 offset)
{
    Model::Builder modelBuilder{};


    modelBuilder.vertices =
    {
        //  |         POS         |       COLOUR         |        NORMAL         |        UV      |
        //  |   X      Y      Z   |     R     G     B    |    X      Y      Z    |     U     V    |
            {{ 1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 0.0f, 0.0f }, {1} },
            {{-1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 1.0f, 1.0f }, {1} },
            {{ 1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 1.0f, 0.0f }, {1} },
            {{-1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  1.0f,  0.0f }, { 0.0f, 1.0f }, {1} },

            {{-1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 0.0f, 0.0f }, {2} },
            {{ 1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 1.0f, 1.0f }, {2} },
            {{-1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 1.0f, 0.0f }, {2} },
            {{ 1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f,  0.0f, -1.0f }, { 0.0f, 1.0f }, {2} },

            {{ 1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 0.0f, 0.0f }, {3} },
            {{-1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 1.0f, 1.0f }, {3} },
            {{ 1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 1.0f, 0.0f }, {3} },
            {{-1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-0.0f,  0.0f,  1.0f }, { 0.0f, 1.0f }, {3} },

            {{-1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 0.0f, 0.0f }, {4} },
            {{-1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 1.0f, 1.0f }, {4} },
            {{-1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 1.0f, 0.0f }, {4} },
            {{-1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, {-1.0f,  0.0f, -0.0f }, { 0.0f, 1.0f }, {4} },

            {{ 1.0f,  1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 0.0f, 0.0f }, {5} },
            {{ 1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 1.0f, 1.0f }, {5} },
            {{ 1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 1.0f, 0.0f }, {5} },
            {{ 1.0f,  1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 1.0f,  0.0f,  0.0f }, { 0.0f, 1.0f }, {5} },

            {{-1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f,  0.0f }, { 0.0f, 0.0f }, {6} },
            {{ 1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f,  0.0f }, { 1.0f, 1.0f }, {6} },
            {{-1.0f, -1.0f,  1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f,  0.0f }, { 1.0f, 0.0f }, {6} },
            {{ 1.0f, -1.0f, -1.0f }, { 1.00, 1.00, 1.00 }, { 0.0f, -1.0f,  0.0f }, { 0.0f, 1.0f }, {6} }
    };                                                                     

    modelBuilder.indices =
    {
        0, 1, 2,    1, 0, 3,    //BOTTOM
        4, 5, 6,    5, 4, 7,    //FRONT
        8, 9, 10,   9, 8, 11,   //BACK
        12,13,14,   13,12,15,   //LEFT
        16,17,18,   17,16,19,   //RIGHT
        20,21,22,   21,20,23    //TOP
    };

    for (auto& v : modelBuilder.vertices)
    {
        v.pos += offset;
    }

    return std::make_unique<Model>(device, modelBuilder);
}