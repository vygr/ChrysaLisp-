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

//////////
//Lisp_Obj
//////////

Lisp_Obj::Lisp_Obj()
{}

Lisp_Obj::~Lisp_Obj()
{}

void Lisp_Obj::print()
{
	std::cout << "error";
}

Lisp_Type Lisp_Obj::is_type(Lisp_Type t) const
{
	return (Lisp_Type)(t & type_mask_obj);
}

/////////////
//Lisp_Number
/////////////

Lisp_Number::Lisp_Number(long long num)
	: Lisp_Obj()
	, m_value(num)
{}

void Lisp_Number::print()
{
	std::cout << m_value;
}

Lisp_Type Lisp_Number::is_type(Lisp_Type t) const
{
	return (Lisp_Type)(t & type_mask_number);
}

///////////
//Lisp_List
///////////

Lisp_List::Lisp_List()
	: Lisp_Seq()
{}

Lisp_Type Lisp_List::is_type(Lisp_Type t) const
{
	return (Lisp_Type)(t & type_mask_list);
}

void Lisp_List::print()
{
	std::cout << '(';
	for (auto itr = begin(m_v); itr != end(m_v); ++itr)
	{
		(*itr)->print();
		if (itr != end(m_v) - 1) std::cout << ' ';
	}
	std::cout << ')';
}

int Lisp_List::length()
{
	return m_v.size();
}

std::shared_ptr<Lisp_Obj> Lisp_List::elem(int i)
{
	return m_v[i];
}

std::shared_ptr<Lisp_Obj> Lisp_List::slice(int s, int e)
{
	auto slc = std::make_shared<Lisp_List>();
	for (auto itr = begin(m_v) + s; itr != begin(m_v) + e; ++itr) slc->m_v.push_back(*itr);
	return slc;
}

std::shared_ptr<Lisp_Obj> Lisp_List::cat(const std::shared_ptr<Lisp_List> &args)
{
	auto c = std::make_shared<Lisp_List>();
	c->m_v.reserve(std::accumulate(begin(args->m_v), end(args->m_v), 0,
		[] (auto n, auto &o) { return n + std::static_pointer_cast<Lisp_List>(o)->length(); }));
	for (auto &o : args->m_v)
	{
		auto lst = std::static_pointer_cast<Lisp_List>(o);
		for (auto &o : lst->m_v) c->m_v.push_back(o);
	}
	return c;
}

/////////////
//Lisp_Symbol
/////////////

Lisp_String::Lisp_String()
	: Lisp_Seq()
{}

Lisp_String::Lisp_String(const std::string &s)
	: Lisp_Seq()
	, m_string(s)
{}

Lisp_String::Lisp_String(char c)
	: Lisp_Seq()
	, m_string(std::string{c})
{}

Lisp_String::Lisp_String(const char *s, int len)
	: Lisp_Seq()
{
	m_string.reserve(len);
	for (auto i = 0; i < len; ++i) m_string.push_back(*(s + i));
}

Lisp_Type Lisp_String::is_type(Lisp_Type t) const
{
	return (Lisp_Type)(t & type_mask_string);
}

void Lisp_String::print()
{
	std::cout << m_string;
}

int Lisp_String::length()
{
	return m_string.size();
}

std::shared_ptr<Lisp_Obj> Lisp_String::elem(int i)
{
	return std::make_shared<Lisp_String>(m_string[i]);
}

std::shared_ptr<Lisp_Obj> Lisp_String::slice(int s, int e)
{
	return std::make_shared<Lisp_String>(std::string{begin(m_string) + s, begin(m_string) + e});
}

std::shared_ptr<Lisp_Obj> Lisp_String::cat(const std::shared_ptr<Lisp_List> &args)
{
	auto c = std::make_shared<Lisp_String>();
	for (auto &o : args->m_v)
	{
		auto s = std::static_pointer_cast<Lisp_String>(o);
		c->m_string += s->m_string;
	}
	return c;
}

int Lisp_String::cmp(const std::shared_ptr<Lisp_String> &str1, const std::shared_ptr<Lisp_String> &str2)
{
	auto c = 0;
	if (str1 != str2)
	{
		auto l1 = str1->length();
		auto l2 = str2->length();
		if (l1 != 0 && l2 != 0)
		{
			auto itr1 = (const unsigned char*)str1->m_string.data();
			auto itr2 = (const unsigned char*)str2->m_string.data();
			for (auto i = 0u;i != l1 && i != l2; ++i)
			{
				c = itr1[i] - itr2[i];
				if (c) return c;
			}
		}
		c = l1 - l2;
	}
	return c;
}

