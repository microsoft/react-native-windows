// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace Microsoft.ReactNative.Managed.CodeGen.UnitTests.Analysis
{
  [TestClass]
  public class ViewManagerAnalysisTests : AnalysisTestBase
  {

    [TestMethod]
    public void DirectInherit()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        using Microsoft.ReactNative;
        using Windows.UI.Xaml;

        public class TestClass : IViewManager
        {
          public string Name => ""Name"";
          public FrameworkElement CreateView()
          {
            return null;
          }
        }");

      Assert.IsTrue(codeAnalyzer.IsViewManager(type));
    }

    [TestMethod]
    public void InsideNiestedClass()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        using Microsoft.ReactNative;
        using Windows.UI.Xaml;

        public class OuterClass {
          public class TestClass : IViewManager
          {
            public string Name => ""Name"";
            public FrameworkElement CreateView()
            {
              return null;
            }
          }
        }");

      Assert.IsTrue(codeAnalyzer.IsViewManager(type));
    }

    [TestMethod]
    public void MultiLevelInterfaceInherit()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        using Microsoft.ReactNative;
        using Windows.UI.Xaml;

        public interface IIntermediate : IViewManager
        {
        }

        public class TestClass : IIntermediate
        {
          public string Name => ""Name"";
          public FrameworkElement CreateView()
          {
            return null;
          }
        }");

      Assert.IsTrue(codeAnalyzer.IsViewManager(type));
    }

    [TestMethod]
    public void AbstractIsNot()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        using Microsoft.ReactNative;
        using Windows.UI.Xaml;

        public abstract class TestClass : IViewManager
        {
          public string Name => ""Name"";
          public FrameworkElement CreateView()
          {
            return null;
          }
        }");

      Assert.IsFalse(codeAnalyzer.IsViewManager(type));
    }

    [TestMethod]
    public void InherittedAbstractIs()
    {
      var (codeAnalyzer, type) = AnalyzeFile(@"
        using Microsoft.ReactNative;
        using Windows.UI.Xaml;

        public abstract class TestBase : IViewManager
        {
          public string Name => ""Name"";
          public FrameworkElement CreateView()
          {
            return null;
          }
        }
        public class TestClass : TestBase
        {
        }
        ");

      Assert.IsTrue(codeAnalyzer.IsViewManager(type));
    }
  }
}
