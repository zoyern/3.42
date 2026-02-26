# ParticleIR — Représentation Intermédiaire pour 3.42

## Architecture SSA 2 niveaux

**ParticleIR (haut)** : préserve sémantique bosons/fermions/gluons
**MachineIR (bas)** : traduit vers LLVM IR

---

## Format canonique

```
; Opération boson
result = BOSON(operand1, operand2) -> value : type @arena #spin

; Fermion (données SSA)
fermion(literal_or_ref) : type @arena #spin

; Gluon (conteneur)
gluon.KIND { contents... }

; Sphère (état quantique)
sphere(theta, phi, radius, spin) : type @arena
```

---

## Table des bosons en IR

| Boson | Verbe | ParticleIR | MachineIR | CPU |
|-------|-------|-----------|-----------|-----|
| < | SORTIR | `SORTIR(val)` | `ret` / `store [stdout]` | RET/MOV |
| > | ENTRER | `ENTRER(size)` | `arena_bump` / `load` | PUSH/MOV |
| . | ACCÉDER | `ACCÉDER(base, field)` | `getelementptr` | LEA |
| ? | MESURER | `MESURER(val)` | `cmp` / `test` | CMP/TEST |
| ! | INVERSER | `INVERSER(val)` | `not` / `neg` | NOT/NEG |
| \| | CONNECTER | `CONNECTER(src, fn)` | `call` + async buffer | CALL |
| ~ | DIFFUSER | `DIFFUSER(val, fn)` | `broadcast` | GPU dispatch |
| ^ | SUPERPOSER | `SUPERPOSER(amps...)` | tensor ops / QPU | Hadamard/CNOT |
| % | PROPORTIONNER | `PROPORTIONNER(a, b)` | `urem` / `srem` | DIV (remainder) |
| / | DIVISER | `DIVISER(a, b)` | `udiv` / `sdiv` | DIV (quotient) |
| : | DÉFINIR | `DÉFINIR(name, type)` | `alloca` + type | label/MOV |
| = | LIER | `LIER(name, val)` | `store` | MOV |
| ; | TERMINER | `TERMINER()` | nop | NOP |
| + | ACCUMULER | `ACCUMULER(a, b)` | `add` | ADD/INC |
| - | RÉDUIRE | `RÉDUIRE(a, b)` | `sub` | SUB/DEC |
| * | TRANSFORMER | `TRANSFORMER(a, b)` | `mul` | MUL/IMUL |
| # | MARQUER | `MARQUER(val)` | set overflow flag | TAG |
| _ | IGNORER | `IGNORER(val)` | nop | NOP |
| @ | RÉFÉRENCER | `RÉFÉRENCER(val)` | `lea` / borrow | LEA |

## Spins en IR

| Spin | Bit | Sémantique IR | Usage |
|------|-----|--------------|-------|
| + | 00 | succès / positif | retour normal |
| - | 01 | échec / négatif | erreur |
| # | 10 | erreur / overflow | exception |
| _ | 11 | void / ignoré | discard |
| * | flag | indéterminé | quantique / wildcard |

---

## EXEMPLE 1 : Hello World

### Source 342
```
msg = "Bonjour le monde";
< msg;
```

### ParticleIR
```
%arena_main = gluon.body {
    %msg = fermion("Bonjour le monde") : String @arena_main #_
    %0 = SORTIR(%msg) -> () : void @arena_main #+
}
```

### MachineIR → LLVM IR
```llvm
define i32 @main() {
entry:
    %arena = call ptr @arena_alloc(i64 256)
    %msg = call ptr @arena_store_str(ptr %arena, ptr @.str.hello)
    call void @io_print(ptr %msg)
    call void @arena_free(ptr %arena)
    ret i32 0
}
@.str.hello = private constant [18 x i8] c"Bonjour le monde\00"
```

---

## EXEMPLE 2 : Fibonacci (récursif)

### Source 342
```
fib : (n : i64) : i64 = {
    n ? {
        + : << + (fib(n - 1) + fib(n - 2));
        _ : << + n;
    };
};
result = fib(10);
```

