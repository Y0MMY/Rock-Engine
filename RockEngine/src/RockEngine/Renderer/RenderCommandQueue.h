#pragma once

namespace RockEngine
{
	class RenderCommandQueue
	{
	public:
		typedef void(*RenderCommandFn)(void*);
		RenderCommandQueue();
		~RenderCommandQueue();

		void* Allocate(RenderCommandFn fn, uint32_t size);

		void Execute();
	private:
		byte* m_CommandBuffer;
		byte* m_CommandBufferPtr;
		uint32_t m_CommandCount = 0;
	};
}
