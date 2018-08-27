#ifndef DM_OS_H
#define DM_OS_H

#include <io.h>      // _get_osfhandle and _isatty support
#include <process.h> //  _get_pid support
#include <windows.h>

#include <string>
#include <stdio.h>

constexpr static const char folder_sep = '\\';

inline void sleep_for_millis(int milliseconds) {
    ::Sleep(milliseconds);
}

inline size_t filesize(FILE *f)
{
    if (f == nullptr)
    {
        throw spdlog_ex("Failed getting file size. fd is null");
    }
#if defined(_WIN32) && !defined(__CYGWIN__)
    int fd = _fileno(f);
#if _WIN64 // 64 bits
    struct _stat64 st;
    if (_fstat64(fd, &st) == 0)
    {
        return st.st_size;
    }

#else // windows 32 bits
    long ret = _filelength(fd);
    if (ret >= 0)
    {
        return static_cast<size_t>(ret);
    }
#endif

#else // unix
    int fd = fileno(f);
    // 64 bits(but not in osx or cygwin, where fstat64 is deprecated)
#if !defined(__FreeBSD__) && !defined(__APPLE__) && (defined(__x86_64__) || defined(__ppc64__)) && !defined(__CYGWIN__)
    struct stat64 st;
    if (fstat64(fd, &st) == 0)
    {
        return static_cast<size_t>(st.st_size);
    }
#else // unix 32 bits or cygwin
    struct stat st;

    if (fstat(fd, &st) == 0)
    {
        return static_cast<size_t>(st.st_size);
    }
#endif
#endif
    throw spdlog_ex("Failed getting file size from fd", errno);
}

inline bool file_exists(const std::string &filename) {
    auto attribs = GetFileAttributesA(filename.c_str());
    return (attribs != INVALID_FILE_ATTRIBUTES && !(attribs & FILE_ATTRIBUTE_DIRECTORY));
}

inline bool is_color_terminal() {
    return true;
}

inline bool in_terminal(FILE *file){
    return _isatty(_fileno(file)) != 0;
}

#endif