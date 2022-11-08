import { TurboModuleRegistry } from 'react-native';

class TestHostModuleFunctions {
    addValues(a, b) {
      TurboModuleRegistry.get('TestHostModule').returnResult(a + b);
    }
}

// Accessing TestHostModule has a side effect of initializing global.__fbBatchedBridge
if (TurboModuleRegistry.get('TestHostModule')) {
  global.__fbBatchedBridge.registerLazyCallableModule('TestHostModuleFunctions', () => new TestHostModuleFunctions());

  // Start running tests.
  TurboModuleRegistry.get('TestHostModule').startTests();
}
