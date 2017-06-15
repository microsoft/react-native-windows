using Newtonsoft.Json.Linq;
using ReactNative.Bridge;
using ReactNative.UIManager;
using System.Collections.Generic;

namespace ReactNative.Modules.Wrappers
{
    class UIManagerModuleWrapper : NativeModuleWrapperBase<UIManagerModule>, ILifecycleEventListener, IOnBatchCompleteListener
    {
        private readonly UIManagerModule _module;

        public UIManagerModuleWrapper(UIManagerModule module)
            : base(module)
        {
            _module = module;
        }

        public override IReadOnlyDictionary<string, INativeMethod> Methods
        {
            get
            {
                return new Dictionary<string, INativeMethod>
                {
                    { nameof(_module.clearJSResponder), CreateAsyncMethod((_, __) => _module.clearJSResponder()) },
                    { nameof(_module.configureNextLayoutAnimation), CreateAsyncMethod(ConfigureNextAnimationLayout) },
                    { nameof(_module.createView), CreateAsyncMethod(CreateView) },
                    { nameof(_module.dispatchViewManagerCommand), CreateAsyncMethod(DispatchViewManagerCommand) },
                    { nameof(_module.findSubviewIn), CreateAsyncMethod(FindSubviewIn) },
                    { nameof(_module.manageChildren), CreateAsyncMethod(ManageChildren) },
                    { nameof(_module.measure), CreateAsyncMethod(Measure) },
                    { nameof(_module.measureInWindow), CreateAsyncMethod(MeasureInWindow) },
                    { nameof(_module.measureLayout), CreateAsyncMethod(MeasureLayout) },
                    { nameof(_module.measureLayoutRelativeToParent), CreateAsyncMethod(MeasureLayoutRelativeToParent) },
                    { nameof(_module.removeRootView), CreateAsyncMethod(RemoveRootView) },
                    { nameof(_module.removeSubviewsFromContainerWithID), CreateAsyncMethod(RemoveSubviewsFromContainerWithID) },
                    { nameof(_module.replaceExistingNonRootView), CreateAsyncMethod(ReplaceExistingNonRootView) },
                    { nameof(_module.setChildren), CreateAsyncMethod(SetChildren) },
                    { nameof(_module.setJSResponder), CreateAsyncMethod(SetJSResponder) },
                    { nameof(_module.showPopupMenu), CreateAsyncMethod(ShowPopupMenu) },
                    { nameof(_module.updateView), CreateAsyncMethod(UpdateView) },
                };
            }
        }

        #region ILifecycleEventListener

        public void OnSuspend()
        {
            _module.OnSuspend();
        }

        public void OnResume()
        {
            _module.OnResume();
        }

        public void OnDestroy()
        {
            _module.OnDestroy();
        }

        #endregion

        #region IOnBatchCompleteListener

        public void OnBatchComplete()
        {
            _module.OnBatchComplete();
        }

        #endregion

        #region Proxied Methods

        private void ConfigureNextAnimationLayout(IReactInstance instance, JArray arguments)
        {
            _module.configureNextLayoutAnimation((JObject)arguments[0], new Callback(arguments[1].Value<int>(), instance), new Callback(arguments[2].Value<int>(), instance));
        }

        private void CreateView(IReactInstance instance, JArray arguments)
        {
            _module.createView(arguments[0].Value<int>(), arguments[1].Value<string>(), arguments[2].Value<int>(), (JObject)arguments[3]);
        }

        private void DispatchViewManagerCommand(IReactInstance instance, JArray arguments)
        {
            _module.dispatchViewManagerCommand(arguments[0].Value<int>(), arguments[1].Value<int>(), (JArray)arguments[2]);
        }

        private void FindSubviewIn(IReactInstance instance, JArray arguments)
        {
            _module.findSubviewIn(arguments[0].Value<int>(), (JArray)arguments[1], new Callback(arguments[2].Value<int>(), instance));
        }

        private void ManageChildren(IReactInstance instance, JArray arguments)
        {
            _module.manageChildren(
                arguments[0].Value<int>(), 
                ToValueArray<int>((JArray) arguments[1]),
                ToValueArray<int>((JArray) arguments[2]), 
                ToValueArray<int>((JArray) arguments[3]),
                ToValueArray<int>((JArray) arguments[4]), 
                ToValueArray<int>((JArray) arguments[5]));
        }

        private void Measure(IReactInstance instance, JArray arguments)
        {
            _module.measure(arguments[0].Value<int>(), new Callback(arguments[1].Value<int>(), instance));
        }

        private void MeasureInWindow(IReactInstance instance, JArray arguments)
        {
            _module.measureInWindow(arguments[0].Value<int>(), new Callback(arguments[1].Value<int>(), instance));
        }

        private void MeasureLayout(IReactInstance instance, JArray arguments)
        {
            _module.measureLayout(
                arguments[0].Value<int>(),
                arguments[1].Value<int>(), 
                new Callback(arguments[2].Value<int>(), instance),
                new Callback(arguments[3].Value<int>(), instance));
        }

        private void MeasureLayoutRelativeToParent(IReactInstance instance, JArray arguments)
        {
            _module.measureLayoutRelativeToParent(
                arguments[0].Value<int>(),
                new Callback(arguments[1].Value<int>(), instance),
                new Callback(arguments[2].Value<int>(), instance));
        }

        private void RemoveRootView(IReactInstance instance, JArray arguments)
        {
            _module.removeRootView(arguments[0].Value<int>());
        }

        private void RemoveSubviewsFromContainerWithID(IReactInstance instance, JArray arguments)
        {
            _module.removeSubviewsFromContainerWithID(arguments[0].Value<int>());
        }

        private void ReplaceExistingNonRootView(IReactInstance instance, JArray arguments)
        {
            _module.replaceExistingNonRootView(arguments[0].Value<int>(), arguments[1].Value<int>());
        }

        private void SetChildren(IReactInstance instance, JArray arguments)
        {
            _module.setChildren(arguments[0].Value<int>(), ToValueArray<int>((JArray)arguments[1]));
        }

        private void SetJSResponder(IReactInstance instance, JArray arguments)
        {
            _module.setJSResponder(arguments[0].Value<int>(), arguments[1].Value<bool>());
        }

        private void ShowPopupMenu(IReactInstance instance, JArray arguments)
        {
            _module.showPopupMenu(
                arguments[0].Value<int>(), 
                ToValueArray<string>((JArray)arguments[1]),
                new Callback(arguments[2].Value<int>(), instance),
                new Callback(arguments[3].Value<int>(), instance));
        }

        private void UpdateView(IReactInstance instance, JArray arguments)
        {
            _module.updateView(
                arguments[0].Value<int>(),
                arguments[1].Value<string>(),
                (JObject)arguments[2]);
        }

        #endregion
    }
}
