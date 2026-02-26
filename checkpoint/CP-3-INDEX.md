# 3.42 — INDEX DE NAVIGATION — CP-3
## Audit global + État de l'art 2025-2026

---

## STRUCTURE DES FICHIERS

```
342/
├── grammar/
│   └── 342.peg              ← PEG formelle COURANTE (CP-2, à maj pour CP-3)
├── checkpoint/
│   ├── CP-3-INDEX.md         ← CE FICHIER
│   ├── CP-2-DECISIONS.md      ← 60 décisions (D1-D60, 1 obsolète, 2 corrigées)
│   ├── CP-3-OPEN.md          ← Questions ouvertes (Q1-Q24 TOUTES résolues)
│   ├── CP-3-RESEARCH-2026.md ← État de l'art 2025-2026
│   ├── CP-3.2-AUDIT-FINAL.md ← Audit complet logique/cohérence/perf
│   ├── AUDIT-GEMINI.md       ← Audit conversation Gemini
│   ├── CP-2-*                ← Archived
│   └── CP-1-*                ← Archived
├── designs/
│   ├── Q19-PARTICLEIR-SPEC.md     ← IR spécification + 5 exemples
│   ├── Q20-*.md                   ← Système Diffable (7 fichiers)
│   ├── GRAVITONS-COMPLETE-SPEC.md ← 19 gravitons (3 tiers) spécification
│   └── MEMORY_SAFETY_*.md         ← Sécurité mémoire formelle (Q21)
├── 342-sphere.html           ← Visualisation 3D interactive (Three.js)
├── archive/                  ← Tout le reste
└── src/                      ← Code source (trit C11, 73 tests)
```

---

## CLÉS DE LECTURE RAPIDE

**19 bosons** : `< > . ? ! | ~ ^ % / : = ; + - * # _ @`
**5 gluons** : `{} () [] "" ''`
**4 spins** : `+ - # _` (2 bits), `*` = flag wildcard
**19 gravitons** : 3 tiers (8 fondamentaux + 6 puissants + 5 spécialisés)
**1 règle** : `A B = A(B)` (composition, profondeur N)
**0 keywords** : `:` suffit. `type`/`trait` = sugar optionnel (D35)
**Trio compute** : `|` CPU, `~` GPU, `^` QPU
**Pipeline** : Source → AST → ParticleIR → MachineIR → QBE|LLVM → Binaire
**Mémoire** : MOVE default + arènes O(1) + @ lecture + @@ écriture séquentielle

---

## NOUVEAUTÉS CP-3 (depuis CP-2)

### Décisions corrigées
1. D36 corrigé : Copy = taille ≤64 bits ET pas de @ (évite aliasing pointeurs)
2. D41 corrigé : `||` reste OR logique. Parallélisme = gravitons `|{} | |{}`
3. D42 clarifié : Arena-scoped access control, PAS borrow checker Rust
4. D35 clarifié : `:` = primaire, `type`/`trait` = sugar valide

### Nouvelles décisions
5. D40 : Gravitons = contextes d'exécution (`|{} ~{} ^{}`)
6. D43 : QBE prototype, LLVM production
7. D44 : Sphère = organisation, pas adressage mémoire
8. D45 : Symétrie CPU formalisée (6 paires + 7 solitaires)

### Recherche 2025-2026
9. Mojo 1.0 → H1 2026, Zig 1.0 → 2026
10. Cranelift = production-ready pour dev (20% plus rapide que LLVM codegen)
11. Google Willow 105 qubits, IBM Nighthawk 120 qubits → avantage quantique fin 2026
12. Huawei ternaire confirmé (CNT-SGT : 45% moins d'espace, 30% moins d'énergie)
13. DARPA TRACTOR : migration C → Rust avec IA
14. mimalloc 3.2.8 actif, jemalloc archivé (juin 2025)
15. Move (Aptos/Sui) : fonctions d'ordre supérieur, types signés
16. W3C DID 1.0 standard existant
17. Concurrence structurée : adoptée par Kotlin, Swift, Java

---

## QUESTIONS TOUTES RÉSOLUES

| Q | Statut | Résumé |
|---|--------|--------|
| Q1-Q4 | ✅ CP-1 | Bubble-up, %, $, classes |
| Q5-Q18 | ✅ CP-2 | `, debug, quantum gates, IR, swap, parser, etc. |
| Q19 | ✅ CP-2 | ParticleIR spécifié (5 exemples) |
| Q20 | ✅ CP-2 | Diffable trait complet (7 fichiers) |
| Q21 | ✅ CP-2 | Sécurité mémoire formelle (9 invariants) |
| Q22 | ✅ CP-3 | Blockchain → W3C DID + Move langue |
| Q23 | ✅ CP-3 | Sphère = organisation (CGA), pas mémoire |
| Q24 | ✅ CP-3 | SH = compression continue, pas pour diff |

---

## ÉTAT DE L'ART — OÙ SE SITUE 342

| Critère | Meilleur actuel | 342 objectif | Avantage 342 |
|---------|----------------|--------------|-------------|
| Sécurité mémoire | Rust (borrow checker) | Arena-scoped + MOVE + warnings | Plus simple, ~même résultat |
| Ergonomie | Go (GC simple) | Symboles 1 char + 0 keywords | Plus compact, plus rapide à parser |
| Compile speed | Zig/TCC | QBE prototype (~1s) | Compétitif |
| Runtime perf | C/Rust | Arènes O(1) + LLVM prod | Compétitif |
| Concurrence | Pony (prouvé thread-safe) | Gravitons + MOVE + |! async | Émergent des symboles |
| Quantique | Qiskit (Python) | ^{} natif dans le langage | Intégré, pas une lib |
| Ternaire | Huawei patent | Agnosticisme D28 | Ready by design |
| Undo/Redo | Aucun langage | Diffable natif (D20) | Unique |
| Debug visuel | Aucun langage | Énergie + couleurs + sphère | Unique |

---

## NOUVEAUTÉS CP-3.1

### Gravitons complets (D57-D60)
- **19 gravitons** définis en 3 tiers (8 fondamentaux + 6 puissants + 5 spécialisés)
- **D54 corrigé** : |! reste async, pas NOT gate. |* = map, |- = filter, |+ = fold (transducteurs)
- **Stream fusion** (D58) : `*{} -{} +{}` composent en 1 passe (zéro collection intermédiaire)
- **Matrice symétrique** (D59) : même suffixe × |/~/^ = CPU/GPU/QPU cohérent
- **!{} vs !?** (D60) : defer block vs lazy eval, coexistent
- **Total** : 60 décisions, 1 obsolète (D40→D46), 2 corrigées (D41, D54)

### Fichiers ajoutés
- `designs/GRAVITONS-COMPLETE-SPEC.md` — spécification complète des 19 gravitons

---

## PROCHAINES ÉTAPES

1. **Parser Pest** : grammar/342.peg → .pest + 50 tests
2. **AST → ParticleIR** : Lowering en Rust (~500 lignes)
3. **QBE backend** : ParticleIR → QBE IL → binaire (hello world)
4. **10 exemples** : Programmes complets en 342
5. **Gravitons PEG** : Ajouter les 19 gravitons dans la grammaire
6. **Sugar C** : if/while/return → ?/??/<< (premier sugar)
7. **Transducteurs** : Implémenter |* |- |+ avec stream fusion
