#include "natives.h"
#include "modules/variants.h"

template <size_t... Indices>
class value_at
{
	using value_ftype = typename dyn_factory<Indices...>::type;
	using result_ftype = typename dyn_result<Indices...>::type;

public:
	// native Variant:var_new(value, ...);
	template <value_ftype Factory>
	static cell AMX_NATIVE_CALL var_new(AMX *amx, cell *params)
	{
		return variants::create(Factory(amx, params[Indices]...));
	}

	// native var_get(VariantTag:var, ...);
	template <result_ftype Factory>
	static cell AMX_NATIVE_CALL var_get(AMX *amx, cell *params)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		return Factory(amx, *var, params[Indices]...);
	}
};

namespace Natives
{
	// native Variant:var_new(AnyTag:value, tag_id=tagof(value));
	AMX_DEFINE_NATIVE(var_new, 2)
	{
		return value_at<1, 2>::var_new<dyn_func>(amx, params);
	}

	// native Variant:var_new_arr(const AnyTag:value[], size=sizeof(value), tag_id=tagof(value));
	AMX_DEFINE_NATIVE(var_new_arr, 3)
	{
		return value_at<1, 2, 3>::var_new<dyn_func_arr>(amx, params);
	}

	// native Variant:var_new_arr_2d(const AnyTag:value[][], size=sizeof(value), size2=sizeof(value[]), tag_id=tagof(value));
	AMX_DEFINE_NATIVE(var_new_arr_2d, 4)
	{
		return value_at<1, 2, 3, 4>::var_new<dyn_func_arr>(amx, params);
	}

	// native Variant:var_new_arr_3d(const AnyTag:value[][], size=sizeof(value), size2=sizeof(value[]), size3=sizeof(value[]), tag_id=tagof(value));
	AMX_DEFINE_NATIVE(var_new_arr_3d, 5)
	{
		return value_at<1, 2, 3, 4, 5>::var_new<dyn_func_arr>(amx, params);
	}

	// native Variant:var_new_buf(size, tag_id=0);
	AMX_DEFINE_NATIVE(var_new_buf, 1)
	{
		return variants::create(amx, nullptr, params[1], optparam(2, 0));
	}

	// native Variant:var_new_str(const value[]);
	AMX_DEFINE_NATIVE(var_new_str, 1)
	{
		return value_at<1>::var_new<dyn_func_str>(amx, params);
	}

	// native Variant:var_new_str_s(StringTag:value);
	AMX_DEFINE_NATIVE(var_new_str_s, 1)
	{
		return value_at<1>::var_new<dyn_func_str_s>(amx, params);
	}

	// native Variant:var_new_var(VariantTag:value);
	AMX_DEFINE_NATIVE(var_new_var, 1)
	{
		return value_at<1>::var_new<dyn_func_var>(amx, params);
	}

	// native Variant:var_acquire(VariantTag:var);
	AMX_DEFINE_NATIVE(var_acquire, 1)
	{
		decltype(variants::pool)::ref_container *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		if(!variants::pool.acquire_ref(*var)) return 0;
		return params[1];
	}

	// native Variant:var_release(VariantTag:var);
	AMX_DEFINE_NATIVE(var_release, 1)
	{
		decltype(variants::pool)::ref_container *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		if(!variants::pool.release_ref(*var)) return 0;
		return params[1];
	}

	// native bool:var_delete(VariantTag:var);
	AMX_DEFINE_NATIVE(var_delete, 1)
	{
		return variants::pool.remove_by_id(params[1]);
	}

	// native bool:var_valid(VariantTag:var);
	AMX_DEFINE_NATIVE(var_valid, 1)
	{
		dyn_object *var;
		return variants::pool.get_by_id(params[1], var);
	}

