#pragma once

#include "RockEngine/Renderer/VertexBuffer.h"
#include <RockEngine/Core/Buffer.h>

namespace RockEngine
{
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(void* data, u32 size);
		OpenGLVertexBuffer(u32 size);

		~OpenGLVertexBuffer();

		virtual void SetData(void* data, u32 size, u32 offset) override;
		virtual void Bind() const override;

		virtual u32 GetSize() const { return m_Size; }
	private:
		Buffer m_LocalData;
		u32 m_Size = 0;

		RendererID m_RendererID = 0;
	};
}