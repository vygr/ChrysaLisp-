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
		if (value->type() != lisp_type_error) return value; 
		auto value1 = repl_eval(args->m_v[2]);
		if (value1->type() == lisp_type_error
			|| value1 != m_sym_nil) return value1;
		return value;
	}
	return repl_error("(catch form eform)", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::progn(const std::shared_ptr<Lisp_List> &args)
{
	auto l = args->length();
	if (l) return args->m_v[l - 1];
	return m_sym_nil;
}

std::shared_ptr<Lisp_Obj> Lisp::lambda(const std::shared_ptr<Lisp_List> &args)
{
	return args;
}

std::shared_ptr<Lisp_Obj> Lisp::quote(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2) return args->m_v[1];
	return repl_error("(quote form)", error_msg_wrong_num_of_args, args);
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
	return repl_error("(quasi-quote form)", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::cond(const std::shared_ptr<Lisp_List> &args)
{
	auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
	for (auto itrc = begin(args->m_v) + 1; itrc != end(args->m_v); ++itrc)
	{
		auto &&cnd = *itrc;
		if (!cnd->is_type(lisp_type_list))
			return repl_error("(cond (tst body) ...)", error_msg_not_a_list, args);
		auto lst = std::static_pointer_cast<Lisp_List>(cnd);
		if (!lst->length())
			return repl_error("(cond (tst body) ...)", error_msg_wrong_num_of_args, args);
		auto tst = repl_eval(lst->m_v[0]);
		if (tst->type() == lisp_type_error) return tst;
		if (tst != m_sym_nil)
		{
			for (auto itr = begin(lst->m_v) + 1; itr != end(lst->m_v); ++itr)
			{
				value = repl_eval(*itr);
				if (value->type() == lisp_type_error) break;
			}
			break;
		}
	}
	return value;
}

std::shared_ptr<Lisp_Obj> Lisp::lwhile(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1)
	{
		for (;;)
		{
			auto value = repl_eval(args->m_v[1]);
			if (value->type() == lisp_type_error
				|| value == m_sym_nil) return value;
			for (auto itr = begin(args->m_v) + 2; itr != end(args->m_v); ++itr)
			{
				value = repl_eval(*itr);
				if (value->type() == lisp_type_error) return value;
			}
		}
	}
	return repl_error("(while tst body)", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::time(const std::shared_ptr<Lisp_List> &args)
{
	if (!args->length())
	{
		auto now = std::chrono::high_resolution_clock::now();
		auto now_ms = std::chrono::time_point_cast<std::chrono::microseconds>(now);
		auto value = now_ms.time_since_epoch().count();
		return std::make_shared<Lisp_Integer>(value);
	}
	return repl_error("(time)", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::age(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		struct stat result;
		if (stat(std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string.c_str(), &result) == 0)
			return std::make_shared<Lisp_Integer>(result.st_mtime);
		return std::make_shared<Lisp_Integer>(0);
	}
	return repl_error("(age path)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::type(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1)
	{
		return std::make_shared<Lisp_Integer>(args->m_v[0]->type());
	}
	return repl_error("(type? obj)", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::eval(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1) return repl_eval(args->m_v[0]);
	if (args->length() == 2)
	{
		if (args->m_v[1]->is_type(lisp_type_env))
		{
			auto old_env = m_env;
			m_env = std::static_pointer_cast<Lisp_Env>(args->m_v[1]);
			auto value = repl_eval(args->m_v[0]);
			m_env = old_env;
			return value;
		}
		return repl_error("(eval form [env])", error_msg_not_an_environment, args);
	}
	return repl_error("(eval form [env])", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::apply(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[1]->is_type(lisp_type_list))
	{
		return repl_apply(args->m_v[0], std::static_pointer_cast<Lisp_List>(args->m_v[1]));
	}
	return repl_error("(apply lambda list)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::sym(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1)
	{
		if (args->m_v[0]->is_type(lisp_type_string))
		{
			if (args->m_v[0]->type() == lisp_type_symbol) return args->m_v[0];
			return intern(std::make_shared<Lisp_Symbol>(std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string));
		}
		return repl_error("(sym form)", error_msg_not_a_string, args);
	}
	return repl_error("(sym form)", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::lthrow(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		return repl_error(std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string, error_msg, args->m_v[1]);
	}
	return repl_error("(throw str form)", error_msg_wrong_types, args);
}
