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

#ifndef LISP_H
#define LISP_H

#include <ctype.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <vector>
#include <numeric>
#include <string>

enum Lisp_Type
{
	lisp_type_obj = 1 << 0,
	lisp_type_seq = 1 << 1,
	lisp_type_string = 1 << 2,
	lisp_type_symbol = 1 << 3,
	lisp_type_number = 1 << 4,
	lisp_type_env = 1 << 5,
	lisp_type_func = 1 << 6,
	lisp_type_list = 1 << 7,
};

class Lisp;
class Lisp_Obj;
class Lisp_List;
class Lisp_Symbol;

typedef std::shared_ptr<Lisp_Obj> (Lisp::*lisp_func_ptr)(const std::shared_ptr<Lisp_List> &args);
typedef std::map<std::shared_ptr<Lisp_Symbol>, std::shared_ptr<Lisp_Obj>> Lisp_Env_Map;

const int type_mask_obj = lisp_type_obj;
class Lisp_Obj
{
public:
	Lisp_Obj();
	virtual ~Lisp_Obj();
	virtual const Lisp_Type type() const { return lisp_type_obj; }
	virtual Lisp_Type is_type(Lisp_Type t) const;
	virtual void print();
};

const int type_mask_number = type_mask_obj | lisp_type_number;
class Lisp_Number : public Lisp_Obj
{
public:
	Lisp_Number(long long num = 0);
	const Lisp_Type type() const override { return lisp_type_number; }
	Lisp_Type is_type(Lisp_Type t) const override;
	void print() override;
	long long m_value;
};

const int type_mask_seq = type_mask_obj | lisp_type_seq;
class Lisp_Seq : public Lisp_Obj
{
public:
	Lisp_Seq()
		: Lisp_Obj()
	{}
	virtual int length() = 0;
	virtual std::shared_ptr<Lisp_Obj> elem(int i) = 0;
	virtual std::shared_ptr<Lisp_Obj> slice(int s, int e) = 0;
	virtual std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args) = 0;
};

const int type_mask_list = type_mask_seq | lisp_type_list;
class Lisp_List : public Lisp_Seq
{
public:
	Lisp_List();
	const Lisp_Type type() const override { return lisp_type_list; }
	Lisp_Type is_type(Lisp_Type t) const override;
	void print() override;
	int length() override;
	std::shared_ptr<Lisp_Obj> elem(int i) override;
	std::shared_ptr<Lisp_Obj> slice(int s, int e) override;
	std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args) override;
	std::vector<std::shared_ptr<Lisp_Obj>> m_v;
};

const int type_mask_string = type_mask_seq | lisp_type_string;
class Lisp_String : public Lisp_Seq
{
public:
	Lisp_String();
	Lisp_String(const std::string &s);
	Lisp_String(char c);
	Lisp_String(const char *s, int len);
	const Lisp_Type type() const override { return lisp_type_string; }
	Lisp_Type is_type(Lisp_Type t) const override;
	void print() override;
	int length() override;
	std::shared_ptr<Lisp_Obj> elem(int i) override;
	std::shared_ptr<Lisp_Obj> slice(int s, int e) override;
	std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args) override;
	int cmp(const std::shared_ptr<Lisp_String> &str1, const std::shared_ptr<Lisp_String> &str2);
	std::string m_string;
};

const int type_mask_symbol = type_mask_string | lisp_type_symbol;
class Lisp_Symbol : public Lisp_String
{
public:
	Lisp_Symbol();
	Lisp_Symbol(const std::string &s);
	Lisp_Symbol(char c);
	Lisp_Symbol(const char *s, int len);
	const Lisp_Type type() const override { return lisp_type_symbol; }
	Lisp_Type is_type(Lisp_Type t) const override;
};

const int type_mask_env = type_mask_obj | lisp_type_env;
class Lisp_Env : public Lisp_Obj
{
public:
	Lisp_Env();
	const Lisp_Type type() const override { return lisp_type_env; }
	Lisp_Type is_type(Lisp_Type t) const override;
	void print() override;
	void set_parent(const std::shared_ptr<Lisp_Env> &env);
	std::shared_ptr<Lisp_Env> get_parent() const;
	Lisp_Env_Map::iterator find(const std::shared_ptr<Lisp_Symbol> &sym);
	Lisp_Env_Map::iterator set(const std::shared_ptr<Lisp_Symbol> &sym, const std::shared_ptr<Lisp_Obj> &obj);
	std::shared_ptr<Lisp_Obj> get(const std::shared_ptr<Lisp_Symbol> &sym);
	Lisp_Env_Map m_map;
	std::shared_ptr<Lisp_Env> m_parent;
};

