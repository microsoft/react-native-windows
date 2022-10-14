// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Reflection.Metadata;
using System.Runtime.CompilerServices;

namespace Microsoft.ReactNative.Managed
{
  public struct ReactSettingsSnapshot
  {
    public string BundleRootPath => IsValid ? Handle.BundleRootPath : null;

    public string DebugBundlePath => IsValid ? Handle.DebugBundlePath : null;

    public bool DebuggerBreakOnNextLine => IsValid ? Handle.DebuggerBreakOnNextLine : false;

    public ushort DebuggerPort => IsValid ? Handle.DebuggerPort : (ushort)0;

    public string JavaScriptBundleFile => IsValid ? Handle.JavaScriptBundleFile : null;

    public bool RequestInlineSourceMap => IsValid ? Handle.RequestInlineSourceMap : false;

    public string SourceBundleHost => IsValid ? Handle.SourceBundleHost : null;

    public ushort SourceBundlePort => IsValid ? Handle.SourceBundlePort : (ushort)0;

    public bool UseDirectDebugger => IsValid ? Handle.UseDirectDebugger : false;

    public bool UseFastRefresh => IsValid ? Handle.UseFastRefresh : false;

    public bool UseWebDebugger => IsValid ? Handle.UseWebDebugger : false;

    public IReactSettingsSnapshot Handle { get; }

    public bool IsValid => Handle != null;

    public static explicit operator bool(ReactSettingsSnapshot settingsSnapshot) => settingsSnapshot.IsValid;

    public ReactSettingsSnapshot(IReactSettingsSnapshot handle = null)
    {
      Handle = handle;
    }
  }
}
