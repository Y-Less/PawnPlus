#include "../natives.h"
#include "../variants.h"
#include <unordered_map>
#include <iterator>

typedef std::unordered_map<dyn_object, dyn_object> map_t;

template <size_t... KeyIndices>
class key_at
{
	using key_ftype = typename dyn_factory<KeyIndices...>::type;

public:
	template <size_t... ValueIndices>
	class value_at
	{
		using value_ftype = typename dyn_factory<ValueIndices...>::type;
		using result_ftype = typename dyn_result<ValueIndices...>::type;

	public:
		// native bool:map_add(Map:map, key, value, ...);
		template <typename key_ftype KeyFactory, typename value_ftype ValueFactory>
		static cell AMX_NATIVE_CALL map_add(AMX *amx, cell *params)
		{
			auto ptr = reinterpret_cast<map_t*>(params[1]);
			if(ptr == nullptr) return -1;
			auto ret = ptr->insert(std::make_pair(KeyFactory(amx, params[KeyIndices]...), ValueFactory(amx, params[ValueIndices]...)));
			return static_cast<cell>(ret.second);
		}

		// native bool:map_set(Map:map, key, value, ...);
		template <typename key_ftype KeyFactory, typename value_ftype ValueFactory>
		static cell AMX_NATIVE_CALL map_set(AMX *amx, cell *params)
		{
			auto ptr = reinterpret_cast<map_t*>(params[1]);
			if(ptr == nullptr) return 0;
			(*ptr)[KeyFactory(amx, params[KeyIndices]...)] = ValueFactory(amx, params[ValueIndices]...);
			return 1;
		}

		// native map_get(Map:map, key, ...);
		template <typename key_ftype KeyFactory, typename result_ftype ValueFactory>
		static cell AMX_NATIVE_CALL map_get(AMX *amx, cell *params)
		{
			auto ptr = reinterpret_cast<map_t*>(params[1]);
			if(ptr == nullptr) return 0;
			auto it = ptr->find(KeyFactory(amx, params[KeyIndices]...));
			if(it != ptr->end())
			{
				return ValueFactory(amx, it->second, params[ValueIndices]...);
			}
			return 0;
		}
	};

	// native bool:map_remove(Map:map, key, ...);
	template <typename key_ftype KeyFactory>
	static cell AMX_NATIVE_CALL map_remove(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		auto it = ptr->find(KeyFactory(amx, KeyIndices...));
		if(it != ptr->end())
		{
			ptr->erase(it);
			return 1;
		}
		return 0;
	}
	
	// native bool:map_set_cell(Map:map, key, offset, AnyTag:value, ...);
	template <typename key_ftype KeyFactory, size_t TagIndex = 0>
	static cell AMX_NATIVE_CALL map_set_cell(AMX *amx, cell *params)
	{
		if(params[3] < 0) return 0;
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		auto it = ptr->find(KeyFactory(amx, params[KeyIndices]...));
		if(it != ptr->end())
		{
			auto &obj = it->second;
			if(TagIndex && !obj.check_tag(amx, params[TagIndex])) return 0;
			return obj.set_cell(params[3], params[4]);
		}
		return 0;
	}

	// native map_tagof(Map:map, key, ...);
	template <typename key_ftype KeyFactory>
	static cell AMX_NATIVE_CALL map_tagof(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		auto it = ptr->find(KeyFactory(amx, params[KeyIndices]...));
		if(it != ptr->end())
		{
			return it->second.get_tag(amx);
		}
		return 0;
	}

	// native map_sizeof(Map:map, key, ...);
	template <typename key_ftype KeyFactory>
	static cell AMX_NATIVE_CALL map_sizeof(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		auto it = ptr->find(KeyFactory(amx, params[KeyIndices]...));
		if(it != ptr->end())
		{
			return it->second.get_size();
		}
		return 0;
	}
};

template <size_t... ValueIndices>
class value_at
{
	using result_ftype = typename dyn_result<ValueIndices...>::type;

public:
	// native map_key_at(Map:map, index, ...);
	template <typename result_ftype ValueFactory>
	static cell AMX_NATIVE_CALL map_key_at(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		auto it = ptr->begin();
		std::advance(it, params[2]);
		if(it != ptr->end())
		{
			return ValueFactory(amx, it->first, params[ValueIndices]...);
		}
		return 0;
	}

