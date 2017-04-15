/**
 * Copyright (c) 2017 Eduard Bloch <edi@gmx.de>
 * All rights reserved
 *
 * License: WTFPL
 *
 */
#pragma once

#include <string>
#include <functional>
#include <sys/stat.h>

namespace xdirscan
{
/**
 *  Callbacks when a file/directory is found or a directory was scanned
 *  @param Full path starting at specified root directory
 *  @param stat information
 *  @return True to continue scanning, False for immediate cancelation
 */
typedef std::function<bool(const std::string &, const struct stat&)> visitorCB;
bool FindFiles(const std::string & sRootDir, const visitorCB& onDirFile,
		const visitorCB& onDirLeaving = xdirscan::visitorCB(), bool bFollowSymlinks = true);
}
