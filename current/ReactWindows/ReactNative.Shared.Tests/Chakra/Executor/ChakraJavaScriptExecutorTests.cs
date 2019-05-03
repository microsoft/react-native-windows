// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Newtonsoft.Json.Linq;
using NUnit.Framework;
using System.Threading.Tasks;

namespace ReactNative.Tests.Chakra.Executor
{
    [TestFixture]
    public class ChakraJavaScriptExecutorTests
    {
        [Test]
        public async Task ThrowsWithCorrectParameterNameWhenGivenNull()
        {
            await JavaScriptHelpers.Run((executor, jsQueueThread) =>
            {
                Assert.That(
                    () => executor.CallFunctionReturnFlushedQueue(null, "foo", new JArray()),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("moduleName")
                );

                Assert.That(
                    () => executor.CallFunctionReturnFlushedQueue("foo", null, new JArray()),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("methodName")
                );

                Assert.That(
                    () => executor.CallFunctionReturnFlushedQueue("foo", "bar", null),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("arguments")
                );

                Assert.That(
                    () => executor.InvokeCallbackAndReturnFlushedQueue(0, null),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("arguments")
                );

                Assert.That(
                    () => executor.RunScript(null, "foo"),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("sourcePath")
                );

                Assert.That(
                    () => executor.RunScript("", null),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("sourceUrl")
                );

                Assert.That(
                    () => executor.SetGlobalVariable(null, "[]"),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("propertyName")
                );

                Assert.That(
                    () => executor.SetGlobalVariable("foo", null),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("value")
                );

                Assert.That(
                    () => executor.GetGlobalVariable(null),
                    Throws.ArgumentNullException.With.Property("ParamName").EqualTo("propertyName")
                );
            });
        }
    }
}
