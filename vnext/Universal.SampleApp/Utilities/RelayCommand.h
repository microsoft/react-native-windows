// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <functional>

namespace WindowsSampleApp {
[Windows::Foundation::Metadata::WebHostHidden] public ref class RelayCommand
    sealed : [Windows::Foundation::Metadata::Default] Windows::UI::Xaml::Input::
                 ICommand {
 public:
  virtual event Windows::Foundation::EventHandler<Platform::Object ^> ^
      CanExecuteChanged;
  virtual bool CanExecute(Platform::Object ^ parameter);
  virtual void Execute(Platform::Object ^ parameter);
  virtual ~RelayCommand();

  internal : RelayCommand(
                 std::function<bool(Platform::Object ^)> canExecuteCallback,
                 std::function<void(Platform::Object ^)> executeCallback);
  void RaiseCanExecuteChanged();

 private:
  std::function<bool(Platform::Object ^)> _canExecuteCallback;
  std::function<void(Platform::Object ^)> _executeCallback;
};
}