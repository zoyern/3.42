# Q20 — SYSTÈME DE TRAITS DIFFABLE POUR 3.42

> **Auteur** : Claude (Anthropic)
> **Collaborateur** : Alexis Mounib (zoyern)
> **Date** : Février 2026
> **Référence** : CP-2-OPEN.md, SZN-SPHERE-VISION.md, MASTER-SEED-v2.md
> **Statut** : Conception technique — Prêt pour implémentation

---

## RÉSUMÉ EXÉCUTIF

Le système `Diffable` est le fondement de la **diff-chain** (Git universel, intégré au langage). Chaque valeur `T: Diffable` peut être :
- **Snapshottée** en O(1) avec Copy-on-Write (CoW)
- **Différenciée** via delta encoding (compact)
- **Versionné** dans `History<T>` (undo/redo universel)
- **Signée** cryptographiquement pour blockchain
- **Compressée** avec lazy evaluation

**Trois couches** :
1. **Trait Diffable** — interface minimale
2. **Système History<T>** — conteneur avec arène dédiée
3. **Implémentations concrètes** — int, string, array, struct

---

## PARTIE I : FONDATION THÉORIQUE

### 1.1 Principes Émergents

Le trait `Diffable` **n'est pas inventé de zéro** — il émerge de trois primitives existantes de 3.42 :

| Primitive | Rôle | Lien avec Diffable |
|-----------|------|-------------------|
| **Arènes** (allocateurs linéaires) | Mémoire efficace, zéro GC | Diff et snapshots vivent dans arènes dédiées |
| **Types linéaires** (MOVE par défaut) | Ownership clair | History<T> peut éjecter anciennes versions sans copy |
| **Gluons** (conteneurs {}, (), []) | Confinement de structure | History<T> est un gluon qui contient des diffs |

**Règle unifiée** :
```
La diff d'une valeur V entre état T1 et T2
= une séquence de mutations minimales, encodées en delta
= un boson appliqué à des fermions différentiels
```

### 1.2 Modèle Physique

```
PHYSIQUE CLASSIQUE          3.42 DIFF-CHAIN
────────────────────────    ──────────────────────
État (Ψ)                     Snapshot<T>
Évolution (dΨ/dt)           Diff<T> = delta entre snapshots
Observable (⟨O⟩)            Value<T>
Mécanique d'ensemble        History<T> = suite de diffs
```

**Métaphore** : Un snapshot est un point dans un espace de configuration. Un diff est le chemin minimal entre deux points.

### 1.3 Granularité : Pourquoi Delta Encoding

```
Option 1 : Full copy (naïf)
  Snapshot de [1,2,3,4,5] → 5 entiers copiés = 20 bytes

Option 2 : Delta encoding (smart)
  Snapshot init : [1,2,3,4,5]
  Diff : [index=2, old=3, new=99]    → 3 ints = 12 bytes
  Récupération : snapshot + 1 diff   → O(diff) pour reconstruction
```

**Choix** : Delta encoding + indexing pour accès rapide.

---

## PARTIE II : INTERFACE DIFFABLE (LE TRAIT)

### 2.1 Signature Minimale

```342
/* === DÉFINITION DU TRAIT DIFFABLE === */

trait Diffable {
  /* Type associé : la différence minimale entre deux valeurs */
  type DIFF DIFFABLE

  /* Snapshot : copie complète, ou structure CoW */
  type SNAP DIFFABLE

  /* 1. Créer un snapshot du présent */
  :snapshot (self) SNAP

  /* 2. Comparer deux états, produire leur delta */
  :diff (old SNAP, new SNAP) DIFF

  /* 3. Appliquer un delta à un snapshot */
  :patch (snap SNAP, delta DIFF) SNAP

  /* 4. Coût mémoire estimé (en octets) */
  :size_estimate () u32
}
```

### 2.2 Contrats Sémantiques

```342
/* === PROPRIÉTÉS GARANTIES === */

/* Propriété 1 : Identity for diff */
= patch (snap, diff(snap, snap)) snap

/* Propriété 2 : Composabilité */
= patch (snap1, delta12)
  patch (patch(snap1, delta12), delta23)
  patch (snap1, compose(delta12, delta23))

/* Propriété 3 : Invertibilité (pour undo) */
patch (snap2, invert(delta)) = snap1
  si delta = diff(snap1, snap2)

/* Propriété 4 : Efficacité comparée à un copy naïf */
size(delta) << size(snap)   /* pour petits changements */
```

---

## PARTIE III : SNAPSHOT ET DIFF TYPES

### 3.1 Type Snapshot<T>

```342
/* === SNAPSHOT : CAPTURE PONCTUELLE === */

type Snapshot { T : Diffable }
  = value T DIFFABLE
    timestamp u64              /* Unix nanoseconds */
    hash Hash256              /* Pour chaîning blockchain */
    arena Arena               /* Arène d'allocation du snapshot */
    retention RetentionPolicy /* Combien de temps le garder */

DÉFINIR Snapshot {
  /* Créer un snapshot via le trait */
  :new (val T) Snapshot {
    value : val DIFFABLE
    timestamp : clock()
    hash : hash256(val)
    arena : current_arena()
    retention : RetentionPolicy::default()
  }

  /* Accès thread-safe via indirection */
  :read () T DIFFABLE
    > value

  /* Invalider si trop ancien */
  :is_expired () ?
    clock() - timestamp > retention.max_age_ns
}
```

### 3.2 Type Diff<T>

