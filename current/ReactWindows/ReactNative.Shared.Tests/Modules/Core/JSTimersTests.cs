// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.Modules.Core;

namespace ReactNative.Tests.Modules.Core
{
    [TestFixture]
    public class JSTimersTests
    {
        public void JSTimers_callTimers_Invoke()
        {
            var module = new JSTimers();

            var name = default(string);
            var args = default(object[]);
            module.InvocationHandler = new MockInvocationHandler((n, a) =>
            {
                name = n;
                args = a;
            });

            var ids = new[] { 42 };
            module.callTimers(ids);
            Assert.AreEqual(nameof(JSTimers.callTimers), name);
            Assert.AreEqual(1, args.Length);
            Assert.AreSame(ids, args[0]);
        }

        public void JSTimers_callIdleCallbacks_Invoke()
        {
            var module = new JSTimers();

            var name = default(string);
            var args = default(object[]);
            module.InvocationHandler = new MockInvocationHandler((n, a) =>
            {
                name = n;
                args = a;
            });

            var frameTime = 42L;
            module.callIdleCallbacks(frameTime);
            Assert.AreEqual(nameof(JSTimers.callIdleCallbacks), name);
            Assert.AreEqual(1, args.Length);
            Assert.AreSame(frameTime, args[0]);
        }
    }
}
