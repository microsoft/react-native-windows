// Core RN code expects to be able to use #include <FBReactNativeSpec/FBReactNativeSpecJSI.h> to import the generated
// headers. We should look into moving the codegen output into a FBReactNativeSpec folder, and running codegen using
// FBReactNative as the library name But for now this redirection header will suffice
// https://github.com/microsoft/react-native-windows/issues/14129 tracks removing this file
#include <codegen/rnwcoreJSI.h>