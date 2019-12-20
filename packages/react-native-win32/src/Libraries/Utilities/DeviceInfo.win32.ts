/**
 * Device info doesn't can't be a global set of data for win32
 * (probably not for android either).
 *
 * Since the API as designed by facebook can't work, we just return null here to crash when used
 * @providesModule DeviceInfo
 */

'use strict';

export = null;
