// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace Microsoft.ReactNative.Managed
{
  [Obsolete("Use Action<T1>")]
  delegate void ReactCallback<T>(T value);

  [Obsolete("Use Action<T1>, or Action, Action<T1, T2>, etc.")]
  delegate void ReactEvent<T>(T value);
}
