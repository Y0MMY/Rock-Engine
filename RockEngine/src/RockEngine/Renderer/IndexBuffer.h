#pragma once

#include "RockEngine/Renderer/RendererAPI.h"

namespace RockEngine
{
	class IndexBuffer : public RefCounted
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void SetData(void* data, u32 size, u32 offset = 0) = 0;
		virtual void Bind() const = 0;

		virtual u32 GetCount() const = 0;
		virtual u32 GetSize() const = 0;

		static Ref<IndexBuffer> Create(void* data, u32 size);
		static Ref<IndexBuffer> Create(u32 size);
	};
}