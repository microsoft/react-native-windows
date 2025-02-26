'use strict';
let _interopRequireDefault=require("@babel/runtime/helpers/interopRequireDefault");
Object.defineProperty(exports,"__esModule",{value:true});
exports.default=exports.__INTERNAL_VIEW_CONFIG=void 0;
let _codegenNativeComponent=_interopRequireDefault(require("react-native/Libraries/Utilities/codegenNativeComponent"));
let NativeComponentRegistry=require('react-native/Libraries/NativeComponent/NativeComponentRegistry');
let nativeComponentName='FabricXamlControl';
let __INTERNAL_VIEW_CONFIG=exports.__INTERNAL_VIEW_CONFIG={uiViewClassName:'FabricXamlControl',validAttributes:{xamlTypeName:true}};
let _default=exports.default=NativeComponentRegistry.get(nativeComponentName,() =>{return __INTERNAL_VIEW_CONFIG;});