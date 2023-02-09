
/**
 * This code was generated by [react-native-codegen](https://www.npmjs.com/package/react-native-codegen).
 *
 * Do not edit this file as changes may cause incorrect behavior and will be lost
 * once the code is regenerated.
 *
 * @generated by codegen project: GenerateEventEmitterH.js
 */
#pragma once

#include <react/renderer/components/view/ViewEventEmitter.h>
#include <jsi/jsi.h>

namespace facebook {
namespace react {

class JSI_EXPORT ModalHostViewEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  struct OnRequestClose {
      
    };

  struct OnShow {
      
    };

  struct OnDismiss {
      
    };

  enum class OnOrientationChangeOrientation {
    Portrait,
    Landscape
  };

  static char const *toString(const OnOrientationChangeOrientation value) {
    switch (value) {
      case OnOrientationChangeOrientation::Portrait: return "portrait";
      case OnOrientationChangeOrientation::Landscape: return "landscape";
    }
  }

  struct OnOrientationChange {
      OnOrientationChangeOrientation orientation;
    };

  void onRequestClose(OnRequestClose value) const;

  void onShow(OnShow value) const;

  void onDismiss(OnDismiss value) const;

  void onOrientationChange(OnOrientationChange value) const;
};
class JSI_EXPORT ActivityIndicatorViewEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  

  
};
class JSI_EXPORT AndroidDrawerLayoutEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  struct OnDrawerSlide {
      Float offset;
    };

  struct OnDrawerStateChanged {
      int drawerState;
    };

  struct OnDrawerOpen {
      
    };

  struct OnDrawerClose {
      
    };

  void onDrawerSlide(OnDrawerSlide value) const;

  void onDrawerStateChanged(OnDrawerStateChanged value) const;

  void onDrawerOpen(OnDrawerOpen value) const;

  void onDrawerClose(OnDrawerClose value) const;
};
class JSI_EXPORT PLYIconEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  

  
};
class JSI_EXPORT AndroidProgressBarEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  

  
};
class JSI_EXPORT AndroidSwipeRefreshLayoutEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  struct OnRefresh {
      
    };

  void onRefresh(OnRefresh value) const;
};
class JSI_EXPORT PullToRefreshViewEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  struct OnRefresh {
      
    };

  void onRefresh(OnRefresh value) const;
};
class JSI_EXPORT SafeAreaViewEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  

  
};
class JSI_EXPORT AndroidHorizontalScrollContentViewEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  

  
};
class JSI_EXPORT AndroidSwitchEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  struct OnChange {
      bool value;
    int target;
    };

  void onChange(OnChange value) const;
};
class JSI_EXPORT SwitchEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  struct OnChange {
      bool value;
    int target;
    };

  void onChange(OnChange value) const;
};
class JSI_EXPORT InputAccessoryEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  

  
};
class JSI_EXPORT UnimplementedNativeViewEventEmitter : public ViewEventEmitter {
 public:
  using ViewEventEmitter::ViewEventEmitter;

  

  
};

} // namespace react
} // namespace facebook
