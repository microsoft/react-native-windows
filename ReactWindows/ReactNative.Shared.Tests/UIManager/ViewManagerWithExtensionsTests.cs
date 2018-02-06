using Newtonsoft.Json.Linq;
using NUnit.Framework;
using ReactNative.UIManager;
using System;
using System.Collections.Generic;
using System.Linq;
#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.Tests.UIManager
{
    [TestFixture]
    class ViewManagerWithExtensionsTests
    {
        [Test]
        public void ViewManagerWithExtensions_ArgumentChecks()
        {
            AssertEx.Throws<ArgumentNullException>(
                () => new ViewManagerWithExtensions(null, null),
                ex => Assert.AreEqual("viewManager", ex.ParamName));

            AssertEx.Throws<ArgumentNullException>(
                () => new ViewManagerWithExtensions(new TestViewManager(), null),
                ex => Assert.AreEqual("extensions", ex.ParamName));
        }

        [Test]
        public void ViewManagerWithExtensions_CreateView()
        {
            var vm = new TestViewManager();
            var vmx = new TestViewManagerExtension();
            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var count = 0;
            var xcount = 0;
            vm.CreateViewFunc = () =>
            {
                count++;
                return null;
            };

            vmx.OnCreateViewAction = () => xcount++;

            vmplusx.CreateView(null);
            Assert.AreEqual(1, count);
            Assert.AreEqual(1, xcount);
        }

        [Test]
        public void ViewManagerWithExtensions_DropView()
        {
            var vm = new TestViewManager();
            var vmx = new TestViewManagerExtension();
            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var count = 0;
            var xcount = 0;
            vm.OnDropViewInstanceAction = () => count++;
            vmx.OnDropViewInstanceAction = () => xcount++;
            vmplusx.OnDropViewInstance(null, null);
            Assert.AreEqual(1, count);
            Assert.AreEqual(1, xcount);
        }

        [Test]
        public void ViewManagerWithExtensions_UpdateProperties()
        {
            var vm = new TestViewManager();
            var vmx = new TestViewManagerExtension();
            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var count = 0;
            var xcount = 0;
            var diffMap = new ReactStylesDiffMap(new JObject());

            vm.UpdatePropertiesAction = x =>
            {
                Assert.AreSame(diffMap, x);
                count++;
            };

            vmx.UpdatePropertiesAction = x =>
            {
                Assert.AreSame(diffMap, x);
                xcount++;
            };

            vmplusx.UpdateProperties(null, diffMap);

            Assert.AreEqual(1, count);
            Assert.AreEqual(1, xcount);
        }

        [Test]
        public void ViewManagerWithExtensions_ReceiveCommand()
        {
            var vm = new TestViewManager
            {
                CommandsMap = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                CommandsMap = TestMapDifferent,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var args = new JArray();
            var count = 0;
            var xcount = 0;

            var id = TestMap.Values.OfType<int>().First();
            var xid = TestMapDifferent.Values.OfType<int>().First();

            vm.ReceiveCommandAction = (i, a) =>
            {
                Assert.AreSame(args, a);
                Assert.AreEqual(id, i);
                count++;
            };

            vmx.ReceiveCommandAction = (i, a) =>
            {
                Assert.AreSame(args, a);
                Assert.AreEqual(xid, i);
                xcount++;
            };

            vmplusx.ReceiveCommand(null, id, args);
            vmplusx.ReceiveCommand(null, xid, args);

            Assert.AreEqual(1, count);
            Assert.AreEqual(1, xcount);
        }

        [Test]
        public void ViewManagerWithExtensions_ViewManagerPassThrough()
        {
            var vm = new TestViewManager();
            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension>(0));

            // Name

            Assert.AreEqual(vm.Name, vmplusx.Name);

            // ShadowNodeType

            Assert.AreEqual(vm.ShadowNodeType, vmplusx.ShadowNodeType);

            // CreateShadowNodeInstance

            var count = 0;
            var shadowNode = new ReactShadowNode();
            vm.CreateShadowNodeInstanceFunc = () =>
            {
                count++;
                return shadowNode;
            };

            object result = vmplusx.CreateShadowNodeInstance();
            Assert.AreSame(shadowNode, result);
            Assert.AreEqual(1, count);

            // GetDimensions

            var dimensions = new Dimensions
            {
                X = 42,
            };

            count = 0;
            vm.GetDimensionsFunc = () =>
            {
                count++;
                return dimensions;
            };

            result = vmplusx.GetDimensions(null);
            Assert.AreEqual(dimensions, result);
            Assert.AreEqual(1, count);

            // SetDimensions

            count = 0;
            vm.SetDimensionsAction = d =>
            {
                Assert.AreEqual(dimensions, d);
                count++;
            };

            vmplusx.SetDimensions(null, dimensions);
            Assert.AreEqual(1, count);

            // UpdateExtraData

            var data = new object();
            count = 0;
            vm.UpdateExtraDataAction = x =>
            {
                Assert.AreSame(data, x);
                count++;
            };

            vmplusx.UpdateExtraData(null, data);
            Assert.AreEqual(1, count);
        }

        [Test]
        public void ViewManagerWithExtensions_MergeDirectEvents()
        {
            var vm = new TestViewManager
            {
                ExportedCustomDirectEventTypeConstants = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                ExportedCustomDirectEventTypeConstants = TestMapDifferent,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var result = vmplusx.ExportedCustomDirectEventTypeConstants;

            foreach (var pair in TestMap)
            {
                Assert.AreEqual(TestMap[pair.Key], result[pair.Key]);
            }

            foreach (var pair in TestMapDifferent)
            {
                Assert.AreEqual(TestMapDifferent[pair.Key], result[pair.Key]);
            }
        }

        [Test]
        public void ViewManagerWithExtensions_MergeBubblingEvents()
        {
            var vm = new TestViewManager
            {
                ExportedCustomBubblingEventTypeConstants = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                ExportedCustomBubblingEventTypeConstants = TestMapDifferent,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var result = vmplusx.ExportedCustomBubblingEventTypeConstants;

            foreach (var pair in TestMap)
            {
                Assert.AreEqual(TestMap[pair.Key], result[pair.Key]);
            }

            foreach (var pair in TestMapDifferent)
            {
                Assert.AreEqual(TestMapDifferent[pair.Key], result[pair.Key]);
            }
        }

        [Test]
        public void ViewManagerWithExtensions_MergeConstants()
        {
            var vm = new TestViewManager
            {
                ExportedViewConstants = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                ExportedViewConstants = TestMapDifferent,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var result = vmplusx.ExportedViewConstants;

            foreach (var pair in TestMap)
            {
                Assert.AreEqual(TestMap[pair.Key], result[pair.Key]);
            }

            foreach (var pair in TestMapDifferent)
            {
                Assert.AreEqual(TestMapDifferent[pair.Key], result[pair.Key]);
            }
        }

        [Test]
        public void ViewManagerWithExtensions_MergeCommands()
        {
            var vm = new TestViewManager
            {
                CommandsMap = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                CommandsMap = TestMapDifferent,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var result = vmplusx.CommandsMap;

            foreach (var pair in TestMap)
            {
                Assert.AreEqual(TestMap[pair.Key], result[pair.Key]);
            }

            foreach (var pair in TestMapDifferent)
            {
                Assert.AreEqual(TestMapDifferent[pair.Key], result[pair.Key]);
            }
        }

        [Test]
        public void ViewManagerWithExtensions_MergeProps()
        {
            var vm = new TestViewManager
            {
                NativeProperties = TestMapString,
            };

            var vmx = new TestViewManagerExtension
            {
                NativeProperties = TestMapDifferentString,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            var result = vmplusx.NativeProperties;

            foreach (var pair in TestMapString)
            {
                Assert.AreEqual(TestMapString[pair.Key], result[pair.Key]);
            }

            foreach (var pair in TestMapDifferentString)
            {
                Assert.AreEqual(TestMapDifferentString[pair.Key], result[pair.Key]);
            }
        }

        [Test]
        public void ViewManagerWithExtensions_MergeDirectEvents_NoOverride()
        {
            var vm = new TestViewManager
            {
                ExportedCustomDirectEventTypeConstants = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                ExportedCustomDirectEventTypeConstants = TestMapSameKey,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            Assert.Throws<InvalidOperationException>(() =>
            {
                var x = vmplusx.ExportedCustomDirectEventTypeConstants;
            });
        }

        [Test]
        public void ViewManagerWithExtensions_MergeBubblingEvents_NoOverride()
        {
            var vm = new TestViewManager
            {
                ExportedCustomBubblingEventTypeConstants = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                ExportedCustomBubblingEventTypeConstants = TestMapSameKey,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            Assert.Throws<InvalidOperationException>(() =>
            {
                var x = vmplusx.ExportedCustomBubblingEventTypeConstants;
            });
        }

        [Test]
        public void ViewManagerWithExtensions_MergeCommands_NoOveride()
        {
            var vm = new TestViewManager
            {
                CommandsMap = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                CommandsMap = TestMapSameKey,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            Assert.Throws<InvalidOperationException>(() =>
            {
                var x = vmplusx.CommandsMap;
            });
        }

        [Test]
        public void ViewManagerWithExtensions_MergeConstants_NoOveride()
        {
            var vm = new TestViewManager
            {
                ExportedViewConstants = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                ExportedViewConstants = TestMapSameKey,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            Assert.Throws<InvalidOperationException>(() =>
            {
                var x = vmplusx.ExportedViewConstants;
            });
        }

        [Test]
        public void ViewManagerWithExtensions_MergeProps_NoOverride()
        {
            var vm = new TestViewManager
            {
                NativeProperties = TestMapString,
            };

            var vmx = new TestViewManagerExtension
            {
                NativeProperties = TestMapSameKeyString,
            };

            var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });

            Assert.Throws<InvalidOperationException>(() =>
            {
                var x = vmplusx.NativeProperties;
            });
        }

        [Test]
        public void ViewManagerWithExtensions_MergeCommands_UniqueValues()
        {
            var vm = new TestViewManager
            {
                CommandsMap = TestMap,
            };

            var vmx = new TestViewManagerExtension
            {
                CommandsMap = TestMapSameValue,
            };

            Assert.Throws<InvalidOperationException>(() =>
            {
                var vmplusx = new ViewManagerWithExtensions(vm, new List<IViewManagerExtension> { vmx });
            });
        }

        #region Test Classes

        class TestViewManager : IViewManager
        {
            public string Name => "Test";

            public Type ShadowNodeType => typeof(ReactShadowNode);

            public IReadOnlyDictionary<string, object> CommandsMap { get; set; }

            public IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants { get; set; }

            public IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants { get; set; }

            public IReadOnlyDictionary<string, object> ExportedViewConstants { get; set; }

            public IReadOnlyDictionary<string, string> NativeProperties { get; set; }

            public Func<ReactShadowNode> CreateShadowNodeInstanceFunc { get; set; }

            public ReactShadowNode CreateShadowNodeInstance() => CreateShadowNodeInstanceFunc?.Invoke();

            public Func<DependencyObject> CreateViewFunc { get; set; }

            public DependencyObject CreateView(ThemedReactContext reactContext) => CreateViewFunc?.Invoke();

            public Action OnDropViewInstanceAction { get; set; }

            public void OnDropViewInstance(ThemedReactContext reactContext, DependencyObject view) => OnDropViewInstanceAction?.Invoke();

            public Action<int, JArray> ReceiveCommandAction { get; set; }

            public void ReceiveCommand(DependencyObject view, int commandId, JArray args) => ReceiveCommandAction?.Invoke(commandId, args);

            public Action<ReactStylesDiffMap> UpdatePropertiesAction { get; set; }

            public void UpdateProperties(DependencyObject viewToUpdate, ReactStylesDiffMap props) => UpdatePropertiesAction?.Invoke(props);

            public Action<object> UpdateExtraDataAction { get; set; }

            public void UpdateExtraData(DependencyObject root, object extraData) => UpdateExtraDataAction?.Invoke(extraData);

            public Func<Dimensions> GetDimensionsFunc { get; set; }

            public Dimensions GetDimensions(DependencyObject view) => GetDimensionsFunc?.Invoke() ?? default(Dimensions);

            public Action<Dimensions> SetDimensionsAction { get; set; }

            public void SetDimensions(DependencyObject view, Dimensions dimensions) => SetDimensionsAction?.Invoke(dimensions);
        }

        class TestViewManagerExtension : IViewManagerExtension
        {
            public Type ViewManagerType => typeof(TestViewManager);

            public IReadOnlyDictionary<string, object> CommandsMap { get; set; }

            public IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants { get; set; }

            public IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants { get; set; }

            public IReadOnlyDictionary<string, object> ExportedViewConstants { get; set; }

            public IReadOnlyDictionary<string, string> NativeProperties { get; set; }

            public Action OnCreateViewAction { get; set; }

            public void OnCreateView(ThemedReactContext reactContext, DependencyObject view) => OnCreateViewAction?.Invoke();

            public Action OnDropViewInstanceAction { get; set; }

            public void OnDropViewInstance(ThemedReactContext reactContext, DependencyObject view) => OnDropViewInstanceAction?.Invoke();

            public Action<int, JArray> ReceiveCommandAction { get; set; }

            public void ReceiveCommand(DependencyObject view, int commandId, JArray args) => ReceiveCommandAction?.Invoke(commandId, args);

            public Action<ReactStylesDiffMap> UpdatePropertiesAction { get; set; }

            public void UpdateProperties(DependencyObject viewToUpdate, ReactStylesDiffMap props) => UpdatePropertiesAction?.Invoke(props);
        }

        #endregion

        #region Test Constants

        private static readonly Dictionary<string, object> TestMap =
            new Dictionary<string, object>
            {
                { "foo", 42 },
            };

        private static readonly Dictionary<string, string> TestMapString =
            TestMap.ToDictionary(p => p.Key, p => p.Value.ToString());

        private static readonly Dictionary<string, object> TestMapSameKey =
            new Dictionary<string, object>
            {
                { "foo", 43 },
            };

        private static readonly Dictionary<string, string> TestMapSameKeyString =
            TestMapSameKey.ToDictionary(p => p.Key, p => p.Value.ToString());

        private static readonly Dictionary<string, object> TestMapSameValue =
            new Dictionary<string, object>
            {
                { "bar", 42 },
            };

        private static readonly Dictionary<string, object> TestMapDifferent =
            new Dictionary<string, object>
            {
                { "bar", 43 },
            };

        private static readonly Dictionary<string, string> TestMapDifferentString =
            TestMapDifferent.ToDictionary(p => p.Key, p => p.Value.ToString());

        #endregion
    }
}
