using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.Bridge.Queue;
using ReactNative.Chakra.Executor;
using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;
using Windows.Storage;

namespace ReactNative.Tests
{
    static class JavaScriptHelpers
    {
        public static Task Run(Action<ChakraJavaScriptExecutor, IMessageQueueThread> action)
        {
            return Run((executor, jsQueueThread) =>
            {
                action(executor, jsQueueThread);
                return Task.CompletedTask;
            });
        }

        public static async Task Run(Func<ChakraJavaScriptExecutor, IMessageQueueThread, Task> action)
        {
            using (var jsQueueThread = CreateJavaScriptThread())
            {
                var executor = await jsQueueThread.CallOnQueue(() => new ChakraJavaScriptExecutor());
                try
                {
                    await Initialize(executor, jsQueueThread);
                    await action(executor, jsQueueThread);
                }
                finally
                {
                    await jsQueueThread.CallOnQueue(() =>
                    {
                        executor.Dispose();
                        return true;
                    });
                }
            }
        }

        public static async Task Initialize(ChakraJavaScriptExecutor executor, IMessageQueueThread jsQueueThread)
        {
            var scriptUris = new[]
            {
                @"ms-appx:///Resources/test.js",
            };

            var scripts = new KeyValuePair<string, string>[scriptUris.Length];

            for (var i = 0; i < scriptUris.Length; ++i)
            {
                var uri = scriptUris[i];
                var storageFile = await StorageFile.GetFileFromApplicationUriAsync(new Uri(uri));
                using (var stream = await storageFile.OpenStreamForReadAsync())
                using (var reader = new StreamReader(stream))
                {
                    scripts[i] = new KeyValuePair<string, string>(uri, reader.ReadToEnd());
                }
            }

            await jsQueueThread.CallOnQueue(() =>
            {
                foreach (var script in scripts)
                {
                    executor.RunScript(script.Value, script.Key);
                }

                return true;
            });
        }

        private static MessageQueueThread CreateJavaScriptThread()
        {
            return MessageQueueThread.Create(MessageQueueThreadSpec.Create("js", MessageQueueThreadKind.BackgroundSingleThread), ex => { Assert.Fail(); });
        }
    }
}
