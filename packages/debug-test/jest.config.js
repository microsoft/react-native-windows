module.exports = {
	// Set up the automation environment before running tests
	testEnvironment: '@react-native-windows/automation',

	// Only run a single test at a time
	maxWorkers: 1,

	// Set up @react-native-windows/automation specific options (see below)
	testEnvironmentOptions: {
		app: 'RNPlayground',
	},

	// Default timeout of a test in milliseconds
	// TODO: test development, revert/lower once tests are working
	testTimeout: 3600000,

	transform: {
		'^.+\\.ts$': 'ts-jest'
	}
};