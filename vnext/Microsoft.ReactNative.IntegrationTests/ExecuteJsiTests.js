import { TurboModuleRegistry } from 'react-native';
const testExecuteJsiModule = TurboModuleRegistry.getEnforcing('TestExecuteJsiModule');

testExecuteJsiModule.testHostFunction();
testExecuteJsiModule.testHostObject(); 
testExecuteJsiModule.testSameJsiRuntime();
testExecuteJsiModule.testExecuteJsiPromise();
