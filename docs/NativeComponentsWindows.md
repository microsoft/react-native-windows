---
id: native-components-windows
title: Native UI Components
layout: docs
category: Guides (Windows)
permalink: docs/native-components-windows.html
next: running-on-device-windows
---

React Native has several of the most critical platform components already wrapped, like `ScrollView` and `TextInput`, but not all of them, and certainly not ones you might have written yourself for a previous app. Fortunately, it's quite easy to wrap up these existing components for seamless integration with your React Native application.

Like the [native module guide](NativeModulesWindows.md), this too is a more advanced guide that assumes you are somewhat familiar with the Universal Windows SDK programming. This guide will show you how to build a native UI component, walking you through the implementation of a subset of the existing `ImageView `component available in the core React Native library.

## ImageView example

For this example we are going to walk through the implementation requirements to allow the use of ImageViews in JavaScript.

Native views are created and manipulated by extending `ViewManager` or more commonly `SimpleViewManager` . A `SimpleViewManager` is convenient in this case because it applies common properties such opacity and Flexbox layout.

These subclasses are essentially singletons - only one instance of each is created by the bridge. They vend native views to the `NativeViewHierarchyManager`, which delegates back to them to set and update the properties of the views as necessary. The `ViewManagers` are also typically the delegates for the views, sending events back to JavaScript via the bridge.

Vending a view is simple:

1. Create the ViewManager subclass.
2. Implement the `CreateViewInstance` method
3. Expose view property setters using the `[ReactProp]` (or `[ReactPropGroup]`) attribute
4. Register the manager in `CreateViewManagers` of the applications package.
5. Implement the JavaScript module

## 1. Create the `ViewManager` subclass

In this example we create view manager class `ReactImageManager` that extends `SimpleViewManager` with generic argument `Border`. `Border` is the type of framework element managed by the manager, this will be the custom native view. The name returned by the `Name` property is used to reference the native view type from JavaScript. The `Border` type is used to support border radii on images, the background of the `Border` is set to the appropriate `ImageSource`.

```csharp
...

public class ReactImageManager : SimpleViewManager<Border>
{

    public override string Name
    {
        get
        {
            return "RCTImageView";
        }
    }
}
```

## 2. Implement method `CreateViewInstance`

Views are created in the `CreateViewInstance` method, the view should initialize itself in its default state, any properties will be set via a follow up call to `UpdateView.`

```csharp
protected override Border CreateViewInstance(ThemedReactContext reactContext)
{
    return new Border
    {
        Background = new ImageBrush()
    };
}
```

## 3. Expose view property setters using the `[ReactProp]` (or `[ReactPropGroup]`) attribute

