#ifndef OS_H
#define OS_H
#include <string>

constexpr static const char folder_sep = '/';

namespace dm {
namespace os {
inline size_t filesize(FILE *f) {
    if (f == nullptr){
        throw dm_error("Failed getting file size. fd is null");
    }
    int fd = fileno(f);
    // 64 bits(but not in osx or cygwin, where fstat64 is deprecated)
#if !defined(__FreeBSD__) && !defined(__APPLE__) && (defined(__x86_64__) || defined(__ppc64__)) && !defined(__CYGWIN__)
    struct stat64 st;
    if (fstat64(fd, &st) == 0)
    {
        return static_cast<size_t>(st.st_size);
    }
#else
    struct stat st;

    if (fstat(fd, &st) == 0)
    {
        return static_cast<size_t>(st.st_size);
    }
#endif
    throw dm_error("Failed getting file size from fd", errno);
}

inline bool file_exists(const std::string &filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

}
}	  // namespace dm
#endif	// OS_H