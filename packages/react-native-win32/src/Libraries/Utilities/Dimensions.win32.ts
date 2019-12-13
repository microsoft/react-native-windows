/**
 * @providesModule Dimensions
 */
'use strict';
/* tslint:disable:no-any */

class Dimensions {
  // tslint:disable-next-line no-reserved-keywords function-name -- get name matching facebook implementation
  public static get(dim: string) {
    throw new Error('Having a global Dimensions object is too simplistic for Win32, so this API does not work');
  }
  // tslint:disable-next-line no-reserved-keywords function-name -- type name matching facebook implementation
  public static addEventListener(type: string, handler: any) {
    throw new Error('Having a global Dimensions object is too simplistic for Win32, so this API does not work');
  }

  // tslint:disable-next-line no-reserved-keywords function-name -- type name matching facebook implementation
  public static removeEventListener(type: string, handler: any) {
    throw new Error('Having a global Dimensions object is too simplistic for Win32, so this API does not work');
  }
}

export = Dimensions;
