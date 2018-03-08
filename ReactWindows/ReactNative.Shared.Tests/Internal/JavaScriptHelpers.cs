// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.Bridge;
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

        public static Task Run(Func<ChakraJavaScriptExecutor, IActionQueue, Task> action)
        {
            return Run(action, () => new ChakraJavaScriptExecutor(), @"Resources/test.js");
        }

        public static async Task Run<TJavaScriptExecutor>(Func<TJavaScriptExecutor, IActionQueue, Task> action, Func<TJavaScriptExecutor> jsFactory, string scriptPath)
            where TJavaScriptExecutor : IJavaScriptExecutor
        {
            using (var jsQueueThread = CreateJavaScriptQueue())
            {
                var executor = await jsQueueThread.RunAsync(jsFactory);
                try
                {
                    await Initialize(executor, jsQueueThread, scriptPath);
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

        public static async Task Initialize(IJavaScriptExecutor executor, IActionQueue jsQueueThread, string scriptPath)
        {
#if WINDOWS_UWP
            var storageFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri("ms-appx://" + "/" + scriptPath)); 
            var filePath = storageFile.Path;
#else
            var assembly = Assembly.GetAssembly(typeof(JavaScriptHelpers));
            var assemblyName = assembly.GetName();
            var pathToAssembly = Path.GetDirectoryName(assemblyName.CodeBase);
            if (pathToAssembly == null) throw new FileNotFoundException($"Could not get directory name for code base of '{assemblyName}'.");
            var pathToAssemblyResource = Path.Combine(pathToAssembly, scriptPath);

            var u = new Uri(pathToAssemblyResource);
            var filePath = u.LocalPath;
#endif

            await jsQueueThread.RunAsync(() =>
            {
                executor.RunScript(filePath, filePath);
            });
        }

        private static IActionQueue CreateJavaScriptQueue()
        {
            return new ActionQueue(ex => { Assert.Fail(); }, NewThreadScheduler.Default);
        }
    }
}
