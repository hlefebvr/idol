//
// Created by henri on 20.06.24.
//

#ifndef IDOL_LIMITEDWIDTHSTREAM_H
#define IDOL_LIMITEDWIDTHSTREAM_H
#include <iostream>
#include <sstream>
#include <streambuf>
#include <ostream>

namespace idol {

    class LimitedWidthBuffer : public std::streambuf {
    public:
        LimitedWidthBuffer(std::streambuf* sbuf, std::size_t max_width)
                : original_buffer(sbuf), max_width(max_width), current_width(0) {}

    protected:
        virtual int overflow(int ch) override {
            if (ch == '\n' || ch == EOF) {
                current_width = 0;
                return original_buffer->sputc(ch);
            } else {
                if (current_width >= max_width) {
                    if (original_buffer->sputc('\n') == EOF) {
                        return EOF;
                    }
                    current_width = 0;
                }
                ++current_width;
                return original_buffer->sputc(ch);
            }
        }

        virtual std::streamsize xsputn(const char* s, std::streamsize n) override {
            std::streamsize written = 0;
            while (written < n) {
                if (current_width >= max_width) {
                    if (original_buffer->sputc('\n') == EOF) {
                        return written;
                    }
                    current_width = 0;
                }
                std::streamsize remaining = n - written;
                std::streamsize space_left = max_width - current_width;
                std::streamsize to_write = std::min(remaining, space_left);
                std::streamsize just_written = original_buffer->sputn(s + written, to_write);
                if (just_written == 0) {
                    return written;
                }
                written += just_written;
                current_width += just_written;
            }
            return written;
        }

    private:
        std::streambuf* original_buffer;
        std::size_t max_width;
        std::size_t current_width;
    };

    class LimitedWidthStream : public std::ostream {
    public:
        LimitedWidthStream(std::ostream& os, std::size_t max_width)
                : std::ostream(&buffer), buffer(os.rdbuf(), max_width) {}

    private:
        LimitedWidthBuffer buffer;
    };

}

#endif //IDOL_LIMITEDWIDTHSTREAM_H
