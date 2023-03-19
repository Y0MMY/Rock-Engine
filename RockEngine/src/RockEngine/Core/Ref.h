#pragma once

#include <stdint.h>

namespace RockEngine
{
	class RefCounted
	{
	public:
		void IncRefCount() const
		{
			m_RefCount++;
		}

		void DecRefCount() const
		{
			m_RefCount--;
		}
	private:
		mutable uint32_t m_RefCount = 0;
	};

	template <typename T>
	class Ref
	{
	public:
		Ref()
			: m_Instance(nullptr) 
		{
		}

		Ref(std::nullptr_t n)
			: m_Instance(nullptr)
		{
		}

		Ref(T* instance)
			:m_Instance(instance)
		{
			static_assert(std::is_base_of<RefCounted, T>::value, "Class is not RefCounted!");
			IncRef()
		}
	private:
		void IncRef()
		{

		}

		void DecRef()
		{

		}
	private:
		T* m_Instance;
	};
}