# Change Log - react-native-windows

This log was last generated on Thu, 17 Oct 2019 23:10:13 GMT and should not be manually modified.

## 0.60.0-vnext.37
Thu, 17 Oct 2019 23:10:13 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.8 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.36
Thu, 17 Oct 2019 20:58:22 GMT

### Changes

- Fix two issues: 1) you cannot animated 2 subchannels of the same property with different animations. to fix this we animated yet another property set for translation and scale owned by the props nodes and use one animation to animate all of the subchannels for the uiElement. 2) Reference parameter names which started with a multi digit number are unsupported so i added an n to the start of each name, which was previously just the node's tag. (stpete@microsoft.com)
## 0.60.0-vnext.35
Thu, 17 Oct 2019 16:18:52 GMT

### Changes

- Support autoCapitalize Characters (dida@ntdev.microsoft.com)
## 0.60.0-vnext.34
Thu, 17 Oct 2019 02:48:28 GMT

### Changes

- Support onSubmitEditing event (dida@ntdev.microsoft.com)
## 0.60.0-vnext.33
Thu, 17 Oct 2019 00:14:32 GMT

### Changes

- exclude Generated Files (kmelmon@microsoft.com)
## 0.60.0-vnext.32
Wed, 16 Oct 2019 02:19:39 GMT

### Changes

- Extend flyout shadow fix to Popups (kenander@microsoft.com)
## 0.60.0-vnext.31
Tue, 15 Oct 2019 23:45:52 GMT

### Changes

- Reduced and simplified sample native modules (jthysell@microsoft.com)
## 0.60.0-vnext.30
Tue, 15 Oct 2019 22:35:30 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.7 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.29
Tue, 15 Oct 2019 13:17:22 GMT

### Changes

- dont new up exceptions (adamgor@microsoft.com)
## 0.60.0-vnext.28
Mon, 14 Oct 2019 23:02:23 GMT

### Changes

- Microsoft.ReactNative attribute-based NativeModules (jthysell@microsoft.com)
## 0.60.0-vnext.27
Sat, 12 Oct 2019 16:49:47 GMT

### Changes

- add tests (kmelmon@microsoft.com)
## 0.60.0-vnext.26
Fri, 11 Oct 2019 22:15:37 GMT

### Changes

- add Common.vcxproj to CLI solutions (kmelmon@microsoft.com)
## 0.60.0-vnext.25
Fri, 11 Oct 2019 20:47:08 GMT

### Changes

- Support caretHidden (dida@ntdev.microsoft.com)
## 0.60.0-vnext.24
Fri, 11 Oct 2019 10:21:12 GMT

### Changes

- add support for offline bundles (kmelmon@microsoft.com)
## 0.60.0-vnext.23
Wed, 09 Oct 2019 18:17:42 GMT

### Changes

- Add support for forever animations. (stpete@microsoft.com)
## 0.60.0-vnext.22
Wed, 09 Oct 2019 15:13:21 GMT

### Changes

- Updating react-native to version: 0.60.0-microsoft.5 (53619745+rnbot@users.noreply.github.com)
## 0.60.0-vnext.21
Tue, 08 Oct 2019 18:33:16 GMT

### Changes

- re-enable Chakra Core on UWP (stecrain@microsoft.com)
## 0.60.0-vnext.20
Mon, 07 Oct 2019 21:46:39 GMT

### Changes

- Bump V8 and Hermes JSI package versions (tudorm@microsoft.com)
## 0.60.0-vnext.19
Fri, 04 Oct 2019 19:04:20 GMT

### Changes

- Update ChakraCore to v1.11.13 (acoates@microsoft.com)
## 0.60.0-vnext.18
Fri, 04 Oct 2019 16:32:45 GMT

### Changes

- Update to react-native@0.60.0-microsoft.3 (acoates@microsoft.com)
## 0.60.0-vnext.17
Thu, 03 Oct 2019 22:17:13 GMT

### Changes

