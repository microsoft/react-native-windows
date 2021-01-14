import { NativeModules } from 'react-native';

const { TestExecuteJsiModule } = NativeModules;

TestExecuteJsiModule.testSimpleExecuteJsi();
TestExecuteJsiModule.testHostFunction();
TestExecuteJsiModule.testHostObject();
TestExecuteJsiModule.testSameJsiRuntime();
