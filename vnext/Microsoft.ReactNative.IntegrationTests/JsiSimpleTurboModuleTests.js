import { TurboModuleRegistry } from 'react-native';
const myTrivialTurboModule = TurboModuleRegistry.getEnforcing('MyTrivialTurboModule');

myTrivialTurboModule.startFromJS();
