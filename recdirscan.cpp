/**
 * Copyright (c) 2017 Eduard Bloch <edi@gmx.de>
 * All rights reserved
 *
 * License: WTFPL
 *
 */

#include "recdirscan.h"

#include <unordered_map>
#include <set>

#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

using namespace std;

namespace xdirscan
{

typedef pair<dev_t,ino_t> tPairDevIno;
typedef set<tPairDevIno> tDupeFilter;

class dnode
{
	string sPath;
	dnode *m_parent;
	struct stat m_stinfo;

	// not to be copied
	dnode& operator=(const dnode&);
	dnode(const dnode&);

public:
	dnode(dnode *parent) : m_parent(parent) {};
	dnode(const string& path) : m_parent(0), sPath(path) {};

	bool Walk(const visitorCB& onFound, const visitorCB& onDirLeaving, tDupeFilter* pFilter, bool bFollowSymlinks)
	{
		if(bFollowSymlinks)
		{
			if(stat(sPath.c_str(), &m_stinfo))
				return true; // slight risk of missing information here... bug ignoring is safer
		}
		else
		{
			auto r=lstat(sPath.c_str(), &m_stinfo);
			if(r)
				return true; // slight risk of missing information here... bug ignoring is safer
			// yeah, and we ignore symlinks here
			if(S_ISLNK(m_stinfo.st_mode))
				return true;
		}

		bool ret = true;
		if(onFound)
			ret = onFound(sPath, m_stinfo);

		if(!ret || !S_ISDIR(m_stinfo.st_mode))
			return ret;

		// ok, we are a directory, scan it and descend where needed

		// seen this in the path before? symlink cycle?
		for(dnode *cur=m_parent; cur!=nullptr; cur=cur->m_parent)
		{
			if (m_stinfo.st_dev == cur->m_stinfo.st_dev && m_stinfo.st_ino == cur->m_stinfo.st_ino)
				return true;
		}
		// also make sure we are not visiting the same directory through some symlink construct
		if(pFilter)
		{
			auto key_isnew = pFilter->emplace(m_stinfo.st_dev, m_stinfo.st_ino);
			if (!key_isnew.second)
				return true; // visited this before, recursion detected
		}
		DIR *dir = opendir(sPath.c_str());
		auto exitWalk = [&ret, &dir, &onDirLeaving, this]
		{
			if(dir)
			closedir(dir);
			if(onDirLeaving && ret)
			ret = onDirLeaving(sPath, m_stinfo);
			return ret;
		};
		if (!dir)
			return exitWalk();

		struct dirent *dp;
		dnode childbuf(this);
		while (nullptr != (dp = readdir(dir)))
		{
			if (strcmp(dp->d_name, ".") && strcmp(dp->d_name, ".."))
			{
				childbuf.sPath = sPath + '/' + dp->d_name;
				ret = childbuf.Walk(onFound, onDirLeaving, pFilter,
						bFollowSymlinks);
				if (!ret)
					return exitWalk();
			}
		}

		return exitWalk();
	}
};

bool FindFiles(const std::string & sRootDir, const visitorCB& onDirFile,
		const visitorCB& onDirLeaving, bool bFollowSymlinks)
{
	dnode root(sRootDir);
	tDupeFilter filter;
	return root.Walk(onDirFile, onDirLeaving, &filter, bFollowSymlinks);
}


}
