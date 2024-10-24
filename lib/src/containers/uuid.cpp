//
// Created by henri on 21.10.24.
//
#include "idol/utils/uuid.h"

std::string idol::generate_uuid_v4() {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 15);
    std::uniform_int_distribution<int> dist2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dist(rd);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dist(rd);
    ss << "-4"; // UUID version 4
    for (int i = 0; i < 3; i++) ss << dist(rd);
    ss << "-";
    ss << dist2(rd); // UUID variant 1
    for (int i = 0; i < 3; i++) ss << dist(rd);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dist(rd);
    return ss.str();
}
