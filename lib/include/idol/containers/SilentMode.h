//
// Created by henri on 21.06.24.
//

#ifndef IDOL_SILENTMODE_H
#define IDOL_SILENTMODE_H

#include <cstdio>
#include <fcntl.h>
#include <csignal>

namespace idol {
    class SilentMode;
}

class idol::SilentMode {
    int m_original_stdout_fd;
    int m_null_fd;
public:
    explicit SilentMode(bool t_silent = true) {
        m_original_stdout_fd = dup(fileno(stdout));
        m_null_fd = open("/dev/null", O_WRONLY);
        dup2(m_null_fd, fileno(stdout));
    }

    ~SilentMode() {
        dup2(m_original_stdout_fd, fileno(stdout));
        close(m_null_fd);
        close(m_original_stdout_fd);
    }
};

#endif //IDOL_SILENTMODE_H
