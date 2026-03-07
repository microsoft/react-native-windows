"use strict";
var __createBinding = (this && this.__createBinding) || (Object.create ? (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    var desc = Object.getOwnPropertyDescriptor(m, k);
    if (!desc || ("get" in desc ? !m.__esModule : desc.writable || desc.configurable)) {
      desc = { enumerable: true, get: function() { return m[k]; } };
    }
    Object.defineProperty(o, k2, desc);
}) : (function(o, m, k, k2) {
    if (k2 === undefined) k2 = k;
    o[k2] = m[k];
}));
var __setModuleDefault = (this && this.__setModuleDefault) || (Object.create ? (function(o, v) {
    Object.defineProperty(o, "default", { enumerable: true, value: v });
}) : function(o, v) {
    o["default"] = v;
});
var __importStar = (this && this.__importStar) || function (mod) {
    if (mod && mod.__esModule) return mod;
    var result = {};
    if (mod != null) for (var k in mod) if (k !== "default" && Object.prototype.hasOwnProperty.call(mod, k)) __createBinding(result, mod, k);
    __setModuleDefault(result, mod);
    return result;
};
Object.defineProperty(exports, "__esModule", { value: true });
const MovingLightNativeComponent_1 = __importStar(require("./MovingLightNativeComponent"));
const React = __importStar(require("react"));
;
function MovingLight(props, ref) {
    const { onSomething, ...restProps } = props;
    const handleSomething = (event) => {
        void onSomething?.(event.nativeEvent.value);
    };
    React.useImperativeHandle(ref, () => ({
        setLightOn(value) {
            if (nativeComponentRef.current != null) {
                MovingLightNativeComponent_1.Commands.setLightOn(nativeComponentRef.current, value);
            }
        }
    }), []);
    const nativeComponentRef = React.useRef(null);
    return (React.createElement(MovingLightNativeComponent_1.default, { ref: nativeComponentRef, onSomething: handleSomething, ...restProps }));
}
const MovingLightWithForwardedRef = React.forwardRef(MovingLight);
exports.default = MovingLightWithForwardedRef;
//# sourceMappingURL=MovingLight.js.map