/**
 * @providesModule RCTNetworking
 */
'use strict';

// Do not require the native RCTNetworking module directly! Use this wrapper module instead.
// It will add the necessary requestId, so that you don't have to generate it yourself.

import { RCTNetworking } from './Network';

export = new RCTNetworking();
