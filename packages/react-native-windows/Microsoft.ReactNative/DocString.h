// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#ifdef USE_DOCSTRING
#define DOC_STRING(x) [doc_string(x)]
#define DOC_DEFAULT(x) [doc_default(x)]
import "DocString.idl";
#else
#define DOC_STRING(x)
#define DOC_DEFAULT(x)
#endif
