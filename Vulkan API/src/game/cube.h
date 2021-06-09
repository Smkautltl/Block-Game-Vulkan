#pragma once
#include "../core/model.h"

namespace Vulkan
{
	struct Block
	{
	public:
		Block(Device& device_, glm::vec3 colour)
		{
			model = std::make_unique<Model>(device_, 
				std::vector<Model::Vertex>
			{

				// left face (white)
				{ {-.5f, -.5f, -.5f},		colour},
				{ {-.5f, .5f, .5f},		colour },
				{ {-.5f, -.5f, .5f},		colour },
				{ {-.5f, -.5f, -.5f},		colour },
				{ {-.5f, .5f, -.5f},		colour },
				{ {-.5f, .5f, .5f},		colour },

					// right face (yellow)
				{ {.5f, -.5f, -.5f},		colour },
				{ {.5f, .5f, .5f},			colour },
				{ {.5f, -.5f, .5f},		colour },
				{ {.5f, -.5f, -.5f},		colour },
				{ {.5f, .5f, -.5f},		colour },
				{ {.5f, .5f, .5f},			colour },

					// top face (orange)
				{ {-.5f, -.5f, -.5f},		colour },
				{ {.5f, -.5f, .5f},		colour },
				{ {-.5f, -.5f, .5f},		colour },
				{ {-.5f, -.5f, -.5f},		colour },
				{ {.5f, -.5f, -.5f},		colour },
				{ {.5f, -.5f, .5f},		colour },

					// bottom face (red)
				{ {-.5f, .5f, -.5f},		colour },
				{ {.5f, .5f, .5f},			colour },
				{ {-.5f, .5f, .5f},		colour },
				{ {-.5f, .5f, -.5f},		colour },
				{ {.5f, .5f, -.5f},		colour },
				{ {.5f, .5f, .5f},			colour },

					// nose face (blue)
				{ {-.5f, -.5f, 0.5f},		colour },
				{ {.5f, .5f, 0.5f},		colour },
				{ {-.5f, .5f, 0.5f},		colour },
				{ {-.5f, -.5f, 0.5f},		colour },
				{ {.5f, -.5f, 0.5f},		colour },
				{ {.5f, .5f, 0.5f},		colour },

					// tail face (green)
				{ {-.5f, -.5f, -0.5f},		colour },
				{ {.5f, .5f, -0.5f},		colour },
				{ {-.5f, .5f, -0.5f},		colour },
				{ {-.5f, -.5f, -0.5f},		colour },
				{ {.5f, -.5f, -0.5f},		colour },
				{ {.5f, .5f, -0.5f},		colour },

			});
			colour_ = colour;
		}

		std::shared_ptr<Model> Get() { return model; }
		
		std::shared_ptr<Model> GetCulled(Device& device_, std::vector<int> facesToCull)
		{
			std::vector<Model::Vertex> vertices;

			if (facesToCull[0] == 0)
			{
				vertices.push_back({ {-.5f, -.5f, -.5f},	colour_ });
				vertices.push_back({ {-.5f, .5f, .5f},		colour_ });
				vertices.push_back({ {-.5f, -.5f, .5f},	colour_ });
				vertices.push_back({ {-.5f, -.5f, -.5f},	colour_ });
				vertices.push_back({ {-.5f, .5f, -.5f},	colour_ });
				vertices.push_back({ {-.5f, .5f, .5f},		colour_ });
			}
			if (facesToCull[1] == 0)
			{
				vertices.push_back({ {.5f, -.5f, -.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, .5f},		colour_ });
				vertices.push_back({ {.5f, -.5f, .5f},		colour_ });
				vertices.push_back({ {.5f, -.5f, -.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, -.5f},		colour_ });
				vertices.push_back({ {.5f, .5f, .5f},		colour_ });
			}
			if (facesToCull[2] == 0)
			{
				vertices.push_back({ {-.5f, -.5f, -.5f},	colour_ });
				vertices.push_back({ {.5f, -.5f, .5f},		colour_ });
				vertices.push_back({ {-.5f, -.5f, .5f},	colour_ });
				vertices.push_back({ {-.5f, -.5f, -.5f},	colour_ });
				vertices.push_back({ {.5f, -.5f, -.5f},	colour_ });
				vertices.push_back({ {.5f, -.5f, .5f},		colour_ });
			}																	
			if (facesToCull[3] == 0)											
			{																	
				vertices.push_back({ {-.5f, .5f, -.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, .5f},		colour_ });
				vertices.push_back({ {-.5f, .5f, .5f},		colour_ });
				vertices.push_back({ {-.5f, .5f, -.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, -.5f},		colour_ });
				vertices.push_back({ {.5f, .5f, .5f},		colour_ });
			}																
			if (facesToCull[4] == 0)										
			{																
				vertices.push_back({ {-.5f, -.5f, 0.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, 0.5f},		colour_ });
				vertices.push_back({ {-.5f, .5f, 0.5f},	colour_ });
				vertices.push_back({ {-.5f, -.5f, 0.5f},	colour_ });
				vertices.push_back({ {.5f, -.5f, 0.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, 0.5f},		colour_ });
																			
			}																
			if (facesToCull[5] == 0)										
			{																
				vertices.push_back({ {-.5f, -.5f, -0.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, -0.5f},	colour_ });
				vertices.push_back({ {-.5f, .5f, -0.5f},	colour_ });
				vertices.push_back({ {-.5f, -.5f, -0.5f},	colour_ });
				vertices.push_back({ {.5f, -.5f, -0.5f},	colour_ });
				vertices.push_back({ {.5f, .5f, -0.5f},	colour_ });
			}

			return std::make_unique<Model>(device_, vertices);
		}
	
	private:
		std::shared_ptr<Model> model;
		glm::vec3 colour_;
	};
}
