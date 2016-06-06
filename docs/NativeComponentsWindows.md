---
id: native-components-windows
title: Native UI Components
layout: docs
category: Guides (Windows)
permalink: docs/native-components-windows.html
next: running-on-device-windows
---

There are tons of native UI widgets out there ready to be used in the latest apps - some of them are part of the platform, others are available as third-party libraries, and still more might be in use in your very own portfolio. React Native has several of the most critical platform components already wrapped, like `ScrollView` and `TextInput`, but not all of them, and certainly not ones you might have written yourself for a previous app. Fortunately, it's quite easy to wrap up these existing components for seamless integration with your React Native application.

Like the native module guide, this too is a more advanced guide that assumes you are somewhat familiar with the Universal Windows SDK programming. This guide will show you how to build a native UI component, walking you through the implementation of a subset of the existing `ImageView `component available in the core React Native library.