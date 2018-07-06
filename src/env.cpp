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

void Lisp::env_push()
{
	auto env = std::make_shared<Lisp_Env>();
	env->set_parent(m_env);
	m_env = env;
}

void Lisp::env_pop()
{
	m_env = m_env->get_parent();
}

std::shared_ptr<Lisp_Obj> Lisp::env_bind(const std::shared_ptr<Lisp_Obj> &lst, const std::shared_ptr<Lisp_Obj> &seq)
{
	if (!lst->is_type(lisp_type_list)) repl_error("(bind (param ...) seq)", error_msg_not_a_list, lst);
	if (!seq->is_type(lisp_type_seq)) repl_error("(bind (param ...) seq)", error_msg_not_a_sequence, seq);

	auto index_vars = 0;
	auto index_vals = 0;
	auto state = 0;
	auto vars = std::static_pointer_cast<Lisp_List>(lst);
	auto vals = std::static_pointer_cast<Lisp_Seq>(seq);
	auto len_vars = vars->length();
	auto len_vals = vals->length();
	auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
	while (index_vars != len_vars)
	{
		auto sym = vars->elem(index_vars);
		if (sym == m_sym_rest)
		{
			state = 1;
			index_vars++;
		}
		else if (sym == m_sym_optional)
		{
			state = 2;
			index_vars++;
		}
		if (index_vars == len_vars) break;
		sym = vars->elem(index_vars);
		if (sym->type() == lisp_type_symbol)
		{
			if (state == 1)
			{
				//rest
				value = vals->slice(index_vals, len_vals);
				index_vars++;
				index_vals = len_vals;
			}
			else if (state == 2)
			{
				//optional
				if (index_vals != len_vals) goto normal;
				value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
				index_vars++;
			}
			else
			{
				//normal
				if (index_vals == len_vals) goto error;
			normal:
				value = vals->elem(index_vals++);
				index_vars++;
			}
			m_env->m_map[std::static_pointer_cast<Lisp_Symbol>(sym)] = value;
		}
		else if (sym->type() == lisp_type_list
			&& index_vals != len_vals)
		{
			value = env_bind(sym, vals->elem(index_vals++));
			index_vars++;
			if (value->type() == lisp_type_error) return value;
		}
		else return repl_error("(bind (param ...) seq)", error_msg_not_a_symbol, lst);
	}
	if (index_vals == len_vals) return value;
error:
	return repl_error("(bind (param ...) seq)", error_msg_wrong_num_of_args, seq);
}

std::shared_ptr<Lisp_Obj> Lisp::env(const std::shared_ptr<Lisp_List> &args)
{
	return m_env;
}

std::shared_ptr<Lisp_Obj> Lisp::defq(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if (len >= 3 && (len & 1))
	{
		auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			if (!(*itr)->is_type(lisp_type_symbol)) return repl_error("(func ?)", error_msg_wrong_types, args);
			auto sym = std::static_pointer_cast<Lisp_Symbol>(*itr);
			value = repl_eval(*(++itr));
			if (value->type() == lisp_type_obj) break;
			m_env->m_map[sym] = value;
		}
		return value;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::setq(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if (len >= 3 && (len & 1))
	{
		auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			if (!(*itr)->is_type(lisp_type_symbol)) return repl_error("(func ?)", error_msg_wrong_types, args);
			auto sym = std::static_pointer_cast<Lisp_Symbol>(*itr);
			value = repl_eval(*(++itr));
			if (value->type() == lisp_type_obj) break;
			if (m_env->set(sym, value) == end(m_env->m_map)) return repl_error("(func ?)", error_msg_wrong_types, args);
		}
		return value;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::def(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if (len >= 3 && (len & 1)
		&& args->m_v[0]->is_type(lisp_type_env))
	{
		auto env = std::static_pointer_cast<Lisp_Env>(args->m_v[0]);
		auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			if (!(*itr)->is_type(lisp_type_symbol)) return repl_error("(func ?)", error_msg_wrong_types, args);
			auto sym = std::static_pointer_cast<Lisp_Symbol>(*itr);
			value = (*(++itr));
			env->m_map[sym] = value;
		}
		return value;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::set(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if (len >= 3 && (len & 1)
		&& args->m_v[0]->is_type(lisp_type_env))
	{
		auto env = std::static_pointer_cast<Lisp_Env>(args->m_v[0]);
		auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			if (!(*itr)->is_type(lisp_type_symbol)) return repl_error("(func ?)", error_msg_wrong_types, args);
			auto sym = std::static_pointer_cast<Lisp_Symbol>(*itr);
			value = (*(++itr));
			if (env->set(sym, value) == end(env->m_map)) return repl_error("(func ?)", error_msg_wrong_types, args);
		}
		return value;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::defined(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_symbol))
	{
		auto sym = std::static_pointer_cast<Lisp_Symbol>(args->m_v[0]);
		auto itr = m_env->find(sym);
		if (itr != end(m_env->m_map)) return itr->second;
		return m_sym_nil;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::defmacro(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 3
		&& args->m_v[1]->is_type(lisp_type_symbol)
		&& args->m_v[2]->is_type(lisp_type_list))
	{
		auto body = args->slice(1, args->length());
		auto sym = std::static_pointer_cast<Lisp_Symbol>(args->m_v[1]);
		std::static_pointer_cast<Lisp_List>(body)->m_v[0] = m_sym_macro;
		m_env->m_map[sym] = body;
		return sym;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::gensym(const std::shared_ptr<Lisp_List> &args)
{
	if (!args->length())
	{
		return intern(std::make_shared<Lisp_Symbol>(std::string{'G'} + std::to_string(m_next_sym++)));
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bind(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2)
	{
		return env_bind(args->m_v[0], args->m_v[1]);
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}
