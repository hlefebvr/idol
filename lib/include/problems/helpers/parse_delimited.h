//
// Created by henri on 03/11/22.
//

#ifndef IDOL_PARSE_DELIMITED_H
#define IDOL_PARSE_DELIMITED_H

#include <fstream>
#include <sstream>
#include <vector>

std::vector<std::vector<std::string>> parse_delimited(const std::string& t_filename, char t_delimiter) {
    std::vector<std::vector<std::string>> result;

    std::ifstream file(t_filename);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file.");
    }

    while (file) {
        std::string line;
        if (!getline(file, line)) { break; }

        std::istringstream line_stream(line);

        std::vector<std::string> line_cells;
        while (line_stream) {
            std::string cell_content;
            if (!getline(line_stream, cell_content, t_delimiter)) break;
            line_cells.push_back(cell_content);
        }

        result.emplace_back(std::move(line_cells));
    }

    return result;
}

#endif //IDOL_PARSE_DELIMITED_H
