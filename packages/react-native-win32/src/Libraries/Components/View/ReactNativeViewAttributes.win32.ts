const ReactNativeViewAttributes = require('./ReactNativeViewAttributes.js');

// win32 currently uses an extra property on text
// We add it to view, since TextAttributes isn't split out into a separate module the same way that view is.
// Once we flush out our JS theming story this property will no longer be needed
ReactNativeViewAttributes.UIView.textStyle = true;
ReactNativeViewAttributes.UIView.tooltip = true;

export = ReactNativeViewAttributes;
