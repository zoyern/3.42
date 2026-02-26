# 3.42 — QUESTIONS OUVERTES — CP-2 (RÉVISÉ)
## Toutes les questions répondues, recherchées et vérifiées

---

## QUESTIONS RÉSOLUES

### Résolues depuis CP-1

| Q | Question | Réponse | Décision |
|---|----------|---------|----------|
| Q1 | Bubble-up explicite ou par défaut ? | **Par défaut.** Escape analysis libère tôt. Rust lifetimes = overhead similaire mais explicite. Zero-cost si non utilisé. | D21 ✓ |
| Q2 | % compositions utiles ? | **Toutes.** %% = normalize (DIV+scale). %? = proba (RDRAND+CMP). %"fmt" = compile-time format. | D17 ✓ |
| Q3 | $ interpolation conflits ? | **Résolu PEG.** $name = longest id match. ${expr} = explicite. \$ = littéral. $ nu = littéral. | D18 ✓ |
| Q4 | Classes/structs émergentes ? | **Oui.** type + trait + impl. Héritage = trait A : B + C. Virtual dispatch = ? sur trait. self = @ implicite. | D34 ✓ |

### Résolues CP-2

| Q | Question | Réponse | Source |
|---|----------|---------|--------|
| Q5 | ` comme gluon ? | **Émergent.** Suit la règle gluon (entre 2 bornes + expr). `code` = CODE gluon. Implémentation différée (pas prioritaire). Le pattern gluon suffit : entre 2 bornes identiques, contient une expression. | Règle gluon universelle |
| Q6 | Debug universel via sugar ? | **Lossy OK.** C/Rust → 342 AST perd info spécifique (templates C++, lifetimes Rust) mais gagne energy tracking. C'est un outil d'analyse statique, PAS un transpiler complet. Prototype : fichier C → 342 AST → énergie. | Recherche IR design |
| Q8 | Portes quantiques Y et Z ? | **Émergent.** ^+ = Y gate (rotation spin +), ^- = Z gate (rotation spin -). Paramétriques : ^(θ) = Rz(θ). Set universel : {^!, ^~, ^|, ^(θ)} = universel (X+H+CNOT+Rz = complet). T gate = ^(π/4), S gate = ^(π/2). Toffoli = ^||(a,b,c). | Wikipedia quantum gates, IBM Qiskit glossary |
| Q13 | 342-IR format ? | **SSA-based, 2 niveaux.** ParticleIR (haut) préserve bosons/fermions/gluons. MachineIR (bas) → LLVM IR. Inspiré de Cranelift (SSA + block params, pas φ-nodes) et Swift SIL (sémantique préservée). | Cranelift, MIR Rust, Swift SIL |
| Q14 | Swap émergent ? | **Pattern-based.** Le compilateur détecte `a,b = b,a` → émet swap optimal par arch. x86: XCHG (registres), ARM: 3×MOV, RISC-V: 3×MOV, WASM: temp variable. XOR swap = BANNI (lent, dangereux si aliasing). LLVM optimise déjà les swaps naïfs. | Wikipedia XOR swap, LLVM codegen |
| Q15 | Prototype parser ? | **Pest (Rust).** Grammar DSL claire (.pest files), packrat caching intégré, bon support erreurs. Alternative : nom (plus performant, parser combinators). Error recovery : labeled failures + farthest failure position. | pest.rs, arXiv:1806.11150 |
| Q16 | , comme particule ? | **Non.** Séparateur syntaxique, pas un conteneur (pas entre 2 bornes), pas d'instruction CPU. Reste dans la syntaxe pure. | Analyse structurelle |
| Q17 | Wavelets dans compilateur ? | **Spéculatif.** Wavelets analysent signaux, pas code. Potentiel pour analyse de patterns mémoire (fréquence d'accès). Basse priorité, revenir si profiling le justifie. | Nature Comp. Sci. 2021 |
| Q18 | Sugar terminal/POSIX ? | **Naturel.** | est déjà pipe. > et < sont déjà I/O. La syntaxe 342 est intrinsèquement shell-like. Tester en mode REPL quand le parser existe. | Analyse syntaxique |

---

## QUESTIONS RÉSOLUES CP-2 (HAUTE PRIORITÉ)

#### Q19 : 342-IR — prototype concret ✅ RÉSOLU
**Réponse** : ParticleIR spécifié et prototypé avec 5 exemples complets.
- Format SSA : `BOSON(operands) -> result : type @arena #spin`
- 6 types de gluons IR : body, function, branch, quantum, select
- 19 bosons mappés → ParticleIR → MachineIR → LLVM IR
- 5 exemples : Hello World, Fibonacci, Pipe chain, Quantum circuit, Arènes imbriquées
**Fichier** : `designs/Q19-PARTICLEIR-SPEC.md`