Properties that are to be reflected in JavaScript needs to be exposed as setter method annotated with `[ReactProp]` (or `[ReactPropGroup]`). Setter method should take view to be updated (of the current view type) as a first argument and property value as a second argument. Setter should be declared as a `void` method and should be `public`. The prop type sent to JS is determined automatically based on the type of parameter for the setter. This project uses [Newtonsoft Json.NET](http://www.newtonsoft.com/json) to provide interoperability with JavaScript types. The prop parameter type may be of any type that can be deserialized using Json.NET. Be aware that the use of composite types such as arrays, generics, and user-defined classes may not be supported out-of-the-box with [.NET Native](https://msdn.microsoft.com/en-us/library/dn584397.aspx) pre-compilation, and you may need to add information to the [runtime directives (rd.xml)](https://msdn.microsoft.com/en-us/library/dn600639.aspx) file, or just manually deconstruct the JSON by using [`JArray`](http://www.newtonsoft.com/json/help/html/T_Newtonsoft_Json_Linq_JArray.htm) or [`JObject`](http://www.newtonsoft.com/json/help/html/T_Newtonsoft_Json_Linq_JObject.htm) as the parameter type. 

Attribute `[ReactProp]` has one obligatory argument `Name` of type `String`. Name assigned to the `[ReactProp]` attribute linked to the setter method is used to reference the property on JS side.

Except from `Name`, `[ReactProp]` attribute may take various optional arguments, e.g.: `DefaultBoolean`, `DefaultInt32`, `DefaultDouble`. Those arguments should be of the corresponding primitive type (accordingly `boolean`, `int`, `double`) and the value provided will be passed to the setter method in case when the property that the setter is referencing has been removed from the component. Note that "default" values are only provided for primitive types, in case when setter is of some complex type, `null` will be provided as a default value in case when corresponding property gets removed.

Setter declaration requirements for methods annotated with `[ReactPropGroup]` are different than for `[ReactProp]`, instead of taking a single name for the prop, it takes an array of names, corresponding to the set of parameters used in the group setter, and does not support any optional default value arguments. 

**IMPORTANT!** in ReactJS updating the property value will result in setter method call. Note that one of the ways we can update component is by removing properties that has been set before. In that case setter method will be called as well to notify view manager that property has changed. In that case "default" value will be provided (for primitive types "default" can value can be specified using `DefaultBoolean`, `DefaultDouble`, etc. arguments of `[ReactProp]` attribute, for complex types setter will be called with value set to `null`).

```csharp
[ReactProp("borderRadius")]
public void SetBorderRadius(Border view, double radius)
{
    view.CornerRadius = new CornerRadius(radius);
}

[ReactProp("borderColor", CustomType = "Color")]
public void SetBorderColor(Border view, uint? color)
{
    view.BorderBrush = color.HasValue
        ? new SolidColorBrush(ColorHelpers.Parse(color.Value))
        : null;
}

[ReactProp("src")]
public void SetSource(Border view, string source)
{
    var imageBrush = (ImageBrush)view.Background;
    imageBrush.ImageSource = new BitmapImage(new Uri(source));

    view.GetReactContext()
        .GetNativeModule<UIManagerModule>()
        .EventDispatcher
        .DispatchEvent(
            new ReactImageLoadEvent(
                view.GetTag(),
                ReactImageLoadEvent.OnLoadStart));
}
```

## 4. Register the `ViewManager`

The final step is to register the ViewManager to the application, this happens in a similar way to [Native Modules](docs/native-modules-windows.html), via the applications package member function `CreateViewManagers.`

```csharp
public IReadOnlyList<IViewManager> CreateViewManagers(
    ReactContext reactContext)
{
    return new List<IViewManager>
    {
        new ReactImageManager()
    };
}
```

## 5. Implement the JavaScript module

The very final step is to create the JavaScript module that defines the interface layer between .NET and JavaScript for the users of your new view. Much of the effort is handled by internal React code in .NET and JavaScript and all that is left for you is to describe the `propTypes`.

```js
// ImageView.js

import { PropTypes } from 'react';
import { requireNativeComponent, View } from 'react-native';

var iface = {
  name: 'ImageView',
  propTypes: {
    src: PropTypes.string,
    borderRadius: PropTypes.number,
    resizeMode: PropTypes.oneOf(['cover', 'contain', 'stretch']),
    ...View.propTypes // include the default view properties
  },
};

module.exports = requireNativeComponent('RCTImageView', iface);
```

`requireNativeComponent` commonly takes two parameters, the first is the name of the native view and the second is an object that describes the component interface. The component interface should declare a friendly `name` for use in debug messages and must declare the `propTypes` reflected by the Native View. The `propTypes` are used for checking the validity of a user's use of the native view.  Note that if you need your JavaScript component to do more than just specify a name and propTypes, like do custom event handling, you can wrap the native component in a normal react component.  In that case, you want to pass in the wrapper component instead of `iface` to `requireNativeComponent`.  This is illustrated in the `ReactTextBox` example below.

# Events

So now we know how to expose native view components that we can control easily from JS, but how do we deal with events from the user, like pinch-zooms or panning? When a native event occurs the native code should issue an event to the JavaScript representation of the View, and the two views are linked with the value returned from the `GetTag()` method.

```csharp
class ReactTextChangedEvent : Event
{
    ...

    public override string EventName
    {
        get
        {
            return "topChange";
        }
    }

    public override void Dispatch(RCTEventEmitter rctEventEmitter)
    {
        var contentSize = new JObject
        {
            { "width", _contextWidth },
            { "height", _contentHeight },
        };

        var eventData = new JObject
        {
            { "text", _text },
            { "contentSize", contentSize },
            { "eventCount", _eventCount },
            { "target", ViewTag },
        };

        rctEventEmitter.receiveEvent(ViewTag, EventName, eventData);
    }
}

class ReactTextBox : TextBox, ILayoutManager
{
    public ReactTextBox()
    {
        LayoutUpdated += OnLayoutUpdated;
    }

    ...
    private void OnLayoutUpdated(object sender, object e)
    {
        var width = ActualWidth;
        var height = ActualHeight;
        if (width != _lastWidth || height != _lastHeight)
        {
            _lastWidth = width;
            _lastHeight = height;

            this.GetReactContext()
                .GetNativeModule<UIManagerModule>()
                .EventDispatcher
                .DispatchEvent(
                    new ReactTextChangedEvent(
                        this.GetTag(),
                        Text,
                        width,
                        height,
                        IncrementEventCount()));
        }
    }
}
```

The event name `topChange` maps to the `onChange` callback prop in JavaScript (mappings are in `UIManagerModule.Constants.cs`). This callback is invoked with the raw event, which we typically process in the wrapper component to make a simpler API:

```js
// MyCustomView.js

class MyCustomView extends React.Component {
  constructor() {
    this._onChange = this._onChange.bind(this);
  }
  _onChange(event: Event) {
    if (!this.props.onChangeMessage) {
      return;
    }
    this.props.onChangeMessage(event.nativeEvent.message);
  }
  render() {
    return <RCTMyCustomView {...this.props} onChange={this._onChange} />;
  }
}
MyCustomView.propTypes = {
  /**
   * Callback that is called continuously when the user is dragging the map.
   */
  onChangeMessage: React.PropTypes.func,
  ...
};

var RCTMyCustomView = requireNativeComponent(`RCTMyCustomView`, MyCustomView, {
  nativeOnly: {onChange: true}
});
```

Note the use of `nativeOnly` above.  Sometimes you'll have some special properties that you need to expose for the native component, but don't actually want them as part of the API for the associated React component.  For example, `Switch` has a custom `onChange` handler for the raw native event, and exposes an `onValueChange` handler property that is invoked with just the boolean value rather than the raw event (similar to `onChangeMessage` in the example above).  Since you don't want these native only properties to be part of the API, you don't want to put them in `propTypes`, but if you don't you'll get an error.  The solution is simply to call them out via the `nativeOnly` option.
