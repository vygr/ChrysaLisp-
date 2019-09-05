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

////////////
//Lisp_Error
////////////

Lisp_Error::Lisp_Error(const std::string &msg, const std::string &file, int line_num, const std::shared_ptr<Lisp_Obj> &o)
	: Lisp_Obj()
	, m_msg(msg)
	, m_file(file)
	, m_line_num(line_num)
	, m_obj(o)
{}

void Lisp_Error::print(std::ostream &out) const
{
	out << "Error: " << m_msg << " ! < ";
	m_obj->print(out);
	out << " > File: " << m_file << "(" << m_line_num << ")";
}

//////////////
//Lisp_Integer
//////////////

Lisp_Integer::Lisp_Integer(long long num)
	: Lisp_Obj()
	, m_value(num)
{}

void Lisp_Integer::print(std::ostream &out) const
{
	out << m_value;
}

///////////
//Lisp_List
///////////

Lisp_List::Lisp_List()
	: Lisp_Seq()
{}

void Lisp_List::print(std::ostream &out) const
{
	out << '(';
	for (auto itr = begin(m_v); itr != end(m_v); ++itr)
	{
		(*itr)->print(out);
		if (itr != end(m_v) - 1) out << ' ';
	}
	out << ')';
}

long long Lisp_List::length() const
{
	return m_v.size();
}

std::shared_ptr<Lisp_Obj> Lisp_List::elem(long long i) const
{
	return m_v[i];
}

std::shared_ptr<Lisp_Obj> Lisp_List::slice(long long s, long long e) const
{
	auto slc = std::make_shared<Lisp_List>();
	slc->m_v.reserve(e - s);
	for (auto itr = begin(m_v) + s; itr != begin(m_v) + e; ++itr) slc->m_v.push_back(*itr);
	return slc;
}

std::shared_ptr<Lisp_Obj> Lisp_List::cat(const std::shared_ptr<Lisp_List> &args) const
{
	auto c = std::make_shared<Lisp_List>();
	c->m_v.reserve(std::accumulate(begin(args->m_v), end(args->m_v), 0,
		[] (auto n, auto &o) { return n + std::static_pointer_cast<Lisp_List>(o)->length(); }));
	for (auto &o : args->m_v) for (auto &o : std::static_pointer_cast<Lisp_List>(o)->m_v) c->m_v.push_back(o);
	return c;
}

/////////////
//Lisp_String
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

void Lisp_String::print(std::ostream &out) const
{
	out << '"' << m_string << '"';
}

void Lisp_String::print1(std::ostream &out) const
{
	out << m_string;
}

long long Lisp_String::length() const
{
	return m_string.size();
}

std::shared_ptr<Lisp_Obj> Lisp_String::elem(long long i) const
{
	return std::make_shared<Lisp_String>(m_string[i]);
}

std::shared_ptr<Lisp_Obj> Lisp_String::slice(long long s, long long e) const
{
	return std::make_shared<Lisp_String>(std::string{begin(m_string) + s, begin(m_string) + e});
}

std::shared_ptr<Lisp_Obj> Lisp_String::cat(const std::shared_ptr<Lisp_List> &args) const
{
	auto c = std::make_shared<Lisp_String>();
	c->m_string.reserve(std::accumulate(begin(args->m_v), end(args->m_v), 0,
		[] (auto n, auto &o) { return n + std::static_pointer_cast<Lisp_String>(o)->length(); }));
	for (auto &o : args->m_v) c->m_string += std::static_pointer_cast<Lisp_String>(o)->m_string;
	return c;
}

