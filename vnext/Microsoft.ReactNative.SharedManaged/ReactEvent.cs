// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.Managed
{
  // The ReactEvent<T> makes it explicit that a field is an event.
  // Though, it is perfectly fine to use Action<T>, or any other delegate with the same signature.
  delegate void ReactEvent<T>(T value);

  // A helper class to initialize event handlers.
  // See the ReactEventInfo.MakeEvent for usage.
  static class ReactEventHelper
  {
    public static ReactArgWriter ArgWriter<T>(T arg) => (IJSValueWriter writer) => writer.WriteValue(arg);
  }
}
