/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 *
 * @providesModule HMRLoadingView
 * @flow
 */

'use strict';

class HMRLoadingView {
  // tslint:disable-next-line function-name -- matching facebook name
  public static showMessage(message: string) {
    console.warn('HMRLoadingView is not yet supported in win32');
  }

  // tslint:disable-next-line function-name -- matching facebook name
  public static hide() {
    console.warn('HMRLoadingView is not yet supported in win32');
  }
}

export = HMRLoadingView;
