import { NativeModules } from 'react-native';

class SampleModuleCpp {
    addValues(a, b) {
        NativeModules.TestHostModule.returnInt(a + b);
    }
}

global.__fbBatchedBridge.registerLazyCallableModule('SampleModuleCpp', () => new SampleModuleCpp());