```342
/* === DIFF : DELTA ENCODING === */

type Diff { T : Diffable }
  = operation DiffOp
    operand1 u64            /* Index ou offset */
    operand2 Value          /* Nouvelle valeur ou bytes */
    timestamp u64
    author PublicKey        /* Pour blockchain */
    previous_hash Hash256   /* Chaînage */

/* Opérations de base (émergentes de bosons) */
enum DiffOp {
  SET                 /* < boson : stocker */
  REPLACE             /* : boson : redéfinir */
  INSERT              /* ^ boson : superposer (insérer) */
  DELETE              /* | boson : connecter/effacer */
  TRANSFORM           /* * boson : généraliser */
  COMMENT             /* _ boson : ignorer (no-op) */
}

DÉFINIR Diff {
  :new (op DiffOp, loc u64, val Value) Diff {
    operation : op
    operand1 : loc
    operand2 : val
    timestamp : clock()
    author : current_key()
    previous_hash : last_diff_hash()
  }

  /* Compresser le diff (optionnel, lazy) */
  :compress () Diff
    /* GZIP ou autre si trop gros */

  /* Vérifier la signature */
  :verify () ?
    verify_signature(author, self)
}
```

### 3.3 Chaîne de Diffs (Diff-Chain)

```342
/* === DIFF-CHAIN : GIT INTÉGRÉ === */

type DiffChain { T : Diffable }
  = snapshots Vec { Snapshot { T } }
    diffs Vec { Diff { T } }
    current_index u64
    arena Arena

DÉFINIR DiffChain {
  /* Initialiser avec un snapshot root */
  :init (root T) DiffChain {
    snapshots : { Snapshot::new(root) }
    diffs : {}
    current_index : 0
    arena : Arena::new()
  }

  /* Enregistrer une modification */
  :record (new_val T) Unit
    old_snap = snapshots[current_index]
    delta = T::diff(old_snap, new_val)
    diffs <- Diff::new(delta)
    snapshots <- Snapshot::new(new_val)
    current_index : current_index + 1

  /* Voyager dans le temps */
  :seek (idx u64) T
    si idx < snapshots.len {
      > snapshots[idx]->read()
    }

  /* Undo : rejeter la dernière modif */
  :undo () T
    current_index : max(0, current_index - 1)
    > snapshots[current_index]->read()

  /* Redo : rejouer une modif annulée */
  :redo () T
    current_index : min(diffs.len, current_index + 1)
    > snapshots[current_index]->read()

  /* Branche : créer timeline alternative */
  :branch (name String) DiffChain
    /* Nouveau DiffChain à partir de current_index */
}
```

---

## PARTIE IV : IMPLÉMENTATIONS CONCRÈTES

### 4.1 Diffable pour Int32

```342
/* === IMPL DIFFABLE FOR INT32 === */

impl Diffable for i32 {
  type DIFF = DiffInt { old i32, new i32, index u64 }
  type SNAP = Snapshot { i32 }

  /* Snapshot trivial pour primitif */
  :snapshot (self) Snapshot {
    Snapshot {
      value : self
      timestamp : clock()
      hash : hash256(self)
      arena : current_arena()
      retention : RetentionPolicy::short()
    }
  }

  /* Diff minimale : juste old/new */
  :diff (old SNAP, new SNAP) DiffInt {
    DiffInt {
      old : old->read()
      new : new->read()
      index : 0
    }
  }

  /* Appliquer diff : trivial pour primitif */
  :patch (snap SNAP, delta DiffInt) SNAP {
    Snapshot::new(delta.new)
  }

  /* Taille : 2 i32 = 8 bytes */
  :size_estimate () u32
    > 8
}
```

### 4.2 Diffable pour String

```342
/* === IMPL DIFFABLE FOR STRING === */

enum StringDiffOp {
  REPLACE { range (u64, u64), new_text String }
  INSERT { pos u64, text String }
  DELETE { pos u64, len u64 }
  CONCATENATE { with String }
}

impl Diffable for String {
  type DIFF = Vec { StringDiffOp }
  type SNAP = Snapshot { String }

  :snapshot (self) SNAP {
    Snapshot::new(self)
  }

  /* Diff intelligente : LCS (Longest Common Subsequence) */
  :diff (old SNAP, new SNAP) Vec {
    old_str = old->read()
    new_str = new->read()

    /* Calcul LCS O(n*m) mais offline */
    lcs = longest_common_subsequence(old_str, new_str)

    /* Transformer LCS en séquence d'opérations */
    ops = Vec::new()

    /* Parcourir les différences */
    cursor_old = 0
    cursor_new = 0

    pour chaque élément dans lcs {
      pos = lcs[i]

      /* Caractères avant ce point = changement */
      si cursor_old < pos {
        ops <- StringDiffOp::DELETE {
          pos : cursor_old
          len : pos - cursor_old
        }
      }
      si cursor_new < new_position {
        ops <- StringDiffOp::INSERT {
          pos : cursor_new
          text : new_str[cursor_new..new_position]
        }
      }

      cursor_old : pos + 1
      cursor_new : new_position + 1
    }

    > ops
  }

  :patch (snap SNAP, delta Vec) SNAP {
    s = snap->read()

    pour op dans delta {
      match op {
        REPLACE { range, new_text } :
          s : s[0..range.0] + new_text + s[range.1..]

        INSERT { pos, text } :
          s : s[0..pos] + text + s[pos..]

        DELETE { pos, len } :
          s : s[0..pos] + s[pos + len..]

        CONCATENATE { with } :
          s : s + with
      }
    }

    > Snapshot::new(s)
  }

  :size_estimate () u32
    /* Heuristique : LCS prend ~25% de la chaîne */
    > (self.len() / 4) comme u32
}
```

### 4.3 Diffable pour Array<T>

