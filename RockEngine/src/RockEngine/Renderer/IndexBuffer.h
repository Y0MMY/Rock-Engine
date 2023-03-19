#pragma once

#include "RockEngine/Renderer/Renderer.h"

namespace RockEngine
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void SetData(void* data, u32 size, u32 offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual u32 GetSize() const = 0;

		static IndexBuffer* Create(void* data, u32 size);
		static IndexBuffer* Create(u32 size);
	};
}