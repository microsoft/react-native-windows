// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "AppStateModule.h"

namespace facebook {
namespace react {

//
// AppState
//

AppState::AppState() = default;
AppState::~AppState() = default;

// TODO: real implementation
const char *AppState::getState() {
  return "active";
}

//
// AppStateModule
//
const char *AppStateModule::name = "RCTAppState";

AppStateModule::AppStateModule(std::shared_ptr<AppState> &&appState) : m_appState(std::move(appState)) {}

std::string AppStateModule::getName() {
  return name;
}

std::map<std::string, folly::dynamic> AppStateModule::getConstants() {
  return {{"initialAppState", m_appState->getState()}};
}

auto AppStateModule::getMethods() -> std::vector<facebook::xplat::module::CxxModule::Method> {
  return {Method(
      "getCurrentAppState",
      [this](folly::dynamic args, Callback cbSuccess, Callback /*cbFailure*/) {
        cbSuccess({folly::dynamic::object("app_state", m_appState->getState())});
      },
      AsyncTag)};
}

} // namespace react
} // namespace facebook
