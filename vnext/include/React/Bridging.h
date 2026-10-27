#pragma once

// The upstream umbrella at <react/bridging/React/Bridging.h> includes
// Dynamic.h unconditionally. Windows excludes that header because it pulls
// Folly/double-conversion, which the public Cxx include setup does not provide
// (#11644). Preserve the umbrella context and forward the patched Bridging.h.
#pragma push_macro("RN_UMBRELLA_CONTEXT")
#undef RN_UMBRELLA_CONTEXT
#define RN_UMBRELLA_CONTEXT 1

#include <react/bridging/AString.h>
#include <react/bridging/Array.h>
#include <react/bridging/ArrayBuffer.h>
#include <react/bridging/Base.h>
#include <react/bridging/Bool.h>
#include <react/bridging/Bridging.h>
#include <react/bridging/CallbackWrapper.h>
#include <react/bridging/Class.h>
#include <react/bridging/Convert.h>
#include <react/bridging/Error.h>
#include <react/bridging/EventEmitter.h>
#include <react/bridging/Function.h>
#include <react/bridging/HighResTimeStamp.h>
#include <react/bridging/LongLivedObject.h>
#include <react/bridging/Number.h>
#include <react/bridging/Object.h>
#include <react/bridging/Promise.h>
#include <react/bridging/Value.h>

#undef RN_UMBRELLA_CONTEXT
#pragma pop_macro("RN_UMBRELLA_CONTEXT")
