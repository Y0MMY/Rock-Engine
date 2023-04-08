#include <unordered_map>
#include <memory>
#include <typeindex>

class REVector {
public:
	template<typename T>
	void add(T item) {
		data.emplace_back(new DataHolder<T>(item));
	}

	template <class T, class... Args>
	T& emplace(Args&&... args) {
		data.emplace_back(new DataHolder<T>(std::forward<Args>(args)...));
		return static_cast<DataHolder<T>*>(data.back().get())->item;
	}

	template<typename T>
	void remove() {
		auto it = data.begin();
		while (it != data.end()) {
			if (std::type_index(typeid(T)) == (*it)->type()) {
				it = data.erase(it);
			}
			else {
				++it;
			}
		}
	}

	template<typename T>
	bool has() const {
		for (const auto& datum : data) {
			if (std::type_index(typeid(T)) == datum->type()) {
				return true;
			}
		}
		return false;
	}

	template<typename T>
	T& get(int index) const {
		return static_cast<DataHolder<T>*>(data[index].get())->item;
	}

	int size() const {
		return data.size();
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

	std::vector<std::unique_ptr<Data>> data;
};