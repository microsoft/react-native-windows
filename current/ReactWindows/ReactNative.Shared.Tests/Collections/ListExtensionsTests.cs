// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NMock;
using NUnit.Framework;
using ReactNative.Collections;
using System;
using System.Collections.Generic;

namespace ReactNative.Tests.Collections
{
    [TestFixture]
    public class ListExtensionsTests
    {
        [Test]
        public void ListExtensions_LocalMin()
        {
            var list = new List<int>{ 1, 3, 7, 12, 18 };
            Assert.IsTrue(list.LocalMin(v => Math.Abs(v)) == 1);
            Assert.IsTrue(list.LocalMin(v => Math.Abs(v - 19)) == 18);
            Assert.IsTrue(list.LocalMin(v => Math.Abs(v - 4)) == 3);
            Assert.IsTrue(list.LocalMin(v => Math.Abs(v - 6)) == 7);
            Assert.IsTrue(list.LocalMin(v => Math.Abs(v - 13)) == 12);
            Assert.IsTrue(list.LocalMin(v => Math.Abs(v - 17)) == 18);
            Assert.IsTrue(list.LocalMin(v => Math.Abs(v - 7)) == 7);
        }
    }
}
