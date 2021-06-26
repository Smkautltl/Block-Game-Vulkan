C:\VulkanSDK\1.2.162.1\Bin32\glslc.exe shader.vert -o vert.spv
C:\VulkanSDK\1.2.162.1\Bin32\glslc.exe shader.frag -o frag.spv

MOVE /Y "vert.spv" "C:\Users\Matt\Documents\Programming Learning\C++\Vulkan Block Game\x64\Debug\Shaders" 
MOVE /Y "frag.spv" "C:\Users\Matt\Documents\Programming Learning\C++\Vulkan Block Game\x64\Debug\Shaders" 

pause