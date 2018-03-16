// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestPlatform.UnitTestFramework;
using System.Threading.Tasks;

namespace ReactNative.Tests.UIManager.Events
{
    [TestClass]
    public class EventTests
    {
        [TestMethod]
        public void Event_Initialize_Dispose()
        {
            var e = new MockEvent(42, "Test");

            Assert.IsTrue(e.CanCoalesce);
            Assert.IsTrue(e.IsInitialized);

            Assert.AreEqual(42, e.ViewTag);
            Assert.AreEqual(0, e.CoalescingKey);
            Assert.IsTrue(e.CanCoalesce);

            e.Dispose();
            Assert.IsFalse(e.IsInitialized);
        }

        [TestMethod]
        public async Task Event_Timestamp()
        {
            var e = new MockEvent(42, "foo");
            await Task.Delay(5);
            var b = new MockEvent(42, "bar");
            Assert.IsTrue(e.Timestamp < b.Timestamp);
        }
    }
}
