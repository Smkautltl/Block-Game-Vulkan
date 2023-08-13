
#include "Buffer.h"

VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) 
{
    if (minOffsetAlignment > 0) 
    {
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
    }
    return instanceSize;
}

Buffer::Buffer(Device& device_, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
    : device{ device_ }, instanceSize{ instanceSize }, instanceCount{ instanceCount }, usageFlags{ usageFlags }, memoryPropertyFlags{ memoryPropertyFlags } 
{
    alignmentSize = GetAlignment(instanceSize, minOffsetAlignment); bufferSize = alignmentSize * instanceCount;
    device.CreateBuffer(bufferSize, usageFlags, memoryPropertyFlags, buffer, memory);
}
Buffer::~Buffer() 
{
    Unmap();
    vkDestroyBuffer(device.vkDevice(), buffer, nullptr);
    vkFreeMemory(device.vkDevice(), memory, nullptr);
}

VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset) 
{
    VK_CORE_ASSERT(buffer && memory, "Called map on buffer before create");
    return vkMapMemory(device.vkDevice(), memory, offset, size, 0, &mapped);
}
void Buffer::Unmap() 
{
    if (mapped) 
    {
        vkUnmapMemory(device.vkDevice(), memory);
        mapped = nullptr;
    }
}

void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) 
{
    VK_CORE_ASSERT(mapped, "Cannot copy to unmapped buffer");

    if (size == VK_WHOLE_SIZE) 
    {
        memcpy(mapped, data, bufferSize);
    }
    else 
    {
        char* memOffset = (char*)mapped;
        memOffset += offset;
        memcpy(memOffset, data, size);
    }
}

VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset) 
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(device.vkDevice(), 1, &mappedRange);
}
VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset) 
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(device.vkDevice(), 1, &mappedRange);
}
VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset) 
{
    return VkDescriptorBufferInfo
    {
        buffer,
        offset,
        size,
    };
}

void Buffer::WriteToIndex(void* data, int index) 
{
    WriteToBuffer(data, instanceSize, index * alignmentSize);
}
VkResult Buffer::FlushIndex(int index) 
{ 
    return Flush(alignmentSize, index * alignmentSize); 
}
VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index) 
{
    return DescriptorInfo(alignmentSize, index * alignmentSize);
}
VkResult Buffer::InvalidateIndex(int index) 
{
    return Invalidate(alignmentSize, index * alignmentSize);
}