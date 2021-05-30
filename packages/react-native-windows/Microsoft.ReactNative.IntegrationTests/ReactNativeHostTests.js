import { NativeModules } from 'react-native';

class TestHostModuleFunctions {
    addValues(a, b) {
        NativeModules.TestHostModule.returnResult(a + b);
    }
}

// Accessing TestHostModule has a side effect of initializing global.__fbBatchedBridge
if (NativeModules.TestHostModule) {
  global.__fbBatchedBridge.registerLazyCallableModule('TestHostModuleFunctions', () => new TestHostModuleFunctions());

  // Start running tests.
  NativeModules.TestHostModule.startTests();
}
