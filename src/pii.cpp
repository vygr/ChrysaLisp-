#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/stat.h>
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
	#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
	#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

#include "lisp.h"

#include <sys/types.h>
#include <fcntl.h>
#include <string>
#include <stdio.h>
#include <memory>
#ifdef _WIN64
	#define _CRT_SECURE_NO_WARNINGS
	#define DELTA_EPOCH_IN_MICROSECS 11644473600000000Ui64
	#include <time.h>
	#include <io.h>
	#include <windows.h>
	#include <tchar.h>
#else
	#include <sys/mman.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <dirent.h>
#endif

char dirbuf[1024];

#ifdef _WIN64
std::string pii_dirlist(const char *path)
{
	auto out = std::string{};
	size_t path_len = strlen(path);
	size_t cwd_len = GetCurrentDirectory(1024, dirbuf);
	HANDLE hFind;
	WIN32_FIND_DATA FindData;
	dirbuf[cwd_len++] = '\\';
	strcpy(dirbuf + cwd_len, path);
	cwd_len += path_len;
	dirbuf[cwd_len++] = '\\';
	dirbuf[cwd_len++] = '*';
	dirbuf[cwd_len++] = 0;
	hFind = FindFirstFile(dirbuf, &FindData);
	if (hFind == INVALID_HANDLE_VALUE) return out;
	do
	{
		out.append(indData.cFileName);
		out.append(",");
		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) out.push_back("4");
		else out.push_back("8");
		out.append(",");
	} while (FindNextFile(hFind, &FindData) != 0);
	FindClose(hFind);
	return out;
}
#else
std::string dirlist(const char *path)
{
	auto out = std::string{};
	struct dirent *entry;
	DIR *dir = opendir(path);
	if (dir == NULL) return out;
	while ((entry = readdir(dir)) != NULL)
	{
		out.append(entry->d_name);
		out.append(",");
		out.push_back((char)(entry->d_type + '0'));
		out.append(",");
	}
	closedir(dir);
	return out;
}
#endif

void rmkdir(const char *path)
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

std::shared_ptr<Lisp_Obj> Lisp::piidirlist(const std::shared_ptr<Lisp_List> &args)
{
	if (args->length() == 1)
	{
		if (args->m_v[0]->is_type(lisp_type_string))
		{
			auto path = std::static_pointer_cast<Lisp_String>(args->m_v[0])->m_string;
			return std::make_shared<Lisp_String>(dirlist(path.data()));
		}
		return repl_error("(pii-dirlist path)", error_msg_not_a_string, args);
	}
	return repl_error("(pii-dirlist path)", error_msg_wrong_num_of_args, args);
}
