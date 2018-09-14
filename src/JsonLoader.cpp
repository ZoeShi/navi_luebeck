//
// Created by l on 08.09.18.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include "JsonLoader.h"

Graph JsonLoader::run(std::string filename) {
    readJson(filename);
    setJsonVariables();
    return nodes;
}

void JsonLoader::readJson(std::string file){
    std::ifstream ifs(file);
    j = nlohmann::json::parse(ifs);
}

void JsonLoader::readEdge( Node &node, nlohmann::json &edgeElement){
    Edge edge;
    edge.distance = stoi(static_cast<std::string>(edgeElement["@distance"]));
    edge.to = stoi(static_cast<std::string>(edgeElement["@to"]));
    node.edges.push_back(edge);
}

void JsonLoader::setJsonVariables(){
    for (auto& element : j["nodes"]["node"]) {
        int id = std::stoi(static_cast<std::string>(element["@id"]));
        Node node;
        node.name = element["@name"];
        node.id = id;
        node.coordinates.latitude = readDouble(element["coordinates"]["latitude"]);
        node.coordinates.longitude = readDouble(element["coordinates"]["longitude"]);
        auto& beforeEdgeElement = element["edges"]["edge"];
        if(beforeEdgeElement.is_array()) {
            for (auto &edgeElement : beforeEdgeElement) {
                readEdge(node, edgeElement);
            }
        } else{
            readEdge(node, beforeEdgeElement);
        }
        nodes[id] = node;
    }
}

double JsonLoader::readDouble(nlohmann::json &sub_element) const {
    double value;
    std::string strval = sub_element;
    std::istringstream istr (strval);
    istr.imbue(std::locale::classic());
    istr >> value;
    return value;
}


