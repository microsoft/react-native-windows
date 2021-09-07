# @react-native-windows/automation-commands

`@react-native-windows/automation-commands` provides built-in commands that may
be used with `@react-native-windows/automation-channel` to make it easier to
test your application.

**This package is a work in progress**

## Adding to your application

**TBD**

## Commands

### dumpVisualTree

The `dumpVisualTree` command creates a JSON object corresponding to the XAML
tree under a given testID. This can be used in conjunction with snapshot
testing to validate that your native UI looks how you could expect.

```ts
import {dumpVisualTree} from '@react-native-windows/automation-commands';

test('Widget', async () => {
  const dump = await dumpVisualTree('widget-test-id');
  expect(dump).toMatchSnapshot();
});
```

#### Options
**TBD**