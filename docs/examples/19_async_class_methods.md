# Zephyr Async Class Methods

This document details async method definitions in classes, spawn functionality, parallel execution, Promise.all patterns, mixed async programming, const parameter protection, and advanced async object-oriented programming.

---

## Overview

Async class methods in Zephyr allow you to run object-oriented asynchronous operations in parallel, with full support for const parameters and error handling. Spawn works with both sync and async methods, enabling powerful concurrent workflows.

---

## Async Method Definitions in Classes with Const Parameters

**Description:**  
Define async methods in classes, protecting parameters with `const`.

```zephyr
class Worker {
    async func do_work(const job) {
        print("Working on", job)
        return "Done: " + job
    }
}
w = Worker()
print(await w.do_work("Task"))
```

---

## Spawn Functionality with Sync and Async Method Calls

**Description:**  
Run both sync and async methods in parallel using `spawn`.

```zephyr
class Runner {
    async func run(const x) { return x * 2 }
    func sync_run(x) { return x + 1 }
}
r = Runner()
promise1 = spawn r.run(10)
promise2 = spawn r.sync_run(5)
print(await promise1, await promise2)
```

---

## Parallel Execution of Object-Oriented Async Operations

**Description:**  
Execute multiple async class methods concurrently and aggregate results.

```zephyr
class Downloader {
    async func download(const url) { return "Downloaded " + url }
}
d = Downloader()
promises = [spawn d.download("a.com"), spawn d.download("b.com")]
results = await all(promises)
print(results)
```

---

## Promise.all Patterns with Spawned Method Calls

**Description:**  
Wait for multiple spawned method calls to complete using `all()`.

```zephyr
class Batch {
    async func process(const id) { return "Batch " + id }
}
b = Batch()
promises = [spawn b.process("A"), spawn b.process("B")]
print(await all(promises))
```

---

## Mixed Function and Method Call Async Programming

**Description:**  
Combine async functions and class methods in parallel workflows.

```zephyr
async func get(x) { return x }
class C { async func f(x) { return x * 2 } }
c = C()
promises = [spawn get(1), spawn c.f(2)]
print(await all(promises))
```

---

## Const Parameter Protection in Async Class Context

**Description:**  
Ensure async method parameters marked as `const` remain immutable.

```zephyr
class Protector {
    async func safe(const val) {
        try { val = 99 } catch e { print("Const error:", e) }
        return val
    }
}
p = Protector()
print(await p.safe(42))
```

---

## Advanced Async Object-Oriented Programming Patterns

**Description:**  
Build complex async workflows with classes, spawn, and Promise.all.

```zephyr
class Pipeline {
    async func stage(const x) { return x + 1 }
}
pipe = Pipeline()
promises = []
for i = 1; i <= 3; i++ { append(promises, spawn pipe.stage(i)) }
print(await all(promises))
```

---
