# Q20 — IMPLEMENTATION ROADMAP

> **Statut** : Prêt pour développement
> **Effort estimé** : 4-6 semaines (équipe de 2-3 développeurs)
> **Dépendances** : Arena allocators (D4), Types linéaires (D5), Cryptographie

---

## PHASE 1 : FONDATION (SEMAINE 1-2)

### 1.1 Trait Diffable Core

**Fichier** : `src/lang/traits/diffable.rs`

```rust
pub trait Diffable {
    type Diff: Diffable;
    type Snap: Diffable;

    fn snapshot(&self) -> Self::Snap;
    fn diff(old: &Self::Snap, new: &Self::Snap) -> Self::Diff;
    fn patch(snap: &Self::Snap, delta: &Self::Diff) -> Self::Snap;
    fn size_estimate(&self) -> u32;
}
```

**Tests** (15 tests) :
- [ ] Snapshots créés avec timestamp correct
- [ ] Diff entre valeurs identiques = vide
- [ ] Patch appliqué reconstruit correctement
- [ ] Size estimate raisonnable

**Temps estimé** : 2-3 heures

---

### 1.2 Snapshot<T> Type

**Fichier** : `src/types/snapshot.rs`

```rust
pub struct Snapshot<T: Diffable> {
    pub value: T,
    pub timestamp: u64,
    pub hash: Hash256,
    pub arena: ArenaRef,
    pub retention: RetentionPolicy,
}

impl<T: Diffable> Snapshot<T> {
    pub fn new(value: T) -> Self { ... }
    pub fn read(&self) -> T { ... }
    pub fn is_expired(&self) -> bool { ... }
}
```

**Tests** (10 tests) :
- [ ] Snapshot création, allocation
- [ ] is_expired correct selon policy
- [ ] Hash calculé correctement

**Temps estimé** : 2 heures

---

### 1.3 Diff<T> Type

**Fichier** : `src/types/diff.rs`

```rust
pub struct Diff<T: Diffable> {
    pub operation: DiffOp,
    pub operand1: u64,
    pub operand2: Value,
    pub timestamp: u64,
    pub author: Option<PublicKey>,
    pub previous_hash: Option<Hash256>,
}

pub enum DiffOp {
    Set,
    Replace,
    Insert,
    Delete,
    Transform,
    Comment,
}

impl<T: Diffable> Diff<T> {
    pub fn new(op: DiffOp, loc: u64, val: Value) -> Self { ... }
    pub fn verify(&self) -> Result<(), SignatureError> { ... }
}
```

**Tests** (12 tests) :
- [ ] Diff creation
- [ ] Signature verification if present
- [ ] Size estimate

**Temps estimé** : 2 heures

---

### 1.4 RetentionPolicy Enum

**Fichier** : `src/types/retention.rs`

```rust
pub enum RetentionPolicy {
    Forever,
    LastN { count: usize },
    TTL { max_age_ns: u64 },
    Compressed { every_n_snapshots: usize },
    Smart { sample_rate: f64 },
}

impl RetentionPolicy {
    pub fn should_keep(&self, idx: usize, now: u64) -> bool { ... }
    pub fn default() -> Self { ... }
}
```

**Tests** (20 tests) :
- [ ] FOREVER keeps everything
- [ ] LAST_N drops old correctly
- [ ] TTL based on timestamp
- [ ] COMPRESSED keeps every n
- [ ] SMART uses probability

**Temps estimé** : 3 heures

---

## PHASE 2 : HISTORY<T> CONTAINER (SEMAINE 2-3)

### 2.1 History<T> Structure

**Fichier** : `src/types/history.rs`

```rust
pub struct History<T: Diffable> {
    snapshots: Vec<Snapshot<T>>,
    diffs: Vec<Diff<T>>,
    current_idx: usize,
    arena: ArenaRef,
    retention: RetentionPolicy,
    lazy_evaluation: bool,
    branches: HashMap<String, History<T>>,
}

impl<T: Diffable> History<T> {
    pub fn new(initial: T, retention: RetentionPolicy) -> Self { ... }
    pub fn record(&mut self, new_val: T) { ... }
    pub fn current(&self) -> T { ... }
    pub fn at(&self, idx: usize) -> Option<T> { ... }
    pub fn undo(&mut self) { ... }
    pub fn redo(&mut self) { ... }
}
```

**Tests** (30 tests) :
- [ ] Creation avec snapshot initial
- [ ] record() crée snapshot et diff
- [ ] current() retourne bon état
- [ ] at() retourne bon index
- [ ] undo/redo navigation
- [ ] undo/redo au boundary (0, len-1)

**Temps estimé** : 6 heures

---

### 2.2 Branching

**Fichier** : `src/types/history.rs` (continué)

