# Q20 — DIFFABLE CHEATSHEET

Référence rapide du système de traits Diffable pour 3.42.

---

## DÉFINITIONS RAPIDES

### Trait Diffable (Core)

```342
trait Diffable {
  type DIFF DIFFABLE
  type SNAP DIFFABLE

  :snapshot (self) SNAP              /* Capturer l'état actuel */
  :diff (old SNAP, new SNAP) DIFF    /* Calculer la différence */
  :patch (snap SNAP, delta DIFF) SNAP /* Appliquer une modification */
  :size_estimate () u32              /* Estimer la taille mémoire */
}
```

### Types Clés

```342
type Snapshot { T: Diffable } {
  value T
  timestamp u64
  hash Hash256
  arena Arena
  retention RetentionPolicy
}

type Diff { T: Diffable } {
  operation DiffOp
  operand1 u64
  operand2 Value
  timestamp u64
  author PublicKey
  previous_hash Hash256
}

type History { T: Diffable } {
  snapshots Vec { Snapshot { T } }
  diffs Vec { Diff { T } }
  current_idx u64
  arena Arena
  retention RetentionPolicy
  lazy_evaluation bool
  branches Map { String, History { T } }
}
```

---

## OPÉRATIONS COURANTES

### Créer un Historique

```342
hist : History::new(initial_value, RetentionPolicy::FOREVER)
hist : History::new(value, RetentionPolicy::LAST_N { count: 1000 })
hist : History::new(value, RetentionPolicy::TTL { max_age_ns: 3600_000_000_000 })
```

### Enregistrer une Modification

```342
hist:record(new_value)          /* Créer un snapshot + diff */
hist:record_lazy(new_value)     /* Lazy : diff calculé à la demande */
```

### Voyager dans le Temps

```342
current = hist:current()        /* État actuel */
past = hist:at(index)           /* État à l'index i */
hist:undo()                     /* Annuler la dernière modif */
hist:redo()                     /* Rétablir une modif annulée */
```

### Branching et Merge

```342
alt_branch = hist:branch("name")
hist:merge(other_branch)
```

### Exporter Blockchain

```342
certified_chain = hist:to_certified_chain()
```

---

## RETENTION POLICIES

| Policy | Usage | Exemple |
|--------|-------|---------|
| `FOREVER` | Tests, dev | `RetentionPolicy::FOREVER` |
| `LAST_N` | Production | `LAST_N { count: 1000 }` |
| `TTL` | Serveurs | `TTL { max_age_ns: 3600_000_000_000 }` |
| `COMPRESSED` | Gros volumes | `COMPRESSED { every_n: 10 }` |
| `SMART` | Adaptable | `SMART { sample_rate: 0.1 }` |

---

## IMPLÉMENTATIONS

### Pour i32

```342
impl Diffable for i32 {
  type DIFF = DiffInt { old i32, new i32 }
  type SNAP = Snapshot { i32 }

  :snapshot (self) SNAP { Snapshot::new(self) }
  :diff (old, new) DiffInt { { old: old->read(), new: new->read() } }
  :patch (snap, delta) SNAP { Snapshot::new(delta.new) }
  :size_estimate () u32 { > 8 }
}
```

### Pour String (avec LCS)

```342
impl Diffable for String {
  type DIFF = Vec { StringDiffOp }
  type SNAP = Snapshot { String }

  /* Utilise LCS pour compact diffs */
  /* Diffs = [DELETE, INSERT, REPLACE, CONCATENATE] */
}
```

### Pour Vec<T>

```342
impl { T: Diffable } Diffable for Vec { T } {
  type DIFF = Vec { ArrayDiffOp }
  type SNAP = Snapshot { Vec { T } }

  /* Utilise Myers diff */
  /* Diffs = [SET, INSERT, DELETE, MOVE] */
}
```

### Pour Struct (macro #[derive(Diffable)])

```342
#[derive(Diffable)]
type Player {
  name String
  level i32
  health i32
}

/* Macro génère implémentation granulaire par champ */
```

---

## LAZY EVALUATION

Activer/désactiver à la création :

```342
hist.lazy_evaluation : true              /* Diffs à la demande */
hist.lazy_evaluation : false             /* Diffs immédiates (défaut) */

hist:compact(access_threshold: 1)        /* Éviction des peu accédés */
```

---

## PERFORMANCE CHECKLISTS

### Memory
- [ ] Snapshots compressés via CoW
- [ ] Diffs = delta encoding
- [ ] Arena dédiée (O(1) cleanup)
- [ ] LazyDiff si > 10K snapshots

### Latency
- [ ] Indexed snapshots (timestamp_index, hash_index)
- [ ] Per-expression granularity (défaut)
- [ ] Undo = O(1) seek + O(diff) patch

### Safety
- [ ] Cross-arena refs = INTERDIT
- [ ] Types linéaires = zéro dangling ptr
- [ ] Guard pages en debug

---

## EXEMPLES RAPIDES