	// native Variant:var_clone(VariantTag:var);
	AMX_DEFINE_NATIVE(var_clone, 1)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		return variants::pool.get_id(variants::pool.add(var->clone()));
	}

	// native var_get(VariantTag:var, const offsets[]={cellmin}, offsets_size=sizeof(offsets));
	AMX_DEFINE_NATIVE(var_get, 3)
	{
		return value_at<2, 3>::var_get<dyn_func>(amx, params);
	}

	// native var_get_arr(VariantTag:var, AnyTag:value[], const offsets[]={cellmin}, size=sizeof(value), offsets_size=sizeof(offsets));
	AMX_DEFINE_NATIVE(var_get_arr, 5)
	{
		return value_at<2, 3, 4, 5>::var_get<dyn_func_arr>(amx, params);
	}

	// native String:var_get_str_s(VariantTag:var, const offsets[]={cellmin}, offsets_size=sizeof(offsets));
	AMX_DEFINE_NATIVE(var_get_str_s, 3)
	{
		return value_at<2, 3>::var_get<dyn_func_str_s>(amx, params);
	}

	// native bool:var_get_safe(VariantTag:var, &AnyTag:value, const offsets[]={cellmin}, offsets_size=sizeof(offsets), tag_id=tagof(value));
	AMX_DEFINE_NATIVE(var_get_safe, 5)
	{
		return value_at<2, 3, 4, 5>::var_get<dyn_func>(amx, params);
	}

	// native var_get_arr_safe(VariantTag:var, AnyTag:value[], const offsets[]={cellmin}, size=sizeof(value), offsets_size=sizeof(offsets), tag_id=tagof(value));
	AMX_DEFINE_NATIVE(var_get_arr_safe, 6)
	{
		return value_at<2, 3, 4, 5, 6>::var_get<dyn_func_arr>(amx, params);
	}

	// native var_get_str_safe(VariantTag:var, value[], const offsets[]={cellmin}, size=sizeof(value), offsets_size=sizeof(offsets));
	AMX_DEFINE_NATIVE(var_get_str_safe, 5)
	{
		return value_at<2, 3, 4, 5>::var_get<dyn_func_str>(amx, params);
	}

	// native String:var_get_str_safe_s(VariantTag:var, const offsets[]={cellmin}, offsets_size=sizeof(offsets));
	AMX_DEFINE_NATIVE(var_get_str_safe_s, 3)
	{
		return value_at<2, 3, 0>::var_get<dyn_func_str_s>(amx, params);
	}

	// native bool:var_set_cell(VariantTag:var, AnyTag:value, const offsets[]={cellmin}, offsets_size=sizeof(offsets));
	AMX_DEFINE_NATIVE(var_set_cell, 4)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		if(!var->is_array()) return 0;
		cell offsets_size = params[4];
		cell *offsets_addr = get_offsets(amx, params[3], offsets_size);
		return var->set_cell(offsets_addr, offsets_size, params[2]);
	}

	// native bool:var_set_cell_safe(VariantTag:var, AnyTag:value, const offsets[]={cellmin}, offsets_size=sizeof(offsets), tag_id=tagof(value));
	AMX_DEFINE_NATIVE(var_set_cell_safe, 5)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		if(!var->is_array()) return 0;
		if(!var->tag_assignable(amx, params[5])) return 0;
		cell offsets_size = params[4];
		cell *offsets_addr = get_offsets(amx, params[3], offsets_size);
		return var->set_cell(offsets_addr, offsets_size, params[2]);
	}

	// native var_tagof(VariantTag:var);
	AMX_DEFINE_NATIVE(var_tagof, 1)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		return var->get_tag(amx);
	}

	// native tag_uid:var_tag_uid(VariantTag:var);
	AMX_DEFINE_NATIVE(var_tag_uid, 1)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		return var->get_tag()->uid;
	}

	// native var_sizeof(VariantTag:var, const offsets[]={cellmin}, offsets_size=sizeof(offsets));
	AMX_DEFINE_NATIVE(var_sizeof, 3)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		cell offsets_size = params[3];
		cell *offsets_addr = get_offsets(amx, params[2], offsets_size);
		return var->get_size(offsets_addr, offsets_size);
	}
	
	// native Variant:var_bin_op(VariantTag:var1, VariantTag:var2);
	template <dyn_object (dyn_object::*op)(const dyn_object&) const>
	static cell AMX_NATIVE_CALL var_bin_op(AMX *amx, cell *params)
	{
		dyn_object *var1;
		if(!variants::pool.get_by_id(params[1], var1)) return 0;
		dyn_object *var2;
		if(!variants::pool.get_by_id(params[2], var2)) return 0;
		auto var = (var1->*op)(*var2);
		if(var.empty()) return 0;
		return variants::create(std::move(var));
	}

	// native Variant:var_add(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_add, 2)
	{
		return var_bin_op<&dyn_object::operator+>(amx, params);
	}

	// native Variant:var_add(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_sub, 2)
	{
		return var_bin_op<&dyn_object::operator- >(amx, params);
	}

	// native Variant:var_add(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_mul, 2)
	{
		return var_bin_op<&dyn_object::operator*>(amx, params);
	}

	// native Variant:var_add(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_div, 2)
	{
		return var_bin_op<&dyn_object::operator/>(amx, params);
	}

	// native Variant:var_add(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_mod, 2)
	{
		return var_bin_op<&dyn_object::operator% >(amx, params);
	}
	
	// native Variant:var_un_op(VariantTag:var);
	template <dyn_object (dyn_object::*op)() const>
	static cell AMX_NATIVE_CALL var_un_op(AMX *amx, cell *params)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		auto result = (var->*op)();
		if(result.empty()) return 0;
		return variants::create(std::move(result));
	}

	// native Variant:var_neg(VariantTag:var);
	AMX_DEFINE_NATIVE(var_neg, 1)
	{
		return var_un_op<&dyn_object::operator- >(amx, params);
	}

	// native Variant:var_neg(VariantTag:var);
	AMX_DEFINE_NATIVE(var_inc, 1)
	{
		return var_un_op<&dyn_object::inc>(amx, params);
	}

	// native Variant:var_neg(VariantTag:var);
	AMX_DEFINE_NATIVE(var_dec, 1)
	{
		return var_un_op<&dyn_object::dec>(amx, params);
	}

	// native bool:var_log_op(VariantTag:var1, VariantTag:var2);
	template <bool(dyn_object::*op)(const dyn_object&) const>
	static cell AMX_NATIVE_CALL var_log_op(AMX *amx, cell *params)
	{
		dyn_object *var1, *var2;
		if((!variants::pool.get_by_id(params[1], var1) && var1 != nullptr) || (!variants::pool.get_by_id(params[2], var2) && var2 != nullptr)) return 0;
		bool init1 = false, init2 = false;
		if(var1 == nullptr)
		{
			var1 = new dyn_object();
			init1 = true;
		}
		if(var2 == nullptr)
		{
			var2 = new dyn_object();
			init2 = false;
		}
		bool result = (var1->*op)(*var2);
		if(init1)
		{
			delete var1;
		}
		if(init2)
		{
			delete var2;
		}
		return result;
	}

	// native bool:var_eq(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_eq, 2)
	{
		return var_log_op<&dyn_object::operator==>(amx, params);
	}

	// native bool:var_neq(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_neq, 2)
	{
		return var_log_op<&dyn_object::operator!=>(amx, params);
	}

	// native bool:var_lt(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_lt, 2)
	{
		return var_log_op<&dyn_object::operator<>(amx, params);
	}

	// native bool:var_gt(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_gt, 2)
	{
		return var_log_op<(&dyn_object::operator>)>(amx, params);
	}

	// native bool:var_lte(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_lte, 2)
	{
		return var_log_op<&dyn_object::operator<=>(amx, params);
	}

	// native bool:var_gte(VariantTag:var1, VariantTag:var2);
	AMX_DEFINE_NATIVE(var_gte, 2)
	{
		return var_log_op<&dyn_object::operator>=>(amx, params);
	}

	// native bool:var_not(VariantTag:var);
	AMX_DEFINE_NATIVE(var_not, 1)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 1;
		return !(*var);
	}

	// native Variant:var_call_op(VariantTag:var, tag_op:tag_op, AnyTag:...);
	AMX_DEFINE_NATIVE(var_call_op, 2)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		size_t numargs = (params[0] / sizeof(cell)) - 2;
		cell *args = new cell[numargs];
		for(size_t i = 0; i < numargs; i++)
		{
			cell *addr;
			if(amx_GetAddr(amx, params[3 + i], &addr) == AMX_ERR_NONE)
			{
				args[i] = *addr;
			}
		}
		auto result = var->call_op(static_cast<op_type>(params[2]), args, numargs, true);
		delete[] args;
		if(result.empty()) return 0;
		return variants::create(std::move(result));
	}

	// native Variant:var_call_op_raw(VariantTag:var, tag_op:tag_op, AnyTag:...);
	AMX_DEFINE_NATIVE(var_call_op_raw, 2)
	{
		dyn_object *var;
		if(!variants::pool.get_by_id(params[1], var)) return 0;
		size_t numargs = (params[0] / sizeof(cell)) - 2;
		cell *args = new cell[numargs];
		for(size_t i = 0; i < numargs; i++)
		{
			cell *addr;
			if(amx_GetAddr(amx, params[3 + i], &addr) == AMX_ERR_NONE)
			{
				args[i] = *addr;
			}
		}
		auto result = var->call_op(static_cast<op_type>(params[2]), args, numargs, false);
		delete[] args;
		if(result.empty()) return 0;
		return variants::create(std::move(result));
	}
}