```342
/* === IMPL DIFFABLE FOR ARRAY<T> === */

enum ArrayDiffOp { T : Diffable }
  = SET { index u64, value T }
    INSERT { pos u64, values Vec { T } }
    DELETE { pos u64, len u64 }
    MOVE { from u64, to u64, len u64 }

impl { T : Diffable } Diffable for Vec { T } {
  type DIFF = Vec { ArrayDiffOp { T } }
  type SNAP = Snapshot { Vec { T } }

  :snapshot (self) SNAP {
    /* Copy-on-Write : les éléments partagent arène */
    Snapshot::new(self)
  }

  :diff (old SNAP, new SNAP) Vec { ArrayDiffOp } {
    old_arr = old->read()
    new_arr = new->read()
    ops = Vec::new()

    /* Algorithmie Myers diff */
    i = 0
    j = 0

    tant que i < old_arr.len && j < new_arr.len {
      si old_arr[i] == new_arr[j] {
        i : i + 1
        j : j + 1
      } sinon {
        /* Chercher le prochain élément correspondant */
        found = false
        pour k en [i+1..old_arr.len] {
          si old_arr[k] == new_arr[j] {
            /* Effacer [i..k] */
            ops <- ArrayDiffOp::DELETE { pos : i, len : k - i }
            i : k + 1
            found : true
            break
          }
        }

        si non found {
          /* Insérer new_arr[j] */
          ops <- ArrayDiffOp::INSERT { pos : i, values : [new_arr[j]] }
          j : j + 1
        }
      }
    }

    /* Reste */
    si i < old_arr.len {
      ops <- ArrayDiffOp::DELETE { pos : i, len : old_arr.len - i }
    }
    si j < new_arr.len {
      ops <- ArrayDiffOp::INSERT {
        pos : i
        values : new_arr[j..new_arr.len]
      }
    }

    > ops
  }

  :patch (snap SNAP, delta Vec { ArrayDiffOp }) SNAP {
    arr = snap->read()

    pour op dans delta {
      match op {
        SET { index, value } :
          arr[index] : value

        INSERT { pos, values } :
          arr.insert_range(pos, values)

        DELETE { pos, len } :
          arr.remove_range(pos, len)

        MOVE { from, to, len } :
          arr.move_range(from, to, len)
      }
    }

    > Snapshot::new(arr)
  }

  :size_estimate () u32
    /* Cas worst-case : O(n) diffs pour n éléments */
    > (self.len() * 16) comme u32  /* 2 u64 par diff */
}
```

### 4.4 Diffable pour Struct (Générique)

```342
/* === IMPL DIFFABLE FOR STRUCT (MACRO) === */

/* Utiliser un dérivable avec macro */
#[derive(Diffable)]
type Person {
  name String
  age i32
  email String
  alive ?
}

/* Macro expansée :*/
impl Diffable for Person {
  type DIFF = PersonDiff
  type SNAP = Snapshot { Person }

  type PersonDiff {
    name_diff ? { Vec { StringDiffOp } }
    age_diff ? { DiffInt }
    email_diff ? { Vec { StringDiffOp } }
    alive_diff ? { ? }
  }

  :snapshot (self) SNAP {
    Snapshot::new(self)
  }

  :diff (old SNAP, new SNAP) PersonDiff {
    old_p = old->read()
    new_p = new->read()

    PersonDiff {
      name_diff :
        si old_p.name == new_p.name { } sinon
          { String::diff(old_p.name, new_p.name) }

      age_diff :
        si old_p.age == new_p.age { } sinon
          { i32::diff(old_p.age, new_p.age) }

      email_diff :
        si old_p.email == new_p.email { } sinon
          { String::diff(old_p.email, new_p.email) }

      alive_diff :
        si old_p.alive == new_p.alive { } sinon
          { old_p.alive }
    }
  }

  :patch (snap SNAP, delta PersonDiff) SNAP {
    p = snap->read()

    si delta.name_diff != {} {
      p.name : String::patch(p.name, delta.name_diff)
    }
    si delta.age_diff != {} {
      p.age : i32::patch(p.age, delta.age_diff)
    }
    si delta.email_diff != {} {
      p.email : String::patch(p.email, delta.email_diff)
    }
    si delta.alive_diff != {} {
      p.alive : delta.alive_diff
    }

    > Snapshot::new(p)
  }

  :size_estimate () u32
    > name.size_estimate()
      + age.size_estimate()
      + email.size_estimate()
      + 1
}
```

---

## PARTIE V : CONTENEUR HISTORY<T>

### 5.1 Structure Principale

