#include <vector>
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <RockEngine/Core/UUID.h>

namespace RockEngine
{
	using EntityID = uint32_t;
	class REVector {
	public:
		template<typename T>
		void add(EntityID id, T item) {
			data[id][std::type_index(typeid(T))] = std::make_unique<DataHolder<T>>(item);
		}

		template <class T, class... Args>
		T& emplace(EntityID id, Args&&... args) 
		{
			data[id][std::type_index(typeid(T))] = std::make_unique<DataHolder<T>>(std::forward<Args>(args)...);
			return static_cast<DataHolder<T>*>(data[id][std::type_index(typeid(T))].get())->item;
		}

		template<typename T>
		void remove(EntityID id) 
		{
			data[id].erase(std::type_index(typeid(T)));
		}

		template<typename T>
		bool has(EntityID id) const 
		{
			return (data.count(id) != 0) && (data.at(id).count(std::type_index(typeid(T))) != 0);
		}

		template<typename T>
		decltype(auto) group()
		{
			std::vector<DataHolder<T>*> result;
			for (auto& [uuid, innerMap] : data)
			{
				if (innerMap.count(std::type_index(typeid(T))) != 0)
				{
					auto& item = innerMap[std::type_index(typeid(T))];
					if (item != nullptr)
					{
						result.push_back(static_cast<DataHolder<T>*>(item.get()));
					}
				}
			}
			return result;
		}

		template<typename T>
		T& get(EntityID id)
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