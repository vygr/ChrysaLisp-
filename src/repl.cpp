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

std::shared_ptr<Lisp_Symbol> Lisp::intern(const std::shared_ptr<Lisp_Symbol> &obj)
{
	auto itr = m_intern_sym_map.find(obj->m_string);
	if (itr != end(m_intern_sym_map)) return itr->second;
	m_intern_sym_map[obj->m_string] = obj;
	return obj;
}

int Lisp::repl_read_whitespace(std::istream &in) const
{
	int p;
	for (;;)
	{
		p = in.peek();
		if (p == -1) break;
		if (!std::isspace(((unsigned char)p))) break;
		in.get();
	}
	return p;
}

std::shared_ptr<Lisp_Obj> Lisp::repl_read_string(std::istream &in, char term) const
{
	auto obj = std::make_shared<Lisp_String>();
	//skip '"'
	in.get();
	for (;;)
	{
		auto c = in.get();
		if (c == term) break;
		obj->m_string.push_back(c);
	}
	return obj;
}

std::shared_ptr<Lisp_Obj> Lisp::repl_read_symbol(std::istream &in)
{
	auto obj = std::make_shared<Lisp_Symbol>();
	for (;;)
	{
		auto p = in.peek();
		if (p == '(' || p == ')'
			|| std::isspace(((unsigned char)p))) break;
		obj->m_string.push_back(in.get());
	}
	return intern(obj);
}

std::shared_ptr<Lisp_Obj> Lisp::repl_read_number(std::istream &in) const
{
	auto obj = std::make_shared<Lisp_Number>();
	auto p = in.peek();
	auto sign = 1;
	if (p == '-')
	{
		sign = -1;
		in.get();
	}
	auto buffer = std::string{};
	for (;;)
	{
		auto p = in.peek();
		if (p == '.'
			|| (p >= '0' && p <= '9')
			|| (p >= 'a' && p <= 'z')
			|| (p >= 'A' && p <= 'Z'))
		{
			buffer.push_back(in.get());
		}
		else break;
	}
	auto base = 10;
	auto itr = begin(buffer);
	if (buffer[1] == 'x')
	{
		base = 16;
		itr += 2;
	}
	else if (buffer[1] == 'o')
	{
		base = 8;
		itr += 2;
	}
	else if (buffer[1] == 'b')
	{
		base = 2;
		itr += 2;
	}
	auto frac = 0ll;
	for (; itr != end(buffer); ++itr)
	{
		auto c = (int)*itr;
		if (c == '.')
		{
			frac = 1;
			continue;
		}
		if (c >= 'a') c -= 'a' - 10;
		else if (c >= 'A') c -= 'a' - 10;
		else c -= '0';
		obj->m_value = obj->m_value * base + c;
		frac *= base;
	}
	if (frac) obj->m_value = (obj->m_value << 16) / frac;		
	obj->m_value *= sign;
	return obj;
}

std::shared_ptr<Lisp_Obj> Lisp::repl_read_list(std::istream &in)
{
	auto lst = std::make_shared<Lisp_List>();
	//skip '('
	in.get();
	for (;;)
	{
		auto c = repl_read_whitespace(in);
		if (c == ')') break;
		if (c == ';')
		{
			in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			continue;
		}
		lst->m_v.push_back(repl_read(in));
	}
	//skip ')'
	in.get();
	return lst;
}

std::shared_ptr<Lisp_Obj> Lisp::repl_read_rmacro(std::istream &in,  const std::shared_ptr<Lisp_Symbol> &sym)
{
	auto lst = std::make_shared<Lisp_List>();
	lst->m_v.push_back(sym);
	//skip '
	in.get();
	lst->m_v.push_back(repl_read(in));
	return lst;
}