/////////////
//Lisp_Symbol
/////////////

Lisp_Symbol::Lisp_Symbol()
	: Lisp_String()
{}

Lisp_Symbol::Lisp_Symbol(const std::string &s)
	: Lisp_String(s)
{}

Lisp_Symbol::Lisp_Symbol(char c)
	: Lisp_String(c)
{}

Lisp_Symbol::Lisp_Symbol(const char *s, int len)
	: Lisp_String(s, len)
{}

Lisp_Type Lisp_Symbol::is_type(Lisp_Type t) const
{
	return (Lisp_Type)(t & type_mask_symbol);
}

//////////
//Lisp_Env
//////////

Lisp_Env::Lisp_Env()
	: Lisp_Obj()
{}

Lisp_Type Lisp_Env::is_type(Lisp_Type t) const
{
	return (Lisp_Type)(t & type_mask_env);
}

void Lisp_Env::print()
{
	std::cout << '{';
	for (auto itr = begin(m_map); itr != end(m_map); ++itr)
	{
		std::cout << "[";
		itr->first->print();
		std::cout << " : ";
		itr->second->print();
		std::cout << "]";
	}
	std::cout << '}';
}

void Lisp_Env::set_parent(std::shared_ptr<Lisp_Env> &env)
{
	m_parent = env;
}

std::shared_ptr<Lisp_Env> Lisp_Env::get_parent()
{
	return m_parent;
}

Lisp_Env_Map::iterator Lisp_Env::find(std::shared_ptr<Lisp_Symbol> &sym)
{
	auto env = this;
	for (;;)
	{
		auto itr = env->m_map.find(sym);
		if (itr != env->m_map.end()) return itr;
		env = env->get_parent().get();
		if (env == nullptr) return end(m_map);
	}
}

Lisp_Env_Map::iterator Lisp_Env::set(std::shared_ptr<Lisp_Symbol> &sym, std::shared_ptr<Lisp_Obj> &obj)
{
	auto itr = find(sym);
	if (itr != end(m_map)) itr->second = obj;
	return itr;
}

std::shared_ptr<Lisp_Obj> Lisp_Env::get(std::shared_ptr<Lisp_Symbol> &sym)
{
	auto itr = find(sym);
	if (itr != end(m_map)) return itr->second;
	return nullptr;
}

///////////
//Lisp_Func
///////////

Lisp_Func::Lisp_Func(lisp_func_ptr func, int t)
	: Lisp_Obj()
	, m_func(func)
	, m_ftype(t)
{}

Lisp_Type Lisp_Func::is_type(Lisp_Type t) const
{
	return (Lisp_Type)(t & type_mask_func);
}

void Lisp_Func::print()
{
	std::cout << "<function>";
}

//////
//Lisp
//////