```rust
impl<T: Diffable> History<T> {
    pub fn branch(&mut self, name: String) -> History<T> { ... }
    pub fn merge(&mut self, other: History<T>) -> Result<(), MergeError> { ... }
}
```

**Algorithme merge** (3-way) :

```
fn three_way_merge<T>(
    ancestor: &Snapshot<T>,
    ours: &Snapshot<T>,
    theirs: &Snapshot<T>,
) -> Result<Snapshot<T>, Vec<ConflictMarker>> {
    let our_diff = T::diff(ancestor, ours);
    let their_diff = T::diff(ancestor, theirs);

    // Apply both diffs si aucun conflit
    // Marquer conflits si overlapping changes
}
```

**Tests** (20 tests) :
- [ ] Branch creation
- [ ] Branch independence
- [ ] Merge sans conflit
- [ ] Merge avec conflits détectés
- [ ] Conflict markers OK

**Temps estimé** : 8 heures

---

### 2.3 Indexing (Optionnel Phase 1)

**Fichier** : `src/types/history.rs` (continué)

```rust
pub struct History<T: Diffable> {
    // ... autres champs ...
    timestamp_index: HashMap<u64, usize>,
    author_index: HashMap<PublicKey, Vec<usize>>,
    hash_index: HashMap<Hash256, usize>,
    lazy_evaluation: bool,
}

impl<T: Diffable> History<T> {
    pub fn find_by_timestamp(&self, ts: u64) -> Option<T> { ... }
    pub fn find_by_author(&self, author: &PublicKey) -> Vec<usize> { ... }
    pub fn find_by_hash(&self, h: &Hash256) -> Option<usize> { ... }
}
```

**Tests** (15 tests) :
- [ ] Indices créés automatiquement
- [ ] Lookup O(1) correct
- [ ] Index en sync avec snapshots

**Temps estimé** : 4 heures

---

## PHASE 3 : CONCRETE IMPLEMENTATIONS (SEMAINE 3-4)

### 3.1 Impl Diffable for i32

**Fichier** : `src/impls/diffable_int.rs`

```rust
impl Diffable for i32 {
    type Diff = DiffInt;
    type Snap = Snapshot<i32>;

    fn snapshot(&self) -> Self::Snap {
        Snapshot::new(*self)
    }

    fn diff(old: &Self::Snap, new: &Self::Snap) -> Self::Diff {
        DiffInt {
            old: old.read(),
            new: new.read(),
        }
    }

    fn patch(snap: &Self::Snap, delta: &Self::Diff) -> Self::Snap {
        Snapshot::new(delta.new)
    }

    fn size_estimate(&self) -> u32 { 8 }
}
```

**Tests** (10 tests) :
- [ ] snapshot correct
- [ ] diff correct
- [ ] patch rebuilds value
- [ ] Mathematical properties

**Temps estimé** : 1 heure

---

### 3.2 Impl Diffable for String (LCS)

**Fichier** : `src/impls/diffable_string.rs`

```rust
#[derive(Clone)]
pub enum StringDiffOp {
    Replace { range: (usize, usize), text: String },
    Insert { pos: usize, text: String },
    Delete { pos: usize, len: usize },
}

impl Diffable for String {
    type Diff = Vec<StringDiffOp>;
    type Snap = Snapshot<String>;

    fn diff(old: &Self::Snap, new: &Self::Snap) -> Self::Diff {
        let old_str = old.read();
        let new_str = new.read();

        // LCS algorithm
        let lcs = longest_common_subsequence(&old_str, &new_str);

        // Convert LCS to operations
        let mut ops = Vec::new();
        // ... implementation ...
        ops
    }

    fn patch(snap: &Self::Snap, delta: &Self::Diff) -> Self::Snap {
        let mut s = snap.read();
        for op in delta {
            match op {
                StringDiffOp::Replace { range, text } => {
                    s = s[..range.0].to_string() + text + &s[range.1..];
                }
                // ... other ops ...
            }
        }
        Snapshot::new(s)
    }
}
```

**Tests** (25 tests) :
- [ ] LCS correctness (educational tests)
- [ ] Diff generation pour mutations
- [ ] Patch reconstruction exacte
- [ ] Size estimate < snapshot

**Temps estimé** : 6 heures

---

### 3.3 Impl Diffable for Vec<T>

**Fichier** : `src/impls/diffable_vec.rs`