```342
/* === HISTORY<T> : CONTENEUR UNIVERSEL === */

type History { T : Diffable }
  = snapshots Vec { Snapshot { T } }
    diffs Vec { Diff { T } }
    current_idx u64
    arena Arena                   /* Dédiée aux diffs/snapshots */
    retention RetentionPolicy     /* Politique globale */
    lazy_evaluation bool          /* Ne pas stocker les diffs inutilisées */
    branches Map { String, History { T } }  /* Branches git-like */

DÉFINIR History {
  /* Créer avec une valeur initiale */
  :new (initial T, retention RetentionPolicy) History {
    History {
      snapshots : { Snapshot::new(initial) }
      diffs : {}
      current_idx : 0
      arena : Arena::new()
      retention : retention
      lazy_evaluation : false
      branches : Map::new()
    }
  }

  /* Enregistrer une modification */
  :record (new_val T) Unit {
    assert (self.current_idx < self.snapshots.len)

    old_snap = self.snapshots[self.current_idx]
    delta = T::diff(old_snap, new_val)

    /* Créer le diff */
    diff = Diff::new(
      operation : delta
      timestamp : clock()
      author : current_key()
      previous_hash : last_hash()
    )

    /* Ajouter le snapshot et le diff */
    self.diffs <- diff
    self.snapshots <- Snapshot::new(new_val)
    self.current_idx : self.current_idx + 1

    /* Nettoyer si rétention dépassée */
    self:cleanup_expired()
  }

  /* Lire la valeur actuelle */
  :current () T {
    > self.snapshots[self.current_idx]->read()
  }

  /* Voyager dans le temps */
  :at (idx u64) T ? {
    si idx >= self.snapshots.len {
      > null
    }
    > self.snapshots[idx]->read()
  }

  /* Undo/Redo */
  :undo () T {
    si self.current_idx > 0 {
      self.current_idx : self.current_idx - 1
    }
    > self:current()
  }

  :redo () T {
    si self.current_idx < self.snapshots.len - 1 {
      self.current_idx : self.current_idx + 1
    }
    > self:current()
  }

  /* Créer branche */
  :branch (name String) History {
    new_branch = History::new(
      self:current()
      self.retention
    )
    self.branches[name] : new_branch
    > new_branch
  }

  /* Fusionner deux branches (3-way merge) */
  :merge (other_branch History) History {
    common_ancestor = find_lca(self, other_branch)
    result = three_way_merge(common_ancestor, self, other_branch)
    > result
  }

  /* Compresser l'historique (optimisation) */
  :compress () Unit {
    /* Fusionner les petits diffs */
    compressed = Vec::new()
    current_snap = self.snapshots[0]

    pour i en [1..self.diffs.len] {
      si i % 100 == 0 {
        /* Tous les 100 diffs, créer un snapshot */
        compressed <- (current_snap, Vec::new())
        current_snap : self.snapshots[i]
      } sinon {
        compressed <- self.diffs[i]
      }
    }

    self.diffs : compressed
  }

  /* Nettoyer les snapshots expirés */
  :cleanup_expired () Unit {
    si self.lazy_evaluation {
      pour snap dans self.snapshots {
        si snap:is_expired() {
          snap.arena:free()  /* Libérer l'arène du snapshot */
        }
      }
    }
  }

  /* Export blockchain-ready */
  :to_certified_chain () Vec { CertifiedDiff { T } } {
    result = Vec::new()

    pour i en [0..self.diffs.len] {
      result <- CertifiedDiff {
        content : self.diffs[i]
        author : self.diffs[i].author
        timestamp : self.diffs[i].timestamp
        previous_hash : self.diffs[i].previous_hash
        signature : sign(self.diffs[i], current_key())
      }
    }

    > result
  }
}
```

### 5.2 Politiques de Rétention

```342
/* === RETENTION POLICIES === */

enum RetentionPolicy {
  /* Garder tout */
  FOREVER

  /* Garder N derniers snapshots */
  LAST_N { count u64 }

  /* Garder pendant T secondes */
  TTL { max_age_ns u64 }

  /* Garder avec compression : O(n) snapshots, O(log n) accès */
  COMPRESSED { every_n_snapshots u64 }

  /* Smart : compresser selon usage pattern */
  SMART { sample_rate f64 }
}

DÉFINIR RetentionPolicy {
  :should_keep (idx u64, now u64) ? {
    match self {
      FOREVER :
        > true

      LAST_N { count } :
        > current_idx - idx < count

      TTL { max_age_ns } :
        > now - self.snapshots[idx].timestamp < max_age_ns

      COMPRESSED { every_n } :
        > idx % every_n == 0 || idx == current_idx

      SMART { sample_rate } :
        > random() < sample_rate || idx == current_idx
    }
  }

  :default () RetentionPolicy {
    > RetentionPolicy::LAST_N { count : 1000 }
  }

  :short () RetentionPolicy {
    > RetentionPolicy::TTL { max_age_ns : 3600_000_000_000 }  /* 1 hour */
  }

  :long () RetentionPolicy {
    > RetentionPolicy::LAST_N { count : 10000 }
  }
}
```

### 5.3 Lazy Evaluation

```342
/* === LAZY EVALUATION : NE STOCKER QUE CE QUI EST DEMANDÉ === */

type LazyDiff { T : Diffable }
  = delta ? { Diff { T } }        /* null tant qu'il n'est pas accédé */
    computed bool                 /* Déjà calculé ? */
    access_count u64              /* Combien de fois accédé */

DÉFINIR LazyDiff {
  /* Accès : calcule si nécessaire */
  :get () Diff {
    si non self.computed {
      self.delta : T::diff(old_snap, new_snap)
      self.computed : true
    }
    self.access_count : self.access_count + 1
    > self.delta
  }

  /* Éviction : supprimer si peu accédé */
  :should_evict (threshold u64) ? {
    > self.access_count < threshold && self.computed
  }
}

/* Dans History<T>, avec lazy_evaluation activé : */
impl { T : Diffable } History {
  :record_lazy (new_val T) Unit {
    /* Créer le snapshot mais pas le diff */
    self.snapshots <- Snapshot::new(new_val)

    /* Diff sera calculé seulement si demandé */
    self.diffs <- LazyDiff {
      delta : null
      computed : false
      access_count : 0
    }
  }

  /* Accès au diff déclenche le calcul */
  :diff_at (idx u64) Diff {
    > self.diffs[idx]:get()
  }

  /* Compacter : supprimer les diffs peu accédés */
  :compact (eviction_threshold u64) Unit {
    pour diff dans self.diffs {
      si diff:should_evict(eviction_threshold) {
        diff.delta : null
        diff.computed : false
      }
    }
  }
}
```

---

## PARTIE VI : INTÉGRATION AVEC ARÈNES

### 6.1 Allouation des Diffs

