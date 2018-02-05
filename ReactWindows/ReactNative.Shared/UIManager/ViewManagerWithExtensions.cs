using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
#if WINDOWS_UWP
using Windows.UI.Xaml;
#else
using System.Windows;
#endif

namespace ReactNative.UIManager
{
    class ViewManagerWithExtensions : IViewManager
    {
        private readonly IViewManager _viewManager;
        private readonly IReadOnlyList<IViewManagerExtension> _extensions;
        private readonly IReadOnlyDictionary<object, IViewManagerExtension> _reverseCommandsMap;

        public ViewManagerWithExtensions(IViewManager viewManager, IReadOnlyList<IViewManagerExtension> extensions)
        {
            _viewManager = viewManager;
            _extensions = extensions;
            _reverseCommandsMap = CreateReverseCommandsMap(viewManager, extensions);
        }

        public string Name => _viewManager.Name;
   
        public Type ShadowNodeType => _viewManager.ShadowNodeType;

        public IReadOnlyDictionary<string, object> CommandsMap =>
            Merge(vm => vm.CommandsMap, vmx => vmx.CommandsMap);

        public IReadOnlyDictionary<string, object> ExportedCustomBubblingEventTypeConstants =>
            Merge(vm => vm.ExportedCustomBubblingEventTypeConstants, vmx => vmx.ExportedCustomBubblingEventTypeConstants);

        public IReadOnlyDictionary<string, object> ExportedCustomDirectEventTypeConstants =>
            Merge(vm => vm.ExportedCustomDirectEventTypeConstants, vmx => vmx.ExportedCustomDirectEventTypeConstants);

        public IReadOnlyDictionary<string, object> ExportedViewConstants =>
            Merge(vm => vm.ExportedViewConstants, vmx => vmx.ExportedViewConstants);

        public IReadOnlyDictionary<string, string> NativeProperties =>
            Merge(vm => vm.NativeProperties, vmx => vmx.NativeProperties);

        public ReactShadowNode CreateShadowNodeInstance() => _viewManager.CreateShadowNodeInstance();

        public DependencyObject CreateView(ThemedReactContext reactContext)
        {
            var view = _viewManager.CreateView(reactContext);
            foreach (var extension in _extensions)
            {
                extension.OnCreateView(reactContext, view);
            }

            return view;
        }

        public Dimensions GetDimensions(DependencyObject view) => _viewManager.GetDimensions(view);

        public void OnDropViewInstance(ThemedReactContext reactContext, DependencyObject view)
        {
            foreach (var extension in _extensions)
            {
                extension.OnDropViewInstance(reactContext, view);
            }

            _viewManager.OnDropViewInstance(reactContext, view);
        }

        public void ReceiveCommand(DependencyObject view, int commandId, JArray args)
        {
            if (_reverseCommandsMap.TryGetValue(commandId, out IViewManagerExtension extension) && extension != null)
            {
                extension.ReceiveCommand(view, commandId, args);
            }
            else
            {
                _viewManager.ReceiveCommand(view, commandId, args);
            }
        }

        public void SetDimensions(DependencyObject view, Dimensions dimensions)
        {
            _viewManager.SetDimensions(view, dimensions);
        }

        public void UpdateExtraData(DependencyObject root, object extraData)
        {
            _viewManager.UpdateExtraData(root, extraData);
        }

        public void UpdateProperties(DependencyObject viewToUpdate, ReactStylesDiffMap props)
        {
            // TODO: should we split up the ReactStylesDiffMap?
            _viewManager.UpdateProperties(viewToUpdate, props);
            foreach (var extension in _extensions)
            {
                extension.UpdateProperties(viewToUpdate, props);
            }
        }

        private IReadOnlyDictionary<string, TValue> Merge<TValue>(
            Func<IViewManager, IReadOnlyDictionary<string, TValue>> viewManagerSelector,
            Func<IViewManagerExtension, IReadOnlyDictionary<string, TValue>> viewManagerExtensionSelector)
        {
            var result = new Dictionary<string, TValue>();

            var viewManagerData = viewManagerSelector(_viewManager);
            if (viewManagerData != null)
            {
                foreach (var pair in viewManagerData)
                {
                    result.Add(pair.Key, pair.Value);
                }
            }

            foreach (var extension in _extensions)
            {
                var extensionData = viewManagerExtensionSelector(extension);
                Merge(result, extensionData);
            }

            return result.Count > 0 ? result : null;
        }

        private static void Merge<TValue>(IDictionary<string, TValue> sink, IReadOnlyDictionary<string, TValue> source)
        {
            if (sink == null || source == null || source.Count == 0)
            {
                return;
            }

            foreach (var pair in source)
            {
                if (sink.ContainsKey(pair.Key))
                {
                    throw new InvalidOperationException("View manager extension cannot override behaviors.");
                }
                else
                {
                    sink.Add(pair.Key, pair.Value);
                }
            }
        }


        private static IReadOnlyDictionary<object, IViewManagerExtension> CreateReverseCommandsMap(
            IViewManager viewManager,
            IReadOnlyList<IViewManagerExtension> extensions)
        {
            var commands = viewManager.CommandsMap;
            var reverseMap = new Dictionary<object, IViewManagerExtension>();

            if (commands != null)
            {
                // Create a null entry for each view manager command index
                foreach (var command in commands.Values)
                {
                    // Don't use `Add` here, IViewManager puts no restriction on command identifiers.
                    reverseMap[command] = null;
                }
            }

            foreach (var extension in extensions)
            {
                var extensionCommands = extension.CommandsMap;
                if (extensionCommands != null)
                {
                    foreach (var command in extensionCommands)
                    {
                        if (reverseMap.TryGetValue(command, out IViewManagerExtension unused) || /* registered by view manager */ unused == null)
                        {
                            throw new InvalidOperationException("View manager extensions must use unique command identifiers.");
                        }

                        reverseMap.Add(command, extension);
                    }
                }
            }

            return reverseMap;
        }
    }
}
