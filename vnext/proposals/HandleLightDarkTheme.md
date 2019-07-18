# Handling Windows' Light and Dark Themes in React Native

## Summary
With the introduction of React Native for Windows, there needs to be a way to integrate into the Light and Dark themes at the JavaScript layer.

>This feature was pitched in the [React Native for Windows repo here](https://github.com/microsoft/react-native-windows/issues/2206).

## Motivation
Any React Native developer that wishes to follow the guidance of a Windows app will need to have access to the theme brushes, or at the very least a way to know when the system theme has changed.

There are several reasons why this is important to the React Native developer when they run their app on Windows:

- Many native Windows components will honor the Light/Dark theme changes automatically. Giving the app-developer access/knowledge of those changes reduces the "zebra" UI effect *(zebra UI = a page in an app where some controls honor a theme system, while others do not, or honor a non-complimentary one)*
- Having access to a theme changed event allows a React Native developer to better brand and customize their app for Windows, by accommodating their own styles based on the theme selected by the user *(typically set through system settings)*
- Exposing the Windows theme up to the JavaScript layer means React Native apps can blend more seamlessly into the Fluent Design system and native Windows platform more easily
- Both [Android](https://developer.android.com/preview/features/darktheme) and [iOS 13](https://developer.apple.com/documentation/appkit/supporting_dark_mode_in_your_interface) have or will have support for Dark themes, thus it is a necessity that we expose some way of detecting and handling theme changes at the RN layer.

### Scope
There are two core aspects that need to be available to the app developer in order to have a Windows Light/Dark theme sensitive app:

|# |Feature |
|:-|:-|
| 1 | Expose a Windows ThemeChanged API event at the JavaScript layer |
| 2 | Ship a separate Windows Native Module that enables a React Native developer to access our theme brushes |

- The first feature enables a React Native developer to be conscious of the Windows theme change events, and alter their style(s) accordingly if desired
- The second feature expands the theme customization further, by allowing an app-developer at the React Native layer to access our Windows-defined theme brushes in their styles

## Basic examples

### Example 1 : Set up and behaviors on theme changed
In this example, we'll look at three things:

- How to set up your React Native app to be style and event sensitive to the system themes
- How to switch styles when a theme change has occurred
- Handling a theme changed event

#### Setting up your app to be sensitive to theme changes
First import the Platform API into your React Native app.

```JavaScript
import { AppTheme } from 'react-native-windows'
```

Create a local variable to use in style conditionals or to reference elsewhere easily.

```JavaScript
class MyAppClass extends Component {
  state = {
    appThemeState: AppTheme.currentTheme,
  };
  ...
}
```

#### Switching styles based on the app's theme
If the app author wants to switch the style of their component manually based on the system's theme (Dark or Light), they can do so with CSS style conditionals.

```JavaScript
<TextInput
    style={[styles.inputStyle, this.state.appThemeState == AppTheme.dark ? styles.darkInput : styles.lightInput]}
</TextInput>
```

#### Handling a theme changed event
In this case an app author would like to do something or preform some behavior when the app's theme has changed.

> Note: AppThemeState will be a subclass of **NativeEventEmitter**.

```JavaScript
componentDidMount() {
  Platform.currentTheme.addListener('themechanged', this.onAppThemeChanged);
}

onAppThemeChanged = (event) => {
  /*Logic on theme changed goes here*/
  this.setState({appThemeState: AppTheme.currentTheme});
}
```


### Example 2 : Using Windows-defined theme brushes
This example covers how to access and use the Windows sytem theme brushes and apply them in your CSS styles.

#### Using theme brushes in a style

```css
const styles = StyleSheet.create({
  title: {
    fontSize: 19,
    fontWeight: 'bold',
    color: {windowsbrush: 'SystemControlPageTextBaseHighBrush'}
  },
});
```
Typing ``windowsbrush:`` will trigger the intellisense to scope to the system theme brushes available in native generic.xaml.

#### Applying a system accent color variant
In Windows, there are algorithmically generated accent colors - dubbed Light or Dark 1, 2, and 3. This example covers what it would look like to apply that using the windowsbrush object.

```css
const styles = StyleSheet.create({
  title: {
    fontSize: 19,
    fontWeight: 'bold',
    color: {windowsbrush: 'SystemAccentColorLight3'}
  },
});
```

>Note: That the system accent colors are **Color** objects at the native layer, whereas the other examples showed the use of theme brushes are **SolidColorBrush** objects.

#### Using theme brushes in a style as a conditional (P2)
This means that the re-render happens natively, instead of on the JS render.

```css
const styles = StyleSheet.create({
  title: {
    fontSize: 19,
    fontWeight: 'bold',
    color: {
          light: {windowsbrush: 'SystemControlPageTextBaseHighBrush'},
          dark: 'red'
        }
  },
});
```

## API Design Overview
A look at the APIs for the features and scope discribed above.

### Platform.currentTheme
*The specifics of the event or API being fired/used.*

| API | Args | Returns | Discription |
|:---|:----:|:----|:--|
| currentTheme | none | string | currentTheme is a dynamic function that returns the state of the system theme that the user or native app has set. |

#### App theme enum
Platform's ```currentTheme``` returns one of the enum values defined below.

| Property | Type| Description |
|:---|:----:|:----|
| dark | string | A string value defining that the native Windows app is in **Dark** theme. |
| light | string | A string value defining that the native Windows app is in **Light** theme. |
| ishighcontrast | string | A string value defining that the native Windows app is in a **High Contrast** theme (any). |

### windowsbrush
Will only allow access/lookups to theme brushes and colors that exist in generic.xaml. Custom brushes defined at the native layer are out of scope to be used/referenced by this object.

| Name | Type | Key | Key Types |
|:---|:----:|:----|:--|
| windowsbrush | object | string | Any of the 105 theme brushes that exists in genreic.xaml (i.e. SystemControlBackgroundBaseMediumBrush), plus the 7 system accent color variants. |

## Adoption strategy
This API is not a breaking change and will be implemented as part of the ```react-native-windows``` out-of-tree platform.
It *will not* be later introduced to the base ```react-native``` platform, as it is a Windows-specific behavior.

## How we teach this
Once implemented, we will document usage of it on the ```react-native-windows``` repo, and work to get it mentioned in the official ```react-native``` API documentation.

## Unresolved questions

- How or can you style/color a native Windows control at the RN CSS layer?
  - Not directly related to the theme, but if we are exposing theme brush resources, this is an open question
- Can we specify an equivilent of "RequestedTheme" on a View component, so that at the native layer the Dark or Light theme is triggered for all elements within?