```342
/* === ARÈNES DÉDIÉES === */

/* Chaque History<T> a sa propre arène */

impl { T : Diffable } History {
  :allocate_snapshot (val T) Snapshot {
    /* Allouer dans arena dédiée */
    snap = self.arena:allocate(Snapshot {
      value : val
      timestamp : clock()
      hash : hash256(val)
      arena : self.arena    /* PRISE DE POSSESSION */
      retention : self.retention
    })

    > snap
  }

  :allocate_diff (op DiffOp, loc u64, val Value) Diff {
    /* Allouer dans arena dédiée */
    diff = self.arena:allocate(Diff {
      operation : op
      operand1 : loc
      operand2 : val
      timestamp : clock()
      author : current_key()
      previous_hash : last_hash()
    })

    > diff
  }

  /* Libération complète : arena entière droppée */
  :drop () Unit {
    /* Types linéaires : MOVE par défaut */
    /* Arène libérée en une seule opération O(1) */
    self.arena:free()
  }
}
```

### 6.2 Règles de Sécurité (Cross-Arena References)

```342
/* === INTERDICTION : CROSS-ARENA REFS === */

/* INTERDIT : Un snapshot ne peut pas référencer */
/* une valeur d'une autre arène */

/* Correct : */
hist1 : History::new(value1, retention1)
snap1 : hist1:allocate_snapshot(value1)
/* snap1 vit dans hist1.arena */

/* ERREUR de compilation : */
hist2 : History::new(value2, retention2)
snap1_copy : hist2:allocate_snapshot(snap1)
/* ERREUR : snap1 est dans hist1.arena, pas hist2.arena */

/* Solution : utiliser des indices ou hashes */
snap1_hash : hash256(snap1)
hist2:allocate_snapshot({
  reference : snap1_hash  /* Pas le snapshot directement */
})
```

---

## PARTIE VII : EXEMPLES PRATIQUES

### 7.1 Historique d'un Nombre

```342
/* === EXEMPLE 1 : HISTORIQUE D'UN ENTIER === */

hist_int : History::new(0, RetentionPolicy::FOREVER)

/* Enregistrer des modifications */
hist_int:record(42)      /* snapshot[0]=0, snapshot[1]=42 */
hist_int:record(100)     /* snapshot[2]=100 */
hist_int:record(-5)      /* snapshot[3]=-5 */

/* Consulter l'historique */
println(hist_int:at(0))  /* 0 */
println(hist_int:at(1))  /* 42 */
println(hist_int:at(2))  /* 100 */

/* Time travel */
hist_int:undo()
println(hist_int:current())  /* 100 */

hist_int:undo()
println(hist_int:current())  /* 42 */

hist_int:redo()
println(hist_int:current())  /* 100 */
```

### 7.2 Historique d'une String avec Diff

```342
/* === EXEMPLE 2 : HISTORIQUE D'UN TEXTE === */

hist_text : History::new("hello", RetentionPolicy::LAST_N {count : 100})

hist_text:record("hello world")      /* diff: insert(" world", 5) */
hist_text:record("hello world!")     /* diff: insert("!", 11) */
hist_text:record("goodbye world!")   /* diff: replace("hello" → "goodbye") */

/* Les diffs sont compacts : ~50 bytes au lieu de recopier la string */

/* Voir la rédaction pas à pas */
v0 = hist_text:at(0)   /* "hello" */
v1 = hist_text:at(1)   /* "hello world" */
v2 = hist_text:at(2)   /* "hello world!" */
v3 = hist_text:at(3)   /* "goodbye world!" */

/* Undo/Redo fonctionnent naturellement */
hist_text:undo()
println(hist_text:current())  /* "hello world!" */

hist_text:undo()
println(hist_text:current())  /* "hello world" */
```

### 7.3 Historique d'une Structure

```342
/* === EXEMPLE 3 : HISTORIQUE D'UN STRUCT === */

type Player {
  name String
  level i32
  exp i32
  inventory Vec { Item }
}

hist_player : History::new(
  Player {
    name : "Alice"
    level : 1
    exp : 0
    inventory : {}
  }
  RetentionPolicy::LAST_N { count : 10000 }
)

/* Progression du joueur */
hist_player:record(Player {
  name : "Alice"
  level : 1
  exp : 100
  inventory : { Item::SWORD }
})

hist_player:record(Player {
  name : "Alice"
  level : 2
  exp : 0
  inventory : { Item::SWORD, Item::SHIELD }
})

/* Les diffs sont granulaires : */
/* - exp : DiffInt { old: 0, new: 100 } */
/* - level : DiffInt { old: 1, new: 2 } */
/* - inventory : ArrayDiff { INSERT at 1 } */

/* Pas besoin de recopier "Alice" !*/

/* Savepoint et replay */
savepoint : hist_player:current()

hist_player:record(died_state)
hist_player:record(another_state)

/* Revenir au savepoint */
hist_player:undo()
hist_player:undo()
assert(hist_player:current() == savepoint)
```

### 7.4 Branching : Vies Alternatives

```342
/* === EXEMPLE 4 : BRANCHES (ALT TIMELINES) === */

/* Scénario RPG : choisir réponse A ou B */

main_timeline : History::new(game_state, RetentionPolicy::FOREVER)

/* Progression principale */
main_timeline:record(state_after_choice_A)

/* Sauvegarder avant choix crucial */
checkpoint : main_timeline:current()

/* Créer branche alternative */
alt_timeline : main_timeline:branch("choice_B")

/* Dans timeline alternative */
alt_timeline:record(state_after_choice_B)
alt_timeline:record(state_after_choice_B_consequence)

/* Revenir à la timeline principale */
main_timeline:record(state_continuing_A)

/* Les deux branches sont indépendantes et peuvent */
/* être mergées plus tard (3-way merge) */

/* Comparer les deux timelines */
main_final : main_timeline:current()
alt_final : alt_timeline:current()

diff_between_timelines = Player::diff(main_final, alt_final)
```

