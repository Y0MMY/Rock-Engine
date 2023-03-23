#pragma once

#include "RockEngine/Renderer/Pipeline.h"

namespace RockEngine
{
	class OpenGLPipeline : public Pipeline
	{
	public:
		OpenGLPipeline(const PipelineSpecification& spec);
		~OpenGLPipeline() override;

		virtual PipelineSpecification& GetSpecification() override { return m_Specification; }

		virtual void Invalidate() override;

		virtual void Bind()override;
	private:
		PipelineSpecification m_Specification;
		RendererID m_RendererID;
	};
}