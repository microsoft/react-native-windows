setlocal
pushd ..\..

call yarn 

popd

call yarn react-native run-windows --msbuildprops RestoreForceEvaluate=true
