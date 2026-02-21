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

## QUESTIONS RESTANTES (nouvelles issues des blueprints)

### PRIORITÉ HAUTE

#### Q19 : 342-IR — prototype concret
**Contexte** : Q13 résolu (SSA 2 niveaux). Reste à prototyper.
**Format proposé** :
```
ParticleIR = List<Node>
Node = Boson { verb: Symbol, operands: [Node], spin: Spin, energy: u32 }
     | Fermion { value: Value, type: Type, lifetime: Arena }
     | Gluon { kind: GluonKind, contents: [Node] }
```
**Action** : Écrire 5 exemples (hello world, fibonacci, pipe, quantum, measure) en ParticleIR

#### Q20 : Diff-chain (des blueprints, oublié)
**Contexte** : Les blueprints originaux spécifient History<T: Diffable> pour TOUT (kernel, compilateur, filesystem, réseau, rendu).
**Questions** :
- Snapshots + diffs = quelle granularité ? (par statement ? par commit ?)
- Rétention policies ? (garder combien d'historique ?)
- Lazy evaluation pour ne pas stocker ce qui n'est pas demandé ?
**Action** : Concevoir le trait Diffable et ses implémentations minimales

#### Q21 : Sécurité mémoire — failles connues arènes
**Contexte** : Recherche révèle 3 vulnérabilités des arena allocators :
1. **Cross-arena references** : arène A → objet dans arène B, B droppée → UAF
2. **Dangling pointers post-dealloc** : arène libérée mais pointer gardé
3. **Aliasing violations** : overlapping arenas
**Mitigations** :
- **Interdiction cross-arena refs** : le borrow checker 342 INTERDIT les refs cross-arène
- **Index-based access** : utiliser indices (pas pointeurs) + bounds check
- **Guard pages** en debug mode (SIGSEGV sur UAF)
- **Types linéaires** : MOVE par défaut = pas de double-free, pas de dangling
**Statut** : Les types linéaires (D5) + arènes (D4) + borrow checker suffisent SI cross-arena refs sont interdites.
**Action** : Formaliser la règle "pas de ref cross-arène" dans la spec

### PRIORITÉ MOYENNE

#### Q22 : Blockchain en toile (des blueprints, CRITIQUE mais long terme)
**Contexte** : Les blueprints décrivent une blockchain vectorielle (pas linéaire) avec cercles de confidentialité.
- Architecture en toile = plus résiliente que linéaire
- PI automatique via hash de code
- Identité décentralisée (3 niveaux de clés)
**Statut** : Fondamental pour l'écosystème mais pas bloquant pour le langage.
**Action** : Créer un document CP-3-ECOSYSTEM séparé

#### Q23 : Sphère de Bloch comme conteneur universel (des blueprints)
**Contexte** : Les blueprints SZN spécifient que TOUS les types (int, float, string) sont encodés sur la sphère (θ, φ, depth, spin).
**Idée** : Chaque valeur = point sur une sphère de Bloch généralisée. Le type = la "couche orbitale". La valeur = la position angulaire.
**Recherche** : Bloch généralisé à N dimensions = SU(N), théorique mais PROUVÉ pour qubits et qutrits.
**Questions** :
- Est-ce que ça apporte un avantage computationnel réel ou c'est juste une métaphore ?
- Comment encoder un int32 sur une sphère de manière efficace ?
**Action** : Évaluer si c'est un modèle de données ou juste une visualisation

#### Q24 : Harmoniques sphériques et compression (des blueprints v4.1)
**Contexte** : Les blueprints mentionnent harmoniques sphériques pour compression de données et rendu SDF.
**Recherche** : Les harmoniques sphériques sont PROUVÉES pour compression (utilisées en graphics, audio).
**Questions** :
- Applicable au code ? (signaux = oui, AST = spéculatif)
- Utile pour le rendu SDF mentionné dans les blueprints ?
**Action** : Basse priorité, revenir quand le runtime graphique existe

#### Q9 : Trit coprocesseur (mis à jour)
- Brevet Huawei 2025 : ternaire viable, -40% transistors
- Timeline : 3-5 ans
- Mapping spins → trits défini (D28)
- **Nouveau** : ISA agnostique possible (recherche Trillium confirme)
- FPGA prototype = première étape concrète

#### Q10 : QPU backend réel
- IBM Qiskit / Google Cirq comme cibles
- ^{} → QASM circuit
- Set universel vérifié : {^!, ^~, ^|, ^(θ)} = complet

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
| Cross-arena refs = UAF potentiel | ⚠️ | Q21 : mitigé par interdiction + types linéaires |
| Portes Y/Z non dans PEG | ℹ️ | Q8 : émergent via ^+/^-, pas besoin de PEG |
| Blueprints écosystème orphelins | ⚠️ | 8 idées majeures à intégrer (backlog) |

---

## PROCHAINES ÉTAPES CONCRÈTES

1. **Parser Pest** : Implémenter 342.peg en .pest, 50+ tests (Q15)
2. **ParticleIR** : Prototyper format SSA avec 5 exemples (Q19)
3. **Sécurité mémoire** : Formaliser interdiction cross-arena refs (Q21)
4. **Exemples** : 10 programmes complets en 342 natif
5. **Sugar C** : Sugar C minimal (if/while/return → ?/??/<<)
6. **Diff-chain** : Designer trait Diffable (Q20)
7. **Visualisation** : 342-sphere.html v2 (amélioré, 3 vues, conteneurs, liaisons)
