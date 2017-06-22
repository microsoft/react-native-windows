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
            _module.configureNextLayoutAnimation((JObject)arguments[0], new Callback(ToInt32(arguments[1]), instance), new Callback(ToInt32(arguments[2]), instance));
        }

        private void CreateView(IReactInstance instance, JArray arguments)
        {
            _module.createView(ToInt32(arguments[0]), ToString(arguments[1]), ToInt32(arguments[2]), (JObject)arguments[3]);
        }

        private void DispatchViewManagerCommand(IReactInstance instance, JArray arguments)
        {
            _module.dispatchViewManagerCommand(ToInt32(arguments[0]), ToInt32(arguments[1]), (JArray)arguments[2]);
        }

        private void FindSubviewIn(IReactInstance instance, JArray arguments)
        {
            _module.findSubviewIn(ToInt32(arguments[0]), (JArray)arguments[1], new Callback(ToInt32(arguments[2]), instance));
        }

        private void ManageChildren(IReactInstance instance, JArray arguments)
        {
            _module.manageChildren(
                ToInt32(arguments[0]), 
                ToValueArray(arguments[1], ToInt32),
                ToValueArray(arguments[2], ToInt32), 
                ToValueArray(arguments[3], ToInt32),
                ToValueArray(arguments[4], ToInt32), 
                ToValueArray(arguments[5], ToInt32));
        }

        private void Measure(IReactInstance instance, JArray arguments)
        {
            _module.measure(ToInt32(arguments[0]), new Callback(ToInt32(arguments[1]), instance));
        }

        private void MeasureInWindow(IReactInstance instance, JArray arguments)
        {
            _module.measureInWindow(ToInt32(arguments[0]), new Callback(ToInt32(arguments[1]), instance));
        }

        private void MeasureLayout(IReactInstance instance, JArray arguments)
        {
            _module.measureLayout(
                ToInt32(arguments[0]),
                ToInt32(arguments[1]), 
                new Callback(ToInt32(arguments[2]), instance),
                new Callback(ToInt32(arguments[3]), instance));
        }

        private void MeasureLayoutRelativeToParent(IReactInstance instance, JArray arguments)
        {
            _module.measureLayoutRelativeToParent(
                ToInt32(arguments[0]),
                new Callback(ToInt32(arguments[1]), instance),
                new Callback(ToInt32(arguments[2]), instance));
        }

        private void RemoveRootView(IReactInstance instance, JArray arguments)
        {
            _module.removeRootView(ToInt32(arguments[0]));
        }

        private void RemoveSubviewsFromContainerWithID(IReactInstance instance, JArray arguments)
        {
            _module.removeSubviewsFromContainerWithID(ToInt32(arguments[0]));
        }

        private void ReplaceExistingNonRootView(IReactInstance instance, JArray arguments)
        {
            _module.replaceExistingNonRootView(ToInt32(arguments[0]), ToInt32(arguments[1]));
        }

        private void SetChildren(IReactInstance instance, JArray arguments)
        {
            _module.setChildren(ToInt32(arguments[0]), ToValueArray(arguments[1], ToInt32));
        }

        private void SetJSResponder(IReactInstance instance, JArray arguments)
        {
            _module.setJSResponder(ToInt32(arguments[0]), ToBoolean(arguments[1]));
        }

        private void ShowPopupMenu(IReactInstance instance, JArray arguments)
        {
            _module.showPopupMenu(
                ToInt32(arguments[0]), 
                ToValueArray(arguments[1], ToString),
                new Callback(ToInt32(arguments[2]), instance),
                new Callback(ToInt32(arguments[3]), instance));
        }

        private void UpdateView(IReactInstance instance, JArray arguments)
        {
            _module.updateView(
                ToInt32(arguments[0]),
                arguments[1].Value<string>(),
                (JObject)arguments[2]);
        }

        #endregion
    }
}
