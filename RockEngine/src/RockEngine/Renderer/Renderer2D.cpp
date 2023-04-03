#include "pch.h"
#include "Renderer.h"
#include "Renderer2D.h"

#include "RockEngine/Renderer/Pipeline.h"
#include "RockEngine/Renderer/VertexBuffer.h"
#include "RockEngine/Renderer/IndexBuffer.h"

namespace RockEngine
{
	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
	};

	struct Renderer2DData
	{
		// Lines
		static const u32 MaxLines = 10000;
		static const u32 MaxLineVertices = MaxLines * 4;
		static const u32 MaxLineIndices = MaxLines * 6;

		Ref<Pipeline> LinePipeline;
		Ref<VertexBuffer> LineVertexBuffer;
		Ref<IndexBuffer> LineIndexBuffer;

		Ref<Shader> LineShader;
		uint32_t LineIndexCount = 0;
		LineVertex* LineVertexBufferBase = nullptr;
		// Quad

		glm::mat4 CameraViewProj;
		bool DepthTest = true;

		Renderer2D::Statistics Stats;

	};
	Renderer2DData* s_Data = nullptr;

	void Renderer2D::Init()
	{
		s_Data = new Renderer2DData();

		// Lines
		{
			s_Data->LineShader = Shader::Create("assets/shaders/Renderer2D_Line.glsl");

			PipelineSpecification spec;
			spec.Layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" },
			};

			s_Data->LinePipeline = Pipeline::Create(spec);
			s_Data->LineVertexBuffer = VertexBuffer::Create(s_Data->MaxLineVertices * sizeof(LineVertex));

			s_Data->LineVertexBufferBase = new LineVertex[s_Data->MaxLineIndices];

			u32 *LineIndices = new u32[s_Data->MaxLineIndices];
			for (u32 i = 0; i < s_Data->MaxLineIndices; i++)
			{
				LineIndices[i] = i;
			}

			s_Data->LineIndexBuffer = IndexBuffer::Create(LineIndices, s_Data->MaxLineIndices);
			delete[] LineIndices;
		}

		// Quad
		{

		}
	}

	void Renderer2D::Shutdown()
	{
		delete s_Data;
	}

	void Renderer2D::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color /* = glm::vec4(1.0f) */)
	{
		if (s_Data->LineIndexCount >= Renderer2DData::MaxLineIndices)
			FlushAndResetLines();

		s_Data->LineVertexBufferBase[s_Data->LineIndexCount].Position = p0;
		s_Data->LineVertexBufferBase[s_Data->LineIndexCount].Color = color;

		s_Data->LineIndexCount++;

		s_Data->LineVertexBufferBase[s_Data->LineIndexCount].Position = p1;
		s_Data->LineVertexBufferBase[s_Data->LineIndexCount].Color = color;

		s_Data->LineIndexCount++;
		s_Data->Stats.LineCount++;
	}

	void Renderer2D::BeginScene(const glm::mat4& viewProj, bool depthTest)
	{
		s_Data->CameraViewProj = viewProj;
		s_Data->DepthTest = depthTest;

		s_Data->LineIndexCount = 0;

	}

	void Renderer2D::EndScene()
	{
		uint32_t dataSize = (uint8_t*)(s_Data->LineVertexBufferBase + s_Data->LineIndexCount) - (uint8_t*)s_Data->LineVertexBufferBase;

		if (dataSize)
		{
			s_Data->LineVertexBuffer->SetData(s_Data->LineVertexBufferBase, dataSize);

			s_Data->LineShader->Bind();
			s_Data->LineShader->SetMat4("u_ViewProjection", s_Data->CameraViewProj);

			s_Data->LinePipeline->Bind();
			s_Data->LineIndexBuffer->Bind();
			Renderer::SetLineThickness(12.0f);
			Renderer::DrawIndexed(s_Data->LineIndexCount, PrimitiveType::Lines, s_Data->DepthTest);
			s_Data->Stats.DrawCalls++;
		}

	}

	void Renderer2D::FlushAndResetLines()
	{
		

	}

	void Renderer2D::ResetStats()
	{
		memset(&s_Data->Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::GetStats()
	{
		return s_Data->Stats;
	}

}