// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// The code is adopted from Facebook's ReactNative implementation for Java.
// It is changed to be more C#-like, but we kept some original comments.
// Below is the original Facebook's copyright notice:
//
//
// Copyright (c) Facebook, Inc. and its affiliates.
//
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.
//

using System;
using System.Collections.Generic;

namespace Microsoft.ReactNative.Managed
{
  // Interface that represents a JavaScript Promise which can be passed to the native module as a
  // method parameter.
  //
  // Methods annotated with ReactMethodAttribute that use a IReactPromise as the last parameter
  // will be marked as "promise" and will return a promise when invoked from JavaScript.
  public interface IReactPromise<T>
  {
    // Successfully resolve the IReactPromise with an optional value.
    void Resolve(T value = default(T));

    // Report an Error.
    void Reject(ReactError error);
  }
}