#### Q20 : Diff-chain — trait Diffable ✅ RÉSOLU
**Réponse** : Système complet conçu avec 7 livrables.
- Trait Diffable avec types associés (Diff, Snap)
- History<T: Diffable> avec undo/redo/branching/merge
- 5 stratégies de rétention (KeepAll, KeepLast(n), TimeBased, SizeBased, Adaptive)
- Lazy evaluation : CoW O(1) pour snapshots
- Granularité configurable (per-expression, per-statement, per-commit)
- Intégration arènes : diffs dans arena dédiée
- Fractal : History<History<T>> fonctionne (méta-versioning)
**Fichiers** : `designs/Q20-*.md` (7 fichiers, ~5000 lignes)

#### Q21 : Sécurité mémoire — formalisée ✅ RÉSOLU
**Réponse** : Spécification formelle complète avec preuves.
- **RULE 6-CA1** (règle centrale) : `∀ ref @A → obj @B : lifetime(A) ≤ lifetime(B)`
- 9 invariants formels (INV-A1 à INV-A4, INV-M1 à INV-M7)
- 7 bugs prouvés impossibles : UAF, double-free, dangling, overflow, leak, data race, stack overflow
- 60+ exemples annotés (patterns sûrs + interdits)
- Comparaison Rust vs 3.42 (arènes = O(1) free vs heap fragmenté)
- Règles de jugement de type : T-MOVE, T-COPY, T-BORROW-READ, T-BORROW-WRITE
- Edge cases formalisés : closures, async, récursion TCO
**Fichiers** : `MEMORY_SAFETY_FORMAL_Q21.md`, `MEMORY_SAFETY_EXAMPLES_Q21.md`, `Q21_SUMMARY.md`

### PRIORITÉ MOYENNE

#### Q22 : Blockchain en toile ✅ RÉSOLU (CP-3)
**Réponse** : Fondamental pour l'écosystème, pas bloquant pour le langage.
- **W3C DID 1.0** standard existe (approuvé 2022, v2.0 en cours)
- **Move** (Aptos/Sui) prouve qu'un langage peut être natif blockchain
- **Architecture** : intégrer via Diffable (Q20) + hash de code = PI automatique
- **Identité** : 3 niveaux de clés alignés avec W3C DID
- **Timeline** : après le compilateur prototype. CP-4-ECOSYSTEM dédié

#### Q23 : Sphère de Bloch ✅ RÉSOLU (CP-3)
**Réponse** : La sphère = ORGANISATION et VISUALISATION, PAS adressage mémoire.
- **Recherche CGA** : Algèbre géométrique conforme prouvée pour transformations (Clifford 1.5.1, Klein, ganja.js)
- **Avantage computationnel** : NON pour les données générales (coût trigo, perte cache)
- **Avantage organisationnel** : OUI pour les particules du langage (bosons, fermions, gluons, gravitons)
- **La sphère scindée** : utile pour séparer les contextes graviton (|{} = hémisphère CPU, ~{} = GPU, ^{} = QPU)
- **Décision D44** : Sphère = métaphore et visu, pas structure de données runtime

#### Q24 : Harmoniques sphériques ✅ RÉSOLU (CP-3)
**Réponse** : Utile pour la compression de données continues (rendu SDF, audio), PAS pour le diff/code.
- **SH compression** : 10-200× prouvé pour données sphériques continues
- **Diff de code** : INCOMPATIBLE (données discrètes, pas continues). On garde delta encoding + LZ4/zstd
- **Rendu SDF** : OUI, quand le runtime graphique existera
- **Timeline** : basse priorité, après compilateur

