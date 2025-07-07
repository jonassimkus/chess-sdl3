#pragma once
#include <vector>
#include "vector2.hpp"

class Peice;

struct Move{
    std::vector<Peice> updatedPeices;
    std::vector<Peice> preupdatedPeices;
    bool safe = true;
}; 