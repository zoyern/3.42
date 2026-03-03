# 3.42 — EXEMPLES DE SÉCURITÉ MÉMOIRE (Q21)
## Code Examples Illustrating Memory Safety Rules

> **Companion to** : MEMORY_SAFETY_FORMAL_Q21.md
> **Format** : Code 3.42 avec annotations de sécurité

---

## Table des Matières

1. [Patterns Sûrs](#1-patterns-sûrs)
2. [Patterns Interdits](#2-patterns-interdits)
3. [Arena Boundaries](#3-arena-boundaries)
4. [Bubble-up Sûr](#4-bubble-up-sûr)
5. [Borrow Checker en Action](#5-borrow-checker-en-action)
6. [Edge Cases Formalisés](#6-edge-cases-formalisés)

---

## 1. Patterns Sûrs

### 1.1 MOVE Default (Linear Types)

```
# ✅ SAFE: Ownership transfer via MOVE

main {
    x = 42;                    // x owns 42, type int @ 'main
    y = x;                     // MOVE: x → y
                               // y now owns 42
                               // x is invalidated
    << y;                      // return y's value
    // x is inaccessible beyond this point
}

// Type error if we tried:
//   z = x;  ← COMPILE ERROR: x was already moved to y
```

### 1.2 COPY Explicit (Linear Types)

```
# ✅ SAFE: Explicit copy when needed

func duplicate(orig) {
    {
        # orig is in parent arena
        # we want a copy in this nested arena
        copy = ** orig;        // COPY: deep copy to current arena
                               // orig still owns original
                               // copy owns new allocation
        process(copy);
        # copy deallocated at scope exit
    }
    # orig deallocated at parent scope exit
    << orig;                   // still valid
}

# USE:
original = { x: 1, y: 2 };
result = duplicate(original);
# Allocations:
#   original @ main arena
#   copy @ nested arena (cleaned up)
#   result @ main arena
```

### 1.3 BORROW Read-Only (@)

```
# ✅ SAFE: Read-only reference (no ownership transfer)

func print_data(data@) {
    # data is borrowed (can read, not write)
    # data's original owner still owns it
    x = data.x;
    y = data.y;
    << { x: x, y: y };
}

main {
    original = { x: 1, y: 2 };
    result = print_data(original@);  # borrow for reading
    # original still accessible
    process(original);              # OK: still owned
    << result;
}
```

### 1.4 EXCLUSIVE BORROW Write (@@ )

```
# ✅ SAFE: Exclusive write access

func modify_data(data@@) {
    # data is borrowed exclusively (can read and write)
    # no other references to data allowed during this borrow
    data.x = 42;
    data.y = 99;
    # changes visible to original owner
}

main {
    config = { x: 0, y: 0 };

    # Can borrow exclusively
    modify_data(config@@);         # exclusive borrow

    # config now has updated values
    config.x ? {
        + : <<+ "x is truthy";
        _ : <<- "x is falsy";
    };
}

# Type error if we tried multiple exclusive borrows:
//   ref1 = config@@;
//   ref2 = config@@;  ← COMPILE ERROR: config already borrowed exclusively
```

### 1.5 Combination: MOVE then COPY

```
# ✅ SAFE: Strategic MOVE and COPY

func process_large_data(data) {
    {
        # data moved into this scope
        expensive_copy = ** data;    # copy for local processing

        result = {
            processed: expensive_copy;
            metadata: compute_meta(expensive_copy@);
        };

        << result;
        # expensive_copy deallocated (never re-used)
    }
    # data already moved, not accessible here
}

main {
    big_data = fetch_data();        # large allocation
    final = process_large_data(big_data);  # MOVE to function

    # big_data is inaccessible now (moved)
    # but final.processed contains result

    output(final.processed);        # processed data accessible
    << final;
}
```

---

## 2. Patterns Interdits

### 2.1 ❌ Use-After-Free

```
# FORBIDDEN: Accessing after MOVE

main {
    x = 42;
    y = x;               # MOVE: x → y, x invalidated

    z = x;               # ❌ COMPILE ERROR: use after free
                         # x was already moved to y

    << z;
}

FIX:
main {
    x = 42;
    y = x;               # MOVE
    z = y;               # move from y instead
    << z;
}
```

### 2.2 ❌ Double-Free

```
# FORBIDDEN: Free same memory twice

main {
    {
        arena_A {
            x = alloc(100);
        }
        # x deallocated (arena A closed)

        # Try to access x in parent scope
        y = x;           # ❌ COMPILE ERROR: x already freed
    }
}

FIX: Use linear types to prevent this
# If x is moved out of arena A, it can still be accessed
main {
    {
        arena_A {
            x = alloc(100);
            y = x;       # MOVE x out
            << y;        # return it
        }
    }
    # y still accessible (owned by parent)
    # x automatically deallocated with arena_A? No, it was MOVED
}
```

### 2.3 ❌ Cross-Arena Reference (FORBIDDEN)

```
# FORBIDDEN: Reference escaping child arena

main {
    ptr = null;
    {
        # Child arena
        x = 42;
        ptr = @x;        # ❌ COMPILE ERROR: cross-arena reference
                         # x @ 'child, ptr escapes to 'parent
                         # when child closes, x deallocated
    }
    # x deallocated, ptr dangling
    y = *ptr;            # UAF!
}

FIX 1: Use MOVE instead
main {
    {
        x = 42;
        y = x;           # MOVE x out (now y owns it)
        << y;            # return it
    }
    # y accessible in parent (never deallocated)
}

FIX 2: Copy data up
main {
    {
        x = 42;
        y = ** x;        # COPY x to parent arena
        << y;
    }
    # y is copy, safe to use
}
```

### 2.4 ❌ Struct with Child Arena Reference

```
# FORBIDDEN: Struct field holding child arena pointer

struct Parser {
    buffer: @ [u8];      # reference field
}

main {
    parser = null;
    {
        # Child arena
        data = [1, 2, 3, 4, 5];
        parser = { buffer: @data };  # ❌ ERROR: struct field holds child ref
                                     # parser escapes, data deallocated
    }
    # parser.buffer dangling
    parser.buffer[0];    # UAF!
}

FIX 1: Copy data into parser
main {
    {
        data = [1, 2, 3, 4, 5];
        parser = { buffer: ** data };  # COPY, not reference
        << parser;                     # parser owns copy
    }
    # parser safely accessible
}

FIX 2: Parser stays in child scope
main {
    {
        data = [1, 2, 3, 4, 5];
        parser = { buffer: @data };
        result = process_with_parser(parser);
        << result;                     # only result escapes
    }
    # parser never leaves scope, no issue
}
```

### 2.5 ❌ Shared Mutable State (Data Race)

```
# FORBIDDEN: Multiple mutable references (no synchronization)

main {
    x = { value: 0 };

    ref1 = @@ x;        # exclusive borrow
    ref2 = @@ x;        # ❌ COMPILE ERROR: x already borrowed exclusively
                        # can't have two exclusive borrows

    *ref1 = 42;
    *ref2 = 99;         # potential data race!
}

FIX: One exclusive borrow at a time
main {
    x = { value: 0 };

    {
        ref1 = @@ x;    # exclusive borrow
        *ref1 = 42;
    }                   # borrow expires

    {
        ref2 = @@ x;    # OK: now exclusively borrow
        *ref2 = 99;
    }

    << x;
}

FIX 2: Use synchronization for true concurrency
main {
    x = { value: 0 };

    task1 = | {
        {
            ref = @@ x;
            *ref = 42;
        }
    };

    task2 = | {
        {
            ref = @@ x;
            *ref = 99;
        }
    };

    !| { };              # fence: synchronization point
    task1 |?;
    task2 |?;

    << x;
}
```

---

## 3. Arena Boundaries

### 3.1 Nested Scopes = Nested Arènes

```
# ✅ SAFE: Proper arena nesting

main {  // Arena 'main
    x = 1;

    {  // Arena 'scope1 (child of main)
        y = 2;
        z = x@ ;         // ✅ OK: reference to parent arena

        {  // Arena 'scope2 (child of scope1, grandchild of main)
            a = 3;
            b = y@ ;     // ✅ OK: reference to parent scope1
            c = z@@ ;    // ❌ FORBIDDEN: z is a reference, can't double-borrow
            d = x@ ;     // ✅ OK: reference jumps scopes (points to ancestor)
        }
        // scope2 closed, a deallocated, b and d references expire

    }
    // scope1 closed, y deallocated, z reference expires

    // x still accessible (it's in 'main arena)
    << x;
}
```

### 3.2 Lifetime Ordering

```
# Lifetimes form a hierarchy:

'main (longest)
  ├─ 'outer_scope
  │   ├─ 'middle_scope
  │   │   └─ 'inner_scope (shortest)
  │   │
  │   └─ 'sibling_scope
  │
  └─ 'another_outer

Rule: Reference cannot point "downward" (to shorter lifetime)

  ref @ 'main → 'inner_scope     ❌ FORBIDDEN
  ref @ 'inner_scope → 'main     ✅ OK
  ref @ 'outer_scope → 'middle   ❌ FORBIDDEN
  ref @ 'middle → 'outer_scope   ✅ OK
```

### 3.3 Bubble-Up as Exception

```
# ✅ SAFE: Controlled cross-boundary via bubble-up

func allocate_and_return() {
    {  // Inner arena
        data = alloc(1000);
        metadata = { size: 1000, valid: + };
        << { data: data, meta: metadata };
    }
    // Inner arena deallocates, BUT:
    // Return value keeps inner arena alive as long as needed
}

main {
    result = allocate_and_return();

    # result.data is in "bubble-up" state
    # It's accessible because result is in main arena
    # The callee's arena is kept alive (not deallocated immediately)

    extracted = result.data;       // MOVE: extract to main arena

    # After extraction:
    # extracted is in main arena
    # result.data is consumed (MOVE semantics)
    # Callee's arena can now be deallocated

    use(extracted);
    << extracted;
}
```

---

## 4. Bubble-up Sûr

### 4.1 Accessing Fields via Bubble-Up

```
# ✅ SAFE: Field access moves data to parent arena

func compute() {
    {  // compute's arena
        result = {
            value: 42,
            debug_info: "computation done",
            stats: { iterations: 100, time_ms: 50 }
        };
        <<+ result;        // return with positive spin
    }
    // compute's arena still open (result referenced by caller)
}

main {
    output = compute();    // output is return value (with bubble-up)

    # Access fields via . operator (causes MOVE)
    value = output.value;           // MOVE value → main arena
    info = output.debug_info;       // MOVE info → main arena
    stats = output.stats;           // MOVE stats → main arena

    # After MOVEs:
    # value, info, stats live in main arena
    # compute's arena can deallocate

    process(value, info, stats);
    << value;
}
```

### 4.2 Partial Consumption

```
# ✅ SAFE: Only access what you need (rest deallocated early)

func expensive_computation() {
    {
        x = expensive_op1();  // huge allocation
        y = expensive_op2();  // huge allocation
        z = expensive_op3();  // huge allocation

        <<+ { x: x, y: y, z: z };
    }
}

main {
    result = expensive_computation();

    # Only use x, ignore y and z
    only_x = result.x;     // MOVE x → main
    # y and z deallocated (never accessed)
    # compute's arena can now release y and z

    use(only_x);
    # y, z freed (optimization: escape analysis)
    # only_x kept in main

    << only_x;
}
```

### 4.3 Chain of Returns (Bubble-up through Layers)

```
# ✅ SAFE: Bubble-up through multiple function calls

func level3() {
    {
        data = [1, 2, 3];
        meta = "level3";
        <<+ { data: data, meta: meta };
    }
}

func level2() {
    {
        inner = level3();     // bubble-up from level3
        info = "level2";

        # bubble-up from inner without consuming all fields
        result = {
            nested: inner,    // MOVE inner (with its bubble-up)
            annotation: info
        };
        <<+ result;
    }
}

func level1() {
    {
        layer = level2();     // bubble-up from level2
        header = "level1";

        final = {
            content: layer,
            header: header
        };
        <<+ final;
    }
}

main {
    top = level1();

    # Access deeply nested via . chain
    data = top.content.nested.data;  // MOVE through chain

    use(data);
    << data;
}
```

---

## 5. Borrow Checker en Action

### 5.1 Read-Only Borrow vs. Modification

```
# ✅ SAFE: Borrow checker enforces read-only

func read_config(cfg@) {
    # cfg is borrowed (read-only)
    # Cannot modify cfg

    x = cfg.timeout;       // ✅ OK: read
    y = cfg.retries;       // ✅ OK: read

    # cfg.timeout = 100;   // ❌ COMPILE ERROR: cfg is read-only

    << { timeout: x, retries: y };
}

main {
    config = { timeout: 30, retries: 3 };

    result = read_config(config@);  // borrow for reading

    # config still accessible
    config.timeout = 60;             // ✅ OK: original owner modifies

    use(result);
    << result;
}
```

### 5.2 Exclusive Borrow for Modification

```
# ✅ SAFE: Exclusive borrow allows modification

func reconfigure(cfg@@) {
    # cfg is borrowed exclusively
    # Can read and write
    # No other access allowed

    cfg.timeout = 100;     // ✅ OK: write
    x = cfg.retries;       // ✅ OK: read
    cfg.retries = 5;       // ✅ OK: write

    << cfg;
}

main {
    config = { timeout: 30, retries: 3 };

    reconfigure(config@@);  // exclusive borrow

    # config has updated values
    config.timeout ? {
        +  : << "timeout set";
        -  : << "timeout invalid";
        _  : << "timeout undefined";
        #  : << "timeout error";
    };
}
```

### 5.3 No Concurrent Borrows

```
# ❌ FORBIDDEN: Concurrent read-only and exclusive

main {
    x = 10;

    ref_read = @x;         // ✅ OK: read-only borrow
    ref_write = @@ x;      // ❌ COMPILE ERROR: x already borrowed
                           // (can't mix read and exclusive)
}

# ✅ SAFE: Sequential borrows

main {
    x = 10;

    {
        ref_read = @x;     // ✅ OK: read-only borrow
        y = *ref_read;
    }                      // borrow expires

    {
        ref_write = @@ x;  // ✅ OK: exclusive borrow (now allowed)
        *ref_write = 20;
    }                      // borrow expires

    << x;
}
```

---

## 6. Edge Cases Formalisés

### 6.1 Recursive Functions with TCO

```
# ✅ SAFE: Tail recursion optimized (no stack growth)

func sum_helper(arr, idx, acc) {
    idx >= arr.len ? {
        + : <<+ acc;
        - : >> sum_helper(arr, idx + 1, acc + arr[idx]);
    };
}

func sum(arr) {
    sum_helper(arr@, 0, 0);
}

# >> (continue) in tail position → compiled to loop, not recursion
# Stack depth = O(1) instead of O(n)
# No new arena per iteration
```

### 6.2 Closures Capturing Outer Scope

```
# ✅ SAFE: Closure captures data from parent scope

main {
    x = 42;

    handler = fn() {
        # handler captures x (from parent scope)
        # x has lifetime 'main
        # handler's lifetime ≤ 'main
        # OK to capture

        << x;
    };

    result = handler();
    << result;
}

# ❌ FORBIDDEN: Closure capturing child scope

main {
    handler = null;

    {
        # Child scope
        x = 42;

        handler = fn() {
            # handler captures x (from child scope)
            # x has lifetime 'child
            # handler captured in parent (lifetime 'parent)
            # parent > child → ERROR

            << x;
        };
    }

    # x deallocated, handler.x dangling
    handler();  // UAF!
}

# FIX: Move or copy
main {
    handler_list = [];

    {
        x = 42;
        x_copy = ** x;           // COPY to parent arena

        handler = fn() {
            << x_copy;           // handler owns copy
        };

        handler_list.push(handler);
    }

    # x deallocated, but x_copy lives in parent
    for handler in handler_list {
        handler();               // ✅ OK
    };
}
```

### 6.3 Exception Safety

```
# ✅ SAFE: Exceptions trigger defer cleanup

func risky_operation() {
    {
        x = allocate(1000);
        y = allocate(1000);

        !? {
            # Cleanup code (runs on any exit: normal or exception)
            deallocate_resources();
        };

        result = dangerous_call();  // might throw

        process(x, y);
        << result;
    }
    # Even if exception:
    # 1. !? cleanup runs
    # 2. Arena x, y deallocated
    # 3. Exception propagates
}
```

### 6.4 Async/Await with Arena Isolation

```
# ✅ SAFE: Task has own lifetime, accessed properly

main {
    data = { value: 42 };

    task = | {
        # Task is spawned with 'task lifetime
        # Can access data (from main) via borrow

        local_x = data@;         // borrow from main arena
        processed = local_x + 1;

        <<+ processed;
    };

    result = task |?;            // await task

    # result from task accessible (moved to main arena)
    use(*result);
    << *result;
}

# ❌ FORBIDDEN: Task capturing child scope

main {
    task = null;

    {
        x = 42;

        task = | {
            <<+ x;  // ❌ ERROR: x @ 'child, task @ 'parent
        };
    }

    # Task tries to access deallocated x
}

# FIX: Move or copy
main {
    task = null;

    {
        x = 42;
        x_copy = ** x;    // COPY

        task = | {
            <<+ x_copy;   // task owns copy
        };
    }

    result = task |?;
    << *result;
}
```

---

## Résumé des Patterns

```
╔═══════════════════════════════════════════════════════════════╗
║        SAFE vs FORBIDDEN: Quick Reference                    ║
╚═══════════════════════════════════════════════════════════════╝

PATTERN                  STATUS    REASON
──────────────────────   ────────  ─────────────────────────
x = y (MOVE)             ✅ Safe   Linear types prevent re-use
x = ** y (COPY)          ✅ Safe   Explicit deep copy
ref = @x (borrow)        ✅ Safe   Read-only, lifetime checked
ref = @@x (excl borrow)  ✅ Safe   Exclusive access, no races
return from child scope  ✅ Safe   Bubble-up keeps arena alive
access result.field      ✅ Safe   MOVE semantics, no dangling
use after MOVE           ❌ Error  Linear types prevent it
cross-arena ref escape   ❌ Error  Lifetime checking prevents it
multiple excl borrows    ❌ Error  INV-A4 prevents it
concurrent read+write    ❌ Error  Exclusive borrow prevents it
recursive unbounded      ❌ Error  TCO enforced, limits checked
child scope ref escape   ❌ Error  Borrow checker prevents it
```

---

> **Ces exemples peuvent être testés dans le compilateur 3.42 une fois implémenté.**
>
> Voir aussi : MEMORY_SAFETY_FORMAL_Q21.md pour la formalisation mathématique complète.
