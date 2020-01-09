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

std::shared_ptr<Lisp_Obj> Lisp::add(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n + std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(add num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::sub(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n - std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(sub num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::mul(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n * std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(mul num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::div(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n / std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(div num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::mod(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n % std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(mod num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::fmul(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return (n * std::static_pointer_cast<Lisp_Integer>(o)->m_value) >> 16;
		}));
	}
	return repl_error("(fmul num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::fdiv(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return (n << 16) / std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(fdiv num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::max(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return std::max(n, std::static_pointer_cast<Lisp_Integer>(o)->m_value);
		}));
	}
	return repl_error("(max num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::min(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto init = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return std::min(n, std::static_pointer_cast<Lisp_Integer>(o)->m_value);
		}));
	}
	return repl_error("(min num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::eq(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto val = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			if (val != std::static_pointer_cast<Lisp_Integer>(*itr)->m_value) return m_sym_nil;
		}
		return m_sym_t;
	}
	return repl_error("(eq num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::ne(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		for (auto itr = begin(args->m_v); itr != end(args->m_v); ++itr)
		{
			auto val = std::static_pointer_cast<Lisp_Integer>(*itr)->m_value;
			for (auto itr1 = itr + 1; itr1 != end(args->m_v); ++itr1)
			{
				if (val == std::static_pointer_cast<Lisp_Integer>(*itr1)->m_value) return m_sym_nil;
			}
		}
		return m_sym_t;
	}
	return repl_error("(ne num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::lt(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto val = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Integer>(*itr)->m_value;
			if (val >= val1) return m_sym_nil;
			val = val1;
		}
		return m_sym_t;
	}
	return repl_error("(lt num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::gt(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto val = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Integer>(*itr)->m_value;
			if (val <= val1) return m_sym_nil;
			val = val1;
		}
		return m_sym_t;
	}
	return repl_error("(gt num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::le(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto val = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Integer>(*itr)->m_value;
			if (val > val1) return m_sym_nil;
			val = val1;
		}
		return m_sym_t;
	}
	return repl_error("(le num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::ge(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		auto val = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Integer>(*itr)->m_value;
			if (val < val1) return m_sym_nil;
			val = val1;
		}
		return m_sym_t;
	}
	return repl_error("(ge num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::band(const std::shared_ptr<Lisp_List> &args)
{
	if (std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v), end(args->m_v), -1, [] (auto n, auto &o)
		{
			return n & std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(logand [num] ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bor(const std::shared_ptr<Lisp_List> &args)
{
	if (std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v), end(args->m_v), 0, [] (auto n, auto &o)
		{
			return n | std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(logior [num] ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bxor(const std::shared_ptr<Lisp_List> &args)
{
	if (std::all_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_integer); }))
	{
		return std::make_shared<Lisp_Integer>(std::accumulate(begin(args->m_v), end(args->m_v), 0, [] (auto n, auto &o)
		{
			return n ^ std::static_pointer_cast<Lisp_Integer>(o)->m_value;
		}));
	}
	return repl_error("(logxor [num] ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bshl(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_integer)
		&& args->m_v[1]->is_type(lisp_type_integer))
	{
		auto n = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		auto c = std::static_pointer_cast<Lisp_Integer>(args->m_v[1])->m_value;
		return std::make_shared<Lisp_Integer>(n << c);
	}
	return repl_error("(shl num cnt)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bshr(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_integer)
		&& args->m_v[1]->is_type(lisp_type_integer))
	{
		unsigned long long n = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		auto c = std::static_pointer_cast<Lisp_Integer>(args->m_v[1])->m_value;
		return std::make_shared<Lisp_Integer>(n >> c);
	}
	return repl_error("(shr num cnt)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::basr(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_integer)
		&& args->m_v[1]->is_type(lisp_type_integer))
	{
		auto n = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		auto c = std::static_pointer_cast<Lisp_Integer>(args->m_v[1])->m_value;
		return std::make_shared<Lisp_Integer>(n >> c);
	}
	return repl_error("(asr num cnt)", error_msg_wrong_types, args);
}

static unsigned long long seed = 1234567890;

std::shared_ptr<Lisp_Obj> Lisp::random(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_integer))
	{
		auto n = std::static_pointer_cast<Lisp_Integer>(args->m_v[0])->m_value;
		seed = (seed * 17) ^ 0xa5a5a5a5a5a5a5a5;
		return std::make_shared<Lisp_Integer>(seed % n);
	}
	return repl_error("(random num)", error_msg_wrong_types, args);
}
