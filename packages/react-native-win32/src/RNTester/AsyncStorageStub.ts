/**
 * Stub implementation of AsyncStorage for RNTester
 *
 * If we get a NativeModule implementation of AsyncStorage then this can be replaced
 * with a real implementation.  Currently the real implementation is a reka module in devmain
 */
'use strict';

/**
 * `AsyncStorage` is a simple, unencrypted, asynchronous, persistent, key-value
 * storage system that is global to the app.  It should be used instead of
 * LocalStorage.
 *
 * See http://facebook.github.io/react-native/docs/asyncstorage.html
 */
class AsyncStorage {
  private _localStorage: { [key: string]: string } = {};

  /**
   * Fetches an item for a `key` and invokes a callback upon completion.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#getitem
   */
  public getItem = (key: string, callback?: (error?: Error, result?: string) => void): Promise<string> => {
    return new Promise((resolve, _reject) => {
      const r = this._localStorage[key] === undefined ? null : this._localStorage[key];
      if (callback) {
        callback(null, r);
      }
      resolve(r);
    });
  };

  /**
   * Sets the value for a `key` and invokes a callback upon completion.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#setitem
   */
  public setItem = (key: string, value: string, callback?: (error?: Error) => void): Promise<void> => {
    return new Promise((resolve, _reject) => {
      this._localStorage[key] = value;
      if (callback) {
        callback();
      }
      resolve();
    });
  };

  /**
   * Removes an item for a `key` and invokes a callback upon completion.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#removeitem
   */
  public removeItem = (key: string, callback?: (error?: Error) => void): Promise<void> => {
    return new Promise((resolve, _reject) => {
      delete this._localStorage[key];
      if (callback) {
        callback();
      }
      resolve();
    });
  };

  /**
   * Erases *all* `AsyncStorage` for all clients, libraries, etc. You probably
   * don't want to call this; use `removeItem` or `multiRemove` to clear only
   * your app's keys.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#clear
   */
  public clear = (callback?: (error?: Error) => void): Promise<void> => {
    return new Promise((resolve, _reject) => {
      this._localStorage = {};
      if (callback) {
        callback();
      }
      resolve();
    });
  };

  /**
   * Gets *all* keys known to your app; for all callers, libraries, etc.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#getallkeys
   */
  public getAllKeys = (callback?: (error?: Error, keys?: Array<string>) => void): Promise<string[]> => {
    return new Promise((resolve, _reject) => {
      if (callback) {
        callback(null, Object.keys(this._localStorage));
      }
      resolve(Object.keys(this._localStorage));
    });
  };

  /**
   * The following batched functions are useful for executing a lot of
   * operations at once, allowing for native optimizations and provide the
   * convenience of a single callback after all operations are complete.
   *
   * These functions return arrays of errors, potentially one for every key.
   * For key-specific errors, the Error object will have a key property to
   * indicate which key caused the error.
   */

  /**
   * Flushes any pending requests using a single batch call to get the data.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#flushgetrequests
   * */
  public flushGetRequests = (): void => {
    return;
  };

  /**
   * This allows you to batch the fetching of items given an array of `key`
   * inputs. Your callback will be invoked with an array of corresponding
   * key-value pairs found.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#multiget
   */
  public multiGet = (
    keys: Array<string>,
    callback?: (errors?: Array<Error>, result?: Array<Array<string>>) => void
  ): Promise<Array<Array<string>>> => {
    return new Promise((resolve, _reject) => {
      const res = [];
      for (const key of keys) {
        const r = this._localStorage[key] === undefined ? null : this._localStorage[key];
        res.push([key, r]);
      }

      if (callback) {
        callback(null, res);
      }
      resolve(res);
    });
  };

  /**
   * Use this as a batch operation for storing multiple key-value pairs. When
   * the operation completes you'll get a single callback with any errors.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#multiset
   */
  public multiSet = (keyValuePairs: Array<Array<string>>, callback?: (errors?: Array<Error>) => void): Promise<void> => {
    return new Promise((resolve, _reject) => {
      for (const pair of keyValuePairs) {
        this._localStorage[pair[0]] = pair[1];
      }
      if (callback) {
        callback(null);
      }
      resolve();
    });
  };

  /**
   * Call this to batch the deletion of all keys in the `keys` array.
   *
   * See http://facebook.github.io/react-native/docs/asyncstorage.html#multiremove
   */
  public multiRemove = (keys: Array<string>, callback?: (errors?: Array<Error>) => void): Promise<void> => {
    return new Promise((resolve, _reject) => {
      for (const key of keys) {
        delete this._localStorage[key];
      }
      if (callback) {
        callback(null);
      }
      resolve();
    });
  };
}

/**
 * Batch operation to merge in existing and new values for a given set of
 * keys. This assumes that the values are stringified JSON.
 *
 * **NOTE**: This is not supported by all native implementations.
 *
 * See http://facebook.github.io/react-native/docs/asyncstorage.html#multimerge
 */
/*
  multiMerge: function(
    keyValuePairs: Array<Array<string>>,
    callback?: ?(errors: ?Array<Error>) => void
  ): Promise {
    return new Promise((resolve, reject) => {
      RCTAsyncStorage.multiMerge(keyValuePairs, function(errors) {
        var error = convertErrors(errors);
        callback && callback(error);
        if (error) {
          reject(error);
        } else {
          resolve(null);
        }
      });
    });
  },
};

// Not all native implementations support merge.
if (!RCTAsyncStorage.multiMerge) {
  delete AsyncStorage.mergeItem;
  delete AsyncStorage.multiMerge;
}
*/

export = new AsyncStorage();
