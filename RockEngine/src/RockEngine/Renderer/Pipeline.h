#pragma once

#include "Shader.h"
#include "VertexBuffer.h"

namespace RockEngine
{
	struct PipelineSpecification
	{
		VertexBufferLayout Layout;
	};

	class Pipeline : public RefCounted
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineSpecification& GetSpecification() = 0;

		virtual void Invalidate() = 0;

		virtual void Bind() = 0;

		static Ref<Pipeline> Create(const PipelineSpecification& spec);
	};
}