Lisp::Lisp()
{
	//prebound symbols
	env_push();
	m_sym_rest = intern(std::make_shared<Lisp_Symbol>("&rest"));
	m_sym_optional = intern(std::make_shared<Lisp_Symbol>("&optional"));
	m_sym_macro = intern(std::make_shared<Lisp_Symbol>("macro"));
	m_sym_lambda = intern(std::make_shared<Lisp_Symbol>("lambda"));
	m_sym_cat = intern(std::make_shared<Lisp_Symbol>("cat"));
	m_sym_list = intern(std::make_shared<Lisp_Symbol>("list"));
	m_sym_quote = intern(std::make_shared<Lisp_Symbol>("quote"));
	m_sym_qquote = intern(std::make_shared<Lisp_Symbol>("quasi-quote"));
	m_sym_unquote = intern(std::make_shared<Lisp_Symbol>("unquote"));
	m_sym_splicing = intern(std::make_shared<Lisp_Symbol>("unquote-splicing"));
	m_sym_nil = intern(std::make_shared<Lisp_Symbol>("nil"));
	m_sym_t = intern(std::make_shared<Lisp_Symbol>("t"));
	m_env->m_map[m_sym_nil] = m_sym_nil;
	m_env->m_map[m_sym_t] = m_sym_t;

	//prebound functions
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("add"))] = std::make_shared<Lisp_Func>(&Lisp::add);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("sub"))] = std::make_shared<Lisp_Func>(&Lisp::sub);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("mul"))] = std::make_shared<Lisp_Func>(&Lisp::mul);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("div"))] = std::make_shared<Lisp_Func>(&Lisp::div);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("mod"))] = std::make_shared<Lisp_Func>(&Lisp::mod);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("max"))] = std::make_shared<Lisp_Func>(&Lisp::max);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("min"))] = std::make_shared<Lisp_Func>(&Lisp::min);

	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("eq"))] = std::make_shared<Lisp_Func>(&Lisp::eq);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("ne"))] = std::make_shared<Lisp_Func>(&Lisp::ne);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("lt"))] = std::make_shared<Lisp_Func>(&Lisp::lt);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("gt"))] = std::make_shared<Lisp_Func>(&Lisp::gt);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("le"))] = std::make_shared<Lisp_Func>(&Lisp::le);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("ge"))] = std::make_shared<Lisp_Func>(&Lisp::ge);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("eql"))] = std::make_shared<Lisp_Func>(&Lisp::eql);

	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("bit-and"))] = std::make_shared<Lisp_Func>(&Lisp::band);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("bit-or"))] = std::make_shared<Lisp_Func>(&Lisp::bor);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("bit-xor"))] = std::make_shared<Lisp_Func>(&Lisp::bxor);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("bit-shl"))] = std::make_shared<Lisp_Func>(&Lisp::bshl);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("bit-shr"))] = std::make_shared<Lisp_Func>(&Lisp::bshr);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("bit-asr"))] = std::make_shared<Lisp_Func>(&Lisp::basr);

	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("list"))] = std::make_shared<Lisp_Func>(&Lisp::list);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("push"))] = std::make_shared<Lisp_Func>(&Lisp::push);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("pop"))] = std::make_shared<Lisp_Func>(&Lisp::pop);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("length"))] = std::make_shared<Lisp_Func>(&Lisp::length);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("elem"))] = std::make_shared<Lisp_Func>(&Lisp::elem);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("slice"))] = std::make_shared<Lisp_Func>(&Lisp::slice);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("cat"))] = std::make_shared<Lisp_Func>(&Lisp::cat);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("clear"))] = std::make_shared<Lisp_Func>(&Lisp::clear);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("copy"))] = std::make_shared<Lisp_Func>(&Lisp::copy);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("find"))] = std::make_shared<Lisp_Func>(&Lisp::find);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("merge-sym"))] = std::make_shared<Lisp_Func>(&Lisp::merge);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("split"))] = std::make_shared<Lisp_Func>(&Lisp::split);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("match?"))] = std::make_shared<Lisp_Func>(&Lisp::match);

	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("cmp"))] = std::make_shared<Lisp_Func>(&Lisp::cmp);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("code"))] = std::make_shared<Lisp_Func>(&Lisp::code);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("char"))] = std::make_shared<Lisp_Func>(&Lisp::lchar);

	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("catch"))] = std::make_shared<Lisp_Func>(&Lisp::lcatch, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("lambda"))] = std::make_shared<Lisp_Func>(&Lisp::list, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("macro"))] = std::make_shared<Lisp_Func>(&Lisp::list, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("quote"))] = std::make_shared<Lisp_Func>(&Lisp::quote, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("quasi-quote"))] = std::make_shared<Lisp_Func>(&Lisp::qquote, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("cond"))] = std::make_shared<Lisp_Func>(&Lisp::cond, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("while"))] = std::make_shared<Lisp_Func>(&Lisp::lwhile, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("progn"))] = std::make_shared<Lisp_Func>(&Lisp::progn);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("apply"))] = std::make_shared<Lisp_Func>(&Lisp::apply);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("eval"))] = std::make_shared<Lisp_Func>(&Lisp::eval);

	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("prin"))] = std::make_shared<Lisp_Func>(&Lisp::prin);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("print"))] = std::make_shared<Lisp_Func>(&Lisp::print);

	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("defmacro"))] = std::make_shared<Lisp_Func>(&Lisp::defmacro, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("env"))] = std::make_shared<Lisp_Func>(&Lisp::env, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("defq"))] = std::make_shared<Lisp_Func>(&Lisp::defq, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("setq"))] = std::make_shared<Lisp_Func>(&Lisp::setq, 1);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("def"))] = std::make_shared<Lisp_Func>(&Lisp::def);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("set"))] = std::make_shared<Lisp_Func>(&Lisp::set);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("def?"))] = std::make_shared<Lisp_Func>(&Lisp::defined);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("sym"))] = std::make_shared<Lisp_Func>(&Lisp::sym);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("gensym"))] = std::make_shared<Lisp_Func>(&Lisp::gensym);
	m_env->m_map[intern(std::make_shared<Lisp_Symbol>("bind"))] = std::make_shared<Lisp_Func>(&Lisp::bind);
}