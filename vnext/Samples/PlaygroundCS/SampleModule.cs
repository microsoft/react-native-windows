// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.ReactNative.Bridge;

namespace Playground
{
  public class SampleModule : NativeModuleBase
  {
    protected override string Name => nameof(SampleModule);

    protected override IReadOnlyDictionary<string, object> Constants { get; }
      = new Dictionary<string, object>()
    {
      { "a", "\"b\"" },
    };

    protected override IReadOnlyList<MethodInfo> Methods { get; }
      = new List<MethodInfo>()
      {
        // Fire and forget Actions
        new MethodInfo(
          "method1", ReturnType.Void,
          (_, __, ___) => {
        Debug.WriteLine("SampleModule.method1()");
        }),
        // Methods with Callbacks
        new MethodInfo(
          "method2", ReturnType.Callback,
          (args, callback, _) => {
            Debug.WriteLine($"SampleModule.method2({String.Join(", ", args)})");

            // Build up a complex object to pass back as arguments when invoking
            // the callback.  They'll automatically be converted to an equivalent
            // type in JavaScript when the JS function is called.
            var parameters = new List<object> {
              84,
              new[] { 0, 1, 2, 3, 4 },
              new[] { 0.02f, 1.0f, 0.42f },
              new[] { "a", "bb" },
              new object[] { new object[] { "First", 0x48 }, "Last" }
            };
            callback(parameters.AsReadOnly());
          }),
        // Referencing a static method
        new MethodInfo("method3", ReturnType.Callback, Method3),
        // Methods with Promise
        new MethodInfo(
          "method4", ReturnType.Promise,
          (args, resolve, reject) => {
            try
            {
              resolve(new object[] {true});
            }
            catch (Exception e)
            {
              reject(new object[] {"0x80000000", e.Message, e.StackTrace, e});
            }
          })
      };

    public static void Method3(
      IReadOnlyList<object> args,
      Callback callback,
      Callback ignored)
    {
      // At the moment the input args are packed into a single JSON-formatted
      // string.
      Debug.WriteLine($"SampleModule.method3({String.Join(", ", args)})");

      // Invoke the callback with some arguments. Using JSON isn't required.  We
      // could have used other basic types instead (e.g. int, double, float,
      // bool, etc.)
      string text = @"{ ""result"" : true }";
      // This uses the built-in Windows.Data.Json.JsonObject, but any
      // object whose ToString() returns a json-formatted text could be used
      // (e.g. the Newtonsoft.Json.Linq.JObject). No automatic conversion
      // is done on a string passed directly as arguments. It comes out as a
      // string on the other side and would need to be parsed (e.g.
      // JSON.parse()).  Simple... what goes in is what comes out.
      var json = Windows.Data.Json.JsonObject.Parse(text);
      callback(new[] { json });
    }

    protected override void Initialize()
    {
      Debug.WriteLine("Initialized");
    }
  }
}
