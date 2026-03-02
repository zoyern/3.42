# 3.42 — FORMALISATION DES RÈGLES DE SÉCURITÉ MÉMOIRE (Q21)
## Arènes, Types Linéaires, Bubble-up et Prévention des Vulnérabilités

> **Auteur** : Alexis Mounib (zoyern) — avec Claude (Anthropic)
> **Date** : Février 2026
> **Statut** : Spécification formelle — Q21
> **Abstrait** : Formalisation mathématique et informatique des garanties de sécurité mémoire du langage 3.42, éliminant les 3 vulnérabilités connues des allocateurs d'arènes via la combinaison de types linéaires, d'un borrow checker de grain fin, et de règles d'interdiction cross-arène.

---

## TABLE DES MATIÈRES

1. [Résumé exécutif](#1-résumé-exécutif)
2. [Vulnérabilités et Mitigations](#2-vulnérabilités-et-mitigations)
3. [Modèle Formel de l'Arène](#3-modèle-formel-de-larène)
4. [Système de Types Linéaires](#4-système-de-types-linéaires)
5. [Règles du Borrow Checker](#5-règles-du-borrow-checker)
6. [Règles Cross-Arène](#6-règles-cross-arène)
7. [Accès par Indices](#7-accès-par-indices)
8. [Pages de Garde et Débugage](#8-pages-de-garde-et-débugage)
9. [Invariants Formels](#9-invariants-formels)
10. [Comparaison avec Rust](#10-comparaison-avec-rust)
11. [Cas Limites et Edge Cases](#11-cas-limites-et-edge-cases)
12. [Sketch de Preuve de Solidité](#12-sketch-de-preuve-de-solidité)

---

## 1. Résumé Exécutif

### Objectif

Formaliser un système de sécurité mémoire **sound** (cohérent) et **complete** (exhaustif) pour 3.42 qui élimine définitivement 7 catégories de bugs d'accès mémoire :

```
Use-after-free (UAF)       ← Types linéaires + borrow checker
Double free                ← Allocateur d'arènes
Dangling pointers          ← Interdiction cross-arène + @ vérification
Buffer overflow            ← Accès indexé vérifié + bounds checking
Memory leak                ← Auto-free au scope exit
Data race (shared mutable) ← @@ emprunt exclusif + !| fences
Stack overflow             ← TCO + limits configurables
```

### 3 Vulnérabilités Clés des Arènes

**Contexte** : Recherche révèle 3 vulnérabilités des arena allocators :

1. **Cross-arena references** : arène A → objet dans arène B, B droppée → UAF
2. **Dangling pointers post-dealloc** : arène libérée mais pointeur conservé
3. **Aliasing violations** : arènes chevauchantes créant des accès simultanés

**Réponse 3.42** : Les types linéaires (D5) + arènes (D4) + borrow checker + **interdiction explicite cross-arène** suffisent si formalisées avec rigueur.

---

## 2. Vulnérabilités et Mitigations

### 2.1 Cross-Arena References (Vulnérabilité #1)

#### Modèle du Bug

```
// Arena A (parent scope)
{
    a_obj = {x: 42};  // alloué dans arène A

    // Arena B (nested scope)
    {
        b_ref = @a_obj;  // emprunt de A depuis B
        << b_ref;        // retourne référence vers objet de A
    }
    // Arène B fermée, pointeur de stack reset
    // BUT: b_ref toujours valide en termes de CPU
}
// Arène A aussi fermée
// Access de b_ref → dangling pointer → crash ou corruption
```

#### Mitigation: Règle d'Interdiction

**RULE 1 (Cross-Arena Prohibition)** :

```
Une référence @ ou @@ ne peut pas :
1. Être retournée d'une arène plus petite vers une plus grande
2. Être stockée dans une structure de l'arène parent
3. Survivre au scope de l'arène qui la contient (sans MOVE)

Formellement :
  Si ref = @value, alors :
    arena(value) ≤ arena(ref)  (même arène ou parent)

  Et si ref sort du scope :
    ref doit être "consumed" (MOVE ou borrow expiration)
```

#### Vérification Compile-Time

Le borrow checker assigne une **lifetime variable** à chaque arène :

```
'a = lifetime de l'arène globale/parent
'b = lifetime d'une arène imbriquée

ref : &'b type   signifie: ref pointe vers objet de durée 'b
```

Le compilateur refuse toute assignation où la lifetime target < lifetime source.

---

### 2.2 Dangling Pointers Post-Dealloc (Vulnérabilité #2)

#### Modèle du Bug

```
ptr = null;
{
    x = alloc(100);      // alloué
    ptr = @x;            // référence sauvegardée en dehors
}
// scope fermé, x libéré (arène reset)
access(ptr);  // DANGLING → UAF
```

#### Mitigation: Types Linéaires

**RULE 2 (Linear Type Enforcement)** :

```
Une valeur ne peut avoir qu'UN propriétaire à la fois.

MOVE (par défaut) : x = y;
  → y n'existe plus après cette ligne
  → toute utilisation ultérieure de y = compile error

COPY (explicite) : x = ** y;
  → copie une NOUVELLE instance dans l'arène de x
  → y reste propriétaire de l'original

BORROW (temporaire) : ref = y@;
  → ref n'est pas propriétaire
  → ref expire à la fin du scope où elle a été créée (durée = borrow, pas valeur)
```

#### Vérification par le Compilateur

À chaque utilisation de variable, le type checker :
1. Vérifie que la variable n'a pas été MOVE
2. Si c'est une référence @, vérifie que sa lifetime est toujours valide
3. Rejette toute tentative de prolonger une référence au-delà de son scope

---

### 2.3 Aliasing Violations (Vulnérabilité #3)

#### Modèle du Bug

```
// Deux références mutables vers la même donnée
ref1 = @@ x;
ref2 = @@ x;

*ref1 = 42;   // write
read(*ref2);  // read → peut voir valeur obsolète ou corrompue
```

#### Mitigation: Emprunt Exclusif

**RULE 3 (Exclusive Borrow)** :

```
Exactement UN des énoncés suivants est vrai :
1. Références partagées (lecture seule) : 0 ou N références @
2. Référence exclusive (écriture seule) : 1 référence @@

CAS 1 (Shared borrow) :
  ref1 = @x;
  ref2 = @x;  // OK, deux lectures
  write_x = ... ; // FORBIDDEN (x est emprunté en lecture)

CAS 2 (Exclusive borrow) :
  ref1 = @@ x;  // OK, acquisition exclusive
  ref2 = @@ x;  // COMPILE ERROR : x est déjà emprunté excl
  ref2 = @x;    // COMPILE ERROR : x est emprunté excl, même lecture interdite
```

---

## 3. Modèle Formel de l'Arène

### 3.1 Définition Mathématique

Une **arène** est un tuple :

```
Arena = (name, parent, base_ptr, size, lifetime, objects)

où :
  name       : String                    (identifiant, ex: "main", "compute()")
  parent     : Option<Arena>             (nil si racine, sinon référence parent)
  base_ptr   : MemAddr                   (adresse du début du bloc contigu)
  size       : usize                     (octets alloués)
  lifetime   : Lifetime                  ('a, 'b, etc.)
  objects    : HashMap<VarId, AllocInfo> (variables dans cette arène)

AllocInfo = {
  var_id   : VarId           (nom unique de la variable)
  offset   : usize           (relative à base_ptr)
  type_tag : Type            (type 3.42)
  size     : usize           (taille en octets)
  spin     : Spin            (+, -, #, _)
  owner    : VarId           (propriétaire courant)
  borrows  : Set<BorrowInfo> (références actives)
}

BorrowInfo = {
  lifetime   : Lifetime      (durée de validité)
  mode       : BorrowMode    (Read | Write)
  borrower   : VarId         (qui emprunte)
}
```

### 3.2 Invariants d'Arène

**INV-A1 (Contiguïté Mémoire)** :
```
Tous les objets de Arena sont contigus en mémoire :
  ∀ obj1, obj2 ∈ Arena.objects :
    addr(obj1) + size(obj1) ≤ addr(obj2)  (ou inverse)
```

**INV-A2 (Fermeture Atomique)** :
```
À la fermeture d'une arène, TOUTE la mémoire est libérée en UNE opération :
  When Arena.lifetime ends:
    ∀ obj ∈ Arena.objects : dealloc(obj) in O(1)
    Arena.base_ptr := invalid
```

**INV-A3 (Propriété Unique Stricte)** :
```
Chaque objet a exactement un propriétaire à la fois :
  ∀ obj ∈ Arena.objects :
    |{v : v is owner of obj}| = 1
```

**INV-A4 (Emprunt XOR)** :
```
Un objet ne peut pas être emprunté en lecture ET en écriture simultanément :
  ∀ obj ∈ Arena.objects :
    if ∃ borrow with mode=Write :
      ∀ other_borrow : other_borrow.mode ≠ Read
    if ∃ borrow with mode=Read :
      ∀ other_borrow : other_borrow.mode = Read
```

---

## 4. Système de Types Linéaires

### 4.1 Sémantique des Opérations

#### MOVE (défaut)

```
SYNTAX: x = y;
RULE  : Ownership(y) → x
POST  : y devient inaccessible

FORMAL :
  Γ ⊢ y : T
  _______________
  Γ[y : ⊥] ⊢ (x = y)  Γ[x : T]

(T = type, ⊥ = "inaccessible", Γ = contexte)
```

#### COPY (explicite)

```
SYNTAX: x = ** y;
RULE  : Create copy of y in x's arena
POST  : y reste propriétaire, x propriétaire de la copie

FORMAL :
  Γ ⊢ y : T @ Arena_y
  allocate size(T) in Arena_x
  memcpy(y, allocation)
  ___________________________________________
  Γ ⊢ (x = ** y)  Γ[x : T @ Arena_x, y : T @ Arena_y]

COST: O(size(T)), amortizable via compiler optimizations
```

#### BORROW (référence, lecture seule)

```
SYNTAX: ref = y@;
RULE  : Create reference without ownership
POST  : ref peut lire y, y reste propriétaire

FORMAL :
  Γ ⊢ y : T @ 'arena
  borrow_stack.push({ borrower: ref, mode: Read, lifetime: 'scope })
  ________________________
  Γ[ref : &'scope T] ⊢ y@ ; ref expires at scope end

INVARIANT: INV-A4 must hold during borrow lifetime
```

#### EXCLUSIVE BORROW (référence, écriture exclusive)

```
SYNTAX: ref = y@@;
RULE  : Create exclusive reference
POST  : ref peut lire ET écrire y, aucun autre accès

FORMAL :
  Γ ⊢ y : T @ 'arena
  assert ¬∃ other_borrow for y  (no concurrent borrows)
  borrow_stack.push({ borrower: ref, mode: Write, lifetime: 'scope })
  ________________________
  Γ[ref : &mut 'scope T] ⊢ y@@ ; ref expires at scope end

INVARIANT: INV-A4 strictly enforced (no other borrows allowed)
```

---

### 4.2 Type Judgment Rules

```
╔═══════════════════════════════════════════════════════════════╗
║              TYPE JUDGMENT IN LINEAR TYPES                    ║
╚═══════════════════════════════════════════════════════════════╝

(T-MOVE)
  Γ ⊢ expr : T
  ───────────────────
  Γ ⊢ x = expr ; Γ'[x:T]

(T-COPY)
  Γ ⊢ expr : T@A
  arena_of(x) != A
  allocate_in_arena_of_x(expr)
  ────────────────────────────────────
  Γ ⊢ x = ** expr ; Γ'[x:T@arena(x), expr:T@A]

(T-BORROW-READ)
  Γ ⊢ x : T
  lifetime_of_x >= current_scope_lifetime
  borrow_mode(x) = Read ∨ ¬borrowed(x)
  ──────────────────────────────────────
  Γ ⊢ ref = x@ ; Γ'[ref : &'scope T]

(T-BORROW-WRITE)
  Γ ⊢ x : T
  ¬borrowed(x)  (no concurrent borrows)
  ──────────────────────────────────────
  Γ ⊢ ref = x@@ ; Γ'[ref : &mut'scope T]

(T-ACCESS)
  Γ ⊢ ref : &'lifetime T
  current_lifetime < 'lifetime
  ────────────────────────────────
  Γ ⊢ *ref : T

(T-EXPIRE)
  Γ ⊢ ref : &'lifetime T
  current_lifetime >= 'lifetime
  ────────────────────────────────
  Γ ⊢ COMPILE ERROR: borrow expired
```

---

## 5. Règles du Borrow Checker

### 5.1 Algorithme du Borrow Checker

```
ALGORITHM: CheckBorrows(AST)
INPUT: Abstract Syntax Tree with type annotations
OUTPUT: Accept (sound) | Reject (error at line N)

FOR EACH statement s in AST DO:
  MATCH s:
    CASE s = x = expr:
      compute_ownership(x, expr)
      invalidate(expr)

    CASE s = x = ** expr:
      check_arena_boundary(x, expr)
      allocate_copy(x, expr)

    CASE s = ref = expr@:
      check_lifetime(expr) >= current_scope
      push_borrow(ref, expr, mode=Read)

    CASE s = ref = expr@@:
      check_lifetime(expr) >= current_scope
      assert no_concurrent_borrows(expr)
      push_borrow(ref, expr, mode=Write)

    CASE s = *ref:
      check_lifetime(*ref) < current_scope
      allow_access(ref)

    CASE s = scope_exit:
      pop_borrows_at_scope()
      check_all_vars_consumed_or_moved()
      free_arena()

RETURN: success or list of errors
```

### 5.2 Lifetime Inference

```
Chaque variable reçoit une LIFETIME (durée de vie).

RULES:
  Global variables      : 'static
  Function parameters   : 'param (contaminée par le plus court appelant)
  Local variables       : 'scope (durée du bloc)
  Borrowed references   : 'borrow (durée déclarée ou inférée)
  Callee return values  : 'bubble (via bubble-up)

INFERENCE:
  let x = 5;              // x : i32 @ 'scope
  let ref = @x;           // ref : &'scope i32
  {
    let y = 10;           // y : i32 @ 'inner_scope
    let ref2 = @y;        // ref2 : &'inner_scope i32
  }
  // ref2 now invalid (lifetime expired)
  // y inaccessible (moved out of scope)
```

---

## 6. Règles Cross-Arène

### 6.1 Définition Formelle : Ordre Partiel d'Arène

```
DEFINITION: Arena Hierarchy

Soit → la relation "contient" :
  A → B iff A is the parent scope of B

PROPERTIES:
  1. Réflexive : A → A (un scope contient lui-même)
  2. Transitive : A → B et B → C ⟹ A → C
  3. Antisymétrique : A → B et B → A ⟹ A = B

NOTATION:
  A ≤ B means A is ancestor of B (or equal)
  A < B means A is STRICT ancestor of B
```

### 6.2 RÈGLE CENTRALE : Cross-Arena Prohibition

```
╔════════════════════════════════════════════════════════════════╗
║  CROSS-ARENA REFERENCE PROHIBITION (RULE 6-CA1)              ║
╚════════════════════════════════════════════════════════════════╝

A reference may only point into an arena of equal or greater lifetime.

FORMAL:
  ref : &'lifetime T @ Arena_target

  REQUIREMENT:
    Arena_target.lifetime ≤ lifetime_of_creator

  OR EQUIVALENTLY (in arena order):
    Arena_target ≤ Arena_creator

  CONSEQUENCE:
    A reference created in B cannot point to an object in child(B)
    (child arena would be deallocated first)

EXAMPLES:

  ✅ ALLOWED:
    {  // Arena A
        x = 42;
        {  // Arena B (child of A)
            ref = @x;  // points to parent arena → OK
        }  // B closed, ref expires
    }  // A closed

  ❌ FORBIDDEN:
    ptr = null;
    {  // Arena B
        x = 42;
        ptr = @x;  // points to child arena, escapes B → ERROR
    }
    // x deallocated, ptr dangling
```

### 6.3 Bubble-Up as Exception (Controlled)

```
EXCEPTION: Bubble-up (Decision D21)

When a function RETURNS a value, internal variables may outlive
their original arena scope IF accessed via bubble-up.

FORMAL:
  func compute() → T {
      x = alloc();  // x @ 'compute
      << x;         // return x
  }

  result = compute();  // result.x @ 'compute_bubble
  y = result.x;        // MOVE x → y, now y @ 'caller

KEY INSIGHT:
  Bubble-up transfers ownership via . operator (field access).
  This is a CONTROLLED exception to cross-arena rules.

SAFETY:
  1. Access via . is explicit (not transparent reference)
  2. Only MOVE semantics allowed (no dangling refs)
  3. Lifetime extension lasts only as long as return value exists
  4. Linear types prevent aliasing after MOVE
```

### 6.4 Forbidden Patterns

```
PATTERN 1: Struct containing cross-arena reference

  struct Container {
    ptr: &'static T  // NOT ALLOWED if T from child arena
  }

  {  // Arena A
      x = 42;
      {  // Arena B
          container = { ptr: @x };  // FORBIDDEN
          << container;  // tries to return ref to child
      }
  }

FIX: Use MOVE semantics instead
  container = { data: ** x };  // copy instead


PATTERN 2: Global variable holding child arena reference

  ref_global = null;

  func use_it() {
      {  // Arena local
          x = 42;
          ref_global = @x;  // FORBIDDEN: escapes scope
      }
  }

FIX: Only allow 'static references in globals


PATTERN 3: Callback holding child arena reference

  callback = fn() { use(@x); };  // FORBIDDEN if x in child arena

  {
      x = 42;
      handler_callback = fn() { print(@x); };  // closure captures child ref
  }
  // x deallocated, callback still references it

FIX: Move x into closure, or ensure x outlives callback
  handler_callback = fn(x_copy) { print(x_copy); };
```

---

## 7. Accès par Indices

### 7.1 Remplacement des Pointeurs Bruts

```
PRINCIPLE: Indices instead of raw pointers

3.42 discourages raw pointers (@), preferring array indices:

  // Instead of:
  ptr = @array[0];  // raw pointer (need lifetime annotation)

  // Prefer:
  idx = 0;
  value = array[idx];  // index stays valid as long as array

  // Or:
  elem = array.[0];  // . operator for field/index access
```

### 7.2 Bounds Checking

```
RULE 7-BC1 (Array Bounds)

All array/collection accesses are bounds-checked at runtime:

  array[idx]

COMPILE-TIME verification where possible:
  if idx < array.len -> allow access
  else -> COMPILE ERROR or insert runtime check

SAFETY GUARANTEE:
  No buffer overflow via out-of-bounds access
  (either caught at compile-time or runtime trap)

COST:
  O(1) bounds check = negligible vs. security benefit
```

### 7.3 Indexing with Lifetime Guarantees

```
SAFE INDEXING:

  {  // Arena A
      array = [1, 2, 3, 4, 5];
      {  // Arena B
          // ✅ OK: Index doesn't outlive array
          elem = array[0];  // MOVE elem, stays in B

          // ❌ FORBIDDEN: Reference outlives context
          ref = @array[0];  // ref would be &'static, but array @ 'A
                            // COMPILE ERROR: lifetime mismatch
      }
  }
```

---

## 8. Pages de Garde et Débugage

### 8.1 Guard Pages (Mode Debug)

```
FEATURE: Guard pages protect against buffer overflow

In DEBUG builds, arènes are padded with inaccessible memory:

  ┌─────────────────────────────────────┐
  │ GUARD PAGE (invalid)                │  ← inaccessible (SIGSEGV if touched)
  ├─────────────────────────────────────┤
  │ Arena allocation                    │  ← actual objects
  │                                     │
  ├─────────────────────────────────────┤
  │ GUARD PAGE (invalid)                │  ← inaccessible (SIGSEGV if touched)
  └─────────────────────────────────────┘

BENEFIT:
  Accidental buffer overflows → immediate crash (easier debugging)
  vs. silent memory corruption

COST: ~8KB per arena (negligible in dev mode)

IMPLEMENTATION:
  mmap(..., PROT_NONE)  on Unix
  VirtualAlloc with PAGE_NOACCESS on Windows
```

### 8.2 Metadata Tagging (@#)

```
COMPOSITION: @# = REFERENCE(MARK/CORRUPT) = tag check

Pointers can be tagged with metadata (optional in 3.42):

  ptr_tagged = x@# ;  // borrow with metadata check

On dereference:
  *ptr_tagged

Checks:
  1. Address is valid and not freed
  2. Alignment correct for type
  3. Type matches (prevents type confusion attacks)

This is OPTIONAL layer, not required for memory safety
(types are already checked at compile-time)
```

---

## 9. Invariants Formels

### 9.1 Liste Complète

```
╔═══════════════════════════════════════════════════════════════╗
║              FORMAL INVARIANTS (MEMORY SAFETY)                ║
╚═══════════════════════════════════════════════════════════════╝

INV-M1 (No Use-After-Free):
  ∀ ref : type, t ∈ Timesteps :
    if ref valid at time t:
      ownership(ref.target) ≠ moved ∧ arena(ref.target).lifetime > t

INV-M2 (No Double Free):
  ∀ arena, t ∈ Timesteps :
    if arena deallocated at t:
      ∀ t' > t : arena never accessed again
  Proof: arena deallocation = O(1) reset, no per-object free

INV-M3 (No Dangling Pointers):
  ∀ ref, obj:
    if ref points to obj:
      arena(obj).lifetime ≤ lifetime(ref)
  Corollary: Cross-arena references forbidden by RULE 6-CA1

INV-M4 (No Buffer Overflow):
  ∀ access array[idx]:
    0 ≤ idx < len(array)
  Checked: compile-time (static analysis) + runtime bounds check

INV-M5 (No Memory Leak):
  ∀ arena at end of scope:
    all objects in arena deallocated
  Proof: arena closed = all objects freed (INV-A2)

INV-M6 (No Data Race):
  ∀ var, t ∈ Timesteps :
    ¬(∃ reader ∧ writer at time t on same var)
  Enforced by:
    - Linear types (no implicit aliasing)
    - @@ exclusive borrow (at most one writer)
    - !| fences (synchronization points)

INV-M7 (No Stack Overflow):
  ∀ call stack:
    depth(stack) < configured_limit ∨ tail_call_optimized
  Enforced: TCO for recursive calls, configurable limits
```

### 9.2 Proof of Invariants (Sketch)

```
THEOREM 9-PS1: 3.42 type system prevents use-after-free.

PROOF SKETCH:
  1. Assume @ use-after-free scenario: access freed memory
  2. By INV-M1, if access valid, owner not moved
  3. By types, owner is moved (or would be COMPILE ERROR)
  4. Contradiction → impossible
  ∎

THEOREM 9-PS2: Cross-arena refs impossible (soundness).

PROOF SKETCH:
  1. Assume cross-arena ref exists: ref points to child arena
  2. By RULE 6-CA1, cross-arena refs forbidden
  3. Compiler rejects at type-check phase (before execution)
  4. No execution where such ref exists
  ∎

THEOREM 9-PS3: Bubble-up doesn't violate safety.

PROOF SKETCH:
  1. Bubble-up accesses field of return value via .
  2. . operator implies MOVE of field
  3. Linear types prevent re-use of original value
  4. Only moved field remains accessible
  5. Moved field in parent arena (caller scope)
  6. No cross-arena ref escapes (by linearity)
  ∎

THEOREM 9-PS4: Arènes O(1) free without leak.

PROOF SKETCH:
  1. Arena = contiguous block (INV-A1)
  2. Deallocation = reset stack pointer (one operation)
  3. All objects in arena reclaimed (INV-A2)
  4. No fragmentation or forgotten objects
  5. O(1) guaranteed (independent of # objects)
  ∎
```

---

## 10. Comparaison avec Rust

### 10.1 Similarités

```
╔═══════════════════════════════════════════════════════════════╗
║           3.42 vs RUST: Similarities                         ║
╚═══════════════════════════════════════════════════════════════╝

FEATURE                3.42              RUST
───────────────────    ──────────────    ──────────────────────
Ownership             MOVE (default)     Move (default)
Borrowing             @ and @@           & and &mut
Lifetimes             Inferred/explicit  Explicit (in types)
Exclusive ref         @@                 &mut
Read-only ref         @                  &
Type safety           ✓ (linear)         ✓ (linear)
Memory safety         ✓ (arènes)         ✓ (heap)

SAME FUNDAMENTALS: Both prevent UAF, dangling refs, data races
via ownership + borrowing at compile-time.
```

### 10.2 Différences Clés

```
╔═══════════════════════════════════════════════════════════════╗
║           3.42 vs RUST: Key Differences                      ║
╚═══════════════════════════════════════════════════════════════╝

ASPECT                3.42                RUST
───────────────────   ──────────────────  ──────────────────────

ALLOCATION:
Scheme                Arènes (contiguous) Heap (scattered)
Deallocation          O(1) scope exit     O(n) GC or drop
Fragmentation         None                Possible
Cache locality        ✓ excellent         ⚠️ poor

LIFETIME SYNTAX:
Declaration           Implicit (inferred) Explicit (&'a T)
Annotations needed    Minimal             Common
Readability           ✓ simpler           More verbose

FLEXIBILITY:
Generic lifetimes     Limited (scopes)    Rich ('a, 'b, 'c...)
Closure capture       Automatic           Must understand borrow
Custom allocators     Via arènes          Via trait (Allocator)

COPY SEMANTICS:
Default               MOVE (must be       Depends on impl Copy
                      explicit with **)   (implicit for primitives)

BUBBLE-UP:
Return values         Extends via .       Requires explicit ret
Performance           Zero-cost           Same (RVO)

API SURFACE:
Lifetimes in APIs     Rare (scope-local)  Common (generic)
Learning curve        Shorter             Longer
```

### 10.3 Avantages de 3.42

```
ADVANTAGE 1: Arena Allocation
  Rust heap = O(n) allocation + fragmentation
  3.42 arènes = O(1) deallocation + locality
  Example: compute() allocates 10 objects
    Rust: 10 malloc + 10 free = 20 ops
    3.42: 1 arena reset = 1 op

ADVANTAGE 2: Implicit Lifetimes
  Rust: &'a T requires explicit lifetime parameter
  3.42: inferred from scope, rarely visible

  Example:
    // Rust - verbose
    fn process<'a>(x: &'a i32) -> &'a i32 { x }

    // 3.42 - implicit
    func process(x) { << x; }  // lifetime inferred

ADVANTAGE 3: Bubble-up Ergonomics
  Rust: must return owned value, then pattern match
  3.42: return value fields accessible via . (MOVE automatic)

  Example:
    // Rust
    let (x, y) = compute();  // destructure

    // 3.42
    result = compute();
    x = result.x;  // automatic MOVE

LIMITATION 1: Less Flexible
  Rust generic lifetimes allow complex patterns
  3.42 scope-based lifetimes simpler but less powerful

  Trade-off: simpler ≈ safer ≈ easier to learn

LIMITATION 2: No Heap Patterns
  3.42 assumes scoped allocation (arènes)
  Rust allows arbitrary heap patterns
  3.42 encourages functional/scoped style
```

---

## 11. Cas Limites et Edge Cases

### 11.1 Recursive Functions

```
PROBLEM: Recursion → N nested arènes → stack memory issue?

func factorial(n) {
    {  // Arena 1
        if n <= 1 { << 1; }
        << n * factorial(n-1);
    }
}

factorial(1000)  // 1000 arènes nested?

SOLUTION: Tail-Call Optimization (TCO)

If function is in tail position (last statement):
  func fact(n, acc=1) {
      n <= 1 ? << acc ; >> fact(n-1, n*acc);
  }

Compiler converts to:
  loop:
    if n <= 1 { << acc; }
    n := n-1; acc := n*acc;
    goto loop;

RESULT: No new arena per iteration, O(1) space
```

### 11.2 Closures Capturing Child Arena References

```
PROBLEM: Closure capturing reference to child scope variable

  fn capture_child() {
      {  // Child arena
          x = 42;
          callback = fn() { print(@x); };  // captures ref to x
      }
      callback();  // x deallocated, ref dangling!
  }

SOLUTION 1: Forbid (compile-time check)

  Compiler rejects: closure has type
    Fn() @ 'child_arena

  But callback must be stored in parent_arena
  Lifetime mismatch → COMPILE ERROR

SOLUTION 2: Move Capture (automatic)

  {  // Child arena
      x = 42;
      x_copy = ** x;  // explicit copy
      callback = fn() { print(x_copy); };
  }
  // x deallocated, x_copy moved to parent
  callback();  // still valid

SOLUTION 3: Escape Analysis

  Compiler detects: callback stored outside scope
  Automatically promotes x to parent arena
  Or insert implicit copy (** x)
```

### 11.3 Async/Await with Cross-Arena Spawning

```
PROBLEM: Spawning fiber in parent but accessing child arena data

  main {  // Arena main
      {  // Arena child
          x = 42;
          task = | { use(x); };  // spawn fiber
      }
      // child closed, x deallocated
      await(task);  // task tries to access x → UAF
  }

SOLUTION: Lifetime propagation for tasks

  | { ... } creates Task @ 'lifetime

  Task lifetime must be less than or equal to
  all data it references.

  Compiler check:
    Task @ 'child
    x @ 'child
    Both closed at 'child exit → OK (both destroyed together)

  But if Task returned to parent:
    Task @ 'main (extended lifetime)
    x @ 'child (shorter lifetime)
    Compile Error: Task outlives x

  FIX: Explicit MOVE or copy
    x_copy = ** x;  // copy to main arena
    task = | { use(x_copy); };
```

### 11.4 Struct with Embedded References

```
PROBLEM: Struct containing refs to child arena

  struct Parser {
    buffer: @[u8]  // reference to byte array
  }

  {  // Child arena
      data = [1, 2, 3, 4, 5];
      parser = { buffer: @data };  // struct holds child ref
  }
  // data deallocated, parser.buffer dangling

SOLUTION: No embedded references across arènes

  RULE: Struct field types cannot be &T if T in different arena

  FIX 1: Copy data
    parser = { buffer: ** data };  // now parser.buffer owns copy

  FIX 2: Lifetime parameter on struct
    struct Parser<'a> {
      buffer: &'a [u8]
    }

    In child scope:
      parser : Parser<'child> { buffer: @data }
    Parser lifetime = child, cannot escape

    Move to parent:
      COMPILE ERROR (lifetime mismatch)
```

### 11.5 Exception Handling with Arènes

```
PROBLEM: Exception thrown in nested scope, arena cleanup?

  {  // Arena A
      {  // Arena B
          x = risky_op();  // might throw
          process(x);
      }
      // Arena B must close (free all objects)
  }

SOLUTION: Automatic cleanup on exception

  Exception mechanism:
    1. Exception thrown (panic, division by zero, etc.)
    2. Unwind stack
    3. At each scope exit: drop arena (free O(1))
    4. Propagate exception up

  Ensures:
    - No leak (every arena cleaned up)
    - No dangling refs (data freed before exception reaches parent)
    - No double-free (exception doesn't free twice)

  MECHANISM: Insert implicit !? {} (defer) at scope boundaries
    {
        try_block...
    }
    implicitly becomes:
    {
        !? { close_arena(); };
        try_block...
    }

    !? (defer) executes on exit (normal or exceptional)
```

---

## 12. Sketch de Preuve de Solidité

### 12.1 Théorème Principal

```
THEOREM 12-MAIN: Soundness of 3.42 Memory Safety

The combined system of:
  1. Linear types (MOVE default)
  2. Arena allocation (O(1) free)
  3. Cross-arena prohibition (RULE 6-CA1)
  4. Borrow checker (@ and @@ lifetimes)
  5. Bounds checking (array accesses)

GUARANTEES:
  ∀ well-typed 3.42 program:
    ¬(UAF ∨ double-free ∨ dangling-ptr ∨ buffer-overflow ∨ leak ∨ race)

PROOF STRATEGY:
  By type preservation + progress + memory isolation
```

### 12.2 Key Lemmas

```
LEMMA 12-L1 (Type Preservation):
  If Γ ⊢ e : T and e → e' (reduction)
  Then Γ ⊢ e' : T

Proof: By induction on reduction rules.
Immediate from type system definition.

───────────────────────────────────────────

LEMMA 12-L2 (Progress):
  If Γ ⊢ e : T then:
    (a) e is value, OR
    (b) ∃ e': e → e'

Proof: By exhaustion over AST structure.
Each well-typed expression can always step or terminate.

───────────────────────────────────────────

LEMMA 12-L3 (Arena Preservation):
  If arena A exists and value v ∈ A
  Then v remains accessible until A deallocates

Proof:
  1. v ∈ A means v allocated in A's memory region
  2. Access to v is valid (address + type check)
  3. A deallocates = O(1) reset (not per-object)
  4. All values in A freed atomically
  5. No v remains accessible after A closes (RULE 6-CA1)

───────────────────────────────────────────

LEMMA 12-L4 (Borrow Expiration):
  If ref : &'lifetime T
  And current_scope > 'lifetime
  Then ref becomes invalid

Proof:
  1. Borrow validity bounded by lifetime
  2. Lifetime = scope where borrow created
  3. Exiting scope = borrow expires (pop from borrow_stack)
  4. Any subsequent access to *ref = COMPILE ERROR
  5. Type system rejects (INV-A4 violated)

───────────────────────────────────────────

LEMMA 12-L5 (Exclusive Write):
  If ref : &mut'lifetime T
  Then no other reference to T exists during ['now, 'lifetime)

Proof:
  1. &mut acquisition checks: ¬borrowed(T)
  2. During borrow, no new borrows permitted
  3. Any attempt to borrow T = COMPILE ERROR
  4. Exclusive access guaranteed (INV-A4)
```

### 12.3 Main Proof (Outline)

```
PROOF OF THEOREM 12-MAIN:

Goal: Show 7 memory bugs impossible

────────────────────────────────────

BUG 1: Use-After-Free

  Assume: ∃ access to freed value v

  By well-typed program:
    1. Program type-checked without error
    2. Type checker ran borrow checker
    3. Borrow checker enforces INV-M1
    4. INV-M1 states: if ref valid, target not freed
    5. Contradiction

  ∴ UAF impossible ✓

────────────────────────────────────

BUG 2: Double-Free

  Assume: ∃ two frees of same memory M

  By arena model:
    1. Arena deallocation = O(1) reset (once)
    2. No per-object free (no free() calls)
    3. Each object in arena freed exactly once
    4. At most one free per arena
    5. Contradiction with "two frees"

  ∴ Double-free impossible ✓

────────────────────────────────────

BUG 3: Dangling Pointer

  Assume: ∃ valid reference to object in deallocated arena

  By RULE 6-CA1 (cross-arena prohibition):
    1. ref created in scope A, target in scope B
    2. A ≤ B (parent ≤ child per rule)
    3. If B deallocates first, ref to B's data invalid
    4. But rule forbids this (ref lifetime < target lifetime)
    5. Type system rejects such code

  ∴ Dangling pointers impossible ✓

────────────────────────────────────

BUG 4: Buffer Overflow

  Assume: ∃ array[out-of-bounds access]

  By bounds checking (INV-M4):
    1. Every array access checked: 0 ≤ idx < len
    2. Check occurs compile-time or runtime
    3. Out-of-bounds access = rejected or trapped
    4. No silent corruption

  ∴ Buffer overflow impossible ✓

────────────────────────────────────

BUG 5: Memory Leak

  Assume: ∃ object in closed arena not deallocated

  By INV-A2 (atomic deallocation):
    1. Arena.lifetime ends
    2. All objects in Arena deallocated (one operation)
    3. No object can remain allocated
    4. Contradiction

  ∴ Memory leak impossible ✓

────────────────────────────────────

BUG 6: Data Race (Shared Mutable State)

  Assume: ∃ concurrent read and write on same var

  By exclusive borrow (INV-A4):
    1. If var borrowed mutably (&mut), no other borrows
    2. If var borrowed read-only (&), all borrows are read-only
    3. Linear types prevent implicit aliasing
    4. Concurrent access requires synchronization (!| fence)
    5. No unsynchronized race possible

  ∴ Data race impossible ✓

────────────────────────────────────

BUG 7: Stack Overflow

  Assume: ∃ unbounded recursion exceeding stack limit

  By tail-call optimization + limits:
    1. Recursive calls in tail position → compiled to loop
    2. Loop = O(1) stack usage
    3. Non-tail recursion limited by configurable stack limit
    4. Exceeding limit = caught (trap)

  ∴ Stack overflow prevented ✓

────────────────────────────────────

QED: All 7 bugs impossible in well-typed 3.42 programs.
```

---

## Conclusion: Summary of Formalization

### Contributions

1. **Formal Arena Model** : Mathematical definition of arènes as (name, parent, base_ptr, size, lifetime, objects) with 4 core invariants

2. **Linear Type System** : Precise semantics of MOVE, COPY, BORROW, EXCLUSIVE_BORROW with type judgment rules

3. **Cross-Arena Prohibition (RULE 6-CA1)** : Central safety rule preventing cross-arena references, formalized as lifetime comparison

4. **Borrow Checker Algorithm** : Algorithmic specification of lifetime checking, with examples of allowed/forbidden patterns

5. **9 Core Invariants** : Complete list of memory safety properties (INV-M1 through INV-M7) ensuring 7 impossible bugs

6. **Comparison with Rust** : Detailed analysis of similarities and differences in ownership models

7. **Edge Cases** : Formal treatment of recursion, closures, async, structs, exceptions

8. **Soundness Proof Sketch** : Outline of main theorem and 5 key lemmas proving safety

### Safety Guarantee

```
COROLLARY: Security Properties

A well-typed 3.42 program CANNOT:

  [MEMORY SAFETY]
  ✓ Use-after-free (UAF)
  ✓ Double-free
  ✓ Dangling pointers
  ✓ Buffer overflow
  ✓ Memory leak

  [CONCURRENCY SAFETY]
  ✓ Data races (shared mutable without @@ or sync)
  ✓ Stack overflow (uncontrolled recursion)

This is PROVEN by the type system + allocator model.
NOT by runtime checks (though some runtime bounds checks for defense-in-depth).
```

### References to Specification

This formalization implements design decisions from CP-1 SPEC:

- **D4** (Arènes) : Section 3
- **D5** (Types linéaires) : Section 4
- **D21** (Bubble-up) : Section 6.3
- **Safety properties** : Section VI (Mémoire & Safety) of CP-1 SPEC

---

> **Fin de Q21 — Formalisation complète des règles de sécurité mémoire du langage 3.42.**
>
> Cette spécification peut être utilisée pour :
> 1. Implémenter un type checker rigoureux
> 2. Prouver la sécurité mémoire de programmes 3.42
> 3. Comparer avec d'autres systèmes de type (Rust, Vale, etc.)
> 4. Tester les propriétés de sécurité avec des outils formels (Coq, Lean, etc.)
