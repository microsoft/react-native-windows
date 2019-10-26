// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <collection.h>

#include "App.xaml.h"
#include "ViewLifetimeControl.h"
#include <unordered_map>

extern std::unordered_map<std::string, std::string> params;

#undef min
#undef max
