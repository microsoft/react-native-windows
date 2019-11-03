# Change Log - rnpm-plugin-windows

This log was last generated on Thu, 17 Oct 2019 20:58:22 GMT and should not be manually modified.

## 0.3.7
Thu, 17 Oct 2019 20:58:22 GMT

### Patches

- Fix two issues: 1) you cannot animated 2 subchannels of the same property with different animations. to fix this we animated yet another property set for translation and scale owned by the props nodes and use one animation to animate all of the subchannels for the uiElement. 2) Reference parameter names which started with a multi digit number are unsupported so i added an n to the start of each name, which was previously just the node's tag. (stpete@microsoft.com)
## 0.3.6
Tue, 15 Oct 2019 13:59:27 GMT

### Patches

- Updates logic for `wpf` command to install correct dependency (erozell@outlook.com)
## 0.3.5
Tue, 01 Oct 2019 18:17:42 GMT

### Patches

- The customer provided examples revealed a few of issues.  One, we were dubble counding the starting value of animatiosn in some cases. Two we were incorrectly relying on JS to tell us to flatten our animated values. Three we were detaching the expression animations that tied the property to the UIElement and in certain cases the JS thread does not inform us that we need to rebuild this cuppling.  There is an open issue with this final part #3280 (stpete@microsoft.com)
## 0.3.0
Sat, 28 Sep 2019 01:21:33 GMT

### Minor changes

- add support for cpp (email not defined)
## 0.2.13
Tue, 24 Sep 2019 19:11:20 GMT

### Patches

- Support latest react-native-cli (acoates@microsoft.com)