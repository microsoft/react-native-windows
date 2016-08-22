using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using Newtonsoft.Json.Linq;
using System;
using System.Threading.Tasks;

namespace ReactNative.Tests.Chakra.Executor
{
    [TestClass]
    public class ChakraJavaScriptExecutorTests
    {
        [TestMethod]
        public async Task ChakraJavaScriptExecutor_ArgumentChecks()
        {
            await JavaScriptHelpers.Run((executor, jsQueueThread) =>
            {
                AssertEx.Throws<ArgumentNullException>(
                    () => executor.CallFunctionReturnFlushedQueue(null, "foo", new JArray()),
                    ex => Assert.AreEqual("moduleName", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.CallFunctionReturnFlushedQueue("foo", null, new JArray()),
                    ex => Assert.AreEqual("methodName", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.CallFunctionReturnFlushedQueue("foo", "bar", null),
                    ex => Assert.AreEqual("arguments", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.InvokeCallbackAndReturnFlushedQueue(0, null),
                    ex => Assert.AreEqual("arguments", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.RunScript(null, "foo"),
                    ex => Assert.AreEqual("script", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.RunScript("", null),
                    ex => Assert.AreEqual("sourceUrl", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.SetGlobalVariable(null, new JArray()),
                    ex => Assert.AreEqual("propertyName", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.SetGlobalVariable("foo", null),
                    ex => Assert.AreEqual("value", ex.ParamName));

                AssertEx.Throws<ArgumentNullException>(
                    () => executor.GetGlobalVariable(null),
                    ex => Assert.AreEqual("propertyName", ex.ParamName));
            });
        }
    }
}