const int type_mask_func = type_mask_obj | lisp_type_func;
class Lisp_Func : public Lisp_Obj
{
public:
	Lisp_Func(lisp_func_ptr func, int t = 0);
	const Lisp_Type type() const override { return lisp_type_func; }
	Lisp_Type is_type(Lisp_Type t) const override;
	void print() override;
	lisp_func_ptr m_func;
	int m_ftype;
};

//lisp class
class Lisp
{
public:
	Lisp();

	std::shared_ptr<Lisp_Symbol> intern(const std::shared_ptr<Lisp_Symbol> &obj);
	void env_push();
	void env_pop();
	std::shared_ptr<Lisp_Obj> env_bind(const std::shared_ptr<Lisp_Obj> &lst, const std::shared_ptr<Lisp_Obj> &seq);

	int read_whitespace(std::istream &in);
	std::shared_ptr<Lisp_Obj> read_string(std::istream &in, char term);
	std::shared_ptr<Lisp_Obj> read_symbol(std::istream &in);
	std::shared_ptr<Lisp_Obj> read_number(std::istream &in);
	std::shared_ptr<Lisp_Obj> read_list(std::istream &in);
	std::shared_ptr<Lisp_Obj> read_rmacro(std::istream &in, const std::shared_ptr<Lisp_Symbol> &sym);
	std::shared_ptr<Lisp_Obj> read(std::istream &in);
	std::shared_ptr<Lisp_Obj> repl(std::istream &in);
	std::shared_ptr<Lisp_Obj> repl_apply(const std::shared_ptr<Lisp_Obj> &func, const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> repl_eval(const std::shared_ptr<Lisp_Obj> &obj);
	int repl_expand(std::shared_ptr<Lisp_Obj> &obj, int cnt);

	std::shared_ptr<Lisp_Obj> prin(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> print(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> add(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> sub(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> mul(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> div(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> mod(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> max(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> min(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> eq(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> ne(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lt(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> gt(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> le(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> ge(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> eql(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> band(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> bor(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> bxor(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> bshl(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> bshr(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> basr(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> list(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> push(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> pop(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> length(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> elem(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> slice(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> clear(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> copy(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> find(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> merge(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> split(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> match(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> cmp(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> code(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lchar(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> quote(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> qquote(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> progn(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> apply(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> cond(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lwhile(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> eval(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lcatch(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> env(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> defq(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> setq(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> def(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> set(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> defined(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> sym(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> gensym(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> defmacro(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> bind(const std::shared_ptr<Lisp_List> &args);

private:
	std::map<std::string, std::shared_ptr<Lisp_Symbol>> m_intern_sym_map;
	std::shared_ptr<Lisp_Env> m_env;
	std::shared_ptr<Lisp_Symbol> m_sym_nil;
	std::shared_ptr<Lisp_Symbol> m_sym_t;
	std::shared_ptr<Lisp_Symbol> m_sym_quote;
	std::shared_ptr<Lisp_Symbol> m_sym_macro;
	std::shared_ptr<Lisp_Symbol> m_sym_lambda;
	std::shared_ptr<Lisp_Symbol> m_sym_cat;
	std::shared_ptr<Lisp_Symbol> m_sym_list;
	std::shared_ptr<Lisp_Symbol> m_sym_rest;
	std::shared_ptr<Lisp_Symbol> m_sym_optional;
	std::shared_ptr<Lisp_Symbol> m_sym_unquote;
	std::shared_ptr<Lisp_Symbol> m_sym_qquote;
	std::shared_ptr<Lisp_Symbol> m_sym_splicing;
	unsigned long m_next_sym = 0;
	friend void qquote1(Lisp *lisp, const std::shared_ptr<Lisp_Obj> &o, std::shared_ptr<Lisp_List> &cat_list);
};

#endif