### ParticleIR
```
%fib = fermion(fn) : Fn(i64) -> i64 @global #_

%fib.body = gluon.function(%n : i64 @frame_fib) {
    %cond = MESURER(%n) -> bool : bool @frame_fib #*

    gluon.branch(#+) {
        %n1 = RÉDUIRE(%n, fermion(1)) -> i64 : i64 @frame_fib #_
        %a = CONNECTER(%fib, %n1) -> i64 : i64 @frame_fib #_
        %n2 = RÉDUIRE(%n, fermion(2)) -> i64 : i64 @frame_fib #_
        %b = CONNECTER(%fib, %n2) -> i64 : i64 @frame_fib #_
        %sum = ACCUMULER(%a, %b) -> i64 : i64 @frame_fib #_
        SORTIR(SORTIR(%sum)) #+ ; << + sum
    }

    gluon.branch(#_) {
        SORTIR(SORTIR(%n)) #+ ; << + n
    }
}

%result = CONNECTER(%fib, fermion(10)) -> i64 : i64 @arena_main #_
```

### MachineIR
```llvm
define i64 @_fib(i64 %n) {
entry:
    %cond = icmp sgt i64 %n, 1
    br i1 %cond, label %recurse, label %base

recurse:
    %n1 = sub i64 %n, 1
    %a = call i64 @_fib(i64 %n1)
    %n2 = sub i64 %n, 2
    %b = call i64 @_fib(i64 %n2)
    %sum = add i64 %a, %b
    ret i64 %sum

base:
    ret i64 %n
}
```

---

## EXEMPLE 3 : Pipe chain

### Source 342
```
data = [1, 2, 3, 4, 5];
result = data
    | (x) { << + (x * 2); }
    | (x) { x ? { + : << + x; }; }
    | collect;
```

### ParticleIR
```
%arena_pipe = gluon.body {
    %data = fermion([1, 2, 3, 4, 5]) : [i64; 5] @arena_pipe #_

    ; Pipe 1 : map × 2
    %map_fn = gluon.function(%x : i64 @frame_map) {
        %doubled = TRANSFORMER(%x, fermion(2)) -> i64 : i64 @frame_map #_
        SORTIR(SORTIR(%doubled)) #+
    }
    %mapped = CONNECTER(%data, %map_fn) -> [i64] : Vec<i64> @arena_pipe #_

    ; Pipe 2 : filter > 0
    %filter_fn = gluon.function(%x : i64 @frame_filter) {
        %ok = MESURER(%x) -> bool : bool @frame_filter #*
        gluon.branch(#+) { SORTIR(SORTIR(%x)) #+ }
    }
    %filtered = CONNECTER(%mapped, %filter_fn) -> [i64] : Vec<i64> @arena_pipe #_

    ; Pipe 3 : collect
    %result = CONNECTER(%filtered, fermion(collect)) -> [i64] : Vec<i64> @arena_pipe #+
}
```

### MachineIR
```llvm
define void @_pipe_example() {
entry:
    %arena = call ptr @arena_alloc(i64 8192)
    %input = call ptr @arena_alloc_array(ptr %arena, i64 5, ptr @.arr.12345)

    ; Map: x * 2
    %mapped = call ptr @vec_map(ptr %arena, ptr %input, i64 5, ptr @_lambda_mul2)

    ; Filter: x > 0
    %filtered = call ptr @vec_filter(ptr %arena, ptr %mapped, i64 5, ptr @_lambda_pos)

    ; Collect: matérialiser
    %result = call ptr @vec_collect(ptr %arena, ptr %filtered)

    call void @arena_free(ptr %arena)
    ret void
}
```

---

## EXEMPLE 4 : Circuit quantique

### Source 342
```
^ {
    q = ^~(0.5, 0.5);    // Hadamard sur superposition
    result = q ?? {
        + : << + 1;
        _ : << + 0;
    };
};
```

### ParticleIR
```
%arena_q = gluon.quantum {
    ; Créer superposition
    %amp0 = sphere(pi/2, 0, 0.707, #*) : Amplitude @arena_q
    %amp1 = sphere(pi/2, pi, 0.707, #*) : Amplitude @arena_q
    %q = SUPERPOSER(%amp0, %amp1) -> Qubit : Qubit @arena_q #*

    ; Porte Hadamard via DIFFUSER
    %H = fermion(hadamard_matrix) : Gate @arena_q #_
    %q_h = DIFFUSER(%q, %H) -> Qubit : Qubit @arena_q #*

    ; Mesure (collapse)
    %measured = MESURER(MESURER(%q_h)) -> bool : bool @arena_q #*

    gluon.branch(#+) {
        SORTIR(SORTIR(fermion(1))) #+
    }
    gluon.branch(#_) {
        SORTIR(SORTIR(fermion(0))) #+
    }
}
```