	// native map_value_at(Map:map, index, ...);
	template <typename result_ftype ValueFactory>
	static cell AMX_NATIVE_CALL map_value_at(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		auto it = ptr->begin();
		std::advance(it, params[2]);
		if(it != ptr->end())
		{
			return ValueFactory(amx, it->second, params[ValueIndices]...);
		}
		return 0;
	}
};

namespace Natives
{
	// native Map:map_new();
	static cell AMX_NATIVE_CALL map_new(AMX *amx, cell *params)
	{
		auto ptr = new map_t();
		return reinterpret_cast<cell>(ptr);
	}

	// native map_delete(Map:map);
	static cell AMX_NATIVE_CALL map_delete(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		delete ptr;
		return 1;
	}

	// native map_size(Map:map);
	static cell AMX_NATIVE_CALL map_size(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return -1;
		return static_cast<cell>(ptr->size());
	}

	// native map_clear(Map:map);
	static cell AMX_NATIVE_CALL map_clear(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		if(ptr == nullptr) return 0;
		ptr->clear();
		return 1;
	}

	// native bool:map_add(Map:map, AnyTag:key, AnyTag:value, key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_add(AMX *amx, cell *params)
	{
		return key_at<2, 4>::value_at<3, 5>::map_add<dyn_func, dyn_func>(amx, params);
	}

	// native bool:map_add_arr(Map:map, AnyTag:key, const AnyTag:value[], value_size=sizeof(value), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_add_arr(AMX *amx, cell *params)
	{
		return key_at<2, 5>::value_at<3, 4, 6>::map_add<dyn_func, dyn_func_arr>(amx, params);
	}

	// native bool:map_add_str(Map:map, AnyTag:key, const value[], key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_add_str(AMX *amx, cell *params)
	{
		return key_at<2, 4>::value_at<3>::map_add<dyn_func, dyn_func_str>(amx, params);
	}

	// native bool:map_add_var(Map:map, AnyTag:key, VariantTag:value, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_add_var(AMX *amx, cell *params)
	{
		return key_at<2, 4>::value_at<3>::map_add<dyn_func, dyn_func_var>(amx, params);
	}

	// native bool:map_arr_add(Map:map, const AnyTag:key[], AnyTag:value, key_size=sizeof(key), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_add(AMX *amx, cell *params)
	{
		return key_at<2, 4, 5>::value_at<3, 6>::map_add<dyn_func_arr, dyn_func>(amx, params);
	}

	// native bool:map_arr_add_arr(Map:map, const AnyTag:key[], const AnyTag:value[], key_size=sizeof(key), value_size=sizeof(value), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_add_arr(AMX *amx, cell *params)
	{
		return key_at<2, 4, 6>::value_at<3, 5, 7>::map_add<dyn_func_arr, dyn_func_arr>(amx, params);
	}

	// native bool:map_arr_add_str(Map:map, const AnyTag:key[], const value[], key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_add_str(AMX *amx, cell *params)
	{
		return key_at<2, 4, 5>::value_at<3>::map_add<dyn_func_arr, dyn_func_str>(amx, params);
	}

	// native bool:map_arr_add_var(Map:map, const AnyTag:key[], VariantTag:value, key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_add_var(AMX *amx, cell *params)
	{
		return key_at<2, 4, 5>::value_at<3>::map_add<dyn_func_arr, dyn_func_var>(amx, params);
	}