### Éditeur de Texte

```342
hist : History::new("", RetentionPolicy::LAST_N { count: 10000 })

/* Typing simulé */
hist:record("hello")
hist:record("hello ")
hist:record("hello world")

hist:undo()  /* "hello " */
hist:redo()  /* "hello world" */
```

### Sauvegarde d'Jeu RPG

```342
player : Player { level: 1, hp: 100, exp: 0 }
hist : History::new(player, RetentionPolicy::FOREVER)

savepoint : hist:current()

/* Jouer... */
hist:record(player_after_quest_1)
hist:record(player_defeated)

/* Recharger */
hist:undo()
hist:undo()
assert(hist:current() == savepoint)
```

### Version Control (Git-like)

```342
file_hist : History::new(code_v1, RetentionPolicy::FOREVER)
file_hist:record(code_v2)
file_hist:record(code_v3)

certified : file_hist:to_certified_chain()
/* Envoyer à blockchain */
```

### Branching (Décisions)

```342
timeline_main : History::new(game_state, RetentionPolicy::FOREVER)
timeline_alt : timeline_main:branch("choice_B")

/* Main continue */
timeline_main:record(consequence_A)

/* Alt diverge */
timeline_alt:record(consequence_B)

/* Les deux existent indépendamment */
```

---

## FRÉQUENT QUESTIONS

### Q : Quel overhead si je n'utilise pas History ?
**A** : Zéro. Le trait Diffable est optionnel, seul appel history:record() crée les snapshots.

### Q : Pourquoi pas juste utiliser Git externe ?
**A** : History<T> est **intégré au langage** → undo/redo partout (kernel, compiler, filesystem, UI) → cohérence globale.

### Q : Peut-on avoir History<History<T>> ?
**A** : Oui ! C'est fractal. History<History<T>> : Diffable si T : Diffable.

### Q : Copy-on-Write quand exactement ?
**A** : À la création du snapshot. Tant que personne n'écrit, la mémoire est partagée.

### Q : Quelle granularité choisir ?
**A** : Par défaut PER_EXPRESSION. PER_STATEMENT pour développement fin, PER_BLOCK pour production.

### Q : Combien d'historique garder ?
**A** : LAST_N { count: 1000 } pour la plupart. TTL si serveur. FOREVER pour tests.

### Q : Sécurité des diffs sur blockchain ?
**A** : Wrapper CertifiedDiff<T> pour signatures. Optionnel.

---

## BOSONS MAPPÉS AUX DIFFS

| Boson | Verbe | Diff operation |
|-------|-------|---|
| `<` | STOCKER | SET |
| `>` | CHARGER | READ (snapshot) |
| `.` | ACCÉDER | INDEX |
| `:` | DÉFINIR | REPLACE |
| `=` | LIER | BIND |
| `^` | SUPERPOSER | INSERT |
| `\|` | CONNECTER | MOVE |
| `*` | TRANSFORMER | GENERALIZE |
| `_` | IGNORER | COMMENT |

---

## ARENA MANAGEMENT

Chaque History<T> a une arène dédiée :

```342
hist : History::new(val, policy)
/* hist.arena créée automatiquement */

hist:drop()
/* hist.arena libérée O(1) — tous les snapshots/diffs disparaissent */
```

**Important** : Pas de cross-arena refs. Sécurité garantie.

---

## INDEXING (OPTIONNEL)

Accélère les recherches :

```342
hist.timestamp_index[ts] → snapshot_index
hist.author_index[pk] → [diff_indices]
hist.hash_index[h256] → snapshot_index
```

Implémentation : HashMap, O(1) lookup.

---

## CHECKLIST IMPLÉMENTATION

Pour implémenter `Diffable` pour un nouveau type T :

- [ ] Définir `type DIFF` et `type SNAP`
- [ ] Implémenter `:snapshot()`
- [ ] Implémenter `:diff()` (algo compacte)
- [ ] Implémenter `:patch()`
- [ ] Implémenter `:size_estimate()`
- [ ] Tester propriétés mathématiques :
  - [ ] `patch(snap, diff(snap, snap)) == snap`
  - [ ] `compose(d1, d2)` fonctionne
  - [ ] `invert(diff)` possible si bijective
- [ ] Benchmarker latency et mémoire
- [ ] Ajouter à derive(Diffable) macro

---

## LINKS & REFERENCES

- **Spec complète** : `Q20-DIFFABLE-TRAIT-SYSTEM.md`
- **Examples** : `Q20-DIFFABLE-EXAMPLES.342`
- **Design decisions** : `Q20-DESIGN-DECISIONS.md`
- **Git internals** : https://git-scm.com/book/en/v2/Git-Internals-Git-Objects
- **VCDIFF (RFC 3284)** : Delta encoding reference
- **3-way merge** : https://en.wikipedia.org/wiki/Merge_(version_control)#Three-way_merge

---

**Claude & Alexis Mounib — Février 2026**
