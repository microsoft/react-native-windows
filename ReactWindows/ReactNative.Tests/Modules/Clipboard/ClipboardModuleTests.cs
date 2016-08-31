using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using ReactNative.Modules.Clipboard;
using System;
using System.Threading;

namespace ReactNative.Tests.Modules.Clipboard
{
    [TestClass]
    public class ClipboardModuleTests
    {
        [TestMethod]
        public void ClipboardModule_ArgumentChecks()
        {
            var clipboard = new MockClipboardInstance();
            var module = new ClipboardModule(clipboard);

            AssertEx.Throws<ArgumentNullException>(
                () => module.getString(null),
                ex => Assert.AreEqual("promise", ex.ParamName));
        }

        [TestMethod]
        public void ClipboardModule_GetString_Method()
        {
            var clipboard = new MockClipboardInstance();
            var module = new ClipboardModule(clipboard);

            var result = "";
            var str = "test string";
            var waitHandle = new AutoResetEvent(false);

            var promise = new MockPromise(resolve => { result = resolve.ToString(); waitHandle.Set(); },
                                          (code, message, e) => { result = message; waitHandle.Set(); });

            module.setString(str);
            module.getString(promise);

            waitHandle.WaitOne();
            Assert.AreEqual(str, result);
        }

        [TestMethod]
        public void ClipboardModule_SetString_Null_Method()
        {
            var clipboard = new MockClipboardInstance();
            var module = new ClipboardModule(clipboard);

            var result = "";
            var waitHandle = new AutoResetEvent(false);

            var promise = new MockPromise(resolve => { result = resolve.ToString(); waitHandle.Set(); },
                                          (code, message, e) => { result = message; waitHandle.Set(); });

            module.setString(null);
            module.getString(promise);

            waitHandle.WaitOne();
            Assert.AreEqual("", result);
        }
    }
}

