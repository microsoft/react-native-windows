setlocal
pushd ..\..

call yarn || goto :fail

popd

call yarn react-native run-windows --msbuildprops RestoreForceEvaluate=true || goto :fail

echo Success!
exit /b 0

echo FAILED.
exit /b 1
