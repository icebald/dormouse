#ifndef DM_FILE_HELPER_H
#define DM_FILE_HELPER_H

#include <cerrno>
#include <chrono>
#include <cstdio>
#include <string>
#include <thread>
#include <tuple>
#include "error.h"
#include "os/os_api.h"

namespace dm {
namespace details {

class file_helper {
public:
    const int open_tries = 5;
    const int open_interval = 10;

    explicit file_helper() = default;

    file_helper(const file_helper &) = delete;
    file_helper &operator=(const file_helper &) = delete;

    ~file_helper() { close(); }

    void open(const std::string &fname, bool truncate = false) {
        close();
        auto *mode = truncate ? "wb" : "ab";
        _filename = fname;
        for (int tries = 0; tries < open_tries; ++tries) {
            if (!dm::os::fopen_s(&fd_, fname, mode)) {
                return;
            }
            dm::os::sleep_for_millis(open_interval);
        }
    }

    void reopen(bool truncate) {
        if (_filename.empty()) {
            return;
        }
        open(_filename, truncate);
    }

    void flush() {
        std::fflush(fd_);
    }

    void close() {
        if (fd_ != nullptr) {
            std::fclose(fd_);
            fd_ = nullptr;
        }
    }

    template <typename Buffer>
    void write(const Buffer &buf) {
        size_t msg_size = buf.size();
        auto data = buf.data();
        if (std::fwrite(data, 1, msg_size, fd_) != msg_size) {
            throw dm_error("Failed writing to file " + _filename, errno);
        }
    }

    void write(const char *data, int length) {
        if (std::fwrite(data, 1, length, fd_) != length) {
            throw dm_error("Failed writing to file " + _filename, errno);
        }
    }

    size_t size() const {
        if (fd_ == nullptr) {
            throw dm_error("Cannot use size() on closed file " + _filename);
        }
        return os::filesize(fd_);
    }

    const std::string &filename() const {
        return _filename;
    }

    static bool file_exists(const std::string &fname)
    {
        return os::file_exists(fname);
    }

    static std::tuple<std::string, std::string> split_by_extenstion(const std::string &fname){
        auto ext_index = fname.rfind('.');

        // no valid extension found - return whole path and empty string as extension
        if (ext_index == std::string::npos || ext_index == 0 || ext_index == fname.size() - 1)
        {
            return std::make_tuple(fname, std::string());
        }

        // treat casese like "/etc/rc.d/somelogfile or "/abc/.hiddenfile"
        auto folder_index = fname.rfind(os::details::folder_sep);
        if (folder_index != fname.npos && folder_index >= ext_index - 1)
        {
            return std::make_tuple(fname, std::string());
        }

        // finally - return a valid base and extension tuple
        return std::make_tuple(fname.substr(0, ext_index), fname.substr(ext_index));
    }

private:
    FILE *fd_{nullptr};
    std::string _filename;
};
} // namespace details
}
#endif
