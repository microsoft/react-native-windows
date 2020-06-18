import * as TurboModuleRegistry from '../Libraries/TurboModule/TurboModuleRegistry';

const sampleTurboModule = TurboModuleRegistry.getEnforcing('SampleTurboModule');

// function calls will be added to cover complete signatures

try {
  sampleTurboModule
    .promiseFunction('something', 1, true);

  sampleTurboModule
    .oneCallback(1, 2, function (r) {
      sampleTurboModule
        .oneCallbackResult(r);
    });

  sampleTurboModule
    .twoCallbacks(true, 123, 'Failed', function (r) {
      sampleTurboModule
        .twoCallbacksResolved(r);
    }, function (r) {
      sampleTurboModule
        .twoCallbacksRejected(r);
    });

  sampleTurboModule
    .twoCallbacks(false, 123, 'Failed', function (r) {
      sampleTurboModule
        .twoCallbacksResolved(r);
    }, function (r) {
      sampleTurboModule
        .twoCallbacksRejected(r);
    });

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
