# 3.42 — INDEX DE NAVIGATION — CP-2
## Comment naviguer dans le projet

---

## STRUCTURE DES FICHIERS

```
342/
├── grammar/
│   └── 342.peg              ← PEG formelle COURANTE (CP-2)
├── checkpoint/
│   ├── CP-2-INDEX.md         ← CE FICHIER (navigation)
│   ├── CP-2-DECISIONS.md     ← 34 décisions + sources scientifiques
│   ├── CP-2-OPEN.md          ← Questions ouvertes (4 résolues, 14 actives)
│   ├── CP-1-INDEX.md         ← Ancien index (archivé)
│   ├── CP-1-SPEC.md          ← Spec complète CP-1 (2300+ lignes)
│   ├── CP-1-DECISIONS.md     ← Ancien log décisions
│   └── CP-1-OPEN.md          ← Anciennes questions
├── 342-sphere.html           ← Visualisation 3D interactive (Three.js)
├── archive/
│   ├── blueprints/           ← Visions originales (v4→v5)
│   ├── specs/                ← Spécifications anciennes (v1→v6)
│   ├── grammar/              ← PEG anciennes (v7, v8)
│   ├── notes/                ← Documents consolidés
│   └── seeds/                ← Plans, roadmaps, recherches
├── examples/                 ← Exemples de code 342
├── src/                      ← Code source (trit C11, 73 tests)
├── idea.md                   ← Notes brutes
└── image.png                 ← Asset
```

---

## CLÉS DE LECTURE RAPIDE

**19 bosons** : `< > . ? ! | ~ ^ % / : = ; + - * # _ @`
**5 gluons** : `{} () [] "" ''`
**4 spins** : `+ - # _` (2 bits), `*` = flag wildcard
**1 règle** : `A B = A(B)` (composition, profondeur N)
**Trio compute** : `|` CPU, `~` GPU, `^` QPU
**Pipeline** : Source → AST → 342-IR → LLVM IR → Assembly → Binaire

---

## NOUVEAUTÉS CP-2 (depuis CP-1)

1. `/` = 19ème boson DIVISER → commentaires `//` et `/* */` émergent
2. Agnosticisme bit/trit/qbit → backend configurable
3. Pipeline de traçabilité → debug à tous les niveaux (sugar → binaire)
4. 342-IR custom → IR intermédiaire avant LLVM, visualisable
5. Compositions à profondeur N → pas de limite arbitraire
6. Bitwise émergent via `bits::` → module standard
7. Paradigme = computation universelle (shell + langage + HDL + math + physique)
8. Classes émergentes → type + trait + impl (pas de keyword class)
9. Visualisation sphérique → 342-sphere.html (prototype Three.js)
10. Q1-Q4 résolues (bubble-up, %, $, classes)

---

## ÉVOLUTION DES IDÉES (TIMELINE)

| Version | Idée clé | Fichier référence |
|---------|----------|-------------------|
| v1-v4 | Syntaxe de base, 13 bosons | archive/blueprints/ |
| v5 | Modules (::), return (<<), break (><) | archive/specs/ |
| v6 | {} universel, range (..), pass-through (><) | archive/specs/ |
| v6.1 | Fractal rules, symbol paradigm | archive/specs/ |
| v7 | Particules (fermion/boson/gluon), '' raw, >> continue, !? defer | archive/grammar/342-v7.0.peg |
| v8 | ^ QPU (17ème boson), quantum block/measure | archive/grammar/342-v8.0.peg |
| CP-1 | % (18ème), $ interpolation, 4 spins, checkpoint system | checkpoint/CP-1-* |
| CP-2 | / (19ème), commentaires émergents, agnosticisme compute, traçabilité | checkpoint/CP-2-*, grammar/342.peg |

---

## COMMENT UTILISER CE PROJET

**Pour une nouvelle IA** : Lis CP-2-INDEX.md (ce fichier) puis grammar/342.peg.
**Pour les détails** : Lis CP-1-SPEC.md (spec complète, 2300 lignes).
**Pour les décisions** : Lis CP-2-DECISIONS.md (34 décisions + sources).
**Pour contribuer** : Lis CP-2-OPEN.md (questions ouvertes et prochaines étapes).
**Pour visualiser** : Ouvre 342-sphere.html dans un navigateur.

---

## SOURCES SCIENTIFIQUES PRINCIPALES

- Sphère de Bloch : arXiv:1111.4427 (qutrits, SU(N))
- Brevet ternaire : Huawei CN119652311A (2025)
- Tables périodiques 3D : Chancourtois 1862, Benfey 1964
- LLVM performance : IR > Assembly (2013+)
- Pauli en informatique quantique : Nature Comm. Physics 2019
