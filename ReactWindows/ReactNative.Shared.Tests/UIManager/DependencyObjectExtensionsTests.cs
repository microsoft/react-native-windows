// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using NUnit.Framework;
using ReactNative.UIManager;
using System;
#if WINDOWS_UWP
using Windows.UI.Xaml.Controls;
#else
using System.Threading;
using System.Windows.Controls;
#endif

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
#if !WINDOWS_UWP
    [Apartment(ApartmentState.STA)]
#endif
    public class DependencyObjectExtensionsTests
    {
        [Test]
        public void FrameworkElementExtensions_ArgumentChecks()
        {
            var element = new Button();

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.SetTag(null, 0),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.SetReactContext(null, null),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.GetTag(null),
                ex => Assert.AreEqual("view", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => DependencyObjectExtensions.GetReactContext(null),
                ex => Assert.AreEqual("view", ex.ParamName));
        }

        [Test]
        public void FrameworkElementExtensions_Get_Set()
        {
            var button = new Button();

            button.SetTag(42);
            Assert.AreEqual(42, button.GetTag());

            button.SetReactContext(null);
            Assert.IsNull(button.GetReactContext());
        }

    }
}
