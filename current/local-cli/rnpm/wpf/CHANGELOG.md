# Change Log - rnpm-plugin-wpf

This log was last generated on Tue, 01 Oct 2019 18:17:42 GMT and should not be manually modified.

## 0.2.14
Tue, 01 Oct 2019 18:17:42 GMT

### Patches

- The customer provided examples revealed a few of issues.  One, we were dubble counding the starting value of animatiosn in some cases. Two we were incorrectly relying on JS to tell us to flatten our animated values. Three we were detaching the expression animations that tied the property to the UIElement and in certain cases the JS thread does not inform us that we need to rebuild this cuppling.  There is an open issue with this final part #3280 (stpete@microsoft.com)