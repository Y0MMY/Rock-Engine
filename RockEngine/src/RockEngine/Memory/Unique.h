#pragma once

namespace RockEngine::Memory
{
	template <typename T>
	class Unique
	{
	public:
		constexpr Unique() noexcept = default;
		constexpr Unique(std::nullptr_t) noexcept {}

		explicit Unique(T* ptr) noexcept
			: m_Ptr(ptr) {}

		~Unique()
		{
			Release();
		}

		template <typename Tother>
		Unique(Unique<Tother>&& other)
		{
			this->Release();
			m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
		}

		void Reset(T* ptr = nullptr)
		{
			Release();
			m_Ptr = ptr;
		}

		void Release()
		{
			if (m_Ptr)
			{
				delete m_Ptr;
				m_Ptr = nullptr;
			}
		}
	public:
		template <typename ...Args>
		static Unique<T> Create(Args&&... args)
		{
			return Unique<T>(new T(std::forward<Args>(args)...));
		}
	public:
		template <typename Tother>
		Unique& operator=(Unique<Tother>&& other)
		{
			this->Release();
			m_Ptr = other.m_Ptr;
			other.m_Ptr = nullptr;
			return *this;
		}
	public:
		inline T& operator*() const noexcept { return *m_Ptr; }
		inline T* operator->() const noexcept { return m_Ptr; }

		bool operator==(const Unique& InOther) const { return m_Ptr == InOther.m_Ptr; }
		bool operator!=(const Unique& InOther) const { return !(*this == InOther); }

		inline T* GetPtr() const noexcept { return m_Ptr; }
	public:
		Unique(const Unique&) = delete;
		Unique& operator=(const Unique&) = delete;
	private:
		T* m_Ptr = nullptr;
	private:
		template<typename TOther>
		friend class Unique;
	};
}