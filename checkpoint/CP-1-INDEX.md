# 3.42 — CHECKPOINT CP-1
## Index de navigation

> **Date** : 20 février 2026
> **Auteur** : Alexis Mounib (zoyern) — 42 Paris
> **Co-théorisé** : Claude (Anthropic)

---

## Structure du projet

```
342/
├── checkpoint/
│   ├── CP-1-INDEX.md          ← TU ES ICI (navigation)
│   ├── CP-1-SPEC.md           ← Spécification complète du langage
│   ├── CP-1-DECISIONS.md      ← Toutes les décisions techniques + rationale
│   └── CP-1-OPEN.md           ← Questions ouvertes + recherche à faire
│
├── grammar/
│   └── 342.peg                ← Grammaire PEG (toujours la dernière version)
│
├── examples/                  ← Exemples de code 342
├── src/                       ← Code source (trit C11, arena, etc.)
│
├── archive/                   ← TOUT l'historique, organisé
│   ├── blueprints/            ← Documents de vision (v1→v5)
│   ├── specs/                 ← Specs techniques (v1→v6.1)
│   ├── grammar/               ← Anciennes PEG (v7.0, v8.0)
│   ├── notes/                 ← Audits, recherches, tables (v7.0→v8.0)
│   └── seeds/                 ← Plans, roadmaps, verdicts
│
├── idea.md                    ← Notes brutes
└── image.png                  ← Assets
```

## Comment utiliser ce projet

**Pour comprendre le langage** → Lis `CP-1-SPEC.md` (tout est dedans)
**Pour comprendre les choix** → Lis `CP-1-DECISIONS.md`
**Pour voir ce qui reste à faire** → Lis `CP-1-OPEN.md`
**Pour la grammaire formelle** → Lis `grammar/342.peg`
**Pour l'historique d'une idée** → Cherche dans `archive/`

## Ce qui a changé depuis le début

```
ÉVOLUTION DES IDÉES (checkpoint par checkpoint)
──────────────────────────────────────────────────
v1-v4    : Syntaxe exploratoire, premiers symboles, théorisation
v5.0-5.2 : Modèle particules (bosons/fermions/gluons), 16 symboles
v6.0-6.1 : Composition fractale, règle unique, émergence CPU
v7.0     : +'' +>> +!? sugar, audit complet, memory safety
v7.1-7.2 : Sugar plugin, table périodique, orbital, QPU ^, recherche
v8.0     : ^ = 17ème boson, consolidation, PEG v8.0

CP-1     : % = 18ème boson, $ interpolation, 4 spins (2-bit),
           stack technique (Rust/LLVM), architecture fichiers propre,
           bubble-up formalisé, portes quantiques émergentes,
           debug universel via sugar, visualisation programme
```

## Idées clés (rappel ultra-condensé)

```
1 RÈGLE     : A B = A(B)
18 BOSONS   : < > . ? ! | ~ ^ % : = ; + - * # _ @
5 GLUONS    : {} () [] "" ''
4 SPINS     : + - # _ (2 bits)  |  * = flag "all"
FERMIONS    : expr, ident, lit (données)
SUGAR       : plugin interchangeable (C, Python, custom)
MÉMOIRE     : arènes + types linéaires + bubble-up
COMPILE     : 342 → AST → LLVM IR → x86/ARM/WASM/GPU
TRIO        : | CPU, ~ GPU, ^ QPU
```
