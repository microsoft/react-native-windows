// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.Bridge.Queue;
using ReactNative.Chakra.Executor;
using System;
using System.Collections.Generic;
using System.Reactive.Concurrency;
using System.Threading.Tasks;
#if WINDOWS_UWP
using Windows.Storage;
#else
using System.IO;
using System.Reflection;
#endif

namespace ReactNative.Tests
{
    static class JavaScriptHelpers
    {
        public static Task Run(Action<ChakraJavaScriptExecutor, IActionQueue> action)
        {
            return Run((executor, jsQueueThread) =>
            {
                action(executor, jsQueueThread);
                return Task.CompletedTask;
            });
        }

        public static async Task Run(Func<ChakraJavaScriptExecutor, IActionQueue, Task> action)
        {
            using (var jsQueueThread = CreateJavaScriptQueue())
            {
                var executor = await jsQueueThread.RunAsync(() => new ChakraJavaScriptExecutor());
                try
                {
                    await Initialize(executor, jsQueueThread);
                    await action(executor, jsQueueThread);
                }
                finally
                {
                    await jsQueueThread.RunAsync(() =>
                    {
                        executor.Dispose();
                        return true;
                    });
                }
            }
        }

        public static async Task Initialize(ChakraJavaScriptExecutor executor, IActionQueue jsQueueThread)
        {
            var scriptUris = new[]
            {
                @"Resources/test.js",
            };

            var scripts = new KeyValuePair<string, string>[scriptUris.Length];
            for (var i = 0; i < scriptUris.Length; ++i)
            {
                var uri = scriptUris[i];
#if WINDOWS_UWP
                var storageFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx://" + "/" + uri)); 
                var filePath = storageFile.Path; 
#else
                var assembly = Assembly.GetAssembly(typeof(JavaScriptHelpers));
                var assemblyName = assembly.GetName();
                var pathToAssembly = Path.GetDirectoryName(assemblyName.CodeBase);
                if (pathToAssembly == null) throw new FileNotFoundException($"Could not get directory name for code base of '{assemblyName}'.");
                var pathToAssemblyResource = Path.Combine(pathToAssembly, uri);

                var u = new Uri(pathToAssemblyResource);
                var filePath = u.LocalPath;
#endif

                scripts[i] = new KeyValuePair<string, string>(uri, filePath);
            }

            await jsQueueThread.RunAsync(() =>
            {
                foreach (var script in scripts)
                {
                    executor.RunScript(script.Value, script.Key);
                }

                return true;
            });
        }

        private static IActionQueue CreateJavaScriptQueue()
        {
            return new ActionQueue(ex => { Assert.Fail(); }, NewThreadScheduler.Default);
        }
    }
}