	// native bool:map_str_add(Map:map, const key[], AnyTag:value, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_str_add(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4>::map_add<dyn_func_str, dyn_func>(amx, params);
	}

	// native bool:map_str_add_arr(Map:map, const key[], const AnyTag:value[], value_size=sizeof(value), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_str_add_arr(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_add<dyn_func_str, dyn_func_arr>(amx, params);
	}

	// native bool:map_str_add_str(Map:map, const key[], const value[]);
	static cell AMX_NATIVE_CALL map_str_add_str(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_add<dyn_func_str, dyn_func_str>(amx, params);
	}

	// native bool:map_str_add_var(Map:map, const key[], VariantTag:value);
	static cell AMX_NATIVE_CALL map_str_add_var(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_add<dyn_func_str, dyn_func_var>(amx, params);
	}

	// native bool:map_var_add(Map:map, VariantTag:key, AnyTag:value, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_var_add(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4>::map_add<dyn_func_var, dyn_func>(amx, params);
	}

	// native bool:map_var_add_arr(Map:map, VariantTag:key, const AnyTag:value[], value_size=sizeof(value), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_var_add_arr(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_add<dyn_func_var, dyn_func_arr>(amx, params);
	}

	// native bool:map_var_add_str(Map:map, VariantTag:key, const value[]);
	static cell AMX_NATIVE_CALL map_var_add_str(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_add<dyn_func_var, dyn_func_str>(amx, params);
	}

	// native bool:map_str_add_var(Map:map, VariantTag:key, VariantTag:value);
	static cell AMX_NATIVE_CALL map_var_add_var(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_add<dyn_func_var, dyn_func_var>(amx, params);
	}

	// native map_add_map(Map:map, Map:other, bool:overwrite);
	static cell AMX_NATIVE_CALL map_add_map(AMX *amx, cell *params)
	{
		auto ptr = reinterpret_cast<map_t*>(params[1]);
		auto ptr2 = reinterpret_cast<map_t*>(params[2]);
		if(ptr == nullptr || ptr2 == nullptr) return -1;
		if(params[3])
		{
			for(auto &pair : *ptr2)
			{
				(*ptr)[pair.first] = pair.second;
			}
		}else{
			ptr->insert(ptr2->begin(), ptr2->end());
		}
		return ptr->size() - ptr2->size();
	}

	// native bool:map_remove(Map:map, AnyTag:key, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_remove(AMX *amx, cell *params)
	{
		return key_at<2, 3>::map_remove<dyn_func>(amx, params);
	}

	// native bool:map_arr_remove(Map:map, const AnyTag:key[], key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_remove(AMX *amx, cell *params)
	{
		return key_at<2, 3, 4>::map_remove<dyn_func_arr>(amx, params);
	}

	// native bool:map_str_remove(Map:map, const key[]);
	static cell AMX_NATIVE_CALL map_str_remove(AMX *amx, cell *params)
	{
		return key_at<2>::map_remove<dyn_func_str>(amx, params);
	}

	// native bool:map_str_remove(Map:map, VariantTag:key);
	static cell AMX_NATIVE_CALL map_var_remove(AMX *amx, cell *params)
	{
		return key_at<2>::map_remove<dyn_func_var>(amx, params);
	}

	// native map_get(Map:map, AnyTag:key, offset=0, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_get(AMX *amx, cell *params)
	{
		return key_at<2, 4>::value_at<3>::map_get<dyn_func, dyn_func>(amx, params);
	}

	// native map_get_arr(Map:map, AnyTag:key, AnyTag:value[], value_size=sizeof(value), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_get_arr(AMX *amx, cell *params)
	{
		return key_at<2, 5>::value_at<3, 4>::map_get<dyn_func, dyn_func_arr>(amx, params);
	}

	// native Variant:map_get_var(Map:map, AnyTag:key, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_get_var(AMX *amx, cell *params)
	{
		return key_at<2, 3>::value_at<>::map_get<dyn_func, dyn_func_var>(amx, params);
	}

	// native bool:map_get_checked(Map:map, AnyTag:key, &AnyTag:value, offset=0, key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_get_checked(AMX *amx, cell *params)
	{
		return key_at<2, 5>::value_at<3, 4, 6>::map_get<dyn_func, dyn_func>(amx, params);
	}

	// native map_get_arr_checked(Map:map, AnyTag:key, AnyTag:value[], value_size=sizeof(value), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_get_arr_checked(AMX *amx, cell *params)
	{
		return key_at<2, 5>::value_at<3, 4, 6>::map_get<dyn_func, dyn_func_arr>(amx, params);
	}

	// native map_arr_get(Map:map, const AnyTag:key[], offset=0, key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_get(AMX *amx, cell *params)
	{
		return key_at<2, 4, 5>::value_at<3>::map_get<dyn_func_arr, dyn_func>(amx, params);
	}

	// native map_arr_get_arr(Map:map, const AnyTag:key[], AnyTag:value[], value_size=sizeof(value), key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_get_arr(AMX *amx, cell *params)
	{
		return key_at<2, 5, 6>::value_at<3, 4>::map_get<dyn_func_arr, dyn_func_arr>(amx, params);
	}

	// native Variant:map_arr_get_var(Map:map, const AnyTag:key[], key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_get_var(AMX *amx, cell *params)
	{
		return key_at<2, 3, 4>::value_at<>::map_get<dyn_func_arr, dyn_func_var>(amx, params);
	}

	// native bool:map_arr_get_checked(Map:map, const AnyTag:key[], &AnyTag:value, offset=0, key_size=sizeof(key), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_get_checked(AMX *amx, cell *params)
	{
		return key_at<2, 5, 6>::value_at<3, 4, 7>::map_get<dyn_func_arr, dyn_func>(amx, params);
	}

	// native map_arr_get_arr_checked(Map:map, const AnyTag:key[], AnyTag:value[], value_size=sizeof(value), key_size=sizeof(key), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_get_arr_checked(AMX *amx, cell *params)
	{
		return key_at<2, 5, 6>::value_at<3, 4, 7>::map_get<dyn_func_arr, dyn_func_arr>(amx, params);
	}

	// native map_str_get(Map:map, const key[], offset=0);
	static cell AMX_NATIVE_CALL map_str_get(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_get<dyn_func_str, dyn_func>(amx, params);
	}

	// native map_str_get_arr(Map:map, const key[], AnyTag:value[], value_size=sizeof(value));
	static cell AMX_NATIVE_CALL map_str_get_arr(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4>::map_get<dyn_func_str, dyn_func_arr>(amx, params);
	}

	// native Variant:map_str_get_var(Map:map, const key[]);
	static cell AMX_NATIVE_CALL map_str_get_var(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<>::map_get<dyn_func_str, dyn_func_var>(amx, params);
	}

	// native bool:map_str_get_checked(Map:map, const key[], &AnyTag:value, offset=0, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_str_get_checked(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_get<dyn_func_str, dyn_func>(amx, params);
	}

	// native map_str_get_arr_checked(Map:map, const key[], AnyTag:value[], value_size=sizeof(value), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_str_get_arr_checked(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_get<dyn_func_str, dyn_func_arr>(amx, params);
	}

	// native map_var_get(Map:map, VariantTag:key, offset=0);
	static cell AMX_NATIVE_CALL map_var_get(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_get<dyn_func_var, dyn_func>(amx, params);
	}

	// native map_var_get_arr(Map:map, VariantTag:key, AnyTag:value[], value_size=sizeof(value));
	static cell AMX_NATIVE_CALL map_var_get_arr(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4>::map_get<dyn_func_var, dyn_func_arr>(amx, params);
	}

	// native Variant:map_var_get_var(Map:map, VariantTag:key);
	static cell AMX_NATIVE_CALL map_var_get_var(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<>::map_get<dyn_func_var, dyn_func_var>(amx, params);
	}

	// native bool:map_var_get_checked(Map:map, VariantTag:key, &AnyTag:value, offset=0, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_var_get_checked(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_get<dyn_func_var, dyn_func>(amx, params);
	}

	// native map_var_get_arr_checked(Map:map, VariantTag:key, AnyTag:value[], value_size=sizeof(value), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_var_get_arr_checked(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_get<dyn_func_var, dyn_func_arr>(amx, params);
	}

	// native bool:map_set(Map:map, AnyTag:key, AnyTag:value, key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_set(AMX *amx, cell *params)
	{
		return key_at<2, 4>::value_at<3, 5>::map_set<dyn_func, dyn_func>(amx, params);
	}

	// native bool:map_set_arr(Map:map, AnyTag:key, const AnyTag:value[], value_size=sizeof(value), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_set_arr(AMX *amx, cell *params)
	{
		return key_at<2, 5>::value_at<3, 4, 6>::map_set<dyn_func, dyn_func_arr>(amx, params);
	}

	// native bool:map_set_str(Map:map, AnyTag:key, const value[], key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_set_str(AMX *amx, cell *params)
	{
		return key_at<2, 4>::value_at<3>::map_set<dyn_func, dyn_func_str>(amx, params);
	}

	// native bool:map_set_var(Map:map, AnyTag:key, VariantTag:value, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_set_var(AMX *amx, cell *params)
	{
		return key_at<2, 4>::value_at<3>::map_set<dyn_func, dyn_func_var>(amx, params);
	}

	// native bool:map_set_cell(Map:map, AnyTag:key, offset, AnyTag:value, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_set_cell(AMX *amx, cell *params)
	{
		return key_at<2, 5>::map_set_cell<dyn_func>(amx, params);
	}

	// native bool:map_set_cell_checked(Map:map, AnyTag:key, offset, AnyTag:value, key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_set_cell_checked(AMX *amx, cell *params)
	{
		return key_at<2, 5>::map_set_cell<dyn_func, 6>(amx, params);
	}

	// native bool:map_arr_set(Map:map, const AnyTag:key[], AnyTag:value, key_size=sizeof(key), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_set(AMX *amx, cell *params)
	{
		return key_at<2, 4, 5>::value_at<3, 6>::map_set<dyn_func_arr, dyn_func>(amx, params);
	}

	// native bool:map_arr_set_arr(Map:map, const AnyTag:key[], const AnyTag:value[], value_size=sizeof(value), key_size=sizeof(key), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_set_arr(AMX *amx, cell *params)
	{
		return key_at<2, 5, 6>::value_at<3, 4, 7>::map_set<dyn_func_arr, dyn_func_arr>(amx, params);
	}

	// native bool:map_arr_set_str(Map:map, const AnyTag:key[], const value[], key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_set_str(AMX *amx, cell *params)
	{
		return key_at<2, 4, 5>::value_at<3>::map_set<dyn_func_arr, dyn_func_str>(amx, params);
	}

	// native bool:map_arr_set_var(Map:map, const AnyTag:key[], VariantTags:value, key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_set_var(AMX *amx, cell *params)
	{
		return key_at<2, 4, 5>::value_at<3>::map_set<dyn_func_arr, dyn_func_var>(amx, params);
	}

	// native bool:map_arr_set_cell(Map:map, const AnyTag:key[], offset, AnyTag:value, key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_set_cell(AMX *amx, cell *params)
	{
		return key_at<2, 5, 6>::map_set_cell<dyn_func_arr>(amx, params);
	}

	// native bool:map_arr_set_cell_checked(Map:map, const AnyTag:key[], offset, AnyTag:value, key_size=sizeof(key), key_tag_id=tagof(key), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_set_cell_checked(AMX *amx, cell *params)
	{
		return key_at<2, 5, 6>::map_set_cell<dyn_func_arr, 7>(amx, params);
	}

	// native bool:map_str_set(Map:map, const key[], AnyTag:value, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_str_set(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4>::map_set<dyn_func_str, dyn_func>(amx, params);
	}

	// native bool:map_str_set_arr(Map:map, const key[], const AnyTag:value[], value_size=sizeof(value), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_str_set_arr(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_set<dyn_func_str, dyn_func_arr>(amx, params);
	}

	// native bool:map_str_set_str(Map:map, const key[], const value[]);
	static cell AMX_NATIVE_CALL map_str_set_str(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_set<dyn_func_str, dyn_func_str>(amx, params);
	}

	// native bool:map_str_set_var(Map:map, const key[], VariantTag:value);
	static cell AMX_NATIVE_CALL map_str_set_var(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_set<dyn_func_str, dyn_func_var>(amx, params);
	}

	// native bool:map_str_set_cell(Map:map, const key[], offset, AnyTag:value);
	static cell AMX_NATIVE_CALL map_str_set_cell(AMX *amx, cell *params)
	{
		return key_at<2>::map_set_cell<dyn_func_str>(amx, params);
	}

	// native bool:map_str_set_cell_checked(Map:map, const key[], offset, AnyTag:value, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_str_set_cell_checked(AMX *amx, cell *params)
	{
		return key_at<2>::map_set_cell<dyn_func_str, 5>(amx, params);
	}

	// native bool:map_var_set(Map:map, VariantTag:key, AnyTag:value, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_var_set(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4>::map_set<dyn_func_var, dyn_func>(amx, params);
	}

	// native bool:map_var_set_arr(Map:map, VariantTag:key, const AnyTag:value[], value_size=sizeof(value), value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_var_set_arr(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3, 4, 5>::map_set<dyn_func_var, dyn_func_arr>(amx, params);
	}

	// native bool:map_var_set_str(Map:map, VariantTag:key, const value[]);
	static cell AMX_NATIVE_CALL map_var_set_str(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_set<dyn_func_var, dyn_func_str>(amx, params);
	}

	// native bool:map_var_set_var(Map:map, VariantTag:key, VariantTag:value);
	static cell AMX_NATIVE_CALL map_var_set_var(AMX *amx, cell *params)
	{
		return key_at<2>::value_at<3>::map_set<dyn_func_var, dyn_func_var>(amx, params);
	}

	// native bool:map_var_set_cell(Map:map, VariantTag:key, offset, AnyTag:value);
	static cell AMX_NATIVE_CALL map_var_set_cell(AMX *amx, cell *params)
	{
		return key_at<2>::map_set_cell<dyn_func_var>(amx, params);
	}

	// native bool:map_var_set_cell_checked(Map:map, VariantTag:key, offset, AnyTag:value, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_var_set_cell_checked(AMX *amx, cell *params)
	{
		return key_at<2>::map_set_cell<dyn_func_var, 5>(amx, params);
	}

	// native map_key_at(Map:map, index, offset=0);
	static cell AMX_NATIVE_CALL map_key_at(AMX *amx, cell *params)
	{
		return value_at<3>::map_key_at<dyn_func>(amx, params);
	}

	// native map_arr_key_at(Map:map, index, AnyTag:key[], key_size=sizeof(key));
	static cell AMX_NATIVE_CALL map_arr_key_at(AMX *amx, cell *params)
	{
		return value_at<3, 4>::map_key_at<dyn_func_arr>(amx, params);
	}

	// native Variant:map_var_key_at(Map:map, index);
	static cell AMX_NATIVE_CALL map_var_key_at(AMX *amx, cell *params)
	{
		return value_at<>::map_key_at<dyn_func_var>(amx, params);
	}

	// native bool:map_key_at_checked(Map:map, index, &AnyTag:key, offset=0, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_key_at_checked(AMX *amx, cell *params)
	{
		return value_at<3, 4, 5>::map_key_at<dyn_func>(amx, params);
	}

	// native map_arr_key_at_checked(Map:map, index, AnyTag:key[], key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_key_at_checked(AMX *amx, cell *params)
	{
		return value_at<3, 4, 5>::map_key_at<dyn_func_arr>(amx, params);
	}

	// native map_value_at(Map:map, index, offset=0);
	static cell AMX_NATIVE_CALL map_value_at(AMX *amx, cell *params)
	{
		return value_at<3>::map_value_at<dyn_func>(amx, params);
	}

	// native map_arr_value_at(Map:map, index, AnyTag:value[], value_size=sizeof(value));
	static cell AMX_NATIVE_CALL map_arr_value_at(AMX *amx, cell *params)
	{
		return value_at<3, 4>::map_value_at<dyn_func_arr>(amx, params);
	}

	// native Variant:map_var_value_at(Map:map, index);
	static cell AMX_NATIVE_CALL map_var_value_at(AMX *amx, cell *params)
	{
		return value_at<>::map_value_at<dyn_func_var>(amx, params);
	}

	// native bool:map_value_at_checked(Map:map, index, &AnyTag:value, offset=0, value_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_value_at_checked(AMX *amx, cell *params)
	{
		return value_at<3, 4, 5>::map_value_at<dyn_func>(amx, params);
	}

	// native map_arr_value_at_checked(Map:map, index, AnyTag:value[], value_size=sizeof(value), key_tag_id=tagof(value));
	static cell AMX_NATIVE_CALL map_arr_value_at_checked(AMX *amx, cell *params)
	{
		return value_at<3, 4, 5>::map_value_at<dyn_func_arr>(amx, params);
	}

	// native map_tagof(Map:map, AnyTag:key, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_tagof(AMX *amx, cell *params)
	{
		return key_at<2, 3>::map_tagof<dyn_func>(amx, params);
	}

	// native map_sizeof(Map:map, AnyTag:key, key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_sizeof(AMX *amx, cell *params)
	{
		return key_at<2, 3>::map_sizeof<dyn_func>(amx, params);
	}

	// native map_arr_tagof(Map:map, const AnyTag:key[], key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_tagof(AMX *amx, cell *params)
	{
		return key_at<2, 3, 4>::map_tagof<dyn_func_arr>(amx, params);
	}

	// native map_arr_sizeof(Map:map, const AnyTag:key[], key_size=sizeof(key), key_tag_id=tagof(key));
	static cell AMX_NATIVE_CALL map_arr_sizeof(AMX *amx, cell *params)
	{
		return key_at<2, 3, 4>::map_sizeof<dyn_func_arr>(amx, params);
	}

	// native map_str_tagof(Map:map, const key[]);
	static cell AMX_NATIVE_CALL map_str_tagof(AMX *amx, cell *params)
	{
		return key_at<2>::map_tagof<dyn_func_str>(amx, params);
	}

	// native map_str_sizeof(Map:map, const key[]);
	static cell AMX_NATIVE_CALL map_str_sizeof(AMX *amx, cell *params)
	{
		return key_at<2>::map_sizeof<dyn_func_str>(amx, params);
	}

	// native map_var_tagof(Map:map, VariantTag:key);
	static cell AMX_NATIVE_CALL map_var_tagof(AMX *amx, cell *params)
	{
		return key_at<2>::map_tagof<dyn_func_var>(amx, params);
	}

	// native map_var_sizeof(Map:map, VariantTag:key);
	static cell AMX_NATIVE_CALL map_var_sizeof(AMX *amx, cell *params)
	{
		return key_at<2>::map_sizeof<dyn_func_var>(amx, params);
	}
}

