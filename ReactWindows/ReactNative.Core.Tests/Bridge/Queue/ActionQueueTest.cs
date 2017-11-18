using System;
using NUnit.Framework;
using ReactNative.Bridge.Queue;

namespace ReactNative.Core.Tests
{
    public class ActionQueueTest
    {
        [Test]
        public void ThrowsWithNullAction()
        {
            Assert.Throws<ArgumentNullException>(() => new ActionQueue(null));
        }
    }
}
