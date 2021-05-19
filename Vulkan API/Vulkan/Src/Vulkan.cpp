#include "vkPCH.h"
#include "Vulkan.h"

//=-=-=-=-=-=-= Public =-=-=-=-=-=-=-=-=-=
void Vulkan::run()
{
    init_window();
    init_vulkan();
    main_loop();
    Renderer.cleanup(device_, window_);
}

//=-=-=-=-=-=-= Private =-=-=-=-=-=-=-=-=
void Vulkan::init_window()
{
    Log::Init();
    VK_CORE_INFO("Application Started!")

        glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window_ = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, Renderer.framebufferResizeCallback);
}

void Vulkan::init_vulkan()
{
    VK_CORE_INFO("Vulkan Setup Started!")

    Renderer.create_instance();

    Renderer.setup_debug_messenger();

    Renderer.create_surface(window_);

    Renderer.pick_physical_device();
    Renderer.create_logical_device(device_);

    Renderer.create_swap_chain(device_, window_);

    Renderer.create_image_views(device_);

    Renderer.create_render_pass(device_);

    Renderer.create_descriptor_set_layout(device_);
    Renderer.create_graphics_pipeline(device_);

    Renderer.create_framebuffers(device_);

    Renderer.create_command_pool(device_);

    Renderer.create_vertex_buffer(device_);
    Renderer.create_index_buffer(device_);
    Renderer.create_uniform_buffers(device_);

    Renderer.create_descriptor_pool(device_);
    Renderer.create_descriptor_sets(device_);

    Renderer.create_command_buffers(device_);

    Renderer.create_sync_objects(device_);
}

void Vulkan::main_loop()
{
    VK_CORE_INFO("Main Loop Started!")
        while (!glfwWindowShouldClose(window_))
        {
            VK_CORE_INFO("-=-=-=-=-=-=- NEW FRAME -=-=-=-=-=-=-")
                glfwPollEvents();
            Renderer.draw_frame(device_, window_);
        }

    vkDeviceWaitIdle(device_);
}