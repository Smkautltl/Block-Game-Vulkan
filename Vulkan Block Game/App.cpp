#include "App.h"

App::App()
{
    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/missing.png"));
    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/SkyBox/Bottom.png"));
    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/SkyBox/Front.png"));
    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/SkyBox/Back.png"));
    
    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/SkyBox/Right.png"));
    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/SkyBox/Left.png"));

    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/SkyBox/Top.png"));
    frameinfo.Textures.push_back(Texture::CreateTextureFromFile(core.GetDevice(), "Textures/SkyBox/acacia_planks.png"));

    playerInstance = &player;
}

void PlayerLoop(App* app)
{
    while (!app->shouldWindowClose)
    {
        app->player.GetCamera().SetPerspectiveProjection(glm::radians(50.f), app->frameinfo.aspect, 0.1f, 1500.f);
        app->player.Update();
    }
}

void App::MainLoop()
{
    core.Setup();
    std::thread playerThread(PlayerLoop, this);
    core.Run();
    shouldWindowClose = true;
    playerThread.join();
}