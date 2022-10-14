import { TurboModuleRegistry } from 'react-native';
const notificationTestModule = TurboModuleRegistry.getEnforcing('NotificationTestModule');
notificationTestModule.runTest();
