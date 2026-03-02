# 3.42

> *Un écosystème OS/IA/Internet unifié inspiré par la physique des particules.*

**18 symboles. Zéro mot-clé. Tout émerge.**

---

## C'est quoi ?

**3.42** (prononcé "trois quarante-deux") est un langage de programmation + système d'exploitation + Internet décentralisé, conçu comme un tout cohérent. Il repose sur un principe unique : **18 symboles** (appelés bosons, comme en physique des particules) composés entre eux génèrent **tout** — types, fonctions, concurrence, mémoire, I/O — sans aucun mot-clé.

```
// Pas de if, while, class, return...
// Tout émerge de la composition de symboles :

age : 0..150 = 25;                         // type = contrainte
result = data |- {x |> x > 0} |* {x |> x * x} |+ {0; (a,b) |> a + b};
t1 = |{ heavy_task(data) };  t1 |;         // thread + wait
```

---

## Par où commencer

| Tu veux... | Lis ça |
|------------|--------|
| Comprendre la **vision globale** (même sans coder) | [`3.42-BLUEPRINT-v6.0.md`](3.42-BLUEPRINT-v6.0.md) |
| Voir la **référence technique du langage** | [`checkpoint/CP-4-UNIFIED.md`](checkpoint/CP-4-UNIFIED.md) |
| Naviguer dans les fichiers | [`checkpoint/CP-4-INDEX.md`](checkpoint/CP-4-INDEX.md) |
| Visualiser (ouvrir dans un navigateur) | [`342-emergence.html`](342-emergence.html) · [`342-language.html`](342-language.html) |

---

## En un coup d'œil

| | |
|---|---|
| **18 bosons** | `< > . ? ! \| ~ ^ % / : = + - * # _ @` |
| **5 gluons** | `{} () [] "" ''` |
| **1 règle** | `A B = A(B)` — composition, profondeur infinie |
| **0 keywords** | `:` suffit. `if/while/class` = sucre optionnel |
| **83 décisions** | Architecture complète, toutes documentées |
| **28 questions** | Toutes résolues |

### Décimal exact
```
0.1 + 0.2 = 0.3    // garanti, pas 0.30000000000000004
```

### Mémoire sans douleur
```
a = [1, 2, 3];
b = a;              // MOVE : a n'existe plus (Pauli)
c = **a;            // copie explicite si besoin
```

### Concurrence en 1 ligne
```
t = |{ calcul(data) };    // thread CPU
result = t |;              // wait/join
~{ gpu_compute(matrix) }; // GPU/SIMD
^{ quantum_search(db) };  // QPU
```

### Sugar : même code, toutes les syntaxes
```
// 342 natif :       x > 0 ? { + : print("oui") };
// Sugar C :         if (x > 0) { printf("oui"); }
// Sugar Python :    if x > 0: print("oui")
```

---

## Structure du repo

```
342/
├── 3.42-BLUEPRINT-v6.0.md      ← Vision complète (pour tout le monde)
├── checkpoint/
│   ├── CP-4-INDEX.md            ← Navigation
│   ├── CP-4-UNIFIED.md          ← Référence technique (16 sections)
│   ├── CP-2-DECISIONS.md        ← 83 décisions détaillées
│   └── CP-3.9-CONVERGENCE.md    ← Dernière convergence
├── designs/                     ← Spécifications (ParticleIR, Diffable, Gravitons)
├── 342-emergence.html           ← Visualisation orbitale (navigateur)
├── 342-language.html            ← Référence langage interactive (navigateur)
├── 342-particles.html           ← Visualisation particules
├── 342-sphere.html              ← Sphère de Bloch 3D
├── grammar/342.peg              ← Grammaire PEG formelle
├── archive/                     ← Anciennes versions
└── src/                         ← Code source (trit C11)
```

---

## Auteur

**Alexis Mounib** (almounib) — [42 Paris](https://42.fr)
GitHub : [github.com/zoyern](https://github.com/zoyern)

Assisté par Claude (Anthropic)
