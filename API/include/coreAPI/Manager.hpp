#pragma once

#include "pife.h"

class Manager {
private:
    static Pife jogoPife_;

public:
    static Pife& getPife();
};