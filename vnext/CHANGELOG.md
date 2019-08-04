# Change Log - react-native-windows

This log was last generated on Sat, 03 Aug 2019 17:26:50 GMT and should not be manually modified.

## 0.59.0-vnext.116
Sat, 03 Aug 2019 17:26:50 GMT

### Changes

- change dev menu shortcut to ctrl+shift+D (kmelmon@microsoft.com)

## 0.59.0-vnext.115
Sat, 03 Aug 2019 04:30:38 GMT

### Changes

- change process.env.TEMP to os.tmpdir() (email not defined)
,- fix onFocus/onBlur events for Touchables (andrewh@microsoft.com)
,- New switch props (thumbColor and trackColor), and fix few bugs found in TextInputViewManager (dida@ntdev.microsoft.com)
,- addedundefined developer menu (kmelmon@microsoft.com)
,- fix image not firing on load. (stecrain@microsoft.com)
,- Delay-load dependencies absent on Win7 (acoates@microsoft.com)
,- Add the SpringAnimationDriver (stpete@microsoft.com)

## 0.59.0-vnext.110
Sun, 28 Jul 2019 12:44:00 GMT

### Changes

- Allow run-windows to be more flexible about project layout to support default c++ layouts (acoates@microsoft.com)

## 0.59.0-vnext.109
Fri, 26 Jul 2019 08:32:04 GMT

### Changes

- add null checks to fix crashes with reload / shutdown with pending text updates (andrewh@microsoft.com)

## 0.59.0-vnext.108
Fri, 26 Jul 2019 07:23:20 GMT

### Changes

- Fix for flyouts unexpectedly dismissing when opening picker (ruaraki@microsoft.com)

## 0.59.0-vnext.107
Fri, 26 Jul 2019 05:43:01 GMT

### Changes

- use win32 file api to avoid runtime broker slowing us down (stecrain@microsoft.com)

## 0.59.0-vnext.106
Thu, 25 Jul 2019 00:29:50 GMT

### Changes

- Fixes to ReactImage lifetime with downloading Images (acoates@microsoft.com)

## 0.59.0-vnext.105
Wed, 24 Jul 2019 21:03:23 GMT

### Changes

- Hookup beachball for publishing (acoates@microsoft.com)