#### Q9 : Trit coprocesseur (mis à jour CP-3)
- Brevet Huawei CN119652311A confirmé (mars 2025) : premier chip ternaire mondial
- CNT-SGT : portes ternaires fonctionnelles, SRAM ternaire, 45% moins d'espace, 30% moins d'énergie
- Réseau neurones ternaire : 100% précision classification images
- Défi : bruit plus problématique avec 3 états
- D28 (agnosticisme) = **CONFIRMÉ VISIONNAIRE** par la recherche 2025-2026
- Timeline : 2-3 ans pour prototypes, 5+ ans pour production

#### Q10 : QPU backend réel (mis à jour CP-3)
- IBM Nighthawk 120 qubits (nov. 2025), Google Willow 105 qubits (oct. 2025)
- **Avantage quantique vérifié visé fin 2026** (IBM et Google convergent)
- Google AlphaQubit : 13000× accélération correction d'erreur
- Théorème de seuil démontré (holy grail physique quantique)
- Qiskit = dominant (37+ frameworks), Cirq = second
- ^{} → QASM circuit. Set universel vérifié : {^!, ^~, ^|, ^(θ)} = complet
- **342 arrive AU BON MOMENT** pour le quantique

#### Q11 : Blockchain pour code (fusionné avec Q22)

#### Q12 : Unification maths/physique
- Sugar math : ∫ f dx → boucle + accumulation
- Sugar Dirac : |ψ⟩ → ^{état}
- Sugar physique : F = m·a → fonction pure
- **Fractal computation** : les types récursifs (µ-types) permettent des structures auto-similaires à l'infini. Lambda calculus + fixpoints = fondation théorique solide.

---

## IDÉES DES BLUEPRINTS À INTÉGRER (BACKLOG)

| Idée | Source | Priorité | Statut |
|------|--------|----------|--------|
| Diff-chain (History<T: Diffable>) | MASTER-SEED-v2 | Haute | Q20 |
| Blockchain en toile | MASTER-SEED-v2 | Moyenne | Q22 |
| Sphère conteneur universel | SZN-SPHERE | Moyenne | Q23 |
| Rendu SDF + streaming seed-diff | SZN-SPHERE sec.6 | Basse | Backlog |
| PNJ déterministes (f(seed,t)→état) | SZN-SPHERE sec.8 | Basse | Backlog |
| Kernel chambres isolées | SZN-SPHERE sec.9 | Basse | Backlog |
| Identité décentralisée 3 niveaux | BLUEPRINT-v4.1 | Moyenne | Backlog |
| PI automation via hash | BLUEPRINT-v4.1 | Moyenne | Q22 |
| Harmoniques sphériques | BLUEPRINT-v4.1 sec.31 | Basse | Q24 |

---

## BUGS CONNUS

| Bug | Sévérité | Statut |
|-----|----------|--------|
| PEG non testé avec parser réel | ⚠️ | Q15 résolu (Pest), reste à implémenter |
| PEG manque 24 règles (gravitons, transducteurs, :.) | ⚠️ | Documenté dans CP-3.2-AUDIT §4 |
| Cross-arena refs = UAF potentiel | ⚠️ | Q21 : mitigé par interdiction + types linéaires |
| Portes Y/Z non dans PEG | ℹ️ | Q8 : émergent via ^+/^-, pas besoin de PEG |
| Blueprints écosystème orphelins | ⚠️ | 8 idées majeures à intégrer (backlog) |

---

## PROCHAINES ÉTAPES CONCRÈTES (CP-3.3)

1. **PEG CP-3** : Ajouter les 24 règles manquantes (19 gravitons + transducteurs + :.)
2. **Parser Pest** : grammar/342.peg → .pest + 50 tests
3. **AST → ParticleIR** : Lowering en Rust (~500 lignes)
4. **Cranelift POC** : ParticleIR → Cranelift → binaire (hello world)
5. **10 exemples** : Programmes complets démontrant les 19 gravitons
6. **Stream fusion** : Implémenter |* |- |+ → 1 boucle fusionnée dans l'IR
7. **Sugar C** : if/while/return → ?/??/<< (premier dictionnaire boson→keyword)
8. **LLVM backend** : Quand Cranelift fonctionne, brancher LLVM pour prod
9. **Benchmarks** : Arena bump vs malloc, fusion vs boucle manuelle
