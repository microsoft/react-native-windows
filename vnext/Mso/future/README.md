# The future liblet

The **future** liblet implements support for task coordination.
It allows specify a continuation for the current future in case if it is
completed successfully or failed. It also allows to coordinate groups of futures
such as observing if all futures in the group are completed, or at least one is
completed.
