# Reconcile Mouse APIs in React Native - Desktop

Windows and macOS support mouse devices today, but their React Native counterparts are not fully flushed out yet to unblock customers attempting to create React Native apps on those devices.

This document will attempt to capture what's needed within React Native for Windows and macOS to enable customer scenarios. This includes:

- Aligning with the [react-native-web](https://github.com/necolas/react-native-web/commit/1afd9de0455965a852bad42ad2ffab957f7ffca0) mouse behavior/implementation [issue TBD]()
- Ensuring mouse behavior is consistent across the two platforms we support
- Once our two desktop platforms are aligned with React Native Web, we upstream this implementation to core [issue TBD]()

## `hovered`
Property on `Pressable` that determines whether or not the mouse is currently within the bounds of the designated content.

| TYPE | REQUIRED | PLATFORM |
|:--:|:--|:--|
| boolean | Yes | Windows, macOS |

[React Native Web](https://github.com/necolas/react-native-web/commit/1afd9de0455965a852bad42ad2ffab957f7ffca0#diff-9fe3ab194c72e9837a8ffc3e7c44f33d) introduced  `hovered` on `Pressable` to enable common mouse scenarios. In order to align and make this complete the work is as follows:

- Windows has added `hovered` and marked `onMouseEnter/Leave` as to be deprecated [issue TBD]()
- Add `hovered` to `Pressable` [issue TBD]()
- macOS has added 'hovered' and marked their View component implementation of `onMouseEnter/Leave` as deprecated [issue TBD]()

### `hovered` example

``` js
<Pressable style={({ hovered, pressed, focused }) => {
          console.log(focused);
          let backgroundColor = 'white';
          if (hovered) {
            backgroundColor = 'lightgray';
          }
          if (pressed) {
            backgroundColor = 'lightblue';
          }
          return {
            padding: 10,
            margin: 10,
            borderWidth: 1,
            borderColor: focused ? 'red' : null,
            backgroundColor,
            outlineWidth: 0
          };
        }}
      >
      <Text>Nested pressables</Text>
    </Pressable>
  </Pressable>
</View>
```

## Outstanding issues
There are some outstanding issues that need to be address specifically for desktop scenarios when it comes to fully implementing mouse hover and leave behavior. They are as follows:

- Prevent mouse input from being 'eaten' in ScrollView scenarios [issue #5867](https://github.com/microsoft/react-native-windows/issues/5867)
- Mouse leave event not firing correctly in Scrollable FlatList scenarios [issue #4614](https://github.com/microsoft/react-native-windows/issues/4614)
- The hover events/indication for overlapping content prioritizes items "closer" in z-order [issue #5901](https://github.com/microsoft/react-native-windows/issues/5901)

## onPointerXX callbacks
The following callbacks are available on `View` component (and get passed through to Pressable) in Windows and macOS to cover the most common use cases where mouse handling would occur.

The below will need to be implemented for macOS as well [issue TBD]()

| API | Args | Returns | Description |
|:---:|:----:|:-------:|----|
| onPointerOver | IPointerEvent | void | Fires when a pointing device is moved within the hit test boundaries of a element. |
| onPointerLeave | IPointerEvent | void | Fires when a pointing device is moved out of the hit test boundaries of an element and all of its children. |

### Example

``` js
<View onPointerOver={this._onPointerOver} />

private onPointerOver_ = (event: IPointerEvent) => {
  this.setState({ lastKeyDown: event.nativeEvent.key });
};
```

### Declarative properties

To co-ordinate the handoffs of these pointer events between the native layer and the JS layer, we are also introducing 2 corresponding properties on the `View`(?) component. Those are:

| Property | Type | Description |
|:---:|:----:|----|
| pointerOverEvents | IHandledPointerEvents[] | Specifies the pointer over events that are handled in the JS layer by the onPointerOver/onPointerOverCapture events |
| pointerLeaveEvents | IHandledPointerEvents[] | Specifies the button or buttons that are handled in the JS layer by the onPointerOut/onPointerOutCapture events |

Where `IHandledPointerEvents` is a new type which takes the following parameters:
- A constrained string parameter named `pointerState` to declare the pointer state (Over, Hover, Out) that is of interest to the JS layer
- An `eventPhase` parameter of type `EventPhase` to declare the routing phase of interest to the JS layer.