static AMX_NATIVE_INFO native_list[] =
{
	AMX_DECLARE_NATIVE(var_new),
	AMX_DECLARE_NATIVE(var_new_arr),
	AMX_DECLARE_NATIVE(var_new_arr_2d),
	AMX_DECLARE_NATIVE(var_new_arr_3d),
	AMX_DECLARE_NATIVE(var_new_buf),
	AMX_DECLARE_NATIVE(var_new_str),
	AMX_DECLARE_NATIVE(var_new_str_s),
	AMX_DECLARE_NATIVE(var_new_var),
	AMX_DECLARE_NATIVE(var_acquire),
	AMX_DECLARE_NATIVE(var_release),
	AMX_DECLARE_NATIVE(var_delete),
	AMX_DECLARE_NATIVE(var_valid),
	AMX_DECLARE_NATIVE(var_clone),

	AMX_DECLARE_NATIVE(var_get),
	AMX_DECLARE_NATIVE(var_get_arr),
	AMX_DECLARE_NATIVE(var_get_str_s),
	AMX_DECLARE_NATIVE(var_get_safe),
	AMX_DECLARE_NATIVE(var_get_arr_safe),
	AMX_DECLARE_NATIVE(var_get_str_safe),
	AMX_DECLARE_NATIVE(var_get_str_safe_s),

	AMX_DECLARE_NATIVE(var_set_cell),
	AMX_DECLARE_NATIVE(var_set_cell_safe),

	AMX_DECLARE_NATIVE(var_tagof),
	AMX_DECLARE_NATIVE(var_tag_uid),
	AMX_DECLARE_NATIVE(var_sizeof),
	AMX_DECLARE_NATIVE(var_add),
	AMX_DECLARE_NATIVE(var_sub),
	AMX_DECLARE_NATIVE(var_mul),
	AMX_DECLARE_NATIVE(var_div),
	AMX_DECLARE_NATIVE(var_mod),
	AMX_DECLARE_NATIVE(var_neg),
	AMX_DECLARE_NATIVE(var_inc),
	AMX_DECLARE_NATIVE(var_dec),
	AMX_DECLARE_NATIVE(var_eq),
	AMX_DECLARE_NATIVE(var_neq),
	AMX_DECLARE_NATIVE(var_lt),
	AMX_DECLARE_NATIVE(var_gt),
	AMX_DECLARE_NATIVE(var_lte),
	AMX_DECLARE_NATIVE(var_gte),
	AMX_DECLARE_NATIVE(var_not),
	AMX_DECLARE_NATIVE(var_call_op),
	AMX_DECLARE_NATIVE(var_call_op_raw),
};

int RegisterVariantNatives(AMX *amx)
{
	return amx_Register(amx, native_list, sizeof(native_list) / sizeof(*native_list));
}
