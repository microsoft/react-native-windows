// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IUIManager.h>
#include <ReactUWP/IReactInstance.h>

#include <cxxreact/CxxModule.h>

namespace react { namespace uwp {

std::shared_ptr<facebook::react::IUIManager> CreateUIManager(std::shared_ptr<IReactInstance> instance);

}}
