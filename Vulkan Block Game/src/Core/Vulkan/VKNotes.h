#pragma once

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	PRESENTATION MODES:
//
//		VK_PRESENT_MODE_IMMEDIATE_KHR 
//			Frames submitted are immediatly show to the screen (May cause tearing)
// 
//		VK_PRESENT_MODE_FIFO_KHR
//			Adds frames to the swapchain queue when not full, then when the display is refreshed it takes the first image in the queue (V-Sync)
// 
//		VK_PRESENT_MODE_FIFO_RELAXED_KHR
//			This mode is the same as the previous mode but if the application is late and the queue is empty then the frame is immediatly sent to the screen (May cause tearing)
//		
//		VK_PRESENT_MODE_MAILBOX_KHR
//			This mode is the same as VK_PRESENT_MODE_FIFO_KHR but when the queue is full the frames that are in the queue get replaced (Unlocked frame-rate)
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	IMAGE SHARING MODES:
// 
//		VK_SHARING_MODE_EXCLUSIVE
//			The image is owned by one queue family and must be explictily transferred (Best Performance)
//		
//		VK_SHARING_MODE_CONCURRENT
//			The image can be used across multiple queues
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	GRAPHICS PIPELINE:
//	
//	FIXED-FUNCTION - You can tweak the operations with parameters, but the function is predefined
//	PROGRAMMABLE - You can use your own custom code and it is sent to the GPU
// 
// 
//		INPUT ASSEMBLER (FIXED-FUNCTION)
//		 |	Collects the raw vertex data from the buffers and sometime index data if applicable
//		 V	
//		VERTEX SHADER (PROGRAMMABLE)
//		 |	Runs for every vertex and generally applies transformations to turn vertex postions from model space to screen space
//		 V
//		TESSELLATION (PROGRAMMABLE) (CAN BE DISABLED)
//		 |	Allows you to sub-divide geometry based on certain rules to increase mesh quality
//		 V
//		GEOMETRY SHADER (PROGRAMMABLE) (CAN BE DISABLED)
//		 |	Runs on every primative (Triangle, line, point) and can discard it or it can output more primatives. Not useful on most graphics cards due to low performance!
//		 V
//		RASTERIZATION (FIXED-FUNCTION)
//		 |	Takes primatives and turns them into fragments, fragments that are off the screen or behind other fragments (Depth Testing) are discarded
//		 V
//		FRAGMENT SHADER (PROGRAMMABLE) (CAN BE DISABLED - FOR SHADOW MAP GENERATION)
//		 |	Run on every remaining fragment and determins which framebuffer(s) the fragments are written to. It can interpolate data from the vertex shader like tex coords and normals
//		 V
//		COLOUR BLENDING (FIXED-FUNCTION)
//			Blends fragments together if they occupy the same pixel in the framebuffer. The blending can either be one colour overwritting another or adding the colours together
// 
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	INPUT ASSEMBLY TOPOLOGY:
//
//		VK_PRIMITIVE_TOPOLOGY_POINT_LIST - Points from vertices
//
//		VK_PRIMITIVE_TOPOLOGY_LINE_LIST	- Line from every 2 vertices without reuse
//
//		VK_PRIMITIVE_TOPOLOGY_LINE_STRIP - The end vertex of every line is used to start vertex for the next line
//
//		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST - Triangle from every 3 vertices without reuse
//
//		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP - The second and third vertex of every triangle are used as the two first in the next triangle
// 
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	LOAD OP:
//
//		VK_ATTACHMENT_LOAD_OP_LOAD - Preserve the existing conents of the attachment
//
//		VK_ATTACHMENT_LOAD_OP_CLEAR	- Clear the values to a constant at the start
//
//		VK_ATTACHMENT_LOAD_OP_DONT_CARE - Existing contents are undefined
//
//	STORE OP:
//
//		VK_ATTACHMENT_STORE_OP_STORE - Rendered contents will be stored in memory and can be read later
//
//		VK_ATTACHMENT_STORE_OP_DONT_CARE - Contents of the framebuffer will be undefined
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	COMMAND BUFFER BEGIN FLAG:
//
//		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT - The command buffer will be rerecorded right after executing 
//
//		VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT - This is a secondary commnad buffer that will be entirely within a single render pass
//
//		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT - The command buffer can be resubmitted while it is also already pending an execution
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	BUFFER USAGE FLAGS:
// 
//		VK_BUFFER_USAGE_TRANSFER_SRC_BIT - Buffer can be used as source in a memory transfer operation.
//
//		VK_BUFFER_USAGE_TRANSFER_DST_BIT - Buffer can be used as destination in a memory transfer operation.
//
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//