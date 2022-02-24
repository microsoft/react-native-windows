import * as TurboModuleRegistry from '../Libraries/TurboModule/TurboModuleRegistry';
const CppTurboModule = TurboModuleRegistry.getEnforcing('CppTurboModule');

// Convert function with one callback to Promise
const promisify1 = fn =>
  (...args) => new Promise(
    res => fn(...args, result => res(result)));

// Convert function with two callbacks to Promise
const promisify2 = fn =>
  (...args) => new Promise(
    res => fn(...args, result => res(result), result => res(result)));

(async function runTests() {
  try {
    const c = CppTurboModule.getConstants();
    CppTurboModule.logAction("constantString", c.constantString);
    CppTurboModule.logAction("constantInt", c.constantInt);
    CppTurboModule.logAction("constantString2", c.constantString2);
    CppTurboModule.logAction("constantInt2", c.constantInt2);

    let result;
    result = await promisify1(CppTurboModule.add)(2, 8);
    CppTurboModule.logAction("add", result);
    result = await promisify1(CppTurboModule.negate)(10);
    CppTurboModule.logAction("negate", result);
    result = await promisify1(CppTurboModule.sayHello)();
    CppTurboModule.logAction("sayHello", result);

    CppTurboModule.sayHello0();
    CppTurboModule.printPoint({ x: 1, y: 2 });
    CppTurboModule.printLine({ x: 1, y: 2 }, { x: 3, y: 4 });

    result = await promisify1(CppTurboModule.addCallback)(7, 8);
    CppTurboModule.logAction("addCallback", result);
    result = await promisify1(CppTurboModule.negateCallback)(15);
    CppTurboModule.logAction("negateCallback", result);
    result = await promisify1(CppTurboModule.negateAsyncCallback)(16);
    CppTurboModule.logAction("negateAsyncCallback", result);
    result = await promisify1(CppTurboModule.negateDispatchQueueCallback)(17);
    CppTurboModule.logAction("negateDispatchQueueCallback", result);
    result = await promisify1(CppTurboModule.negateFutureCallback)(18);
    CppTurboModule.logAction("negateFutureCallback", result);
    result = await promisify1(CppTurboModule.sayHelloCallback)();
    CppTurboModule.logAction("sayHelloCallback", result);

    CppTurboModule.callbackZeroArgs(() => CppTurboModule.logAction("callbackZeroArgs", null));
    CppTurboModule.callbackTwoArgs((x, y) => CppTurboModule.logAction("callbackTwoArgs", { x, y }));
    CppTurboModule.callbackThreeArgs((x, y, msg) => CppTurboModule.logAction("callbackThreeArgs", { x, y, msg }));

    result = await promisify2(CppTurboModule.divideCallbacks)(10, 5);
    CppTurboModule.logAction("divideCallbacks", result);
    result = await promisify2(CppTurboModule.divideCallbacks)(10, 0);
    CppTurboModule.logAction("divideCallbacks.error", result);
    result = await promisify2(CppTurboModule.negateCallbacks)(10);
    CppTurboModule.logAction("negateCallbacks", result);
    result = await promisify2(CppTurboModule.negateCallbacks)(-10);
    CppTurboModule.logAction("negateCallbacks.error", result);
    result = await promisify2(CppTurboModule.negateAsyncCallbacks)(10);
    CppTurboModule.logAction("negateAsyncCallbacks", result);
    result = await promisify2(CppTurboModule.negateAsyncCallbacks)(-10);
    CppTurboModule.logAction("negateAsyncCallbacks.error", result);
    result = await promisify2(CppTurboModule.negateDispatchQueueCallbacks)(10);
    CppTurboModule.logAction("negateDispatchQueueCallbacks", result);
    result = await promisify2(CppTurboModule.negateDispatchQueueCallbacks)(-10);
    CppTurboModule.logAction("negateDispatchQueueCallbacks.error", result);
    result = await promisify2(CppTurboModule.negateFutureCallbacks)(10);
    CppTurboModule.logAction("negateFutureCallbacks", result);
    result = await promisify2(CppTurboModule.negateFutureCallbacks)(-10);
    CppTurboModule.logAction("negateFutureCallbacks.error", result);
    result = await promisify2(CppTurboModule.resolveSayHelloCallbacks)();
    CppTurboModule.logAction("resolveSayHelloCallbacks", result);
    result = await promisify2(CppTurboModule.rejectSayHelloCallbacks)();
    CppTurboModule.logAction("rejectSayHelloCallbacks.error", result);

    const twoCallbacksZeroArgs = useFirst => CppTurboModule.twoCallbacksZeroArgs(useFirst,
      () => CppTurboModule.logAction("twoCallbacksZeroArgs1", "success"),
      () => CppTurboModule.logAction("twoCallbacksZeroArgs2", "failure"));
    twoCallbacksZeroArgs(true);
    twoCallbacksZeroArgs(false);
    const twoCallbacksTwoArgs = useFirst => CppTurboModule.twoCallbacksTwoArgs(useFirst,
      (x, y) => CppTurboModule.logAction("twoCallbacksTwoArgs1", { x, y }),
      (x, y) => CppTurboModule.logAction("twoCallbacksTwoArgs2", { x, y }));
    twoCallbacksTwoArgs(true);
    twoCallbacksTwoArgs(false);
    const twoCallbacksThreeArgs = useFirst => CppTurboModule.twoCallbacksThreeArgs(useFirst,
      (x, y, msg) => CppTurboModule.logAction("twoCallbacksThreeArgs1", { x, y, msg }),
      (x, y, msg) => CppTurboModule.logAction("twoCallbacksThreeArgs2", { x, y, msg }));
    twoCallbacksThreeArgs(true);
    twoCallbacksThreeArgs(false);

    await CppTurboModule.dividePromise(10, 2)
      .then(r => CppTurboModule.logAction("dividePromise", r));
    await CppTurboModule.dividePromise(10, 0)
      .catch(e => CppTurboModule.logAction("dividePromise.error", e.message));
    await CppTurboModule.negatePromise(10)
      .then(r => CppTurboModule.logAction("negatePromise", r));
    await CppTurboModule.negatePromise(-10)
      .catch(e => CppTurboModule.logAction("negatePromise.error", e.message));
    await CppTurboModule.negateAsyncPromise(10)
      .then(r => CppTurboModule.logAction("negateAsyncPromise", r));
    await CppTurboModule.negateAsyncPromise(-10)
      .catch(e => CppTurboModule.logAction("negateAsyncPromise.error", e.message));
    await CppTurboModule.negateAsyncPromise(10)
      .then(r => CppTurboModule.logAction("negateDispatchQueuePromise", r));
    await CppTurboModule.negateAsyncPromise(-10)
      .catch(e => CppTurboModule.logAction("negateDispatchQueuePromise.error", e.message));
    await CppTurboModule.negateAsyncPromise(10)
      .then(r => CppTurboModule.logAction("negateFuturePromise", r));
    await CppTurboModule.negateAsyncPromise(-10)
      .catch(e => CppTurboModule.logAction("negateFuturePromise.error", e.message));
    await CppTurboModule.voidPromise(2)
      .then(() => CppTurboModule.logAction("voidPromise", "success"));
    await CppTurboModule.voidPromise(1)
      .catch(e => CppTurboModule.logAction("voidPromise.error", e.message));
    await CppTurboModule.resolveSayHelloPromise()
      .then(r => CppTurboModule.logAction("resolveSayHelloPromise", r));
    await CppTurboModule.rejectSayHelloPromise()
      .catch(e => CppTurboModule.logAction("rejectSayHelloPromise", e.message));

    CppTurboModule.logAction("addSync", CppTurboModule.addSync(40, 2));
    CppTurboModule.logAction("negateSync", CppTurboModule.negateSync(12));
    CppTurboModule.logAction("sayHelloSync", CppTurboModule.sayHelloSync());
  } catch (err) {
    CppTurboModule.logAction("Error", err.message);
  }
})();
