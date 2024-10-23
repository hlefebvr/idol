//
// Created by henri on 22.10.24.
//
#include "idol/optimizers/mixed-integer-optimization/callbacks/watchers/Server.h"

idol::Server::Server() : m_root("Server", nullptr, nullptr) {

}

idol::Server::~Server() {
    m_root.Run();
}
