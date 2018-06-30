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

std::shared_ptr<Lisp_Obj> Lisp::progn(const std::shared_ptr<Lisp_List> &args)
{
	auto l = args->length();
	if (l) return args->m_v[l - 1];
	return m_sym_nil;
}

std::shared_ptr<Lisp_Obj> Lisp::quote(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2) return args->m_v[1];
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::apply(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[1]->is_type(lisp_type_list))
	{
		return repl_apply(args->m_v[0], std::static_pointer_cast<Lisp_List>(args->m_v[1]));
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::cond(const std::shared_ptr<Lisp_List> &args)
{
	auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
	for (auto &cnd : args->m_v)
	{
		if (!cnd->is_type(lisp_type_list)) return std::make_shared<Lisp_Obj>();
		auto lst = std::static_pointer_cast<Lisp_List>(cnd);
		if (!lst->length()) return std::make_shared<Lisp_Obj>();
		auto tst = repl_eval(lst->m_v[0]);
		if (tst->type() == lisp_type_obj) return tst;
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
	if (args->length() < 2) return std::make_shared<Lisp_Obj>();
	for (;;)
	{
		auto value = repl_eval(args->m_v[1]);
		if (value->type() == lisp_type_obj
			|| value == m_sym_nil) return value;
		for (auto itr = begin(args->m_v) + 2; itr != end(args->m_v); ++itr)
		{
			value = repl_eval(*itr);
			if (value->type() == lisp_type_obj) return value;
		}
	}
}
