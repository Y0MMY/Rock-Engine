#pragma once

#include "RockEngine/Renderer/Renderer.h"

namespace RockEngine
{
	class VertexBuffer : public RefCounted
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void SetData(void* data, u32 size, u32 offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual u32 GetSize() const = 0;

		static Ref<VertexBuffer> Create(void* data, u32 size);
		static Ref<VertexBuffer> Create(u32 size);
	};
}