static AMX_NATIVE_INFO native_list[] =
{
	AMX_DECLARE_NATIVE(map_new),
	AMX_DECLARE_NATIVE(map_delete),
	AMX_DECLARE_NATIVE(map_size),
	AMX_DECLARE_NATIVE(map_clear),
	AMX_DECLARE_NATIVE(map_add),
	AMX_DECLARE_NATIVE(map_add_arr),
	AMX_DECLARE_NATIVE(map_add_str),
	AMX_DECLARE_NATIVE(map_add_var),
	AMX_DECLARE_NATIVE(map_arr_add),
	AMX_DECLARE_NATIVE(map_arr_add_arr),
	AMX_DECLARE_NATIVE(map_arr_add_str),
	AMX_DECLARE_NATIVE(map_arr_add_var),
	AMX_DECLARE_NATIVE(map_str_add),
	AMX_DECLARE_NATIVE(map_str_add_arr),
	AMX_DECLARE_NATIVE(map_str_add_str),
	AMX_DECLARE_NATIVE(map_str_add_var),
	AMX_DECLARE_NATIVE(map_var_add),
	AMX_DECLARE_NATIVE(map_var_add_arr),
	AMX_DECLARE_NATIVE(map_var_add_str),
	AMX_DECLARE_NATIVE(map_var_add_var),
	AMX_DECLARE_NATIVE(map_add_map),
	AMX_DECLARE_NATIVE(map_remove),
	AMX_DECLARE_NATIVE(map_arr_remove),
	AMX_DECLARE_NATIVE(map_str_remove),
	AMX_DECLARE_NATIVE(map_var_remove),
	AMX_DECLARE_NATIVE(map_get),
	AMX_DECLARE_NATIVE(map_get_arr),
	AMX_DECLARE_NATIVE(map_get_var),
	AMX_DECLARE_NATIVE(map_get_checked),
	AMX_DECLARE_NATIVE(map_get_arr_checked),
	AMX_DECLARE_NATIVE(map_arr_get),
	AMX_DECLARE_NATIVE(map_arr_get_arr),
	AMX_DECLARE_NATIVE(map_arr_get_var),
	AMX_DECLARE_NATIVE(map_arr_get_checked),
	AMX_DECLARE_NATIVE(map_arr_get_arr_checked),
	AMX_DECLARE_NATIVE(map_str_get),
	AMX_DECLARE_NATIVE(map_str_get_arr),
	AMX_DECLARE_NATIVE(map_str_get_var),
	AMX_DECLARE_NATIVE(map_str_get_checked),
	AMX_DECLARE_NATIVE(map_str_get_arr_checked),
	AMX_DECLARE_NATIVE(map_var_get),
	AMX_DECLARE_NATIVE(map_var_get_arr),
	AMX_DECLARE_NATIVE(map_var_get_var),
	AMX_DECLARE_NATIVE(map_var_get_checked),
	AMX_DECLARE_NATIVE(map_var_get_arr_checked),
	AMX_DECLARE_NATIVE(map_set),
	AMX_DECLARE_NATIVE(map_set_arr),
	AMX_DECLARE_NATIVE(map_set_str),
	AMX_DECLARE_NATIVE(map_set_var),
	AMX_DECLARE_NATIVE(map_set_cell),
	AMX_DECLARE_NATIVE(map_set_cell_checked),
	AMX_DECLARE_NATIVE(map_arr_set),
	AMX_DECLARE_NATIVE(map_arr_set_arr),
	AMX_DECLARE_NATIVE(map_arr_set_str),
	AMX_DECLARE_NATIVE(map_arr_set_var),
	AMX_DECLARE_NATIVE(map_arr_set_cell),
	AMX_DECLARE_NATIVE(map_arr_set_cell_checked),
	AMX_DECLARE_NATIVE(map_str_set),
	AMX_DECLARE_NATIVE(map_str_set_arr),
	AMX_DECLARE_NATIVE(map_str_set_str),
	AMX_DECLARE_NATIVE(map_str_set_var),
	AMX_DECLARE_NATIVE(map_str_set_cell),
	AMX_DECLARE_NATIVE(map_str_set_cell_checked),
	AMX_DECLARE_NATIVE(map_var_set),
	AMX_DECLARE_NATIVE(map_var_set_arr),
	AMX_DECLARE_NATIVE(map_var_set_str),
	AMX_DECLARE_NATIVE(map_var_set_var),
	AMX_DECLARE_NATIVE(map_var_set_cell),
	AMX_DECLARE_NATIVE(map_var_set_cell_checked),
	AMX_DECLARE_NATIVE(map_key_at),
	AMX_DECLARE_NATIVE(map_arr_key_at),
	AMX_DECLARE_NATIVE(map_var_key_at),
	AMX_DECLARE_NATIVE(map_key_at_checked),
	AMX_DECLARE_NATIVE(map_arr_key_at_checked),
	AMX_DECLARE_NATIVE(map_value_at),
	AMX_DECLARE_NATIVE(map_arr_value_at),
	AMX_DECLARE_NATIVE(map_var_value_at),
	AMX_DECLARE_NATIVE(map_value_at_checked),
	AMX_DECLARE_NATIVE(map_arr_value_at_checked),
	AMX_DECLARE_NATIVE(map_tagof),
	AMX_DECLARE_NATIVE(map_sizeof),
	AMX_DECLARE_NATIVE(map_arr_tagof),
	AMX_DECLARE_NATIVE(map_arr_sizeof),
	AMX_DECLARE_NATIVE(map_str_tagof),
	AMX_DECLARE_NATIVE(map_str_sizeof),
	AMX_DECLARE_NATIVE(map_var_tagof),
	AMX_DECLARE_NATIVE(map_var_sizeof),
};

int RegisterMapNatives(AMX *amx)
{
	return amx_Register(amx, native_list, sizeof(native_list) / sizeof(*native_list));
}