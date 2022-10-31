import { TurboModuleRegistry } from 'react-native';
const myTurboModule = TurboModuleRegistry.getEnforcing('MyTurboModule');

// Convert function with one callback to Promise
const promisify1 = fn =>
  (...args) => new Promise(
    res => fn(...args, result => res(result)));

// Convert function with two callbacks to Promise
const promisify2 = fn =>
  (...args) => new Promise(
    res => fn(...args, result => res(result), result => res(result)));

(async function runTests() {
  const testName = myTurboModule.getTestName();
  try {
    if (testName === "ExecuteSampleTurboModule") {
      const c = myTurboModule.getConstants();
      myTurboModule.logAction("constantString", c.constantString);
      myTurboModule.logAction("constantInt", c.constantInt);
      myTurboModule.logAction("constantString2", c.constantString2);
      myTurboModule.logAction("constantInt2", c.constantInt2);
      myTurboModule.logAction("constantString3", c.constantString3);
      myTurboModule.logAction("constantInt3", c.ConstantInt3);

      let result;
      result = await promisify1(myTurboModule.add)(2, 8);
      myTurboModule.logAction("add", result);
      result = await promisify1(myTurboModule.negate)(10);
      myTurboModule.logAction("negate", result);
      result = await promisify1(myTurboModule.sayHello)();
      myTurboModule.logAction("sayHello", result);

      myTurboModule.sayHello0();
      myTurboModule.printPoint({ x: 1, y: 2 });
      myTurboModule.printLine({ x: 1, y: 2 }, { x: 3, y: 4 });

      result = await promisify1(myTurboModule.addCallback)(7, 8);
      myTurboModule.logAction("addCallback", result);
      result = await promisify1(myTurboModule.negateCallback)(15);
      myTurboModule.logAction("negateCallback", result);
      result = await promisify1(myTurboModule.negateAsyncCallback)(16);
      myTurboModule.logAction("negateAsyncCallback", result);
      result = await promisify1(myTurboModule.negateTaskRunCallback)(17);
      myTurboModule.logAction("negateTaskRunCallback", result);
      result = await promisify1(myTurboModule.sayHelloCallback)();
      myTurboModule.logAction("sayHelloCallback", result);

      myTurboModule.callbackZeroArgs(() => myTurboModule.logAction("callbackZeroArgs", null));
      myTurboModule.callbackTwoArgs((x, y) => myTurboModule.logAction("callbackTwoArgs", { x, y }));
      myTurboModule.callbackThreeArgs((x, y, msg) => myTurboModule.logAction("callbackThreeArgs", { x, y, msg }));

      result = await promisify2(myTurboModule.divideCallbacks)(10, 5);
      myTurboModule.logAction("divideCallbacks", result);
      result = await promisify2(myTurboModule.divideCallbacks)(10, 0);
      myTurboModule.logAction("divideCallbacks.error", result);
      result = await promisify2(myTurboModule.negateCallbacks)(10);
      myTurboModule.logAction("negateCallbacks", result);
      result = await promisify2(myTurboModule.negateCallbacks)(-10);
      myTurboModule.logAction("negateCallbacks.error", result);
      result = await promisify2(myTurboModule.negateAsyncCallbacks)(10);
      myTurboModule.logAction("negateAsyncCallbacks", result);
      result = await promisify2(myTurboModule.negateAsyncCallbacks)(-10);
      myTurboModule.logAction("negateAsyncCallbacks.error", result);
      result = await promisify2(myTurboModule.negateTaskRunCallbacks)(10);
      myTurboModule.logAction("negateTaskRunCallbacks", result);
      result = await promisify2(myTurboModule.negateTaskRunCallbacks)(-10);
      myTurboModule.logAction("negateTaskRunCallbacks.error", result);
      result = await promisify2(myTurboModule.resolveSayHelloCallbacks)();
      myTurboModule.logAction("resolveSayHelloCallbacks", result);
      result = await promisify2(myTurboModule.rejectSayHelloCallbacks)();
      myTurboModule.logAction("rejectSayHelloCallbacks.error", result);

      const twoCallbacksZeroArgs = useFirst => myTurboModule.twoCallbacksZeroArgs(useFirst,
        () => myTurboModule.logAction("twoCallbacksZeroArgs1", "success"),
        () => myTurboModule.logAction("twoCallbacksZeroArgs2", "failure"));
      twoCallbacksZeroArgs(true);
      twoCallbacksZeroArgs(false);
      const twoCallbacksTwoArgs = useFirst => myTurboModule.twoCallbacksTwoArgs(useFirst,
        (x, y) => myTurboModule.logAction("twoCallbacksTwoArgs1", { x, y }),
        (x, y) => myTurboModule.logAction("twoCallbacksTwoArgs2", { x, y }));
      twoCallbacksTwoArgs(true);
      twoCallbacksTwoArgs(false);
      const twoCallbacksThreeArgs = useFirst => myTurboModule.twoCallbacksThreeArgs(useFirst,
        (x, y, msg) => myTurboModule.logAction("twoCallbacksThreeArgs1", { x, y, msg }),
        (x, y, msg) => myTurboModule.logAction("twoCallbacksThreeArgs2", { x, y, msg }));
      twoCallbacksThreeArgs(true);
      twoCallbacksThreeArgs(false);

      await myTurboModule.dividePromise(10, 2)
        .then(r => myTurboModule.logAction("dividePromise", r));
      await myTurboModule.dividePromise(10, 0)
        .catch(e => myTurboModule.logAction("dividePromise.error", e.message));
      await myTurboModule.negatePromise(10)
        .then(r => myTurboModule.logAction("negatePromise", r));
      await myTurboModule.negatePromise(-10)
        .catch(e => myTurboModule.logAction("negatePromise.error", e.message));
      await myTurboModule.negateAsyncPromise(10)
        .then(r => myTurboModule.logAction("negateAsyncPromise", r));
      await myTurboModule.negateAsyncPromise(-10)
        .catch(e => myTurboModule.logAction("negateAsyncPromise.error", e.message));
      await myTurboModule.negateTaskRunPromise(10)
        .then(r => myTurboModule.logAction("negateTaskRunPromise", r));
      await myTurboModule.negateTaskRunPromise(-10)
        .catch(e => myTurboModule.logAction("negateTaskRunPromise.error", e.message));
      await myTurboModule.voidPromise(2)
        .then(() => myTurboModule.logAction("voidPromise", "success"));
      await myTurboModule.voidPromise(1)
        .catch(e => myTurboModule.logAction("voidPromise.error", e.message));
      await myTurboModule.resolveSayHelloPromise()
        .then(r => myTurboModule.logAction("resolveSayHelloPromise", r));
      await myTurboModule.rejectSayHelloPromise()
        .catch(e => myTurboModule.logAction("rejectSayHelloPromise", e.message));

      await myTurboModule.divideTask(10, 2)
        .then(r => myTurboModule.logAction("divideTask", r));
      await myTurboModule.divideTask(10, 0)
        .catch(e => myTurboModule.logAction("divideTask.error", e.message));
      await myTurboModule.divideAsyncTask(10, 5)
        .then(r => myTurboModule.logAction("divideAsyncTask", r));
      await myTurboModule.divideAsyncTask(10, 0)
        .catch(e => myTurboModule.logAction("divideAsyncTask.error", e.message));
      await myTurboModule.voidTask(2)
        .then(() => myTurboModule.logAction("voidTask", "success"));
      await myTurboModule.voidTask(1)
        .catch(e => myTurboModule.logAction("voidTask.error", e.message));
      await myTurboModule.voidAsyncTask(2)
        .then(() => myTurboModule.logAction("voidAsyncTask", "success"));
      await myTurboModule.voidAsyncTask(1)
        .catch(e => myTurboModule.logAction("voidAsyncTask.error", e.message));

      myTurboModule.logAction("addSync", myTurboModule.addSync(40, 2));
      myTurboModule.logAction("negateSync", myTurboModule.negateSync(12));
      myTurboModule.logAction("sayHelloSync", myTurboModule.sayHelloSync());
    }
  } catch (err) {
    myTurboModule.logAction("Error", err.message);
  }
})();
