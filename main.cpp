#include "recdirscan.h"

#include <iostream>

int main(int argc, const char** argv)
{
	if(argc<2)
		return 2;
	return ! xdirscan::FindFiles(argv[1],
		[](const std::string &s, const struct stat&)
		{
			std::cout << s << std::endl;
			return true;
		},
		[](const std::string &s, const struct stat&)
		{
			std::cout << "EXDIR: " << s << std::endl;
			return true;
		}
	);
}
