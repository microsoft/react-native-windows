#pragma once

#include "pch.h"

using namespace Platform;

namespace ChakraBridge {

public delegate String^ CallSyncHandler(int moduleId, int methodId, String^ args);

}
