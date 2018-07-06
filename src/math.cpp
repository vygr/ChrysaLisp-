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
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n + std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(add num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::sub(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n - std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(sub num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::mul(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n * std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(mul num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::div(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n / std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(div num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::mod(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n % std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(mod num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::max(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return std::max(n, std::static_pointer_cast<Lisp_Number>(o)->m_value);
		}));
	}
	return repl_error("(max num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::min(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return std::min(n, std::static_pointer_cast<Lisp_Number>(o)->m_value);
		}));
	}
	return repl_error("(min num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::eq(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o)
			{ return o->is_type(lisp_type_number); }))
	{
		auto val = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			if (val != std::static_pointer_cast<Lisp_Number>(*itr)->m_value) return m_sym_nil;
		}
		return m_sym_t;
	}
	return repl_error("(eq num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::ne(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o)
			{ return o->is_type(lisp_type_number); }))
	{
		for (auto itr = begin(args->m_v); itr != end(args->m_v); ++itr)
		{
			auto val = std::static_pointer_cast<Lisp_Number>(*itr)->m_value;
			for (auto itr1 = itr + 1; itr1 != end(args->m_v); ++itr1)
			{
				if (val == std::static_pointer_cast<Lisp_Number>(*itr1)->m_value) return m_sym_nil;
			}
		}
		return m_sym_t;
	}
	return repl_error("(ne num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::lt(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o)
			{ return o->is_type(lisp_type_number); }))
	{
		auto val = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Number>(*itr)->m_value;
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
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o)
			{ return o->is_type(lisp_type_number); }))
	{
		auto val = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Number>(*itr)->m_value;
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
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o)
			{ return o->is_type(lisp_type_number); }))
	{
		auto val = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Number>(*itr)->m_value;
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
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o)
			{ return o->is_type(lisp_type_number); }))
	{
		auto val = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr)
		{
			auto val1 = std::static_pointer_cast<Lisp_Number>(*itr)->m_value;
			if (val < val1) return m_sym_nil;
			val = val1;
		}
		return m_sym_t;
	}
	return repl_error("(ge num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::band(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n & std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(bit-and num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bor(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n | std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(bit-or num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bxor(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() > 1
		&& std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o) { return o->is_type(lisp_type_number); }))
	{
		auto init = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_Number>(std::accumulate(begin(args->m_v) + 1, end(args->m_v), init, [] (auto n, auto &o)
		{
			return n ^ std::static_pointer_cast<Lisp_Number>(o)->m_value;
		}));
	}
	return repl_error("(bit-xor num num ...)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bshl(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_number)
		&& args->m_v[1]->is_type(lisp_type_number))
	{
		auto n = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		auto c = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
		return std::make_shared<Lisp_Number>(n << c);
	}
	return repl_error("(bit-shl num cnt)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::bshr(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_number)
		&& args->m_v[1]->is_type(lisp_type_number))
	{
		unsigned long long n = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		auto c = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
		return std::make_shared<Lisp_Number>(n >> c);
	}
	return repl_error("(bit-shr num cnt)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::basr(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_number)
		&& args->m_v[1]->is_type(lisp_type_number))
	{
		auto n = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		auto c = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
		return std::make_shared<Lisp_Number>(n >> c);
	}
	return repl_error("(bit-asr num cnt)", error_msg_wrong_types, args);
}