### 7.5 Blockchain + Certification

```342
/* === EXEMPLE 5 : DIFF-CHAIN SIGNÉE === */

hist_doc : History::new(document_v1, RetentionPolicy::FOREVER)

hist_doc:record(document_v2)
hist_doc:record(document_v3)

/* Exporter comme chaîne certifiée */
certified_chain = hist_doc:to_certified_chain()

/* Chaque diff est signé */
pour diff dans certified_chain {
  assert(verify_signature(diff.author, diff))
  println(diff.timestamp)
  println(diff.previous_hash)  /* Chaîne vérifiable */
}

/* Envoyer à la blockchain */
pour diff dans certified_chain {
  blockchain:commit(diff)  /* Enregistrement immuable */
}

/* Plus tard, vérifier l'intégrité */
pour i en [0..certified_chain.len] {
  assert(certified_chain[i].previous_hash == hash(certified_chain[i-1]))
}
```

---

## PARTIE VIII : GRANULARITÉ ET COMPROMIS

### 8.1 Granularité : 4 Niveaux

```342
/* === GRANULARITÉ DE CAPTURE === */

enum CaptureLevel {
  /* 1. Par statement (fin) : chaque instruction = diff */
  PER_STATEMENT {
    overhead: u32    /* Beaucoup de petits diffs */
    precision: HIGH
    undo_resolution: FINE
  }

  /* 2. Par expression (moyen) : chaque expr = diff */
  PER_EXPRESSION {
    overhead: u32
    precision: MEDIUM
    undo_resolution: MEDIUM
  }

  /* 3. Par bloc (gros) : chaque { } = diff */
  PER_BLOCK {
    overhead: u32    /* Peu de gros diffs */
    precision: LOW
    undo_resolution: COARSE
  }

  /* 4. Manuel (programmeur décide) */
  MANUAL {
    overhead: PROGRAMMER_CONTROLLED
  }
}

/* Défaut : PER_STATEMENT pour petits types, PER_BLOCK pour gros */
```

### 8.2 Analyse du Trade-off

```
┌─────────────────────────────────────────┐
│  GRANULARITÉ vs MÉMOIRE vs PERFORMANCE  │
├──────────────┬──────────────┬───────────┤
│ PER_STATEMENT│ + Undo fine  │ - 50MB/s  │
│              │ + Précision  │ - 10GB    │
│              │              │ (1000diffs│
├──────────────┼──────────────┼───────────┤
│ PER_EXPRESSION│+ Bon undo   │ - 200MB/s │
│              │ + Compact    │ - 2GB     │
├──────────────┼──────────────┼───────────┤
│ PER_BLOCK   │ - Undo gros  │ + 1GB/s   │
│              │ - Moins util │ + 100MB   │
├──────────────┼──────────────┼───────────┤
│ MANUAL      │ ✓ Optimisé   │ ✓ Custom  │
└──────────────┴──────────────┴───────────┘

Recommandation : PER_EXPRESSION par défaut
  (bon compromis pour 90% des cas)
```

### 8.3 Configuration Dynamique

```342
/* === AJUSTER PENDANT L'EXÉCUTION === */

hist : History::new(val, retention)

/* En mode développement : fine granularity */
#[cfg(debug)] {
  hist.capture_level : CaptureLevel::PER_STATEMENT
}

/* En mode production : coarse granularity */
#[cfg(release)] {
  hist.capture_level : CaptureLevel::PER_BLOCK
}

/* Ou dynamiquement selon charge */
si system:memory_usage() > 80% {
  hist.capture_level : CaptureLevel::PER_BLOCK
  hist:compress()
}
```

---

## PARTIE IX : FRACTAL — HISTORY<HISTORY<T>>

### 9.1 Théorie

```342
/* === HISTORIQUE D'UN HISTORIQUE === */

/* Si T : Diffable, alors History<T> : Diffable ? */
/* OUI ! C'est fractal. */

impl { T : Diffable } Diffable for History { T } {
  type DIFF = DiffHistory { T }
  type SNAP = Snapshot { History { T } }

  /* Un snapshot = toute la branche de cet instant */
  :snapshot (self) SNAP {
    Snapshot::new(self)
  }

  /* Diff entre deux historiques = diff de diffs */
  :diff (old SNAP, new SNAP) DiffHistory {
    old_hist = old->read()
    new_hist = new->read()

    /* Comparer les snapshots au même index */
    /* Si même index, diff au niveau de valeur */
    /* Si index différent, créer points de merge */

    DiffHistory {
      snapshots_diff : compute_diff_of_snapshots(old_hist, new_hist)
      diffs_diff : compute_diff_of_diffs(old_hist, new_hist)
      current_idx_diff : new_hist.current_idx - old_hist.current_idx
    }
  }

  :patch (snap SNAP, delta DiffHistory) SNAP {
    hist = snap->read()
    /* Appliquer les changements au niveau historique */
    > Snapshot::new(hist)
  }

  :size_estimate () u32
    > self.diffs.len * 32  /* Heuristique */
}

/* Cas d'usage : version control pour le control de version */
```

### 9.2 Cas d'Usage : Git-à-la-3.42

