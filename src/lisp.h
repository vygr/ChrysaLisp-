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
#include <set>
#include <memory>
#include <sstream>
#include <vector>
#include <numeric>
#include <string>
#include <chrono>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN64
#include <algorithm>
#include <cctype>
#endif

enum Lisp_Type
{
	lisp_type_list = 1 << 0,
	lisp_type_integer = 1 << 1,
	lisp_type_string = 1 << 2,
	lisp_type_symbol = 1 << 3,
	lisp_type_function = 1 << 4,

	lisp_type_env = 1 << 5,
	lisp_type_file_stream = 1 << 6,
	lisp_type_string_stream = 1 << 7,
	lisp_type_sys_stream = 1 << 8,
	lisp_type_error = 1 << 9,

	lisp_type_seq = 1 << 10,
	lisp_type_istream = 1 << 11,
	lisp_type_ostream = 1 << 12,
};

const int type_mask_obj = 0;
const int type_mask_error = type_mask_obj | lisp_type_error;
const int type_mask_integer = type_mask_obj | lisp_type_integer;
const int type_mask_seq = type_mask_obj | lisp_type_seq;
const int type_mask_env = type_mask_obj | lisp_type_env;
const int type_mask_function = type_mask_obj | lisp_type_function;
const int type_mask_istream = type_mask_obj | lisp_type_istream;
const int type_mask_ostream = type_mask_obj | lisp_type_ostream;
const int type_mask_list = type_mask_seq | lisp_type_list;
const int type_mask_string = type_mask_seq | lisp_type_string;
const int type_mask_symbol = type_mask_string | lisp_type_symbol;
const int type_mask_sys_stream = type_mask_istream | lisp_type_sys_stream;
const int type_mask_file_stream = type_mask_istream | lisp_type_file_stream;
const int type_mask_string_stream = type_mask_ostream | lisp_type_string_stream;

enum Lisp_Error_Num
{
	error_msg,
	error_msg_not_a_canvas,
	error_msg_not_a_class,
	error_msg_not_a_filename,
	error_msg_not_a_lambda,
	error_msg_not_a_list,
	error_msg_not_a_number,
	error_msg_not_a_pipe,
	error_msg_not_a_sequence,
	error_msg_not_a_stream,
	error_msg_not_a_string,
	error_msg_not_a_symbol,
	error_msg_not_all_lists,
	error_msg_not_all_nums,
	error_msg_not_all_strings,
	error_msg_not_an_environment,
	error_msg_not_valid_index,
	error_msg_open_error,
	error_msg_symbol_not_bound,
	error_msg_wrong_num_of_args,
	error_msg_wrong_types
};

class Lisp;
class Lisp_Obj;
class Lisp_List;
class Lisp_Symbol;

typedef std::shared_ptr<Lisp_Obj> (Lisp::*lisp_func_ptr)(const std::shared_ptr<Lisp_List> &args);

class Lisp_Obj
{
public:
	Lisp_Obj() {};
	virtual ~Lisp_Obj() {};
	virtual const Lisp_Type type() const = 0;
	virtual Lisp_Type is_type(Lisp_Type t) const = 0;
	virtual void print(std::ostream &out) const = 0;
};

class Lisp_Error : public Lisp_Obj
{
public:
	Lisp_Error(const std::string &msg, const std::string &file, int line_num, const std::shared_ptr<Lisp_Obj> &o);
	const Lisp_Type type() const override { return lisp_type_error; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_error); }
	void print(std::ostream &out) const override;
	std::string m_msg;
	std::string m_file;
	long long m_line_num;
	std::shared_ptr<Lisp_Obj> m_obj;
};

class Lisp_Integer : public Lisp_Obj
{
public:
	Lisp_Integer(long long num = 0);
	const Lisp_Type type() const override { return lisp_type_integer; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_integer); }
	void print(std::ostream &out) const override;
	long long m_value;
};

class Lisp_Seq : public Lisp_Obj
{
public:
	Lisp_Seq()
		: Lisp_Obj()
	{}
	virtual long long length() const = 0;
	virtual std::shared_ptr<Lisp_Obj> elem(long long i) const = 0;
	virtual std::shared_ptr<Lisp_Obj> slice(long long s, long long e) const = 0;
	virtual std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args) const = 0;
};