long long Lisp_String::cmp(const std::shared_ptr<Lisp_String> &str1, const std::shared_ptr<Lisp_String> &str2) const
{
	auto c = 0ll;
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

unsigned int Lisp_String::hash()
{
	if (m_hash) return m_hash;
	return m_hash = (unsigned int)std::hash<std::string>{}(m_string);
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

void Lisp_Symbol::print(std::ostream &out) const
{
	out << m_string;
}

//////////
//Lisp_Env
//////////

Lisp_Env::Lisp_Env(long long num_buckets)
	: Lisp_Obj()
{
	m_buckets.resize(num_buckets);
}

void Lisp_Env::resize(long long num_buckets)
{
	if (num_buckets != m_buckets.size())
	{
		auto old_buckets = std::move(m_buckets);
		m_buckets.resize(num_buckets);
		for (auto itr = begin(old_buckets); itr != end(old_buckets); ++itr)
		{
			for (auto itr1 = begin(*itr); itr1 != end(*itr); ++itr1) insert(itr1->first, itr1->second);
		}
	}
}

void Lisp_Env::print(std::ostream &out) const
{
	out << '{';
	for (auto itr = begin(m_buckets); itr != end(m_buckets); ++itr)
	{
		for (auto itr1 = begin(*itr); itr1 != end(*itr); ++itr1)
		{
			out << "[";
			itr1->first->print(out);
			out << " : ";
			itr1->second->print(out);
			out << "]";
		}
	}
	out << '}';
}

void Lisp_Env::set_parent(const std::shared_ptr<Lisp_Env> &env)
{
	m_parent = env;
}

std::shared_ptr<Lisp_Env> Lisp_Env::get_parent() const
{
	return m_parent;
}

Lisp_Env_Buckets::iterator Lisp_Env::get_bucket(const std::shared_ptr<Lisp_Symbol> &sym)
{
	auto s = m_buckets.size();
	auto bucket = begin(m_buckets);
	if (s != 1) bucket += sym->hash() % s;
	return bucket;
}

Lisp_Env_Pair *Lisp_Env::find(const std::shared_ptr<Lisp_Symbol> &sym)
{
	auto env = this;
	for (;;)
	{
		auto bucket = env->get_bucket(sym);
		auto itr = std::find_if(begin(*bucket), end(*bucket), [&] (auto &e) { return e.first == sym; });
		if (itr != end(*bucket)) return &(*itr);
		env = env->get_parent().get();
		if (env == nullptr) return nullptr;
	}
}

Lisp_Env_Pair *Lisp_Env::set(const std::shared_ptr<Lisp_Symbol> &sym, const std::shared_ptr<Lisp_Obj> &obj)
{
	auto itr = find(sym);
	if (itr != nullptr) itr->second = obj;
	return itr;
}

std::shared_ptr<Lisp_Obj> Lisp_Env::get(const std::shared_ptr<Lisp_Symbol> &sym)
{
	auto itr = find(sym);
	if (itr != nullptr) return itr->second;
	return nullptr;
}

void Lisp_Env::insert(const std::shared_ptr<Lisp_Symbol> &sym, const std::shared_ptr<Lisp_Obj> &obj)
{
	auto bucket = get_bucket(sym);
	auto itr = std::find_if(begin(*bucket), end(*bucket), [&] (auto &e) { return e.first == sym; });
	if (itr == end(*bucket)) (*bucket).emplace_back(sym, obj);
	else itr->second = obj;
}

void Lisp_Env::erase(const std::shared_ptr<Lisp_Symbol> &sym)
{
	auto bucket = get_bucket(sym);
	bucket->erase(std::remove_if(begin(*bucket), end(*bucket),
		[&] (auto &e) { return e.first == sym; }), end(*bucket));
}

///////////////
//Lisp_Function
///////////////

Lisp_Function::Lisp_Function(lisp_func_ptr func, int t)
	: Lisp_Obj()
	, m_func(func)
	, m_ftype(t)
{}

void Lisp_Function::print(std::ostream &out) const
{
	out << "<function>";
}

/////////////////
//Lisp_Sys_Stream
/////////////////

Lisp_Sys_Stream::Lisp_Sys_Stream(std::istream &in)
	: Lisp_IStream()
	, m_stream(in)
{
	m_stream >> std::noskipws;
}

void Lisp_Sys_Stream::print(std::ostream &out) const
{
	out << "<stdin stream>";
}

bool Lisp_Sys_Stream::is_open() const
{
	return true;
}

std::istream &Lisp_Sys_Stream::get_stream()
{
	return m_stream;
}

int Lisp_Sys_Stream::read_char()
{
	return m_stream.get();
}

std::string Lisp_Sys_Stream::read_line(bool &state)
{
	std::string line;
	state = true;
	if (std::getline(m_stream, line, '\n')) return line;
	state = false;
	return line;
}

//////////////////
//Lisp_File_Stream
//////////////////

Lisp_File_Stream::Lisp_File_Stream(const std::string &path)
	: Lisp_IStream()
{
	m_stream.open(path, std::ifstream::in);
	m_stream >> std::noskipws;
}

void Lisp_File_Stream::print(std::ostream &out) const
{
	out << "<file stream>";
}

bool Lisp_File_Stream::is_open() const
{
	return m_stream.is_open();
}

std::istream &Lisp_File_Stream::get_stream()
{
	return m_stream;
}

int Lisp_File_Stream::read_char()
{
	return m_stream.get();
}

std::string Lisp_File_Stream::read_line(bool &state)
{
	std::string line;
	state = true;
	if (std::getline(m_stream, line, '\n')) return line;
	state = false;
	return line;
}

////////////////////
//Lisp_String_Stream
////////////////////

Lisp_String_Stream::Lisp_String_Stream(const std::string &s)
	: Lisp_OStream()
{
	m_stream.str(s);
}

void Lisp_String_Stream::print(std::ostream &out) const
{
	out << "<string stream>";
}

void Lisp_String_Stream::print1(std::ostream &out) const
{
	out << m_stream.str();
}

bool Lisp_String_Stream::is_open() const
{
	return true;
}

std::ostream &Lisp_String_Stream::get_stream()
{
	return m_stream;
}

void Lisp_String_Stream::write_char(int c)
{
	m_stream.put(c);
}

void Lisp_String_Stream::write_line(const std::string &s)
{
	m_stream.write(&s[0], s.size());
}

//////
//Lisp
//////

Lisp::Lisp()
{
	//prebound symbols
	env_push();
	m_env->resize(101);
	m_sym_underscore = intern(std::make_shared<Lisp_Symbol>("_"));
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
	m_sym_stream_name = intern(std::make_shared<Lisp_Symbol>("*stream-name*"));
	m_sym_stream_line = intern(std::make_shared<Lisp_Symbol>("*stream-line*"));
	m_env->insert(m_sym_stream_name, std::make_shared<Lisp_String>("ChrysaLisp"));
	m_env->insert(m_sym_stream_line, std::make_shared<Lisp_Integer>(0));

	//prebound functions
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("+")), std::make_shared<Lisp_Function>(&Lisp::add));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("-")), std::make_shared<Lisp_Function>(&Lisp::sub));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("*")), std::make_shared<Lisp_Function>(&Lisp::mul));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("/")), std::make_shared<Lisp_Function>(&Lisp::div));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("%")), std::make_shared<Lisp_Function>(&Lisp::mod));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("fmul")), std::make_shared<Lisp_Function>(&Lisp::fmul));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("fdiv")), std::make_shared<Lisp_Function>(&Lisp::fdiv));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("max")), std::make_shared<Lisp_Function>(&Lisp::max));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("min")), std::make_shared<Lisp_Function>(&Lisp::min));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("=")), std::make_shared<Lisp_Function>(&Lisp::eq));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("/=")), std::make_shared<Lisp_Function>(&Lisp::ne));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("<")), std::make_shared<Lisp_Function>(&Lisp::lt));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>(">")), std::make_shared<Lisp_Function>(&Lisp::gt));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("<=")), std::make_shared<Lisp_Function>(&Lisp::le));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>(">=")), std::make_shared<Lisp_Function>(&Lisp::ge));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("eql")), std::make_shared<Lisp_Function>(&Lisp::eql));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("logand")), std::make_shared<Lisp_Function>(&Lisp::band));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("logior")), std::make_shared<Lisp_Function>(&Lisp::bor));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("logxor")), std::make_shared<Lisp_Function>(&Lisp::bxor));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("shl")), std::make_shared<Lisp_Function>(&Lisp::bshl));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("shr")), std::make_shared<Lisp_Function>(&Lisp::bshr));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("asr")), std::make_shared<Lisp_Function>(&Lisp::basr));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("list")), std::make_shared<Lisp_Function>(&Lisp::list));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("push")), std::make_shared<Lisp_Function>(&Lisp::push));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("pop")), std::make_shared<Lisp_Function>(&Lisp::pop));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("length")), std::make_shared<Lisp_Function>(&Lisp::length));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("elem")), std::make_shared<Lisp_Function>(&Lisp::elem));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("elem-set")), std::make_shared<Lisp_Function>(&Lisp::elemset));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("slice")), std::make_shared<Lisp_Function>(&Lisp::slice));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("cat")), std::make_shared<Lisp_Function>(&Lisp::cat));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("clear")), std::make_shared<Lisp_Function>(&Lisp::clear));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("copy")), std::make_shared<Lisp_Function>(&Lisp::copy));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("find")), std::make_shared<Lisp_Function>(&Lisp::find));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("merge")), std::make_shared<Lisp_Function>(&Lisp::merge));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("split")), std::make_shared<Lisp_Function>(&Lisp::split));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("match?")), std::make_shared<Lisp_Function>(&Lisp::match));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("some!")), std::make_shared<Lisp_Function>(&Lisp::some));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("each!")), std::make_shared<Lisp_Function>(&Lisp::each));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("partition")), std::make_shared<Lisp_Function>(&Lisp::part));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("cmp")), std::make_shared<Lisp_Function>(&Lisp::cmp));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("code")), std::make_shared<Lisp_Function>(&Lisp::code));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("char")), std::make_shared<Lisp_Function>(&Lisp::lchar));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("str")), std::make_shared<Lisp_Function>(&Lisp::str));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("file-stream")), std::make_shared<Lisp_Function>(&Lisp::filestream));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("string-stream")), std::make_shared<Lisp_Function>(&Lisp::strstream));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("read")), std::make_shared<Lisp_Function>(&Lisp::read));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("read-char")), std::make_shared<Lisp_Function>(&Lisp::readchar));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("read-line")), std::make_shared<Lisp_Function>(&Lisp::readline));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("write")), std::make_shared<Lisp_Function>(&Lisp::write));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("write-char")), std::make_shared<Lisp_Function>(&Lisp::writechar));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("prin")), std::make_shared<Lisp_Function>(&Lisp::prin));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("print")), std::make_shared<Lisp_Function>(&Lisp::print));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("load")), std::make_shared<Lisp_Function>(&Lisp::load));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("save")), std::make_shared<Lisp_Function>(&Lisp::save));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("time")), std::make_shared<Lisp_Function>(&Lisp::time));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("age")), std::make_shared<Lisp_Function>(&Lisp::age));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("ffi")), std::make_shared<Lisp_Function>(&Lisp::lambda, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("catch")), std::make_shared<Lisp_Function>(&Lisp::lcatch, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("lambda")), std::make_shared<Lisp_Function>(&Lisp::lambda, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("macro")), std::make_shared<Lisp_Function>(&Lisp::lambda, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("quote")), std::make_shared<Lisp_Function>(&Lisp::quote, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("quasi-quote")), std::make_shared<Lisp_Function>(&Lisp::qquote, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("cond")), std::make_shared<Lisp_Function>(&Lisp::cond, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("while")), std::make_shared<Lisp_Function>(&Lisp::lwhile, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("progn")), std::make_shared<Lisp_Function>(&Lisp::progn));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("apply")), std::make_shared<Lisp_Function>(&Lisp::apply));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("eval")), std::make_shared<Lisp_Function>(&Lisp::eval));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("repl")), std::make_shared<Lisp_Function>(&Lisp::repl));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("type-of")), std::make_shared<Lisp_Function>(&Lisp::type));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("throw")), std::make_shared<Lisp_Function>(&Lisp::lthrow));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("macroexpand")), std::make_shared<Lisp_Function>(&Lisp::macroexpand));

	m_env->insert(intern(std::make_shared<Lisp_Symbol>("defmacro")), std::make_shared<Lisp_Function>(&Lisp::defmacro, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("env")), std::make_shared<Lisp_Function>(&Lisp::env, 0));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("defq")), std::make_shared<Lisp_Function>(&Lisp::defq, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("setq")), std::make_shared<Lisp_Function>(&Lisp::setq, 1));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("def")), std::make_shared<Lisp_Function>(&Lisp::def));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("undef")), std::make_shared<Lisp_Function>(&Lisp::undef));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("set")), std::make_shared<Lisp_Function>(&Lisp::set));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("def?")), std::make_shared<Lisp_Function>(&Lisp::defined));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("sym")), std::make_shared<Lisp_Function>(&Lisp::sym));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("gensym")), std::make_shared<Lisp_Function>(&Lisp::gensym));
	m_env->insert(intern(std::make_shared<Lisp_Symbol>("bind")), std::make_shared<Lisp_Function>(&Lisp::bind));
}
