import { TurboModuleRegistry, registerCallableModule } from 'react-native';

const TestHostModuleFunctions = {
    addValues(a, b) {
      TurboModuleRegistry.get('TestHostModule').returnResult(a + b);
    }
}

if (TurboModuleRegistry.get('TestHostModule')) {
  registerCallableModule('TestHostModuleFunctions', TestHostModuleFunctions);

  // Start running tests.
  TurboModuleRegistry.get('TestHostModule').startTests();
}
