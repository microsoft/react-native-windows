import * as TurboModuleRegistry from '../Libraries/TurboModule/TurboModuleRegistry';
const myTrivialTurboModule = TurboModuleRegistry.getEnforcing('MyTrivialTurboModule');

myTrivialTurboModule.startFromJS();
