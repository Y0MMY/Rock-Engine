#pragma once

#include "RockEngine/Renderer/IndexBuffer.h"
#include <RockEngine/Memory/Buffer.h>

namespace RockEngine
{
	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(void* data, u32 size);
		OpenGLIndexBuffer(u32 size);

		~OpenGLIndexBuffer();

		virtual void SetData(void* data, u32 size, u32 offset) override;
		virtual void Bind() const override;

		virtual u32 GetSize() const { return m_Size; }
		virtual u32 GetCount() const { return m_Size / sizeof(u32); }
	private:
		u32 m_Size = 0;
		RendererID m_RendererID = 0;

		Buffer m_LocalData;
	};
}