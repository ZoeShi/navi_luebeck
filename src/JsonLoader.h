//
// Created by l on 12.09.18.
//

#ifndef NAVI_LUEBECK_JSONLOADER_H
#define NAVI_LUEBECK_JSONLOADER_H

#include <string>
#include <vector>
#include <map>
#include "json/json.hpp"
#include "DataTypes.h"

class JsonLoader{
public:
    Graph run(std::string filename);
private:
    nlohmann::json j;
    Graph nodes;

    void readJson(std::string file);
    void readEdge( Node &node, nlohmann::json &edgeElement);
    void setJsonVariables ();

    double readDouble(nlohmann::json &sub_element) const;
};

#endif //NAVI_LUEBECK_JSONLOADER_H
