/*
    ChrysaLisp++
    Copyright (C) 2018 Chris Hinsley
	chris (dot) hinsley (at) gmail (dot) com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "lisp.h"

std::shared_ptr<Lisp_Obj> Lisp::lcatch(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 3)
	{
		auto value = repl_eval(args->m_v[1]);
		if (value->type() != lisp_type_obj) return value; 
		auto value1 = repl_eval(args->m_v[2]);
		if (value1->type() == lisp_type_obj
			|| value1 != m_sym_nil) return value1;
		return value;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::progn(const std::shared_ptr<Lisp_List> &args)
{
	auto l = args->length();
	if (l) return args->m_v[l - 1];
	return m_sym_nil;
}

std::shared_ptr<Lisp_Obj> Lisp::quote(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2) return args->m_v[1];
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

void qquote1(Lisp *lisp, const std::shared_ptr<Lisp_Obj> &o, std::shared_ptr<Lisp_List> &cat_list)
{
	if (o->type() == lisp_type_list
		&& std::static_pointer_cast<Lisp_List>(o)->length())
	{
		auto olst = std::static_pointer_cast<Lisp_List>(o);
		if (olst->m_v[0] == lisp->m_sym_unquote)
		{
			auto lst = std::make_shared<Lisp_List>();
			lst->m_v.push_back(lisp->m_sym_list);
			lst->m_v.push_back(olst->m_v[1]);
			cat_list->m_v.push_back(lst);
		}
		else if (olst->m_v[0] == lisp->m_sym_splicing)
		{
			cat_list->m_v.push_back(olst->m_v[1]);
		}
		else
		{
			auto i_cat_list = std::make_shared<Lisp_List>();
			i_cat_list->m_v.push_back(lisp->m_sym_cat);
			for (auto &&i : olst->m_v) qquote1(lisp, i, i_cat_list);
			auto lst = std::make_shared<Lisp_List>();
			lst->m_v.push_back(lisp->m_sym_list);
			auto qlst = std::make_shared<Lisp_List>();
			qlst->m_v.push_back(lisp->m_sym_quote);
			qlst->m_v.push_back(lisp->repl_eval(i_cat_list));
			lst->m_v.push_back(qlst);
			cat_list->m_v.push_back(lst);
		}
	}
	else
	{
		auto lst = std::make_shared<Lisp_List>();
		lst->m_v.push_back(lisp->m_sym_list);
		auto qlst = std::make_shared<Lisp_List>();
		qlst->m_v.push_back(lisp->m_sym_quote);
		qlst->m_v.push_back(o);
		lst->m_v.push_back(qlst);
		cat_list->m_v.push_back(lst);
	}
}

std::shared_ptr<Lisp_Obj> Lisp::qquote(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2)
	{
		if (!args->m_v[1]->is_type(lisp_type_list)) return args->m_v[1];
		auto cat_list = std::make_shared<Lisp_List>();
		cat_list->m_v.push_back(m_sym_cat);
		for (auto &&i : std::static_pointer_cast<Lisp_List>(args->m_v[1])->m_v) qquote1(this, i, cat_list);
		return repl_eval(cat_list);
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::apply(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[1]->is_type(lisp_type_list))
	{
		return repl_apply(args->m_v[0], std::static_pointer_cast<Lisp_List>(args->m_v[1]));
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::cond(const std::shared_ptr<Lisp_List> &args)
{
	auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
	for (auto itrc = begin(args->m_v) + 1; itrc != end(args->m_v); ++itrc)
	{
		auto &&cnd = *itrc;
		if (!cnd->is_type(lisp_type_list)) return repl_error("(func ?)", error_msg_wrong_types, args);
		auto lst = std::static_pointer_cast<Lisp_List>(cnd);
		if (!lst->length()) return repl_error("(func ?)", error_msg_wrong_types, args);
		auto tst = repl_eval(lst->m_v[0]);
		if (tst->type() == lisp_type_error) return tst;
		if (tst != m_sym_nil)
		{
			for (auto itr = begin(lst->m_v) + 1; itr != end(lst->m_v); ++itr)
			{
				value = repl_eval(*itr);
				if (value->type() == lisp_type_obj) break;
			}
			break;
		}
	}
	return value;
}

std::shared_ptr<Lisp_Obj> Lisp::lwhile(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() < 2) return repl_error("(func ?)", error_msg_wrong_types, args);
	for (;;)
	{
		auto value = repl_eval(args->m_v[1]);
		if (value->type() == lisp_type_obj
			|| value == m_sym_nil) return value;
		for (auto itr = begin(args->m_v) + 2; itr != end(args->m_v); ++itr)
		{
			value = repl_eval(*itr);
			if (value->type() == lisp_type_error) return value;
		}
	}
}

std::shared_ptr<Lisp_Obj> Lisp::time(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length()) return repl_error("(func ?)", error_msg_wrong_types, args);
	auto now = std::chrono::high_resolution_clock::now();
	auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
	auto value = now_ns.time_since_epoch().count();
	return std::make_shared<Lisp_Number>(value);
}

std::shared_ptr<Lisp_Obj> Lisp::repl_apply(const std::shared_ptr<Lisp_Obj> &func, const std::shared_ptr<Lisp_List> &args)
{
	switch (func->type())
	{
		case lisp_type_func:
		{
			auto f = std::static_pointer_cast<Lisp_Func>(func);
			return (*this.*f->m_func)(args);
		}
		case lisp_type_list:
		{
			auto f = std::static_pointer_cast<Lisp_List>(func);
			if (f->length() > 1
				&& (f->m_v[0] == m_sym_lambda || f->m_v[0] == m_sym_macro))
			{
				env_push();
				auto value = env_bind(f->m_v[1], args);
				if (value->type() != lisp_type_obj)
				{
					//eval the body
					for (auto itr = begin(f->m_v) + 2; itr != end(f->m_v); ++itr)
					{
						value = repl_eval(*itr);
						if (value->type() == lisp_type_obj) break;
					}
				}
				env_pop();
				return value;
			}
			//fall through
		}
		default:
			return repl_error("(func ?)", error_msg_wrong_types, args);
	}
}

std::shared_ptr<Lisp_Obj> Lisp::repl_eval(const std::shared_ptr<Lisp_Obj> &obj)
{
	switch (obj->type())
	{
		case lisp_type_symbol:
		{
			auto sym = std::static_pointer_cast<Lisp_Symbol>(obj);
			auto obj = m_env->get(sym);
			if (obj == nullptr) return repl_error("(eval form [env])", error_msg_symbol_not_bound, sym);
			return obj;
		}
		case lisp_type_list:
		{
			auto lst = std::static_pointer_cast<Lisp_List>(obj);
			if (lst->m_v.empty()) return repl_error("(lambda ([arg ...]) body)", error_msg_not_a_lambda, lst);
			auto func = repl_eval(lst->m_v[0]);
			if (func->type() == lisp_type_error) return func;
			if (func->type() == lisp_type_func
				&& std::static_pointer_cast<Lisp_Func>(func)->m_ftype != 0)
			{
				//give it to me raw
				return repl_apply(func, lst);
			}
			else
			{
				//eval the args
				auto args = std::make_shared<Lisp_List>();
				args->m_v.reserve(lst->length() - 1);
				for (auto itr = begin(lst->m_v) + 1; itr != end(lst->m_v); ++itr)
				{
					auto eo = repl_eval(*itr);
					if (eo->type() == lisp_type_error) return eo;
					args->m_v.push_back(eo);
				}
				return repl_apply(func, args);
			}
		}
		default:
			return obj;
	}
}
