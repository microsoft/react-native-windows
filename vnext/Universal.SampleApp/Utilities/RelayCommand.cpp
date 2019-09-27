// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "RelayCommand.h"

using namespace WindowsSampleApp;

using namespace Windows::Foundation;

bool RelayCommand::CanExecute(Object ^ parameter) {
  return (_canExecuteCallback)(parameter);
}

void RelayCommand::Execute(Object ^ parameter) {
  (_executeCallback)(parameter);
}

void RelayCommand::RaiseCanExecuteChanged() {
  CanExecuteChanged(this, nullptr);
}

RelayCommand::~RelayCommand() {
  _canExecuteCallback = nullptr;
  _executeCallback = nullptr;
};

RelayCommand::RelayCommand(
    std::function<bool(Platform::Object ^)> canExecuteCallback,
    std::function<void(Platform::Object ^)> executeCallback)
    : _canExecuteCallback(canExecuteCallback),
      _executeCallback(executeCallback) {}