```rust
#[derive(Clone)]
pub enum ArrayDiffOp<T: Diffable> {
    Set { index: usize, value: T },
    Insert { pos: usize, values: Vec<T> },
    Delete { pos: usize, len: usize },
    Move { from: usize, to: usize, len: usize },
}

impl<T: Diffable> Diffable for Vec<T> {
    type Diff = Vec<ArrayDiffOp<T>>;
    type Snap = Snapshot<Vec<T>>;

    fn diff(old: &Self::Snap, new: &Self::Snap) -> Self::Diff {
        // Myers diff algorithm
        let mut ops = Vec::new();
        // ... implementation ...
        ops
    }

    fn patch(snap: &Self::Snap, delta: &Self::Diff) -> Self::Snap {
        let mut arr = snap.read();
        for op in delta {
            match op {
                ArrayDiffOp::Set { index, value } => arr[index] = value,
                // ... other ops ...
            }
        }
        Snapshot::new(arr)
    }
}
```

**Tests** (20 tests) :
- [ ] Myers diff correctness
- [ ] Set, Insert, Delete, Move operations
- [ ] Patch reconstruction
- [ ] Performance pour gros arrays

**Temps estimé** : 8 heures

---

### 3.4 Derive(Diffable) Macro

**Fichier** : `src/macros/derive_diffable.rs`

```rust
#[proc_macro_derive(Diffable)]
pub fn derive_diffable(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as DeriveInput);

    let name = &input.ident;
    let fields = /* extract fields */;

    // Generate impl for each field
    let field_diffs = fields.iter().map(|f| {
        let field_name = &f.ident;
        quote! {
            #field_name: if old.#field_name == new.#field_name {
                None
            } else {
                Some(T::diff(&old.#field_name, &new.#field_name))
            }
        }
    });

    // ... generate full impl ...
}
```

**Tests** (15 tests) :
- [ ] Compilation pour simples structs
- [ ] Compilation pour structs avec génériques
- [ ] Champs optionnels
- [ ] Granularité par champ

**Temps estimé** : 6 heures

---

## PHASE 4 : ADVANCED FEATURES (SEMAINE 4-5)

### 4.1 Lazy Evaluation

**Fichier** : `src/types/lazy_diff.rs`

```rust
pub struct LazyDiff<T: Diffable> {
    delta: Option<T::Diff>,
    computed: bool,
    access_count: u64,
}

impl<T: Diffable> LazyDiff<T> {
    pub fn get(&mut self) -> &T::Diff { ... }
    pub fn should_evict(&self, threshold: u64) -> bool { ... }
}

impl<T: Diffable> History<T> {
    pub fn record_lazy(&mut self, new_val: T) { ... }
    pub fn diff_at(&mut self, idx: usize) -> &T::Diff { ... }
    pub fn compact(&mut self, threshold: u64) { ... }
}
```

**Tests** (15 tests) :
- [ ] Lazy diffs non calculés initialement
- [ ] get() calcule si nécessaire
- [ ] Compact évince les peu accédés
- [ ] Memory savings

**Temps estimé** : 4 heures

---

### 4.2 CertifiedDiff & Blockchain

**Fichier** : `src/types/certified_diff.rs`

```rust
pub struct CertifiedDiff<T: Diffable> {
    content: T::Diff,
    author: PublicKey,
    timestamp: u64,
    previous_hash: Hash256,
    signature: Signature,
    merkle_path: Vec<Hash256>,
}

impl<T: Diffable> History<T> {
    pub fn to_certified_chain(&self) -> Vec<CertifiedDiff<T>> { ... }
    pub fn verify_chain(&self, chain: &[CertifiedDiff<T>]) -> bool { ... }
}
```

**Tests** (15 tests) :
- [ ] Signature generation
- [ ] Signature verification
- [ ] Hash chaining
- [ ] Merkle tree construction

**Temps estimé** : 5 heures

---

### 4.3 Fractal : History<History<T>>

**Fichier** : `src/impls/diffable_history.rs`

```rust
impl<T: Diffable> Diffable for History<T> {
    type Diff = HistoryDiff<T>;
    type Snap = Snapshot<History<T>>;

    fn diff(old: &Self::Snap, new: &Self::Snap) -> Self::Diff {
        // Diff between two histories
        // Compares snapshots, tracks branches, etc.
    }

    fn patch(snap: &Self::Snap, delta: &Self::Diff) -> Self::Snap {
        // Apply history-level diff
    }
}
```

**Tests** (10 tests) :
- [ ] History<History<i32>> compilation
- [ ] Diff deux histories
- [ ] Patch history-level
- [ ] Git-like repository structure

**Temps estimé** : 4 heures

---

## PHASE 5 : OPTIMIZATION & TESTING (SEMAINE 5-6)

### 5.1 Benchmarks

**Fichier** : `benches/diffable_bench.rs`

```rust
fn benchmark_snapshot_latency(c: &mut Criterion) {
    c.bench_function("snapshot_i32_1000", |b| {
        let mut hist = History::new(0);
        b.iter(|| {
            for i in 1..1000 {
                hist.record(i);
            }
        });
    });
    // ... other benches ...
}
```

