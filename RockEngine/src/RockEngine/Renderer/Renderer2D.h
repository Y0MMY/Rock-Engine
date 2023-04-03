#pragma once

#include <glm/glm.hpp>

namespace RockEngine
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const glm::mat4& viewProj, bool depthTest = true);
		static void EndScene();



		static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color = glm::vec4(1.0f));

		static void FlushAndReset();
		static void FlushAndResetLines();

		// Stats
		struct Statistics {

			u32 DrawCalls = 0;
			u32 QuadCount = 0;
			u32 LineCount = 0;

			uint32_t GetTotalVertexCount() { return QuadCount * 4 + LineCount * 2; }
			uint32_t GetTotalIndexCount() { return QuadCount * 6 + LineCount * 2; }
		};

		static void ResetStats();
		static Statistics GetStats();
	};
}