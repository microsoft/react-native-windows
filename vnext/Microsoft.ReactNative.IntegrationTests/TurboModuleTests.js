import * as TurboModuleRegistry from '../Libraries/TurboModule/TurboModuleRegistry';

const sampleTurboModule = TurboModuleRegistry.getEnforcing('SampleTurboModule');

// function calls will be added to cover complete signatures
sampleTurboModule.voidFunction();
