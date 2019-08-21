#pragma once

// Assuming this file is included only when "defined(OSS_RN)"

#include <jsi/jsi.h>

void init_tracing(facebook::jsi::Runtime &runtime);