- change file (randyfl@microsoft.com)
- Support contextMenuHidden (dida@ntdev.microsoft.com)
- Remove react-native as a haste root, as all the files should be in react-native-windows (acoates@microsoft.com)
- Connect DeviceInfo dimensions with root element (dida@ntdev.microsoft.com)
## 0.60.0-vnext.16
Tue, 01 Oct 2019 18:17:42 GMT

### Changes

- Turn ControlFlowGuard on by default (acoates@microsoft.com)
- The customer provided examples revealed a few of issues.  One, we were dubble counding the starting value of animatiosn in some cases. Two we were incorrectly relying on JS to tell us to flatten our animated values. Three we were detaching the expression animations that tied the property to the UIElement and in certain cases the JS thread does not inform us that we need to rebuild this cuppling.  There is an open issue with this final part #3280 (stpete@microsoft.com)
## 0.60.0-vnext.11
Mon, 30 Sep 2019 18:28:23 GMT

### Changes

- Update to folly 2019.09.30.00 (acoates@microsoft.com)
## 0.60.0-vnext.10
Mon, 30 Sep 2019 16:21:08 GMT

### Changes

- Bump up to latest published version. (#3275) (julio.rocha@microsoft.com)
## 0.60.0-vnext.9
Sat, 28 Sep 2019 01:21:33 GMT

### Changes

- add support for cpp template app (email not defined)
## 0.60.0-vnext.8
Sat, 28 Sep 2019 00:21:32 GMT

### Changes

- Fix build break when using latest Visual Studio compiler versions (acoates@microsoft.com)
## 0.60.0-vnext.7
Fri, 27 Sep 2019 17:40:33 GMT

### Changes

- Allow focus transfer to Flyout on Open (kenander@microsoft.com)
## 0.60.0-vnext.6
Fri, 27 Sep 2019 17:34:59 GMT

### Changes

- change file (randyfl@microsoft.com)
## 0.60.0-vnext.5
Fri, 27 Sep 2019 00:08:28 GMT

### Changes

- Allow focus transfer to Flyout on Open (kenander@microsoft.com)
## 0.60.0-vnext.4
Wed, 25 Sep 2019 18:32:05 GMT

### Changes

- Enabling Systrace/ETW plumbing by default. The tracing will be controlled by whether the provider is enabled or not (anandrag@microsoft.com)
## 0.60.0-vnext.3
Tue, 24 Sep 2019 23:29:18 GMT

### Changes

- Create stub StatusBarManager to avoid crash when using <StatusBar> (acoates@microsoft.com)
## 0.60.0-vnext.2
Tue, 24 Sep 2019 19:11:20 GMT

### Changes

- Update to react-native 0.60 (acoates@microsoft.com)
## 0.59.0-vnext.190
Thu, 19 Sep 2019 17:02:46 GMT

### Changes

- Support keyboardType for TextInput (dida@ntdev.microsoft.com)
## 0.59.0-vnext.189
Wed, 18 Sep 2019 18:18:37 GMT

### Changes

- Add the SpringAnimationDriver (stpete@microsoft.com)
## 0.59.0-vnext.188
Tue, 17 Sep 2019 17:42:42 GMT

### Changes

- Refined logic for handling Flyout shadows (kenander@microsoft.com)
## 0.59.0-vnext.187
Tue, 17 Sep 2019 11:32:39 GMT

### Changes

- Simplify NuGet packing YAML. (julio@rochsquadron.net)
## 0.59.0-vnext.186
Tue, 17 Sep 2019 05:17:28 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.82 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.185
Sat, 14 Sep 2019 14:09:03 GMT

### Changes

- Add PackageCertificatePassword attribute to MyApp.csproj to solve the build issue in VS2019 due to temp pfx certificate issue (dida@ntdev.microsoft.com)
## 0.59.0-vnext.184
Fri, 13 Sep 2019 21:56:22 GMT

### Changes

- Support live reload for developer menu (dida@ntdev.microsoft.com)
## 0.59.0-vnext.183
Fri, 13 Sep 2019 15:59:25 GMT

### Changes

- MultiLine TextInput should accepts Enter key (dida@ntdev.microsoft.com)
## 0.59.0-vnext.182
Thu, 12 Sep 2019 23:51:35 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.79 (53619745+rnbot@users.noreply.github.com)
- Updating react-native to version: 0.59.0-microsoft.80 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.179
Wed, 11 Sep 2019 18:44:17 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.77 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.178
Tue, 10 Sep 2019 23:25:37 GMT

### Changes

- Fix onPress not firing for Touchable* keyboard events (thshea@microsoft.com)
## 0.59.0-vnext.177
Tue, 10 Sep 2019 19:16:08 GMT

### Changes

- Fix Login E2E test failure (dida@ntdev.microsoft.com)
## 0.59.0-vnext.176
Tue, 10 Sep 2019 00:44:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.74 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.175
Mon, 09 Sep 2019 21:39:46 GMT

### Changes

- Alphabetize def files. (yicyao@microsoft.com)
## 0.59.0-vnext.174
Mon, 09 Sep 2019 19:22:09 GMT

### Changes

- Put desktop unit tests into namespaces. (yicyao@microsoft.com)
## 0.59.0-vnext.173
Sat, 07 Sep 2019 20:17:19 GMT

### Changes

- Support dynamic swapping TextBox/PasswordBox view for SecureTextEntry property (dida@ntdev.microsoft.com)
## 0.59.0-vnext.172
Fri, 06 Sep 2019 18:03:18 GMT

### Changes

- Fix issue with using getInt instead of asDouble for JSI (jthysell@microsoft.com)
## 0.59.0-vnext.171
Thu, 05 Sep 2019 17:37:33 GMT

### Changes

- ReactUWP def cleanup and unit test fixes (jthysell@microsoft.com)
## 0.59.0-vnext.170
Tue, 03 Sep 2019 21:49:49 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.73 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.169
Tue, 03 Sep 2019 20:24:37 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.72 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.168
Fri, 30 Aug 2019 23:08:02 GMT

### Changes

- set IsTabStop = false for SnapPointManagingContentControl (stecrain@microsoft.com)
## 0.59.0-vnext.167
Fri, 30 Aug 2019 20:30:25 GMT

### Changes

- fix double-apply TransformMatrix bug (email not defined)
## 0.59.0-vnext.166
Fri, 30 Aug 2019 20:17:20 GMT

### Changes

- changed alert export (email not defined)
## 0.59.0-vnext.165
Fri, 30 Aug 2019 19:46:18 GMT

### Changes

- Fixed sampleapps build and updated CI (jthysell@microsoft.com)
## 0.59.0-vnext.164
Thu, 29 Aug 2019 22:14:06 GMT

### Changes

- changed alert export (email not defined)
## 0.59.0-vnext.163
Thu, 29 Aug 2019 20:21:10 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.71 (53619745+rnbot@users.noreply.github.com)
- change file (randyfl@microsoft.com)
## 0.59.0-vnext.162
Thu, 29 Aug 2019 19:38:17 GMT

### Changes

- Allow windowsbrush: color syntax to use non-SolidColor brushes (thshea@microsoft.com)
## 0.59.0-vnext.161
Thu, 29 Aug 2019 01:41:01 GMT

### Changes

- Use PreparedScriptStore for V8 in win32 (tudorm@microsoft.com)
## 0.59.0-vnext.160
Thu, 29 Aug 2019 01:33:54 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.70 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.159
Wed, 28 Aug 2019 19:55:48 GMT

### Changes

- Added React.Windows.Desktop.Test project. (julio@rochsquadron.net)
- Add isOverlayEnabled (cknestri@microsoft.com)
## 0.59.0-vnext.158
Wed, 28 Aug 2019 18:34:51 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.69 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.157
Wed, 28 Aug 2019 02:44:14 GMT

### Changes

- Let View process color objects (email not defined)
## 0.59.0-vnext.156
Tue, 27 Aug 2019 04:41:01 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.68 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.155
Mon, 26 Aug 2019 21:10:00 GMT

### Changes

- Enable experimental V8 JSI Executor support (off by default) (tudorm@microsoft.com)
## 0.59.0-vnext.154
Sat, 24 Aug 2019 15:32:08 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.67 (53619745+rnbot@users.noreply.github.com)
## 0.59.0-vnext.153
Sat, 24 Aug 2019 04:24:35 GMT

### Changes

- Fix change log formatting (acoates@microsoft.com)
## 0.59.0-vnext.152
Sat, 24 Aug 2019 04:09:20 GMT

### Changes

- move cppwinrt less exception helper, use in DevSupportManager (andrewh@microsoft.com)
## 0.59.0-vnext.151
Fri, 23 Aug 2019 01:27:19 GMT

### Changes

- turn off RTTI (email not defined)

## 0.59.0-vnext.150
Thu, 22 Aug 2019 21:17:45 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.49 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.149
Thu, 22 Aug 2019 15:43:48 GMT

### Changes

- Improve Networking error reporting (andrewh@microsoft.com)

## 0.59.0-vnext.148
Wed, 21 Aug 2019 20:44:12 GMT

### Changes

- Only send focus/blur from OriginalSource, remove Click event for enter/space (andrewh@microsoft.com)

## 0.59.0-vnext.147
Wed, 21 Aug 2019 06:12:54 GMT

### Changes

- add default message queue that uses CoreDispatcher (email not defined)

## 0.59.0-vnext.146
Wed, 21 Aug 2019 00:42:37 GMT

### Changes

- revert fix, not quite working (email not defined)

## 0.59.0-vnext.145
Tue, 20 Aug 2019 23:40:14 GMT

### Changes

- Light up Inspector debugging tool. (kinhln@microsoft.com)
- Remove old playground app (acoates@microsoft.com)

## 0.59.0-vnext.144
Tue, 20 Aug 2019 21:54:07 GMT

### Changes

- Minor fixes made in RN60 update that can be brought right to master (acoates@microsoft.com)

## 0.59.0-vnext.143
Tue, 20 Aug 2019 07:26:49 GMT

### Changes

- Don't use DefaultNativeMessageQueueThread in live reload (andrewh@microsoft.com)

## 0.59.0-vnext.142
Tue, 20 Aug 2019 03:02:53 GMT

### Changes

- add additional new warning disable to MsoFolly.h (andrewh@microsoft.com)

## 0.59.0-vnext.141
Mon, 19 Aug 2019 20:59:57 GMT

### Changes

- fix flyout full placement mode (kmelmon@microsoft.com)

## 0.59.0-vnext.140
Mon, 19 Aug 2019 16:43:43 GMT

### Changes

- Update placement mapping (cknestri@microsoft.com)
- Start API documentation (acoates@microsoft.com)
- Always use unforked version of folly + Update folly to v2019.08.12.00 (acoates@microsoft.com)
- resolved build error (email not defined)
- Improved fix for overlapping XAML theme shadows on Flyouts (kenander@microsoft.com)
- add batching queue (kmelmon@microsoft.com)
- restore accidental deletion (kmelmon@microsoft.com)
- Microsoft.ReactNative SampleApps (jthysell@microsoft.com)

## 0.59.0-vnext.139
Tue, 13 Aug 2019 19:56:52 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.43 (53619745+rnbot@users.noreply.github.com)
- Change windows template to not use cli wrapper (acoates@microsoft.com)

## 0.59.0-vnext.138
Mon, 12 Aug 2019 23:13:59 GMT

### Changes

- Rename platform uwp->windows (acoates@microsoft.com)

## 0.59.0-vnext.137
Mon, 12 Aug 2019 21:06:24 GMT

### Changes

- Improve output of run-windows command (acoates@microsoft.com)

## 0.59.0-vnext.136
Mon, 12 Aug 2019 20:02:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.42 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.135
Sun, 11 Aug 2019 04:57:41 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.41 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.134
Sat, 10 Aug 2019 19:21:55 GMT

### Changes

- Fix switch shimmy issue (ruaraki@microsoft.com)

## 0.59.0-vnext.133
Fri, 09 Aug 2019 22:37:48 GMT

### Changes

- use more specific typing for currentTheme, improve examples (andrewh@microsoft.com)

## 0.59.0-vnext.132
Fri, 09 Aug 2019 19:20:03 GMT

### Changes

- Update UwpPreparedScriptStore to use shared_ptrs (jthysell@microsoft.com)

## 0.59.0-vnext.131
Fri, 09 Aug 2019 06:07:04 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.40 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.130
Fri, 09 Aug 2019 04:18:45 GMT

### Changes

- Added list, listitem roles to ViewAccessibility.uwp.js (jthysell@microsoft.com)

## 0.59.0-vnext.129
Fri, 09 Aug 2019 03:27:03 GMT

### Changes

- Fix Sample/index.tsx layout, set Tag as Name in debug (andrewh@microsoft.com)

## 0.59.0-vnext.128
Thu, 08 Aug 2019 23:35:13 GMT

### Changes

- fix hit-testing bug (kmelmon@microsoft.com)

## 0.59.0-vnext.127
Thu, 08 Aug 2019 22:19:50 GMT

### Changes

- Add textDecoration support to Nested Text (dida@ntdev.microsoft.com)

## 0.59.0-vnext.126
Thu, 08 Aug 2019 20:57:44 GMT

### Changes

- Moving picker fix (decrowle@microsoft.com)

## 0.59.0-vnext.125
Wed, 07 Aug 2019 17:07:35 GMT

### Changes

- Adding accessibilityPosInSet and accessibilitySizeOfSet props to ViewWindows, #2894 (jthysell@microsoft.com)

## 0.59.0-vnext.124
Wed, 07 Aug 2019 16:00:25 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.38 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.123
Tue, 06 Aug 2019 21:09:10 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.37 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.122
Tue, 06 Aug 2019 18:30:23 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.36 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.121
Tue, 06 Aug 2019 15:46:10 GMT

### Changes

- Modify how we setup build directories to better support monorepos (acoates@microsoft.com)

## 0.59.0-vnext.120
Tue, 06 Aug 2019 00:59:54 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.35 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.119
Mon, 05 Aug 2019 17:37:47 GMT

### Changes

- Put back a couple of win32 dll exports (acoates@microsoft.com)
- Updating react-native to version: 0.59.0-microsoft.34 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.118
Mon, 05 Aug 2019 06:09:37 GMT

### Changes

- Include additional jsi headers in nuget (acoates@microsoft.com)

## 0.59.0-vnext.117
Mon, 05 Aug 2019 00:09:47 GMT

### Changes

- Updating react-native to version: 0.59.0-microsoft.33 (53619745+rnbot@users.noreply.github.com)

## 0.59.0-vnext.116
Sat, 03 Aug 2019 17:26:50 GMT

### Changes

- change dev menu shortcut to ctrl+shift+D (kmelmon@microsoft.com)

## 0.59.0-vnext.115
Sat, 03 Aug 2019 04:30:38 GMT

### Changes

- change process.env.TEMP to os.tmpdir() (email not defined)
- fix onFocus/onBlur events for Touchables (andrewh@microsoft.com)
- New switch props (thumbColor and trackColor), and fix few bugs found in TextInputViewManager (dida@ntdev.microsoft.com)
- addedundefined developer menu (kmelmon@microsoft.com)
- fix image not firing on load. (stecrain@microsoft.com)
- Delay-load dependencies absent on Win7 (acoates@microsoft.com)
- Add the SpringAnimationDriver (stpete@microsoft.com)

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
