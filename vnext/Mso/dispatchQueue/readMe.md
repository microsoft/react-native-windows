# dispatchQueue liblet

The **dispatchQueue** liblet implements concurrent and serial dispatcher queues.

The *serial* dispatcher queues execute items sequentially one-by-one. They can
be used as a replacement for threads. Unlike threads they provide more control
over the execution and are not so expensive because in most common cases
dispatch queues only use threads while performing work, and then release threads
when there is no pending work.

The default *concurrent* dispatcher queue is a thin wrapper over the platform
specific thread pool. There is also a custom concurrent queue that limits number
of simultaneously running tasks.

## Scheduling tasks for execution

There are two ways how a task can be scheduled for execution: post task to the
end of queue, and to try to execute task immediately if it is possible or else
post to the end of queue.

## Yielding tasks

Some tasks may require more time to execute, but it is important to 'play by the
rules' and allow other tasks in the application to make progress. For the long
running tasks or tasks with lower priority it is suggested to use
PostYieldingTask API to schedule a task. The task receives 'shouldYield'
parameter that indicates that task should yield. The task may periodically check
the shouldYield value to finish its work earlier. It may become true for one the
following reasons: queue is shutting down, the task is being cancelled, the time
quota is expired, if there are tasks with higher priority in the queue, or tasks
with that priority are suspended.

## Task execution

Tasks are invoked using the underlying platform execution mechanism such as a
thread or a thread pool. For each task queue establishes its context in the
thread local storage.

## Task batching

Sometimes it is expensive to execute tasks one-by-one, or there is a requirement
that all tasks in a group are completed. For such scenarios, the dispatch queues
are offering batching mechanism. It is important to understand that a task batch
created in a thread that is posting tasks to the target queue to ensure the
atomic execution guarantee. From the point of view of the target dispatcher
queue, the batch is a just a single task. The batch is created in the thread
local storage and must be submitted or cancelled before next batch is started.

## dispatchQueue shutdown

After a queue is shutdown, all newly submitted tasks are cancelled. When we
start shutdown we have a choice what to do with the pending tasks in the queue.
They can be either completed or cancelled. We cannot cancel a task that is being
executed. We notify yielding task about the queue shutdown, but we must wait
until task is finished.

## dispatchQueue local storage

The dispatch queues allow to associate thread local variables with a queue.
Every time the queue starts a task execution, it is swapping all registered TLS
variables from the queue store with the TLS. It swaps them back after the task
completion. When queue is shutdown, it calls destructors for all registered TLS
variables. (TODO: which thread destroys them?)

## Suspending task execution

A dispatch queue has an API that allows to suspend and resume task execution.
This mechanism can be used for low priority tasks that could be suspended during
perf critical operations such as application boot or when user interacting with
the application.