std::shared_ptr<Lisp_Obj> Lisp::repl_read(std::istream &in)
{
	int c;
	for (;;)
	{
		c = repl_read_whitespace(in);
		if (c != ';') break;
		in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
	if (c == -1) return m_sym_nil;
	if (c == ')')
	{
		in.get();
		return repl_error("unexpected )", error_msg, m_sym_nil);
	}
	if (c == '}')
	{
		in.get();
		return repl_error("unexpected }", error_msg, m_sym_nil);
	}
	else if (c == '(') return repl_read_list(in);
	else if (c == '"') return repl_read_string(in, '"');
	else if (c == '{') return repl_read_string(in, '}');
	else if (c == '-' || std::isdigit(((unsigned char)c))) return repl_read_number(in);
	else if (c == 39) return repl_read_rmacro(in, m_sym_quote);
	else if (c == '`') return repl_read_rmacro(in, m_sym_qquote);
	else if (c == ',') return repl_read_rmacro(in, m_sym_unquote);
	else if (c == '~') return repl_read_rmacro(in, m_sym_splicing);
	return repl_read_symbol(in);
}

std::shared_ptr<Lisp_Obj> Lisp::repl(std::istream &in)
{
	in >> std::noskipws;
	for (;;)
	{
		auto obj = repl_read(in);
		if (obj == m_sym_nil) return obj;
		// std::cout << "--INPUT--\n";
		// obj->print();
		while (repl_expand(obj, 0));
		// std::cout << "\n--EXPAND-\n";
		// obj->print();
		// std::cout << "\n--EVAL-\n";
		obj = repl_eval(obj);
		obj->print(std::cout);
		std::cout << "\n";
		std::cout << std::endl;
	}
}

int Lisp::repl_expand(std::shared_ptr<Lisp_Obj> &o, int cnt)
{
	if (o->is_type(lisp_type_list)
		&& std::static_pointer_cast<Lisp_List>(o)->length())
	{
		auto lst = std::static_pointer_cast<Lisp_List>(o);
		auto &obj = lst->m_v[0];
		if (obj == m_sym_quote) return cnt;
		if (obj->is_type(lisp_type_symbol))
		{
			auto sym = std::static_pointer_cast<Lisp_Symbol>(obj);
			auto itr = m_env->find(sym);
			if (itr == end(m_env->m_map)) goto decend;
			if (!itr->second->is_type(lisp_type_list)) goto decend;
			auto macro = std::static_pointer_cast<Lisp_List>(itr->second);
			if (macro->m_v[0] != m_sym_macro) goto decend;
			o = repl_apply(macro, std::static_pointer_cast<Lisp_List>(lst->slice(1, lst->length())));
			cnt++;
		}
		else
		{
		decend:
			for (auto &&o : lst->m_v) cnt = repl_expand(o, cnt);
		}
	}
	return cnt;
}

std::shared_ptr<Lisp_Obj> Lisp::repl_error(const std::string &msg, int type, const std::shared_ptr<Lisp_Obj> &o)
{
	static const std::vector<std::string> errors =
	{
		{"error"},
		{"not_a_canvas"},
		{"not_a_class"},
		{"not_a_filename"},
		{"not_a_lambda"},
		{"not_a_list"},
		{"not_a_number"},
		{"not_a_pipe"},
		{"not_a_sequence"},
		{"not_a_stream"},
		{"not_a_string"},
		{"not_a_symbol"},
		{"not_all_lists"},
		{"not_all_nums"},
		{"not_all_strings"},
		{"not_an_environment"},
		{"not_valid_index"},
		{"open_error"},
		{"symbol_not_bound"},
		{"wrong_num_of_args"},
		{"wrong_types"}
	};

	auto file = std::static_pointer_cast<Lisp_String>(m_env->get(m_sym_stream_name));
	auto line = std::static_pointer_cast<Lisp_Number>(m_env->get(m_sym_stream_line));
	return std::make_shared<Lisp_Error>(msg + " " + errors[type], file->m_string, line->m_value, o);
}

std::shared_ptr<Lisp_Obj> Lisp::eval(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1)
	{
		return repl_eval(args->m_v[0]);
	}
	else if (args->length() == 2
		&& args->m_v[1]->is_type(lisp_type_env))
	{
		auto old_env = m_env;
		m_env = std::static_pointer_cast<Lisp_Env>(args->m_v[1]);
		auto value = repl_eval(args->m_v[0]);
		m_env = old_env;
		return value;
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::sym(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		if (args->m_v[0]->type() == lisp_type_symbol) return args->m_v[0];
		return intern(std::make_shared<Lisp_Symbol>(std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string));
	}
	return repl_error("(func ?)", error_msg_wrong_types, args);
}
