//
// Created by henri on 21.06.24.
//

#ifndef IDOL_SILENTMODE_H
#define IDOL_SILENTMODE_H

#include <cstdio>
#include <fcntl.h>
#include <csignal>
#include <unistd.h>

namespace idol {
    class SilentMode;
}

class idol::SilentMode {
    int m_original_stdout_fd;
    int m_null_fd;
    bool m_silent_mode;
public:
    explicit SilentMode(bool t_silent = true) : m_silent_mode(t_silent) {
        if (!m_silent_mode) { return; }
        m_original_stdout_fd = dup(fileno(stdout));
        m_null_fd = open("/dev/null", O_WRONLY);
        dup2(m_null_fd, fileno(stdout));
    }

    ~SilentMode() {
        if (!m_silent_mode) { return; }
        dup2(m_original_stdout_fd, fileno(stdout));
        close(m_null_fd);
        close(m_original_stdout_fd);
    }
};

#endif //IDOL_SILENTMODE_H
