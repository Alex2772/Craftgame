#include "File.h"

#ifdef WINDOWS
#include <Windows.h>
#endif
#include <sys/stat.h>
#include <vector>
#include "Dir.h"

File::File(std::string _path) :
	path(_path) {

}
bool File::isFile() {
	struct stat info;
	bool b = stat(path.c_str(), &info) == 0 && info.st_mode;
	return b;
}
#ifdef WINDOWS
void File::remove() {
	if (isDir()) {
		std::vector<File> s = Dir(path).list();
		if (!s.empty()) {
			for (std::vector<File>::iterator i = s.begin(); i != s.end(); i++) {
				File(i->path + "/" + i->path).remove();
			}
		}
		RemoveDirectory((path.c_str()));
	}
	else if (isFile())
		std::remove(path.c_str());
}
#else
void File::remove() {
        if (isDir()) {
                std::vector<File> s = Dir(path).list();
                if (!s.empty()) {
                        for (std::vector<File>::iterator i = s.begin(); i != s.end(); i++) {
                                File(i->path + "/" + i->path).remove();
                        }
                }
                rmdir((path.c_str()));
        }
        else if (isFile())
                std::remove(path.c_str());
}
#endif
bool File::isDir() {
	struct stat info;
	bool b = stat(path.c_str(), &info) == 0 && info.st_mode & S_IFDIR;
	return b;
}
