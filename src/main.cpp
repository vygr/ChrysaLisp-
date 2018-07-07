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

void ss_reset(std::stringstream &ss, std::string s)
{
	ss.str(s);
	ss.clear();
}

int main(int argc, char *argv[])
{
	//process comand args
	auto use_file = false;
	std::ifstream arg_infile;
	auto arg_v = 0;
	auto arg_s = 1;

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
			else if (opt == "s") ss >> arg_s;
			else
			{
			help:
				std::cout << "chrysalisp [switches] [filename]\neg. chrysalisp -v 1 prog.lisp\n";
				std::cout << "reads from stdin if no filename.\n";
				std::cout << "-v:  verbosity level 0..1, default 0\n";
				std::cout << "-s:  number of samples, default 1\n";
				exit(0);
			}
		}
		else
		{
			//filename
			arg_infile.open(argv[i], std::ifstream::in);
			use_file = true;
		}
	}

	//reading from stdin or file
	std::istream &in = use_file ? arg_infile : std::cin;

	//repl
	auto lisp = Lisp();
	auto boot_file = "src/boot.inc";
	auto args = std::make_shared<Lisp_List>();
	args->m_v.push_back(std::make_shared<Lisp_File_Stream>(boot_file));
	args->m_v.push_back(std::make_shared<Lisp_String>(boot_file));
	lisp.repl(args);
	std::cout << "\n;;;;;;;;;;;;;;;;;;\n; C++ ChrysaLisp ;\n;;;;;;;;;;;;;;;;;;\n" << std::endl;
	args->m_v.clear();
	args->m_v.push_back(std::make_shared<Lisp_Sys_Stream>(std::cin));
	args->m_v.push_back(std::make_shared<Lisp_String>("stdin"));
	lisp.repl(args);
	return 0;
}
