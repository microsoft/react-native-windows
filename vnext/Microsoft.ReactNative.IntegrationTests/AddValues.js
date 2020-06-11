import { NativeModules } from 'react-native';

class TestHostModuleFunctions {
    addValues(a, b) {
        NativeModules.TestHostModule.returnInt(a + b);
    }
}

global.__fbBatchedBridge.registerLazyCallableModule('TestHostModuleFunctions', () => new TestHostModuleFunctions());

// Native modules are created on demand from JavaScript code.
NativeModules.TestHostModule.start();
