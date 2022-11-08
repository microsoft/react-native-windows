// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Threading.Tasks;

namespace Microsoft.ReactNative.Managed
{
  public static class ReactTaskExtensions
  {
    /// <summary>
    /// Connects the given task to the React Native Module resolve and reject methods.
    /// </summary>
    /// <remarks>
    /// The callback will be scheduled using the default task scheduling using the default continuation
    /// options
    /// </remarks>
    public static Task ContinueWith(this Task task, IJSValueWriter writer, MethodResultCallback resolve,
      MethodResultCallback reject)
    {
      return task.ContinueWith(t =>
      {
        if (t.IsCompletedSuccessfully)
        {
          if (resolve != null)
          {
            writer.WriteArgs();
            resolve(writer);
          }
        }
        else
        {
          if (reject != null)
          {
            WriteReject(writer, t.Exception);
            reject(writer);
          }
        }
      });
    }

    /// <summary>
    /// Connects the given task to the React Native Module resolve and reject methods.
    /// </summary>
    /// <remarks>
    /// The callback will be scheduled using the default task scheduling using the default continuation
    /// options
    /// </remarks>
    public static Task ContinueWith<T>(this Task<T> task, IJSValueWriter writer, MethodResultCallback resolve,
    MethodResultCallback reject)
    {
      return task.ContinueWith(t =>
      {
        if (t.IsCompletedSuccessfully)
        {
          if (resolve != null)
          {
            writer.WriteArgs(t.Result);
            resolve(writer);
          }
        }
        else
        {
          if (reject != null)
          {
            WriteReject(writer, t.Exception);
            reject(writer);
          }
        }
      });
    }

    private static void WriteReject(IJSValueWriter writer, AggregateException exception)
    {
      var innerExceptions = exception.InnerExceptions;
      if (innerExceptions.Count == 1)
      {
        WriteReject(writer, innerExceptions[0]);
      }
      else
      {
        WriteReject(writer, exception.Flatten());
      }
    }

    private static void WriteReject(IJSValueWriter writer, Exception exception)
    {
      ReactPromiseWriter.WriteError(writer,
        code: exception.HResult == 0 ? null : exception.HResult.ToString(),
        message: exception.Message,
        userInfo: null,
        stackTrace: exception.StackTrace);
    }
  }
}
