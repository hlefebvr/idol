//
// Created by henri on 22.10.24.
//

#ifndef IDOL_SERVER_H
#define IDOL_SERVER_H

#include <TApplication.h>
#include <TCanvas.h>
#include <list>

namespace idol {
    class Server;
}

class idol::Server {
    TApplication m_root;

    std::list<std::unique_ptr<TObject>> m_objects;
public:
    Server();

    Server(const Server&) = delete;
    Server(Server&&) = delete;

    Server& operator=(const Server&) = delete;
    Server& operator=(Server&&) = delete;

    ~Server();
};

#endif //IDOL_SERVER_H
