import { NativeModules } from 'react-native';

class TestHostModuleFunctions {
    addValues(a, b) {
        NativeModules.TestHostModule.returnInt(a + b);
    }
}

// Accessing TestHostModule has a side effect of initializing global.__fbBatchedBridge
if (NativeModules.TestHostModule) {
  global.__fbBatchedBridge.registerLazyCallableModule('TestHostModuleFunctions', () => new TestHostModuleFunctions());

  // Native modules are created on demand from JavaScript code.
  NativeModules.TestHostModule.start();
}
