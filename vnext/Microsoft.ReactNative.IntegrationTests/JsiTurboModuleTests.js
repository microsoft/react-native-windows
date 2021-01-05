import * as TurboModuleRegistry from '../Libraries/TurboModule/TurboModuleRegistry';
const mySimpleTurboModule = TurboModuleRegistry.getEnforcing('MySimpleTurboModule');

// The logging of the TurboModule functions is verified against the test action sequence.

mySimpleTurboModule.voidFunc();
mySimpleTurboModule.logAction("getBool result", mySimpleTurboModule.getBool(true));
mySimpleTurboModule.logAction("getBool result", mySimpleTurboModule.getBool(false));
mySimpleTurboModule.logAction("getNumber result", mySimpleTurboModule.getNumber(5));
mySimpleTurboModule.logAction("getNumber result", mySimpleTurboModule.getNumber(Number.NaN));
mySimpleTurboModule.logAction("getNumber result", mySimpleTurboModule.getNumber(Number.POSITIVE_INFINITY));
mySimpleTurboModule.logAction("getString result", mySimpleTurboModule.getString("Hello"));
mySimpleTurboModule.logAction("getString result", mySimpleTurboModule.getString(""));

const arr1 = mySimpleTurboModule.getArray(["Hello", 5, false]);
const arr1Matches = (arr1.length === 3) && (arr1[0] === "Hello") && (arr1[1] === 5) && (arr1[2] === false);
mySimpleTurboModule.logAction("getArray result", arr1Matches ? "OK" : "Failed");

const obj1 = mySimpleTurboModule.getObject({ x: "Hello", y: 5, z: false });
const obj1Matches = (obj1.x === "Hello") && (obj1.y === 5) && (obj1.z === false);
mySimpleTurboModule.logAction("getObject result", obj1Matches ? "OK" : "Failed");

const obj2 = mySimpleTurboModule.getValue(42, "Hello", { x: 5, y: 12 });
const obj2Matches = (obj2.x === 42) && (obj2.y === "Hello") && (obj2.z.x === 5) && (obj2.z.y === 12);
mySimpleTurboModule.logAction("getValue result", obj2Matches ? "OK" : "Failed");

const obj3 = mySimpleTurboModule.getConstants();
const obj3Matches = (obj3.const1 === true) && (obj3.const2 === 375) && (obj3.const3 === "something");
mySimpleTurboModule.logAction("getConstants result", obj3Matches ? "OK" : "Failed");

mySimpleTurboModule.getValueWithCallback(
  (value) => mySimpleTurboModule.logAction("getValueWithCallback result", value));

mySimpleTurboModule.getValueWithPromise(/*error:*/false)
  .then((value) => mySimpleTurboModule.logAction("getValueWithPromise result resolve", value));

mySimpleTurboModule.getValueWithPromise(/*error:*/true).catch(
  (err) => mySimpleTurboModule.logAction("getValueWithPromise result reject", err.message));
