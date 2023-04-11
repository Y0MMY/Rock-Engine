//#include <unordered_map>
//#include <memory>
//#include <typeindex>
//
//class REVector {
//public:
//	template<typename T>
//	void add(T item) {
//		data.emplace_back(new DataHolder<T>(item));
//	}
//
//	template <class T, class... Args>
//	T& emplace(Args&&... args) {
//		data.emplace_back(new DataHolder<T>(std::forward<Args>(args)...));
//		return static_cast<DataHolder<T>*>(data.back().get())->item;
//	}
//
//	template<typename T>
//	void remove() {
//		auto it = data.begin();
//		while (it != data.end()) {
//			if (std::type_index(typeid(T)) == (*it)->type()) {
//				it = data.erase(it);
//			}
//			else {
//				++it;
//			}
//		}
//	}
//
//	template<typename T>
//	bool has() const {
//		for (const auto& datum : data) {
//			if (std::type_index(typeid(T)) == datum->type()) {
//				return true;
//			}
//		}
//		return false;
//	}
//
//	template<typename T>
//	T& get() {
//		for (const auto& datum : data) {
//			if (std::type_index(typeid(T)) == datum->type()) 
//			{
//				return static_cast<DataHolder<T>*>(datum.get())->item;
//			}
//		}
//		RE_CORE_ASSERT(false);
//	}
//
//	int size() const {
//		return data.size();
//	}
//
//private:
//	class Data {
//	public:
//		Data() = default;
//		virtual ~Data() {}
//		virtual std::type_index type() const = 0;
//	};
//
//	template<typename T>
//	class DataHolder : public Data {
//	public:
//		DataHolder() {}
//		DataHolder(const T& item) : item(item) {}
//		T item;
//
//		std::type_index type() const override {
//			return std::type_index(typeid(T));
//		}
//
//		DataHolder(const DataHolder& other) : Data(other), item(other.item) {}
//
//		DataHolder& operator=(const DataHolder& other) {
//			if (this != &other) {
//				Data::operator=(other);
//				item = other.item;
//			}
//			return *this;
//		}
//	};
//
//	std::vector<std::unique_ptr<Data>> data;
//};
#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <RockEngine/Core/UUID.h>

namespace RockEngine
{
	class REVector {
	public:
		template<typename T>
		void add(RockEngine::UUID id, T item) {
			data[id][std::type_index(typeid(T))] = std::make_unique<DataHolder<T>>(item);
		}

		template <class T, class... Args>
		T& emplace(RockEngine::UUID id, Args&&... args) 
		{
			data[id][std::type_index(typeid(T))] = std::make_unique<DataHolder<T>>(std::forward<Args>(args)...);
			return static_cast<DataHolder<T>*>(data[id][std::type_index(typeid(T))].get())->item;
		}

		template<typename T>
		void remove(RockEngine::UUID id) 
		{
			data[id].erase(std::type_index(typeid(T)));
		}

		template<typename T>
		bool has(RockEngine::UUID id) const 
		{
			return (data.count(id) != 0) && (data.at(id).count(std::type_index(typeid(T))) != 0);
		}

		template<typename T>
		T& get(RockEngine::UUID id)
		{
			auto& item = data[id].at(std::type_index(typeid(T)));
			if (item == nullptr) {
				RE_CORE_ASSERT(false);
			}
			return static_cast<DataHolder<T>*>(item.get())->item;
		}

	private:
		class Data {
		public:
			Data() = default;
			virtual ~Data() {}
			virtual std::type_index type() const = 0;
		};

		template<typename T>
		class DataHolder : public Data {
		public:
			DataHolder() {}
			DataHolder(const T& item) : item(item) {}
			T item;

			std::type_index type() const override {
				return std::type_index(typeid(T));
			}
		};

		std::unordered_map<RockEngine::UUID, std::unordered_map<std::type_index, std::unique_ptr<Data>>> data;
	};
}