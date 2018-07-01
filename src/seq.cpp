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

std::shared_ptr<Lisp_Obj> Lisp::list(const std::shared_ptr<Lisp_List> &args)
{
	return args;
}

std::shared_ptr<Lisp_Obj> Lisp::push(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if (len >= 2
		&& args->m_v[0]->is_type(lisp_type_list))
	{
		auto l = std::static_pointer_cast<Lisp_List>(args->m_v[0]);
		l->m_v.reserve(l->length() + len - 1);
		for (auto itr = begin(args->m_v) + 1; itr != end(args->m_v); ++itr) l->m_v.push_back(*itr);
		return l;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::pop(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_list))
	{
		auto l = std::static_pointer_cast<Lisp_List>(args->m_v[0]);
		if (l->m_v.empty()) return m_sym_nil;
		auto o = l->m_v.back();
		l->m_v.pop_back();
		return o;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::clear(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() < 1
		|| std::any_of(begin(args->m_v), end(args->m_v), [] (auto &&o)
			{ return !o->is_type(lisp_type_list); })) return std::make_shared<Lisp_Obj>();
	for (auto &&l : args->m_v)
	{
		auto lst = std::static_pointer_cast<Lisp_List>(l);
		lst->m_v.clear();
	}
	return args->m_v[args->length() - 1];
}

std::shared_ptr<Lisp_Obj> Lisp::length(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_seq))
	{
		auto seq = std::static_pointer_cast<Lisp_Seq>(args->m_v[0]);
		return std::make_shared<Lisp_Number>(seq->length());
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::elem(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_number)
		&& args->m_v[1]->is_type(lisp_type_seq))
	{
		auto i = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		auto seq = std::static_pointer_cast<Lisp_Seq>(args->m_v[1]);
		if (i < 0) i += seq->length() + 1;
		if (i >= 0 && i < seq->length()) return seq->elem(i);
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::elemset(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 3
		&& args->m_v[0]->is_type(lisp_type_number)
		&& args->m_v[1]->is_type(lisp_type_list))
	{
		auto i = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		auto lst = std::static_pointer_cast<Lisp_List>(args->m_v[1]);
		if (i < 0) i += lst->length() + 1;
		if (i >= 0 && i < lst->length()) return lst->m_v[i] = args->m_v[2];
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::part(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 4
		&& args->m_v[1]->is_type(lisp_type_list)
		&& args->m_v[2]->is_type(lisp_type_number)
		&& args->m_v[3]->is_type(lisp_type_number))
	{
		auto lst = std::static_pointer_cast<Lisp_List>(args->m_v[1]);
		auto start = std::static_pointer_cast<Lisp_Number>(args->m_v[2])->m_value;
		auto end = std::static_pointer_cast<Lisp_Number>(args->m_v[3])->m_value;
		auto len = lst->length();
		if (start >= 0 && start < end && end <= len)
		{
			auto params = std::make_shared<Lisp_List>();
			auto lower = begin(lst->m_v) + start;
			auto upper = begin(lst->m_v) + end;
			auto pivot = lower;
			for (auto itr = lower + 1; itr != upper; ++itr)
			{
				params->m_v.clear();
				params->m_v.push_back(*itr);
				params->m_v.push_back(*lower);
				auto value = repl_apply(args->m_v[0], params);
				auto result = 0ll;
				if (value->is_type(lisp_type_number)) result = std::static_pointer_cast<Lisp_Number>(value)->m_value;
				if (result < 0 && ++pivot != itr) std::iter_swap(itr, pivot);
			}
			if (pivot != lower) std::iter_swap(lower, pivot);
			return std::make_shared<Lisp_Number>(pivot - begin(lst->m_v));
		}
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::slice(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 3
		&& args->m_v[0]->is_type(lisp_type_number)
		&& args->m_v[1]->is_type(lisp_type_number)
		&& args->m_v[2]->is_type(lisp_type_seq))
	{
		auto s = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		auto e = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
		auto seq = std::static_pointer_cast<Lisp_Seq>(args->m_v[2]);
		if (s < 0) s += seq->length() + 1;
		if (e < 0) e += seq->length() + 1;
		if (s <= e && s >= 0 && e <= seq->length()) return seq->slice(s, e);
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::cat(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length()
		&& args->m_v[0]->is_type(lisp_type_seq)
		&& std::all_of(begin(args->m_v) + 1, end(args->m_v), [&] (auto &&o)
			{ return o->type() == args->m_v[0]->type(); }))
	{
		auto seq = std::static_pointer_cast<Lisp_Seq>(args->m_v[0]);
		return seq->cat(args);
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::find(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2)
	{
		if (args->m_v[0]->is_type(lisp_type_string)
			&& args->m_v[1]->is_type(lisp_type_string))
		{
			auto str1 = std::static_pointer_cast<Lisp_String>(args->m_v[0]);
			auto str2 = std::static_pointer_cast<Lisp_String>(args->m_v[1]);
			auto itr = std::find(crbegin(str2->m_string), crend(str2->m_string), str1->m_string[0]);
			if (itr == crend(str2->m_string)) return m_sym_nil;
			return std::make_shared<Lisp_Number>((crend(str2->m_string) - itr - 1));
		}
		else if (args->m_v[1]->is_type(lisp_type_list))
		{
			auto lst = std::static_pointer_cast<Lisp_List>(args->m_v[1]);
			auto itr = std::find(crbegin(lst->m_v), crend(lst->m_v), args->m_v[0]);
			if (itr == crend(lst->m_v)) return m_sym_nil;
			return std::make_shared<Lisp_Number>((crend(lst->m_v)) - itr - 1);
		}
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::merge(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_list)
		&& args->m_v[1]->is_type(lisp_type_list))
	{
		auto lst2 = std::static_pointer_cast<Lisp_List>(args->m_v[1]);
		if (std::all_of(cbegin(lst2->m_v), cend(lst2->m_v), [] (auto &&o)
			{ return o->is_type(lisp_type_symbol); }))
		{
			auto lst1 = std::static_pointer_cast<Lisp_List>(args->m_v[0]);
			for (auto &&s : lst2->m_v)
			{
				if (std::find(cbegin(lst1->m_v), cend(lst1->m_v), s) == cend(lst1->m_v)) lst1->m_v.push_back(s);
			}
			return lst1;
		}
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::split(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_string)
		&& args->m_v[1]->is_type(lisp_type_string))
	{
		auto str1 = std::static_pointer_cast<Lisp_String>(args->m_v[0]);
		auto str2 = std::static_pointer_cast<Lisp_String>(args->m_v[1]);
		auto value = std::make_shared<Lisp_List>();
		std::stringstream ss(str1->m_string);
		std::string item;
		while (std::getline(ss, item, str2->m_string[0]))
		{
			value->m_v.push_back(std::make_shared<Lisp_String>(item));
		}
		return value;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::match(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_list)
		&& args->m_v[1]->is_type(lisp_type_list))
	{
		auto lst1 = std::static_pointer_cast<Lisp_List>(args->m_v[0]);
		auto lst2 = std::static_pointer_cast<Lisp_List>(args->m_v[1]);
		if (lst1->length() == lst2->length())
		{
			auto len = lst1->length();
			for (auto i = 0; i < len; ++i)
			{
				auto &&o1 = lst1->m_v[i];
				auto &&o2 = lst2->m_v[i];
				if (o1 == o2) continue;
				if (!o2->is_type(lisp_type_string)) goto nomatch;
				if (std::static_pointer_cast<Lisp_String>(o2)->m_string[0] != '_') goto nomatch;
			}
			return m_sym_t;
		}
	nomatch:
		return m_sym_nil;
	}
	return std::make_shared<Lisp_Obj>();
}

void copy1(std::shared_ptr<Lisp_Obj> &o)
{
	if (o->type() != lisp_type_list) return;
	auto lst = std::static_pointer_cast<Lisp_List>(o);
	o = lst->slice(0, lst->length());
	for (auto &&i : std::static_pointer_cast<Lisp_List>(o)->m_v) copy1(i);
}

std::shared_ptr<Lisp_Obj> Lisp::copy(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1)
	{
		if (args->m_v[0]->type() != lisp_type_list) return args->m_v[0];
		auto lst = std::static_pointer_cast<Lisp_List>(args->m_v[0]);
		auto value = lst->slice(0, lst->length());
		for (auto &&i : std::static_pointer_cast<Lisp_List>(value)->m_v) copy1(i);
		return value;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::cmp(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_string)
		&& args->m_v[1]->is_type(lisp_type_string))
	{
		auto str1 = std::static_pointer_cast<Lisp_String>(args->m_v[0]);
		auto str2 = std::static_pointer_cast<Lisp_String>(args->m_v[1]);
		return std::make_shared<Lisp_Number>(str1->cmp(str1, str2));
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::code(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		auto str = std::static_pointer_cast<Lisp_String>(args->m_v[0]);
		return std::make_shared<Lisp_Number>(str->m_string[0]);
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::lchar(const std::shared_ptr<Lisp_List> &args)
{
	if ((args->length() == 1 || args->length() == 2)
		&& args->m_v[0]->is_type(lisp_type_number))
	{
		auto width = 1;
		if (args->length() == 2
			&& args->m_v[1]->is_type(lisp_type_number))
		{
			width = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
			width = ((width - 1) & 7) + 1;
		}
		auto tmp = (char*)&std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
		return std::make_shared<Lisp_String>(tmp, width);
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::eql(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2)
	{
		if (args->m_v[0] != args->m_v[1])
		{
			if (args->m_v[0]->type() == args->m_v[1]->type())
			{
				if (args->m_v[0]->type() == lisp_type_string)
				{
					if (std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string
						== std::static_pointer_cast<Lisp_String>(args->m_v[1])->m_string) goto same;
				}
				else if (args->m_v[0]->type() == lisp_type_number)
				{
					if (std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value
						== std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value) goto same;
				}
			}
		notsame:
			return m_sym_nil;
		}
	same:
		return m_sym_t;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::some(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 5
		&& (args->m_v[0] == m_sym_nil || args->m_v[0]->is_type(lisp_type_number))
		&& (args->m_v[1] == m_sym_nil || args->m_v[1]->is_type(lisp_type_number))
		&& args->m_v[4]->is_type(lisp_type_list))
	{
		auto max_len = 1000000;
		for (auto &&o : std::static_pointer_cast<Lisp_List>(args->m_v[4])->m_v)
		{
			if (!o->is_type(lisp_type_seq)) goto error;
			max_len = std::min(max_len, std::static_pointer_cast<Lisp_Seq>(o)->length());
		}

		auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
		if (max_len != 1000000)
		{
			auto start = 0;
			if (args->m_v[0]->is_type(lisp_type_number))
			{
				start = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
				if (start < 0) start = max_len + start + 1;
			}
			auto end = max_len;
			if (args->m_v[1]->is_type(lisp_type_number))
			{
				end = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
				if (end < 0) end = max_len + end + 1;
			}
			if (start < 0 || start > max_len || end < 0 || end > max_len) goto error;
			auto dir = 1;
			if (start > end)
			{
				dir = -1;
				--start;
				--end;
			}

			auto params = std::make_shared<Lisp_List>();
			while (start != end)
			{
				m_env->m_map[m_sym_underscore] = std::make_shared<Lisp_Number>(start);
				for (auto &&o : std::static_pointer_cast<Lisp_List>(args->m_v[4])->m_v)
				{
					params->m_v.push_back(std::static_pointer_cast<Lisp_Seq>(o)->elem(start));
				}
				value = repl_apply(args->m_v[3], params);
				if (value->type() == lisp_type_obj) break;
				if (args->m_v[2] != m_sym_nil && value != m_sym_nil) break;
				if (args->m_v[2] == m_sym_nil && value == m_sym_nil) break;
				params->m_v.clear();
				start += dir;
			}
		}
		return value;
	}
error:
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::each(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 5
		&& (args->m_v[0] == m_sym_nil || args->m_v[0]->is_type(lisp_type_number))
		&& (args->m_v[1] == m_sym_nil || args->m_v[1]->is_type(lisp_type_number))
		&& args->m_v[4]->is_type(lisp_type_list))
	{
		auto max_len = 1000000;
		for (auto &&o : std::static_pointer_cast<Lisp_List>(args->m_v[4])->m_v)
		{
			if (!o->is_type(lisp_type_seq)) goto error;
			max_len = std::min(max_len, std::static_pointer_cast<Lisp_Seq>(o)->length());
		}

		auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
		if (max_len != 1000000)
		{
			auto start = 0;
			if (args->m_v[0]->is_type(lisp_type_number))
			{
				start = std::static_pointer_cast<Lisp_Number>(args->m_v[0])->m_value;
				if (start < 0) start = max_len + start + 1;
			}
			auto end = max_len;
			if (args->m_v[1]->is_type(lisp_type_number))
			{
				end = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
				if (end < 0) end = max_len + end + 1;
			}
			if (start < 0 || start > max_len || end < 0 || end > max_len) goto error;
			auto dir = 1;
			if (start > end)
			{
				dir = -1;
				--start;
				--end;
			}

			auto params = std::make_shared<Lisp_List>();
			while (start != end)
			{
				m_env->m_map[m_sym_underscore] = std::make_shared<Lisp_Number>(start);
				for (auto &&o : std::static_pointer_cast<Lisp_List>(args->m_v[4])->m_v)
				{
					params->m_v.push_back(std::static_pointer_cast<Lisp_Seq>(o)->elem(start));
				}
				value = repl_apply(args->m_v[3], params);
				if (value->type() == lisp_type_obj) break;
				if (args->m_v[2] != m_sym_nil)
				{
					params->m_v.clear();
					params->m_v.push_back(value);
					value = repl_apply(args->m_v[2], params);
					if (value->type() == lisp_type_obj) break;
				}
				params->m_v.clear();
				start += dir;
			}
		}
		return value;
	}
error:
	return std::make_shared<Lisp_Obj>();
}
