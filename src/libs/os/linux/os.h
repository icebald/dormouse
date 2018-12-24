#ifndef OS_H
#define OS_H
#include <string>
#include <sys/stat.h>
#include <algorithm>
#include <cstring>

namespace dm {
namespace os {
namespace details {
    constexpr static const char folder_sep = '/';
}

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

inline bool is_color_terminal() {
    static constexpr const char *Terms[] = {
            "ansi", "color", "console", "cygwin", "gnome", "konsole", "kterm", "linux", "msys", "putty", "rxvt", "screen", "vt100", "xterm"};

    const char *env_p = std::getenv("TERM");
    if (env_p == nullptr)
    {
        return false;
    }

    static const bool result = std::any_of(std::begin(Terms), std::end(Terms), [&](const char *term) { return std::strstr(env_p, term) != nullptr; });
    return result;
}

inline bool in_terminal(FILE *file){
    return isatty(fileno(file)) != 0;
}

}
}	  // namespace dm
#endif	// OS_H