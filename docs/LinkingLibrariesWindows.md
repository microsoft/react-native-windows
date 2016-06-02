---
id: linking-libraries-windows
title: Linking Libraries
layout: docs
category: Guides (Windows)
permalink: docs/linking-libraries-windows.html
next: running-on-device-windows
---

Most React Native apps will need to take additional dependencies to expose native modules and components that are not available in the core library.

## Manual Linking

### Step 1 - Add Dependency Project to Solution

If the dependency has native code, there should be a `.csproj` file inside it's folder.

- Open your Visual Studio solution.
- Right-click the solution in the Solution Explorer
- Select Add -> Existing Project
- Choose the `.csproj` of the dependency from the Explorer window.

### Step 2 - Add a Reference from your App Project

- Right-click the App project in the Solution Explorer
- Select Add -> Reference
- Choose the project you just included in the solution

### Step 3 - Update MainPage.cs

If the dependency has native code, there should be an implementation of `IReactPackage` in it's project.

- Open MainPage.cs
- Add the `IReactPackage` implementation to the `Packages` list in MainPage.cs

## Automatic Linking

The plan is to integrate with the [RNPM link plugin](http://github.com/RNPM/rnpm-plugin-link). For now, follow the manual instructions.
