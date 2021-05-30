// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#if !USE_WINUI3
using Windows.UI.Xaml;
#else
using Microsoft.UI.Xaml;
#endif

namespace Microsoft.ReactNative.Managed
{
  public struct ReactContext
  {
    public ReactContext(IReactContext handle)
    {
      Handle = handle;
    }

    public IReactContext Handle { get; }

    public void DispatchEvent<T>(FrameworkElement view, string eventName, T arg)
    {
      var argWriter = arg as JSValueArgWriter;
      if (argWriter != null)
      {
        XamlUIService.FromContext(Handle).DispatchEvent(view, eventName, argWriter);
      }
      else
      {
        XamlUIService.FromContext(Handle).DispatchEvent(view, eventName, (IJSValueWriter writer) => writer.WriteValue(arg));
      }
    }

    public void CallJSFunction(string moduleName, string methodName)
    {
      Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs());
    }

    public void CallJSFunction<T1>(string moduleName, string methodName, T1 arg1)
    {
      var argWriter = arg1 as JSValueArgWriter;
      if (argWriter != null)
      {
        Handle.CallJSFunction(moduleName, methodName, argWriter);
      }
      else
      {
        Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1));
      }
    }

    public void CallJSFunction<T1, T2>(string moduleName, string methodName, T1 arg1, T2 arg2)
    {
      Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2));
    }

    public void CallJSFunction<T1, T2, T3>(string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3)
    {
      Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3));
    }

    public void CallJSFunction<T1, T2, T3, T4>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
      Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4));
    }

    public void CallJSFunction<T1, T2, T3, T4, T5>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
      Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5));
    }

    public void CallJSFunction<T1, T2, T3, T4, T5, T6>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
      Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5, arg6));
    }

    public void CallJSFunction<T1, T2, T3, T4, T5, T6, T7>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
      Handle.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    }

    public void EmitJSEvent(string eventEmitterName, string eventName)
    {
      Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter _) => { });
    }

    public void EmitJSEvent<T1>(string eventEmitterName, string eventName, T1 arg1)
    {
      var argWriter = arg1 as JSValueArgWriter;
      if (argWriter != null)
      {
        Handle.EmitJSEvent(eventEmitterName, eventName, argWriter);
      }
      else
      {
        Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
        {
          writer.WriteValue(arg1);
        });
      }
    }

    public void EmitJSEvent<T1, T2>(string eventEmitterName, string eventName, T1 arg1, T2 arg2)
    {
      Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
      });
    }

    public void EmitJSEvent<T1, T2, T3>(string eventEmitterName, string eventName, T1 arg1, T2 arg2, T3 arg3)
    {
      Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
        writer.WriteValue(arg3);
      });
    }

    public void EmitJSEvent<T1, T2, T3, T4>(
      string eventEmitterName, string eventName, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
      Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
        writer.WriteValue(arg3);
        writer.WriteValue(arg4);
      });
    }

    public void EmitJSEvent<T1, T2, T3, T4, T5>(
      string eventEmitterName, string eventName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
      Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
        writer.WriteValue(arg3);
        writer.WriteValue(arg4);
        writer.WriteValue(arg5);
      });
    }

    public void EmitJSEvent<T1, T2, T3, T4, T5, T6>(
      string eventEmitterName, string eventName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
      Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
        writer.WriteValue(arg3);
        writer.WriteValue(arg4);
        writer.WriteValue(arg5);
        writer.WriteValue(arg6);
      });
    }

    public void EmitJSEvent<T1, T2, T3, T4, T5, T6, T7>(
      string eventEmitterName, string eventName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
      Handle.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
        writer.WriteValue(arg3);
        writer.WriteValue(arg4);
        writer.WriteValue(arg5);
        writer.WriteValue(arg6);
        writer.WriteValue(arg7);
      });
    }
  }
}