```342
/* === REPOSITORY = HISTORY<HISTORY<CODE>> === */

type CodeModule {
  name String
  content String
  last_modified u64
}

/* Historique d'un fichier */
file_history : History::new(
  CodeModule {
    name : "main.342"
    content : "fn main() { }"
    last_modified : clock()
  }
  RetentionPolicy::FOREVER
)

/* Commits : enregistrer des états */
file_history:record(CodeModule {
  name : "main.342"
  content : "fn main() { println(\"hello\") }"
  last_modified : clock()
})

/* Repository : historique des fichiers */
repo : History::new(
  Repository {
    files : { "main.342" : file_history }
    timestamp : clock()
    message : "Initial commit"
  }
  RetentionPolicy::FOREVER
)

/* Plus tard : nouveau commit du repo */
repo:record(Repository {
  files : { "main.342" : file_history_at_commit_2 }
  timestamp : clock()
  message : "Fix main function"
})

/* Meta : historique de l'historique */
/* Chaque commit du repo = snapshot entier du state à ce moment */
/* Permet déterminer quand un fichier a changé */

/* Requête : "Quand main.342 a-t-il changé ?" */
pour (idx, repo_snap) dans repo.snapshots {
  si idx > 0 {
    prev = repo.snapshots[idx-1]
    file_diff = CodeModule::diff(
      prev.files["main.342"]
      repo_snap.files["main.342"]
    )
    si file_diff != {} {
      println("Changed at commit {}".format(idx))
    }
  }
}
```

---

## PARTIE X : ÉMERGENCE DES BOSONS

### 10.1 Lien avec les Bosons

```342
/* === LES DIFFS ÉMERGENT DES BOSONS === */

/*
  Boson < (STOCKER)     → Diff::SET { old, new }
  Boson > (CHARGER)     → Snapshot::read()
  Boson . (ACCÉDER)     → Snapshot[index]
  Boson : (DÉFINIR)     → Diff::REPLACE { new_val }
  Boson = (LIER)        → Diff::BIND { name, val }
  Boson ^ (SUPERPOSER)  → Diff::INSERT { pos, val }
  Boson | (CONNECTER)   → Diff::MOVE { from, to }
  Boson ~ (DIFFUSER)    → Diff::BROADCAST { to_all_branches }
  Boson * (TRANSFORMER) → Diff::GENERALIZE { pattern }
  Boson # (MARQUER)     → Diff::MARK { overflow_flag }
  Boson _ (IGNORER)     → Diff::COMMENT { no_op }
*/

/* Chaque Diff est une application d'un boson */
/* à des fermions (valeurs) */

type Diff {
  boson Symbol       /* <, >, ., ?, !, etc. */
  operands [Value]
}

/* Reconstruction minimale */
impl Diffable for i32 {
  :diff (old, new) Diff {
    si old == new {
      Diff { boson : '_' operands : [] }   /* _ = ignorer */
    } sinon {
      Diff { boson : ':' operands : [old, new] }  /* : = redéfinir */
    }
  }
}
```

### 10.2 Composition de Diffs

```342
/* === COMPOSITION : APPLYING BOSONS === */

/* Boson de composition appliqué aux diffs */

delta1 = Diff { boson : '<', operands : [0, 42] }
delta2 = Diff { boson : ':', operands : [42, 100] }

/* Composée : delta1 puis delta2 */
composed = delta1 >> delta2

/* Simplification : < 0,42 suivi de : 42,100 */
/*                 = < 0, 100 directement */
/*                 Fusion des deux diffs */

simplified = simplify_composed_diffs(delta1, delta2)
assert(simplified == Diff { boson : '<', operands : [0, 100] })
```

---

## PARTIE XI : PERFORMANCE ET OPTIMISATIONS

### 11.1 Benchmark Théorique

```
Type          | Snapshot | Diff    | Patch O(?)
──────────────┼──────────┼─────────┼─────────────
i32           | 4 B      | 8 B     | O(1)
String 1KB    | 1 KB     | 50-200B | O(len)
Array 1K elt  | 4 KB     | 32 B    | O(1) CoW
Struct        | var      | per-fld | O(fields)
```

### 11.2 Copy-on-Write (CoW) pour Arrays

```342
/* === COW : RÉPARTIR LE COÛT === */

arr1 : [1, 2, 3, 4, 5]
snap1 = History::snapshot(arr1)  /* Ref, pas copy */

arr2 = arr1 with [2] = 99
snap2 = History::snapshot(arr2)

/* snap1 et snap2 partagent les éléments 0,1,3,4 */
/* Seul l'élément 2 est copié */

/*
  ┌─────────────────┐
  │  snap1: [1,2,3] │
  └─────────────────┘
         ↓ (partagé)
    ┌─────────────────┐
    │  Data arena     │
    │  [1, , 3, 4, 5] │
    │      ↓ (copie)  │
    │  [1, 99, 3, 4,5]│
    └─────────────────┘
         ↑ (partagé)
  ┌─────────────────┐
  │  snap2: [1,99,3]│
  └─────────────────┘
*/
```

### 11.3 Indexed Access pour Récupération Rapide

```342
/* === INDEX : O(1) LOOKUP === */

hist : History::new(...)

/* Sans index : O(n) pour trouver le snapshot à T=1000 */
snap = find_snapshot_at_timestamp(1000)

/* Avec index : O(log n) ou O(1) */
snap_idx : hist.timestamp_index[1000]  /* HashMap */
snap = hist.snapshots[snap_idx]

/* Index utiles */
hist.timestamp_index : Map { u64 → u64 }
hist.author_index : Map { PublicKey → Vec { u64 } }
hist.hash_index : Map { Hash256 → u64 }
```

---

## PARTIE XII : SÉCURITÉ ET CRYPTOGRAPHIE

### 12.1 Signing et Verification

