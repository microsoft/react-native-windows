# dispatchQueue liblet

The **dispatchQueue** liblet implements concurrent and serial dispatcher queues.

The *serial* dispatcher queues execute items sequentially one-by-one. They can
be used as a replacement for threads. Unlike threads they provide more control
over the execution and are not so expensive because in most common cases
dispatch queues only use threads while performing work, and release threads
when there is no pending work.

The default *concurrent* dispatcher queue is a thin wrapper over the platform
specific thread pool. There is also a custom concurrent queue that limits number
of simultaneously running tasks.

## Scheduling tasks for execution

There are two ways how a task can be scheduled for execution: post task to the
end of queue, and to try to execute task immediately if it is possible or else
post to the end of queue.

## Task execution

Tasks are invoked using the underlying platform execution mechanism such as a
thread or a thread pool. For each task queue establishes its context in the
thread local storage.

## Yielding tasks

Some tasks may require more time to execute, but it is important to 'play by the
rules' and allow other tasks in the application to make progress. For the long
running tasks or tasks with lower priority it is suggested to periodically call
ShouldYield() function that indicates that task should yield. It may become true
for one the following reasons: queue is shutting down, the time quota is expired,
or task execution is suspended.

## Task batching

Sometimes it is expensive to execute tasks one-by-one, or there is a requirement
that all tasks in a group are completed. For such scenarios, the dispatch queues
are offering batching mechanism. It is important to understand that a task batch
created in a thread that is posting tasks to the target queue to ensure the
atomic execution guarantee. From the point of view of the target dispatcher
queue, the batch is a just a single task. The batch is created in the thread
local storage and must be submitted or canceled before next batch is started.

## dispatchQueue shutdown

After a queue is shutdown, all newly submitted tasks are canceled. When we
start shutdown we have a choice what to do with the pending tasks in the queue.
They can be either completed or canceled. We cannot cancel a task that is being
executed. We notify yielding task about the queue shutdown, but we must wait
until task is finished.

## dispatchQueue local storage

The dispatch queues allow to associate thread local variables with a queue.
When task is executed, it can lock a queue local value (QLV) that swaps it with a
TLS variable. Unlocking the queue local value swaps it back with the TLS value.
While the QLV is locked it can be read or written to.
When queue is shutdown, it calls destructors for all registered TLS
variables. The QLV is destroyed in a thread that destroys dispatch queue.

## Suspending task execution

A dispatch queue has an API that allows to suspend and resume task execution.
This mechanism can be used for low priority tasks that could be suspended during
perf critical operations such as application boot or when user interacting with
the application.
