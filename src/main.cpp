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
#include <deque>

int arg_v = 0;

void ss_reset(std::stringstream &ss, std::string s)
{
	ss.str(s);
	ss.clear();
}

int main(int argc, char *argv[])
{
	//process comand args
	auto in_files = std::deque<std::string>{};
	auto arg_b = "src/boot.inc";

	std::stringstream ss;
	for (auto i = 1; i < argc; ++i)
	{
		if (argv[i][0] == '-')
		{
			//option
			std::string opt = argv[i];
			while (!opt.empty() && opt[0] == '-') opt.erase(0, 1);
			if (++i >= argc) goto help;
			ss_reset(ss, argv[i]);
			if (opt == "v") ss >> arg_v;
			else if (opt == "b") arg_b = argv[i];
			else
			{
			help:
				std::cout << "chrysalisp [switches] [filename ...]\neg. chrysalisp -b src/boot.inc prog.lisp\n";
				std::cout << "reads from stdin if no filename.\n";
				std::cout << "-v:  verbosity level 0..1, default 0\n";
				std::cout << "-b:  boot file, default 'src/boot.inc'\n";
				exit(0);
			}
		}
		else
		{
			//filename
			in_files.push_back(argv[i]);
		}
	}

	//repl
	auto lisp = Lisp();
	auto stream = std::make_shared<Lisp_File_IStream>(arg_b);
	if (!stream->is_open())
	{
		std::cout << "No such boot file: " << arg_b << std::endl;
		exit(0);
	}
	auto args = std::make_shared<Lisp_List>();
	args->m_v.push_back(stream);
	args->m_v.push_back(std::make_shared<Lisp_String>(arg_b));
	if (lisp.repl(args) == lisp.m_sym_nil)
	{
		std::cout << "\n;;;;;;;;;;;;;;;;;;\n; C++ ChrysaLisp ;\n;;;;;;;;;;;;;;;;;;\n" << std::endl;
		//from file list
		while (!in_files.empty())
		{
			auto file = in_files.front();
			in_files.pop_front();
			auto stream = std::make_shared<Lisp_File_IStream>(file);
			if (!stream->is_open())
			{
				std::cout << "No such file: " << file << std::endl;
				exit(0);
			}
			args->m_v.clear();
			args->m_v.push_back(stream);
			args->m_v.push_back(std::make_shared<Lisp_String>(file));
			if (lisp.repl(args) != lisp.m_sym_nil) exit(0);
		}
		//from stdin
		auto stream = std::make_shared<Lisp_Sys_Stream>(std::cin);
		auto name = std::make_shared<Lisp_String>("stdin");
		do
		{
			args->m_v.clear();
			args->m_v.push_back(stream);
			args->m_v.push_back(name);
		} while (lisp.repl(args) != lisp.m_sym_nil);
	}
	exit(0);
}