class Lisp_List : public Lisp_Seq
{
public:
	Lisp_List();
	const Lisp_Type type() const override { return lisp_type_list; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_list); }
	void print(std::ostream &out) const override;
	long long length() const override;
	std::shared_ptr<Lisp_Obj> elem(long long i) const override;
	std::shared_ptr<Lisp_Obj> slice(long long s, long long e) const override;
	std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args) const override;
	std::vector<std::shared_ptr<Lisp_Obj>> m_v;
};

class Lisp_String : public Lisp_Seq
{
public:
	Lisp_String();
	Lisp_String(const std::string &s);
	Lisp_String(char c);
	Lisp_String(const char *s, int len);
	const Lisp_Type type() const override { return lisp_type_string; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_string); }
	void print(std::ostream &out) const override;
	void print1(std::ostream &out) const;
	long long length() const override;
	std::shared_ptr<Lisp_Obj> elem(long long i) const override;
	std::shared_ptr<Lisp_Obj> slice(long long s, long long e) const override;
	std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args) const override;
	long long cmp(const std::shared_ptr<Lisp_String> &str1, const std::shared_ptr<Lisp_String> &str2) const;
	unsigned int hash();
	std::string m_string;
	unsigned int m_hash = 0;
};

class Lisp_Symbol : public Lisp_String
{
public:
	Lisp_Symbol();
	Lisp_Symbol(const std::string &s);
	Lisp_Symbol(char c);
	Lisp_Symbol(const char *s, int len);
	const Lisp_Type type() const override { return lisp_type_symbol; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_symbol); }
	void print(std::ostream &out) const override;
};

class Lisp_Function : public Lisp_Obj
{
public:
	Lisp_Function(lisp_func_ptr func, int t = 0);
	const Lisp_Type type() const override { return lisp_type_function; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_function); }
	void print(std::ostream &out) const override;
	lisp_func_ptr m_func;
	int m_ftype;
};

class Lisp_IStream : public Lisp_Obj
{
public:
	Lisp_IStream()
		: Lisp_Obj()
	{}
	virtual bool is_open() const = 0;
	virtual std::istream &get_stream() = 0;
	virtual int read_char() = 0;
	virtual std::string read_line(bool &state) = 0;
};

class Lisp_OStream : public Lisp_Obj
{
public:
	Lisp_OStream()
		: Lisp_Obj()
	{}
	virtual bool is_open() const = 0;
	virtual std::ostream &get_stream() = 0;
	virtual void write_char(int c) = 0;
	virtual void write_line(const std::string &s) = 0;
};

class Lisp_Sys_Stream : public Lisp_IStream
{
public:
	Lisp_Sys_Stream(std::istream &in);
	const Lisp_Type type() const override { return lisp_type_sys_stream; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_sys_stream); }
	void print(std::ostream &out) const override;
	bool is_open() const override;
	std::istream &get_stream() override;
	int read_char() override;
	std::string read_line(bool &state) override;
	std::istream &m_stream;
};

class Lisp_File_Stream : public Lisp_IStream
{
public:
	Lisp_File_Stream(const std::string &path);
	const Lisp_Type type() const override { return lisp_type_file_stream; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_file_stream); }
	void print(std::ostream &out) const override;
	bool is_open() const override;
	std::istream &get_stream() override;
	int read_char() override;
	std::string read_line(bool &state) override;
	std::ifstream m_stream;
};

class Lisp_String_Stream : public Lisp_OStream
{
public:
	Lisp_String_Stream(const std::string &s);
	const Lisp_Type type() const override { return lisp_type_string_stream; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_string_stream); }
	void print(std::ostream &out) const override;
	void print1(std::ostream &out) const;
	bool is_open() const override;
	std::ostream &get_stream() override;
	void write_char(int c) override;
	void write_line(const std::string &s) override;
	std::ostringstream m_stream;
};

