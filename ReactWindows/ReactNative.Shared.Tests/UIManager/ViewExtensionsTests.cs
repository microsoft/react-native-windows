// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.UIManager;
using System;

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    public class ViewExtensionsTests
    {
        [Test]
        public void ViewExtensions_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => ViewExtensions.SetTag(null, 0),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewExtensions.SetReactContext(null, null),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewExtensions.GetTag(null),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => ViewExtensions.GetReactContext(null),
                ex => Assert.AreEqual("view", ex.ParamName));
        }

        [Test]
        public void ViewExtensions_Get_Set()
        {
            var view = new object();

            ViewExtensions.SetTag(view, 42);
            Assert.AreEqual(42, view.GetTag());

            ViewExtensions.SetReactContext(view, null);
            Assert.IsNull(view.GetReactContext());
        }
    }
}
