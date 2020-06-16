import * as TurboModuleRegistry from '../Libraries/TurboModule/TurboModuleRegistry';

const sampleTurboModule = TurboModuleRegistry.getEnforcing('SampleTurboModule');

// function calls will be added to cover complete signatures

try {
  sampleTurboModule
    .promiseFunction('something', 1, true);

  sampleTurboModule
    .syncFunctionResult(
      sampleTurboModule
        .syncFunction('something', 2, false)
    );

  sampleTurboModule
    .constants(
      sampleTurboModule.constantString,
      sampleTurboModule.constantInt
    );

  sampleTurboModule
    .succeeded();
} catch (err) {
  sampleTurboModule.onError(typeof err);
}
