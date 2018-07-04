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

std::shared_ptr<Lisp_Obj> Lisp::filestream(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		auto s = std::make_shared<Lisp_File_Stream>(std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string);
		if (s->is_open()) return s;
		return m_sym_nil;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::strstream(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		return std::make_shared<Lisp_String_Stream>(std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string);
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::read(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_istream)
		&& args->m_v[1]->is_type(lisp_type_number))
	{
		auto value = std::make_shared<Lisp_List>();
		value->m_v.push_back(repl_read(std::static_pointer_cast<Lisp_IStream>(args->m_v[0])->get_stream()));
		value->m_v.push_back(std::make_shared<Lisp_Number>(' '));
		return value;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::readchar(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if ((len == 1
		&& args->m_v[0]->is_type(lisp_type_istream))
		|| (len == 2
			&& args->m_v[0]->is_type(lisp_type_istream)
			&& args->m_v[1]->is_type(lisp_type_number)))
	{
		auto width = 1;
		if (len == 2)
		{
			width = std::static_pointer_cast<Lisp_Number>(args->m_v[1])->m_value;
			width = ((width - 1) & 7) + 1;
		}
		auto value = std::make_shared<Lisp_Number>(0);
		auto chars = (char*) &value->m_value;
		do
		{
			auto c = std::static_pointer_cast<Lisp_IStream>(args->m_v[0])->read_char();
			if (c == -1) return m_sym_nil;
			*(chars++) = c;
		} while (--width);
		return value;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::readline(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_istream))
	{
		bool state;
		auto s = std::static_pointer_cast<Lisp_IStream>(args->m_v[0])->read_line(state);
		if (state) return std::make_shared<Lisp_String>(s);
		return m_sym_nil;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::write(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_ostream)
		&& args->m_v[1]->is_type(lisp_type_string))
	{
		auto stream = std::static_pointer_cast<Lisp_OStream>(args->m_v[0]);
		auto value = std::static_pointer_cast<Lisp_String>(args->m_v[1]);
		stream->write_line(value->m_string);
		return value;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::writechar(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if ((len == 2
		&& args->m_v[0]->is_type(lisp_type_ostream)
		&& args->m_v[1]->is_type(lisp_type_number))
		|| (len == 3
			&& args->m_v[0]->is_type(lisp_type_ostream)
			&& args->m_v[1]->is_type(lisp_type_number)
			&& args->m_v[2]->is_type(lisp_type_number)))
	{
		auto width = 1;
		if (len == 2)
		{
			width = std::static_pointer_cast<Lisp_Number>(args->m_v[2])->m_value;
			width = ((width - 1) & 7) + 1;
		}
		auto value = std::static_pointer_cast<Lisp_Number>(args->m_v[1]);
		auto chars = (char*) &value->m_value;
		do
		{
			std::static_pointer_cast<Lisp_OStream>(args->m_v[0])->write_char(*(chars++));
		} while (--width);
		return value;
	}
	return std::make_shared<Lisp_Obj>();
}

std::shared_ptr<Lisp_Obj> Lisp::writeline(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_ostream)
		&& args->m_v[1]->is_type(lisp_type_string))
	{
		auto stream = std::static_pointer_cast<Lisp_OStream>(args->m_v[0]);
		auto value = std::static_pointer_cast<Lisp_String>(args->m_v[1]);
		stream->write_line(value->m_string);
		stream->write_char('\n');
		return value;
	}
	return std::make_shared<Lisp_Obj>();
}