### MachineIR (simulation classique)
```llvm
define i64 @_quantum_circuit() {
entry:
    %state = call ptr @tensor_alloc(i64 2)
    call void @tensor_set(ptr %state, i64 0, double 0.707)
    call void @tensor_set(ptr %state, i64 1, double 0.707)

    ; Hadamard
    %H = call ptr @hadamard_2x2()
    call void @tensor_matmul(ptr %state, ptr %H)

    ; Mesure probabiliste
    %p0 = call double @tensor_prob(ptr %state, i64 0)
    %rng = call double @random_double()
    %cmp = fcmp olt double %rng, %p0
    %result = select i1 %cmp, i64 1, i64 0

    call void @tensor_free(ptr %state)
    ret i64 %result
}
```

---

## EXEMPLE 5 : Arènes imbriquées

### Source 342
```
{
    {
        x : i64 = 42;
        y : i64 = 13;
        z = x + y;
    };
    // x, y, z inaccessibles

    {
        buf = > 4096;        // alloc 4Ko
        process(@buf);       // borrow
    };
    // buf libéré
};
```

### ParticleIR
```
%arena_outer = gluon.body {

    %arena_inner1 = gluon.body {
        %x = DÉFINIR(fermion(42), i64) -> i64 : i64 @arena_inner1 #_
        %y = DÉFINIR(fermion(13), i64) -> i64 : i64 @arena_inner1 #_
        %z = ACCUMULER(%x, %y) -> i64 : i64 @arena_inner1 #+
    }
    ; arena_inner1 LIBÉRÉE (O(1) : reset bump pointer)

    %arena_inner2 = gluon.body {
        %buf = ENTRER(fermion(4096)) -> [u8; 4096] : [u8] @arena_inner2 #_
        %ref_buf = RÉFÉRENCER(%buf) -> @[u8] : @[u8] @arena_inner2 #_
        ; @ = borrow partagé (lecture seule)
        CONNECTER(fermion(process), %ref_buf) -> () : void @arena_inner2 #_
    }
    ; arena_inner2 LIBÉRÉE

}
; arena_outer LIBÉRÉE (cascade : toutes les sous-arènes aussi)
```

### MachineIR
```llvm
define void @_arena_example() {
entry:
    %outer = call ptr @arena_alloc(i64 16384)

    ; Sous-arène 1
    %inner1 = call ptr @arena_child(ptr %outer, i64 256)
    %x = call ptr @arena_bump(ptr %inner1, i64 8)
    store i64 42, ptr %x
    %y = call ptr @arena_bump(ptr %inner1, i64 8)
    store i64 13, ptr %y
    %xv = load i64, ptr %x
    %yv = load i64, ptr %y
    %z = add i64 %xv, %yv
    call void @arena_reset(ptr %inner1)    ; O(1) free

    ; Sous-arène 2
    %inner2 = call ptr @arena_child(ptr %outer, i64 4096)
    %buf = call ptr @arena_bump(ptr %inner2, i64 4096)
    call void @_process(ptr %buf)           ; borrow (ptr passé en lecture)
    call void @arena_reset(ptr %inner2)    ; O(1) free

    call void @arena_free(ptr %outer)       ; free cascade
    ret void
}
```

---

## Propriétés du format

1. **SSA strict** : chaque `%var` assignée une seule fois
2. **Sémantique préservée** : SORTIR/ENTRER/MESURER = vocabulaire 3.42
3. **Arènes explicites** : `@arena_name` sur chaque opération
4. **Spins tracés** : `#spin` sur chaque résultat
5. **Gluons structurants** : `gluon.body`, `gluon.function`, `gluon.branch`, `gluon.quantum`
6. **Compositions visibles** : `SORTIR(SORTIR(x))` = `<< x` = return
7. **Lowering progressif** : ParticleIR → MachineIR → LLVM IR sans perte d'info critique
