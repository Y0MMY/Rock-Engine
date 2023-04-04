#pragma once

#include "RockEngine/Core/Core.h"
#include "RockEngine/Core/Log.h"

namespace RockEngine
{
	struct Buffer
	{
		byte* Data;
		u32 Size;

		Buffer()
			: Data(nullptr), Size(0)
		{

		}

		Buffer(byte* data, u32 size)
			: Data(data), Size(size)
		{

		}

		void Allocate(u32 size)
		{
			delete[] Data;
			Data = nullptr;

			Data = new byte[size];
			Size = size;
		}

		void ZeroInitialize()
		{
			if (Data)
				memset(Data, 0, Size);
		}

		template <typename T>
		T& Read(u32 offset = 0)
		{
			return *(T*)(Data + offset);
		}

		void Write(void* data, u32 size, u32 offset = 0)
		{
			RE_CORE_ASSERT(size + offset <= Size, "Buffer overflow!");
			memcpy(Data + offset, data, size);
		}

		static Buffer Copy(void* data, u32 size)
		{
			Buffer buffer;
			buffer.Allocate(size);

			memcpy(buffer.Data, data, size);
			return buffer;
		}

		template <typename T>
		T* As() const
		{
			return (T*)(Data);
		}

		operator bool() const {
			return Data;
		}

		byte& operator[](int index)
		{
			return ((byte*)Data)[index];
		}

		byte operator[](int index) const
		{
			return ((byte*)Data)[index];
		}

		inline u32 GetSize() { return Size; }

	};
}