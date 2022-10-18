
/**
 * This code was generated by [react-native-codegen](https://www.npmjs.com/package/react-native-codegen).
 *
 * Do not edit this file as changes may cause incorrect behavior and will be lost
 * once the code is regenerated.
 *
 * @generated by codegen project: GenerateEventEmitterCpp.js
 */

#include <react/renderer/components/rnwcore/EventEmitters.h>

namespace facebook {
namespace react {

void ModalHostViewEventEmitter::onRequestClose(OnRequestClose event) const {
  dispatchEvent("requestClose", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    
    return payload;
  });
}
void ModalHostViewEventEmitter::onShow(OnShow event) const {
  dispatchEvent("show", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    
    return payload;
  });
}
void ModalHostViewEventEmitter::onDismiss(OnDismiss event) const {
  dispatchEvent("dismiss", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    
    return payload;
  });
}
void ModalHostViewEventEmitter::onOrientationChange(OnOrientationChange event) const {
  dispatchEvent("orientationChange", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "orientation", toString(event.orientation));
    return payload;
  });
}

void DatePickerEventEmitter::onChange(OnChange event) const {
  dispatchEvent("change", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "timestamp", event.timestamp);
    return payload;
  });
}
void AndroidDrawerLayoutEventEmitter::onDrawerSlide(OnDrawerSlide event) const {
  dispatchEvent("drawerSlide", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "offset", event.offset);
    return payload;
  });
}
void AndroidDrawerLayoutEventEmitter::onDrawerStateChanged(OnDrawerStateChanged event) const {
  dispatchEvent("drawerStateChanged", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "drawerState", event.drawerState);
    return payload;
  });
}
void AndroidDrawerLayoutEventEmitter::onDrawerOpen(OnDrawerOpen event) const {
  dispatchEvent("drawerOpen", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    
    return payload;
  });
}
void AndroidDrawerLayoutEventEmitter::onDrawerClose(OnDrawerClose event) const {
  dispatchEvent("drawerClose", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    
    return payload;
  });
}


void AndroidSwipeRefreshLayoutEventEmitter::onRefresh(OnRefresh event) const {
  dispatchEvent("refresh", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    
    return payload;
  });
}
void PullToRefreshViewEventEmitter::onRefresh(OnRefresh event) const {
  dispatchEvent("refresh", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    
    return payload;
  });
}


void SliderEventEmitter::onValueChange(OnValueChange event) const {
  dispatchEvent("valueChange", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "value", event.value);
payload.setProperty(runtime, "fromUser", event.fromUser);
    return payload;
  });
}
void SliderEventEmitter::onSlidingComplete(OnSlidingComplete event) const {
  dispatchEvent("slidingComplete", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "value", event.value);
payload.setProperty(runtime, "fromUser", event.fromUser);
    return payload;
  });
}
void AndroidSwitchEventEmitter::onChange(OnChange event) const {
  dispatchEvent("change", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "value", event.value);
    return payload;
  });
}
void SwitchEventEmitter::onChange(OnChange event) const {
  dispatchEvent("change", [event=std::move(event)](jsi::Runtime &runtime) {
    auto payload = jsi::Object(runtime);
    payload.setProperty(runtime, "value", event.value);
    return payload;
  });
}



} // namespace react
} // namespace facebook