typedef std::pair<std::shared_ptr<Lisp_Symbol>, std::shared_ptr<Lisp_Obj>> Lisp_Env_Pair;
typedef std::vector<Lisp_Env_Pair> Lisp_Env_Bucket;
typedef std::vector<Lisp_Env_Bucket> Lisp_Env_Buckets;
class Lisp_Env : public Lisp_Obj
{
public:
	Lisp_Env(long long num_buckets = 1);
	const Lisp_Type type() const override { return lisp_type_env; }
	Lisp_Type is_type(Lisp_Type t) const override { return (Lisp_Type)(t & type_mask_env); }
	void print(std::ostream &out) const override;
	void set_parent(const std::shared_ptr<Lisp_Env> &env);
	std::shared_ptr<Lisp_Env> get_parent() const;
	Lisp_Env_Pair *find(const std::shared_ptr<Lisp_Symbol> &sym);
	Lisp_Env_Pair *set(const std::shared_ptr<Lisp_Symbol> &sym, const std::shared_ptr<Lisp_Obj> &obj);
	std::shared_ptr<Lisp_Obj> get(const std::shared_ptr<Lisp_Symbol> &sym);
	void insert(const std::shared_ptr<Lisp_Symbol> &sym, const std::shared_ptr<Lisp_Obj> &obj);
	void erase(const std::shared_ptr<Lisp_Symbol> &sym);
	void resize(long long num_buckets);
	Lisp_Env_Buckets::iterator get_bucket(const std::shared_ptr<Lisp_Symbol> &sym);
	Lisp_Env_Buckets m_buckets;
	std::shared_ptr<Lisp_Env> m_parent;
};

struct Intern_Cmp
{
    bool operator() (const std::shared_ptr<Lisp_Symbol> &lhs, const std::shared_ptr<Lisp_Symbol> &rhs) const
	{
        return lhs->m_string < rhs->m_string;
    }
};

//lisp class
class Lisp
{
public:
	Lisp();

	void env_push();
	void env_pop();
	std::shared_ptr<Lisp_Symbol> intern(const std::shared_ptr<Lisp_Symbol> &obj);
	std::shared_ptr<Lisp_Obj> env_bind(const std::shared_ptr<Lisp_Obj> &lst, const std::shared_ptr<Lisp_Obj> &seq);

	int repl_read_char(std::istream &in) const;
	int repl_read_whitespace(std::istream &in) const;
	int repl_expand(std::shared_ptr<Lisp_Obj> &obj, int cnt);
	std::shared_ptr<Lisp_Obj> repl_read_string(std::istream &in, char term) const;
	std::shared_ptr<Lisp_Obj> repl_read_symbol(std::istream &in);
	std::shared_ptr<Lisp_Obj> repl_read_number(std::istream &in);
	std::shared_ptr<Lisp_Obj> repl_read_list(std::istream &in);
	std::shared_ptr<Lisp_Obj> repl_read_rmacro(std::istream &in, const std::shared_ptr<Lisp_Symbol> &sym);
	std::shared_ptr<Lisp_Obj> repl_read(std::istream &in);
	std::shared_ptr<Lisp_Obj> repl_apply(const std::shared_ptr<Lisp_Obj> &func, const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> repl_eval(const std::shared_ptr<Lisp_Obj> &obj);
	std::shared_ptr<Lisp_Obj> repl_error(const std::string &msg, int type, const std::shared_ptr<Lisp_Obj> &o);

	std::shared_ptr<Lisp_Obj> add(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> sub(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> mul(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> div(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> mod(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> fmul(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> fdiv(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> fmod(const std::shared_ptr<Lisp_List> &args);
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
	std::shared_ptr<Lisp_Obj> elemset(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> slice(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> cat(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> clear(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> copy(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> find(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> merge(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> split(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> match(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> some(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> each(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> part(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> cmp(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> code(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lchar(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> str(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> filestream(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> strstream(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> read(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> readchar(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> readline(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> write(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> writechar(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> prin(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> print(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> repl(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> save(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> load(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> time(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> age(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> quote(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> qquote(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> progn(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> apply(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> cond(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lwhile(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> eval(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lcatch(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> type(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lthrow(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> macroexpand(const std::shared_ptr<Lisp_List> &args);

	std::shared_ptr<Lisp_Obj> env(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> defq(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> setq(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> def(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> undef(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> set(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> defined(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> sym(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> gensym(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> defmacro(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> lambda(const std::shared_ptr<Lisp_List> &args);
	std::shared_ptr<Lisp_Obj> bind(const std::shared_ptr<Lisp_List> &args);

	std::set<std::shared_ptr<Lisp_Symbol>, Intern_Cmp> m_intern_sym_set;
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
	std::shared_ptr<Lisp_Symbol> m_sym_underscore;
	std::shared_ptr<Lisp_Symbol> m_sym_stream_name;
	std::shared_ptr<Lisp_Symbol> m_sym_stream_line;
	unsigned long m_next_sym = 0;
	friend void qquote1(Lisp *lisp, const std::shared_ptr<Lisp_Obj> &o, std::shared_ptr<Lisp_List> &cat_list);
};

#endif