**Mesures clés** :
- [ ] Snapshot latency (cible : < 1 µs)
- [ ] Diff latency (cible : < 10 µs)
- [ ] Patch latency (cible : < 100 µs)
- [ ] Memory usage pour 1K, 10K, 100K snapshots
- [ ] Compression ratios pour strings, arrays

**Temps estimé** : 6 heures

---

### 5.2 Fuzz Testing

**Fichier** : `tests/fuzz_diffable.rs`

```rust
#[test]
fn fuzz_diffable_properties() {
    for _ in 0..10000 {
        let val1 = random_value();
        let val2 = random_value();

        // Property 1 : patch identity
        let snap1 = val1.snapshot();
        let diff = T::diff(&snap1, &val1.snapshot());
        assert_eq!(T::patch(&snap1, &diff), snap1);

        // Property 2 : patch reconstruction
        let snap2 = val2.snapshot();
        let diff = T::diff(&snap1, &snap2);
        assert_eq!(T::patch(&snap1, &diff), snap2);

        // ... other properties ...
    }
}
```

**Temps estimé** : 4 heures

---

### 5.3 Integration Tests

**Fichier** : `tests/integration_diffable.rs`

```rust
#[test]
fn test_editor_workflow() {
    let mut hist = History::new(String::new(), RetentionPolicy::FOREVER);

    hist.record("hello".to_string());
    hist.record("hello world".to_string());
    hist.record("hello world!".to_string());

    assert_eq!(hist.at(0), Some("hello".to_string()));
    assert_eq!(hist.at(1), Some("hello world".to_string()));

    hist.undo();
    assert_eq!(hist.current(), "hello world".to_string());

    hist.redo();
    assert_eq!(hist.current(), "hello world!".to_string());
}
```

**Temps estimé** : 6 heures

---

## PHASE 6 : DOCUMENTATION & POLISH (SEMAINE 6)

### 6.1 API Documentation

```rust
/// The core Diffable trait.
///
/// Types implementing Diffable can be:
/// - Snapshottted efficiently
/// - Diffed (delta computed)
/// - Patched (delta applied)
/// - Versioned (History<T> container)
///
/// # Examples
///
/// ```
/// let hist = History::new(42, RetentionPolicy::FOREVER);
/// hist.record(100);
/// assert_eq!(hist.current(), 100);
/// hist.undo();
/// assert_eq!(hist.current(), 42);
/// ```
pub trait Diffable { ... }
```

**Temps estimé** : 3 heures

---

### 6.2 User Guide

Create `docs/diffable_guide.md` with:
- Quick start
- Common patterns
- Best practices
- Troubleshooting

**Temps estimé** : 2 heures

---

### 6.3 Release Preparation

- [ ] Changelog
- [ ] Version bumps
- [ ] Feature flags
- [ ] Breaking changes documentation

**Temps estimé** : 2 heures

---

## SUMMARY TIMELINE

| Phase | Semaine | Effort | Outputs |
|-------|---------|--------|---------|
| 1 : Fondation | 1-2 | 9h | Trait + Types core |
| 2 : History<T> | 2-3 | 18h | Container + branching |
| 3 : Impls | 3-4 | 25h | i32, String, Vec, Macro |
| 4 : Advanced | 4-5 | 18h | Lazy, Blockchain, Fractal |
| 5 : Optimization | 5-6 | 20h | Benchmarks + Tests |
| 6 : Polish | 6 | 7h | Docs + Release |

**Total** : ~97 heures = 4 semaines à 2 devs = **2 semaines parallèle**

---

## DEPENDENCIES

- ✅ Arena allocators (D4) — déjà implémentés
- ✅ Types linéaires (D5) — déjà implémentés
- ✅ Crypto (SHA256, Ed25519) — externe (sodiumoxide)
- ✅ Hash tables — Rust std::collections::HashMap

---

## SUCCESS CRITERIA

- [ ] Tous les tests passent (100+ tests)
- [ ] Benchmarks montrent > 4x compression vs full copy
- [ ] Memory use < 100 MB pour 10K String snapshots
- [ ] Snapshot latency < 1 µs
- [ ] Documentation complète + exemples
- [ ] Pas de regressions de performance du compilateur

---

## RISKS & MITIGATION

| Risque | Impact | Mitigation |
|--------|--------|-----------|
| LCS trop lent | Perf | Use approximate diff algo |
| Memory explosion | Safety | Lazy + compress + retention |
| API complexity | UX | Clear examples, guides |
| Macro bugs | Correctness | Extra fuzzing |

---

**Claude & Alexis Mounib — Février 2026**

Ready for development!
