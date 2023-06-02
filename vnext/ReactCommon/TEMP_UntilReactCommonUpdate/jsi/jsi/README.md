# JavaScript Interface 

This folder contains definitions for JSI.
JSI is the public API for Hermes engine.
It is being used by React Native project to work with JS engines.

## JSI versions

JSI has versions associated with the following commit hashes in the 
https://github.com/facebook/hermes repo. 

| Version | Commit Hash                              | Commit Description
|--------:|:-----------------------------------------|------------------------------------------------------
|      10 | `b81666598672cb5f8b365fe6548d3273f216322e` | Clarify const-ness of JSI references
|       9 | `e6d887ae96bef5c71032f11ed1a9fb9fecec7b46` | Add external ArrayBuffers to JSI
|       8 | `4d64e61a1f9926eca0afd4eb38d17cea30bdc34c` | Add BigInt JSI API support
|         | `e8cc57311877464478da5421265bcc898098e136` | Add methods for creating and interacting with BigInt
|       7 | `4efad65b8266e3f26e04e3ca9addf92fc4d6ded8` | Add API for setting/getting native state
|       6 | `bc3cfb87fbfc82732936ec4445c9765bf9a5f08a` | Add BigInt skeleton
|       5 | `2b6d408980d7f23f50602fd88169c8a9881592a6` | Add PropNameID::fromSymbol
|       4 | `a5bee55c8301bb8662e408feee28bbc3e2a1fc81` | Introduce drainMicrotasks to JSI
|       3 | `0c9daa5a5eee7649558a53e3e541b80c89048c42` | Change jsi::Runtime::lockWeakObject to take a mutable ref
|       2 | `e0616e77e1ddc3ea5e2ccbca2e20dd0c4049c637` | Make it possible for a Runtime implementation to provide its own JSON parsing
|       1 | `3ba9615f80913764ecb6456779d502e31dde9e5d` | Fix build break in MSVC (#26462)
|       0 | `f22a18f67da3f03db59c1ec715d6ec3776b03fbf` | Initial commit