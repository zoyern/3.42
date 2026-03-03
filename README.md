# 3.42

> *Un écosystème OS/IA/Internet unifié inspiré par la physique des particules.*

**18 symboles. Zéro mot-clé. Tout émerge.**

---

## C'est quoi ?

**3.42** (prononcé "trois quarante-deux") est un projet d'écosystème numérique complet : un **langage de programmation**, un **système d'exploitation**, et un **Internet décentralisé** — conçus comme un tout cohérent.

Le langage repose sur **18 symboles** (appelés bosons, comme en physique des particules) qui, composés entre eux, génèrent **tout** — types, fonctions, concurrence, mémoire, I/O — sans aucun mot-clé.

```
// Pas de if, while, class, return...
// Tout émerge de la composition de symboles :

age : 0..150 = 25;                         // type = contrainte (D76)
result = data |- {x |> x > 0} |* {x |> x * x} |+ {0; (a,b) |> a + b};
t1 = |{ heavy_task(data) };  t1 |;         // thread + wait
```

---

## Par où commencer

| Tu veux... | Lis ça |
|------------|--------|
| Comprendre la **vision globale** (même sans coder) | [`3.42-BLUEPRINT-v6.0.md`](3.42-BLUEPRINT-v6.0.md) |
| Voir la **référence technique du langage** | [`checkpoint/CP-4-UNIFIED.md`](checkpoint/CP-4-UNIFIED.md) |
| Explorer **interactivement** (ouvrir dans un navigateur) | [`342-presentation.html`](342-presentation.html) |

---

## En un coup d'œil

| | |
|---|---|
| **18 bosons** | `< > . ? ! \| ~ ^ % / : = + - * # _ @` |
| **5 gluons** | `{} () [] "" ''` |
| **3 séparateurs** | `; , espace` |
| **1 règle** | `A B = A(B)` — composition, profondeur infinie |
| **0 keywords** | `:` suffit. `if/while/class` = sucre optionnel |
| **83 décisions** | Architecture complète, toutes documentées |
| **28 questions** | Toutes résolues |

### Décimal exact
```
0.1 + 0.2 = 0.3;    // garanti, pas 0.30000000000000004 (D75)
```

### Mémoire sans douleur
```
a = [1, 2, 3];
b = a;              // MOVE : a n'existe plus (Pauli, D5)
c = **a;            // copie explicite si besoin
```

### Concurrence en 1 ligne
```
t = |{ calcul(data) };    // thread CPU (D37)
result = t |;              // wait/join
~{ gpu_compute(matrix) }; // GPU/SIMD (D65)
^{ quantum_search(db) };  // QPU (D14)
```

### Sugar : même code, toutes les syntaxes
```
// 342 natif :       x > 0 ?{ + : print("oui") };
// Sugar C :         if (x > 0) { printf("oui"); }
// Sugar Python :    if x > 0: print("oui")
```

---

## Structure du repo

```
342/
├── README.md                      ← Ce fichier
├── 3.42-BLUEPRINT-v6.0.md        ← Vision complète (20 parties, pour tout le monde)
├── checkpoint/
│   ├── CP-4-UNIFIED.md           ← Référence technique (16 sections)
│   └── CP-2-DECISIONS.md         ← 83 décisions détaillées (D1-D83)
├── grammar/
│   └── 342.peg                   ← Grammaire PEG formelle (18 bosons)
├── 342-presentation.html         ← Présentation interactive (navigateur)
├── 3.42/src/                     ← Code source prototype (trit, arena, sphere — C11)
├── idea.md                       ← Source originale de la vision
└── archive/                      ← Anciennes versions (CP-1 à CP-3.9, HTML, specs)
```

---

## Compteurs

| Métrique | Valeur |
|----------|--------|
| Bosons (spin 1) | **18** |
| Gluons (spin 0) | **5** |
| Séparateurs | **3** |
| Gravitons (boson + {}) | **18** (3 tiers : 8+6+4) |
| Décisions | **83** (D1-D83) |
| Questions | **28/28 résolues** |
| Tiers orbitaux | **4** (s, p, d, f) |

---

## Auteur

**Alexis Mounib** (almounib) — [42 Paris](https://42.fr)
GitHub : [github.com/zoyern](https://github.com/zoyern)

Assisté par Claude (Anthropic)
