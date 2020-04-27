// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;

namespace Microsoft.ReactNative.Managed
{
  struct ReactContext
  {
    public ReactContext(IReactContext context)
    {
      ContextAbi = context;
    }

    public IReactContext ContextAbi { get; }

    public void DispatchEvent<T>(FrameworkElement view, string eventName, T arg)
    {
      if (arg is JSValueArgWriter argWriter)
      {
        ContextAbi.DispatchEvent(view, eventName, argWriter);
      }
      else
      {
        ContextAbi.DispatchEvent(view, eventName, (IJSValueWriter writer) => writer.WriteValue(arg));
      }
    }

    public void CallJSFunction(string moduleName, string methodName)
    {
      ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs());
    }

    public void CallJSFunction<T1>(string moduleName, string methodName, T1 arg1)
    {
      if (arg1 is JSValueArgWriter argWriter)
      {
        ContextAbi.CallJSFunction(moduleName, methodName, argWriter);
      }
      else
      {
        ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1));
      }
    }

    public void CallJSFunction<T1, T2>(string moduleName, string methodName, T1 arg1, T2 arg2)
    {
      ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2));
    }

    public void CallJSFunction<T1, T2, T3>(string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3)
    {
      ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3));
    }

    public void CallJSFunction<T1, T2, T3, T4>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
      ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4));
    }

    public void CallJSFunction<T1, T2, T3, T4, T5>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
    {
      ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5));
    }

    public void CallJSFunction<T1, T2, T3, T4, T5, T6>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
    {
      ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5, arg6));
    }

    public void CallJSFunction<T1, T2, T3, T4, T5, T6, T7>(
      string moduleName, string methodName, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
    {
      ContextAbi.CallJSFunction(moduleName, methodName, (IJSValueWriter writer) => writer.WriteArgs(arg1, arg2, arg3, arg4, arg5, arg6, arg7));
    }

    public void EmitJSEvent(string eventEmitterName, string eventName)
    {
      ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter _) => { });
    }

    public void EmitJSEvent<T1>(string eventEmitterName, string eventName, T1 arg1)
    {
      if (arg1 is JSValueArgWriter argWriter)
      {
        ContextAbi.EmitJSEvent(eventEmitterName, eventName, argWriter);
      }
      else
      {
        ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
        {
          writer.WriteValue(arg1);
        });
      }
    }

    public void EmitJSEvent<T1, T2>(string eventEmitterName, string eventName, T1 arg1, T2 arg2)
    {
      ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
      });
    }

    public void EmitJSEvent<T1, T2, T3>(string eventEmitterName, string eventName, T1 arg1, T2 arg2, T3 arg3)
    {
      ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
      {
        writer.WriteValue(arg1);
        writer.WriteValue(arg2);
        writer.WriteValue(arg3);
      });
    }

    public void EmitJSEvent<T1, T2, T3, T4>(
      string eventEmitterName, string eventName, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
    {
      ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
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
      ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
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
      ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
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
      ContextAbi.EmitJSEvent(eventEmitterName, eventName, (IJSValueWriter writer) =>
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
