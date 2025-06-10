#pragma once
#include <vector>
#include "vector2.hpp"

class Peice;

struct Move{
    std::vector<Peice*> updatedPeices;
}; 