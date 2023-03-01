#include "node.h"

int Node::getId() {return id;}

Node::Node(int id, NodeType type) { this->id = id; this->type = type; }