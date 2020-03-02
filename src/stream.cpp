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
	return repl_error("(file-stream path)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::strstream(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		return std::make_shared<Lisp_String_Stream>("");
	}
	return repl_error("(string-stream str)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::read(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_istream)
		&& args->m_v[1]->is_type(lisp_type_integer))
	{
		auto value = std::make_shared<Lisp_List>();
		value->m_v.push_back(repl_read(std::static_pointer_cast<Lisp_IStream>(args->m_v[0])->get_stream()));
		value->m_v.push_back(std::make_shared<Lisp_Integer>(' '));
		return value;
	}
	return repl_error("(read stream last_char)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::readchar(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if (len == 1 || len == 2)
	{
		if (args->m_v[0]->is_type(lisp_type_istream))
		{
			auto width = 1ll;
			if (len == 2)
			{
				if (!args->m_v[1]->is_type(lisp_type_integer))
					return repl_error("(read-char stream [width])", error_msg_not_a_number, args);
				width = std::static_pointer_cast<Lisp_Integer>(args->m_v[1])->m_value;
				width = ((width - 1) & 7) + 1;
			}
			auto value = std::make_shared<Lisp_Integer>(0);
			auto chars = (char*) &value->m_value;
			do
			{
				auto c = std::static_pointer_cast<Lisp_IStream>(args->m_v[0])->read_char();
				if (c == -1) return m_sym_nil;
				*(chars++) = c;
			} while (--width);
			return value;
		}
		return repl_error("(read-char stream [width])", error_msg_not_a_stream, args);
	}
	return repl_error("(read-char stream [width])", error_msg_wrong_num_of_args, args);
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
	return repl_error("(read-line stream)", error_msg_wrong_types, args);
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
		return stream;
	}
	return repl_error("(write stream str)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::writechar(const std::shared_ptr<Lisp_List> &args)
{
	auto len = args->length();
	if (len == 2 || len == 3)
	{
		if (args->m_v[0]->is_type(lisp_type_ostream))
		{
			auto width = 1ll;
			if (len == 3)
			{
				if (!args->m_v[2]->is_type(lisp_type_integer))
					return repl_error("(write-char stream list|num [width])", error_msg_not_a_number, args);
				width = std::static_pointer_cast<Lisp_Integer>(args->m_v[2])->m_value;
				width = ((width - 1) & 7) + 1;
			}

			if (args->m_v[1]->is_type(lisp_type_list))
			{
				auto list = std::static_pointer_cast<Lisp_List>(args->m_v[1]);
				if (!list->m_v.empty())
				{
					for (auto &&value : list->m_v)
					{
						if (!value->is_type(lisp_type_integer))
							return repl_error("(write-char stream list|num [width])", error_msg_not_a_number, args);
						auto chars = (char*) &(std::static_pointer_cast<Lisp_Integer>(value))->m_value;
						auto w = width;
						do
						{
							std::static_pointer_cast<Lisp_OStream>(args->m_v[0])->write_char(*(chars++));
						} while (--w);
					}
					return args->m_v[0];
				}
				return repl_error("(write-char stream list|num [width])", error_msg_wrong_num_of_args, args);
			}
			else if (args->m_v[1]->is_type(lisp_type_integer))
			{
				auto value = std::static_pointer_cast<Lisp_Integer>(args->m_v[1]);
				auto chars = (char*) &value->m_value;
				do
				{
					std::static_pointer_cast<Lisp_OStream>(args->m_v[0])->write_char(*(chars++));
				} while (--width);
				return args->m_v[0];
			}
			return repl_error("(write-char stream list|num [width])", error_msg_not_a_number, args);
		}
		return repl_error("(write-char stream list|num [width])", error_msg_not_a_stream, args);
	}
	return repl_error("(write-char stream list|num [width])", error_msg_wrong_num_of_args, args);
}

std::shared_ptr<Lisp_Obj> Lisp::prin(const std::shared_ptr<Lisp_List> &args)
{
	auto value = std::static_pointer_cast<Lisp_Obj>(m_sym_nil);
	for (auto &obj : args->m_v)
	{
		value = obj;
		if (value->type() == lisp_type_string) std::cout << std::static_pointer_cast<Lisp_String>(value)->m_string;
		else value->print(std::cout);
	}
	return value;
}

std::shared_ptr<Lisp_Obj> Lisp::print(const std::shared_ptr<Lisp_List> &args)
{
	auto value = prin(args);
	std::cout << std::endl;
	return value;
}

static void rmkdir(const char *path)
{
	char *p = NULL;
	char dirbuf[256];
	size_t len;
	len = strlen(path);
	memcpy(dirbuf, path, len + 1);
	for (p = dirbuf + 1; *p; p++)
	{
		if(*p == '/')
		{
			*p = 0;
#ifdef WIN32
			mkdir(dirbuf);
#else
			mkdir(dirbuf, S_IRWXU);
#endif
			*p = '/';
		}
	}
}

std::shared_ptr<Lisp_Obj> Lisp::save(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 2
		&& args->m_v[0]->is_type(lisp_type_string)
		&& args->m_v[1]->is_type(lisp_type_string))
	{
		std::ofstream f;
		f.open(std::static_pointer_cast<Lisp_String>(args->m_v[1])->m_string,
			std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
		if (!f.is_open())
		{
			rmkdir(std::static_pointer_cast<Lisp_String>(args->m_v[1])->m_string.c_str());
			f.open(std::static_pointer_cast<Lisp_String>(args->m_v[1])->m_string,
				std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
		}
		if (f.is_open())
		{
			f << std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string;
			return args->m_v[0];
		}
		return repl_error("(save str path)", error_msg_open_error, args);
	}
	return repl_error("(save str path)", error_msg_wrong_types, args);
}

std::shared_ptr<Lisp_Obj> Lisp::load(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1
		&& args->m_v[0]->is_type(lisp_type_string))
	{
		std::ifstream f;
		f.open(std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string,
			std::ifstream::in | std::ifstream::binary);
		if (f.is_open())
		{
			return std::make_shared<Lisp_String>(std::string((std::istreambuf_iterator<char>(f)),
												(std::istreambuf_iterator<char>())));
		}
		return m_sym_nil;
	}
	return repl_error("(load path)", error_msg_wrong_types, args);
}