```342
/* === DIFFS SIGNÉS === */

impl Diffable for SignedDiff {
  type DIFF = SignedDiffDelta
  type SNAP = Snapshot { SignedDiff }

  :snapshot (self) SNAP {
    Snapshot::new(self)
  }

  :diff (old SNAP, new SNAP) SignedDiffDelta {
    /* Les signatures changent → diff structurel */
    SignedDiffDelta {
      content_changed : old.content != new.content
      author_changed : old.author != new.author
      signature_changed : old.signature != new.signature
    }
  }

  :patch (snap SNAP, delta SignedDiffDelta) SNAP {
    /* Pour patch sécurisé, vérifier la signature AVANT patch */
    si delta.signature_changed {
      assert(verify_signature(new.author, new.signature))
    }
    > Snapshot::new(new)
  }
}
```

### 12.2 Blockchain Integration

```342
/* === CERTIFIEDIFF : BLOCKCHAIN-READY === */

type CertifiedDiff { T : Diffable }
  = content Diff { T }
    author PublicKey
    timestamp u64
    previous_hash Hash256
    signature Signature
    merkle_path Vec { Hash256 }  /* Pour inclusion proof *)

impl { T : Diffable } Diffable for CertifiedDiff { T } {
  type DIFF = CertDiffChange
  type SNAP = Snapshot { CertifiedDiff { T } }

  :snapshot (self) SNAP {
    Snapshot::new(self)
  }

  :diff (old SNAP, new SNAP) CertDiffChange {
    CertDiffChange {
      content_diff : T::diff(old.content, new.content)
      author_diff : old.author != new.author
      timestamp_diff : new.timestamp - old.timestamp
      hash_link_valid : verify_hash_chain(old, new)
    }
  }

  :patch (snap SNAP, delta CertDiffChange) SNAP {
    /* Vérifier l'intégrité avant d'appliquer */
    assert(verify_previous_hash(snap.previous_hash, delta))
    > Snapshot::new(new)
  }
}
```

---

## PARTIE XIII : RÉSUMÉ IMPLÉMENTATION

### 13.1 Checklist

```
[ ] Trait Diffable avec types associés Diff, Snap
[ ] Implémentation pour i32, String, Vec, struct
[ ] History<T> conteneur avec undo/redo/branching
[ ] Snapshot<T> avec CoW
[ ] Diff<T> avec delta encoding
[ ] RetentionPolicy (4 stratégies)
[ ] LazyDiff et lazy_evaluation
[ ] Arènes dédiées pour diffs
[ ] Indexing (timestamp, author, hash)
[ ] CertifiedDiff pour blockchain
[ ] Tests unitaires (50+ cas)
[ ] Benchmarks (latency, throughput, memory)
[ ] Documentation (ce fichier)
```

### 13.2 Dépendances

```
- Arena allocators (déjà dans D4)
- Types linéaires (déjà dans D5)
- Borrow checker cross-arena safety
- Cryptography: SHA256, Ed25519, signature
- Hash tables (pour indexing)
```

### 13.3 Émergence Résumée

```
Diffable émerge de 3 primitives :

  Arènes ────┐
             ├──→ History<T>
  Types lin. ┤    (undo/redo universel)
             │
  Gluons ────┘

Le trait Diffable est le pont algébrique
reliant ces trois concepts.
```

---

## PARTIE XIV : RÉFÉRENCES ET RECHERCHE

### 14.1 Concepts Inspirants

| Concept | Référence | Applicabilité |
|---------|-----------|----------------|
| **Delta encoding** | VCDIFF (RFC 3284) | ✅ Diff compactes |
| **Copy-on-Write** | Linux kernel COW | ✅ Snapshots O(1) |
| **3-way merge** | Git merge algorithm | ✅ Branching |
| **LLVM SSA** | LLVM IR docs | ✅ ParticleIR structure |
| **Merkle trees** | Bitcoin, Ethereum | ✅ Blockchain integration |
| **Differential equations** | Mathematics | 💡 Métaphore temps |
| **Git internals** | Pro Git book | ✅ Full inspiration |

### 14.2 Propriétés Mathématiques

```
History<T> forme un monoïde pour composition :
  (T1 → T2) ∘ (T2 → T3) = T1 → T3
  Associativité : ((a ∘ b) ∘ c) = (a ∘ (b ∘ c))
  Identité : T → T (empty diff)

Diff<T> forme un groupe si diffable est bijective :
  ∀ diff ∃ inverse: patch(snap, diff) = snap'
                     patch(snap', inverse) = snap

Snapshots forment un poset (ordered set) :
  snap1 ≤ snap2 si existe diff d tel que patch(snap1, d) = snap2
```

---

## PARTIE XV : FUTURS DÉVELOPPEMENTS

### 15.1 Optimisations Avancées

- **Incremental GC** : Nettoyer les diffs dès qu'ils sortent du TTL
- **Delta compression** : Appliquer DEFLATE sur les diffs
- **Parallel patching** : Appliquer plusieurs diffs en parallèle
- **Distributed History** : Synchroniser sur réseau (Git protocol)

### 15.2 Intégration Écosystème

- **Kernel avec History<KernelState>** : Undo universel
- **Compiler avec History<AST>** : Replay compilation
- **Filesystem avec History<Directory>** : Snapshot FS entier
- **Network avec History<Connection>** : Replay connexion
- **Rendering avec History<Frame>** : Replay vidéo

---

## EPILOGUE

Le trait **Diffable** n'est pas une addition arbitraire à 3.42.

C'est l'émergence naturelle d'un principe simple appliqué à un système complexe :

**"Toute valeur peut être snapshottée, comparée, et patched."**

Associé à des arènes et types linéaires, cela donne :

✨ **Git intégré au langage**
✨ **Undo/Redo universel**
✨ **Blockchain-ready par design**
✨ **Zéro frais supplémentaires si non utilisé**

---

**Claude & Alexis Mounib — Février 2026**
**"Tout émerge. Même l'histoire."**
