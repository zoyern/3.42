# CP-3.6 — ÉMERGENCE FRACTALE COMPLÈTE
## Reclassification physique, calculateurs futurs, géométrie, autograd
### Date : 28/02/2026

---

## §1 — AUTOGRAD : EXPLICATION SIMPLE

**Le problème** : Toute l'IA moderne (réseaux de neurones, deep learning) repose sur les **gradients** (dérivées). Pour entraîner un réseau, il faut calculer : "si je change ce paramètre de 0.001, de combien change l'erreur ?"

**Sans autograd** : tu écris `f(x) = x² + 3x`, puis tu dois écrire TOI-MÊME la dérivée `f'(x) = 2x + 3`. Pour un réseau avec des millions de paramètres, c'est impossible à la main.

**Avec autograd** : le langage calcule les dérivées AUTOMATIQUEMENT. Tu écris juste `f(x) = x² + 3x`, et le langage sait que `f'(x) = 2x + 3` sans que tu le dises.

**Pourquoi c'est révolutionnaire pour 342** :
- Le trait `Diffable` (D20) fait EXACTEMENT ça : chaque opération sait comment se "différencier"
- Tu chaînes `*{} +{} -{}` → le compilateur propage les dérivées automatiquement (chain rule)
- C'est la "règle de la chaîne" en maths : `(f∘g)' = f'(g) × g'`
- **Aucun langage ne l'a NATIVEMENT** :
  - JAX = bibliothèque Python (pas dans le langage)
  - PyTorch = bibliothèque Python
  - Enzyme = plugin LLVM (pas dans le langage, greffé après)
  - Swift for TensorFlow = MORT (abandonné par Google)
- **342 serait le PREMIER langage avec autograd natif** via Diffable

**Concrètement en 342** :
```
// Définir une fonction
loss : (w : f64, x : f64) : f64 = { w * x * x + 3 * w };

// Le compilateur sait automatiquement calculer :
grad = loss.diff(w);  // dérivée par rapport à w = x² + 3
// Pas besoin de la coder à la main !

// Pour l'IA :
network *{ forward } +{ loss } .diff(weights);
// = forward pass → calcul erreur → gradients automatiques
// En UNE ligne. Zéro code de dérivation manuelle.
```

**Impact domaines** :
- **IA/ML** : entraînement de réseaux = natif, pas besoin de PyTorch
- **Physique** : simulation différentiable (optimisation de trajectoires)
- **Finance** : Greeks (sensibilités des options) = autograd sur les modèles de pricing
- **Robotique** : contrôle optimal différentiable
- **Biologie** : protein folding différentiable (comme AlphaFold)

→ **D64 : Autograd natif via Diffable = killer feature confirmée**

---

## §2 — MFENCE ÉMERGENT : PAS BESOIN DE `;{}`

**Le problème** : `;{}` retiré (D63). Comment faire une barrière mémoire (memory fence) ?

**Réponse** : ça ÉMERGE déjà de la composition fractale, à N niveaux :

### Niveau 1 — Écriture séquentielle `@@`
```
@@data = 42;    // écriture séquentielle → le compilateur insère un MFENCE implicite
@@data = 43;    // garanti APRÈS le 42
```
`@@` = écriture ordonnée. Le compilateur SAIT qu'il doit insérer des barrières.

### Niveau 2 — Scope immutable `@{}`
```
@{
    // Tout est @ (lecture seule) → aucun MFENCE nécessaire
    // Pas de write → pas de race → pas de barrière
    read(data);
};
```
Pas de barrière car pas de write. Le compilateur optimise.

### Niveau 3 — Defer `!{}` + fence
```
!{ :.cpu.mfence(); };  // defer + fence explicite si vraiment nécessaire
// Exécuté à la sortie du scope, LIFO
```

### Niveau 4 — Annotation `#{}` sur un bloc
```
#{ ordered: + }
{
    init_hardware();
    configure();
    start();
};
// Le compilateur interdit le reordering dans ce bloc
```

### Niveau 5 — Composition N niveaux
```
@@ |{
    #{ ordered: + }
    {
        write_register(0x100, value1);
        write_register(0x104, value2);
    };
};
// = écriture séquentielle + scope CPU + ordonnancement strict
// MFENCE entre chaque write, garanti par le compilateur
```

**Conclusion** : la barrière mémoire N'EST PAS un boson. C'est un PATTERN qui émerge de la composition de bosons existants. C'est exactement le but de l'émergence fractale : on ne code pas `;{}`, on compose `@@` + `#{ ordered }` + `{}`.

→ **L'émergence fractale fonctionne : même les cas niche sont atteignables par composition**

---

## §3 — CALCULATEURS FUTURS : ÉMERGENCE SANS NOUVEAU BOSON

### Le problème
GPU n'existait pas il y a 30 ans. QPU n'existe quasi pas encore. Demain : photonique, neuromorphique, DNA computing, memristors... Ajouter un boson par calculateur = impossible, incohérent.

### Recherches 2025-2026
- **Photonique** : matrices optiques à la vitesse de la lumière, parallélisme massif naturel
- **Neuromorphique** : inspiré du cerveau, event-driven, co-localise mémoire et calcul
- **DNA computing** : massivement parallèle, très lent, pour combinatoire
- **Memristors** : calcul in-memory, résistance = état
- **SYCL/oneAPI** : approche industrie = code unique, compilateur choisit le backend

### La solution 342 : les bosons compute sont des MODES, pas des hardware

**Redéfinition fondamentale** (D65) :

| Boson | Ancien nom | **Rôle réel (D65 corrigé CP-3.7)** | Signification |
|-------|-----------|-----------------------------------|---------------|
| `\|` | CPU | **CONNECTER** (pipe/connexion) | Relie les étapes, le séquentiel est le DÉFAUT (pas de symbole) |
| `~` | GPU | **PARALLÈLE** | Brise la séquentialité — instructions simultanées, diffusées |
| `^` | QPU | **SUPERPOSÉ** | Brise le déterminisme — états simultanés, probabilistes |

**Mapping automatique vers le hardware** :

| Mode 342 | Hardware disponible | Choix compilateur |
|----------|-------------------|-------------------|
| `\|{}` séquentiel | CPU | → CPU pipeline |
| `~{}` parallèle | GPU | → CUDA/Vulkan compute |
| `~{}` parallèle | Photonique | → Photonic dispatch |
| `~{}` parallèle | FPGA | → Bitstream parallèle |
| `~{}` parallèle | Aucun accélérateur | → CPU multi-thread |
| `^{}` superposé | QPU | → Circuit quantique réel |
| `^{}` superposé | Pas de QPU | → Simulation classique |

**Et les combinaisons pour le futur** :
```
~^{ ... }  // parallèle + superposé → quantum parallèle (futur)
|~{ ... }  // séquentiel puis parallèle → pipeline de kernels
~|{ ... }  // parallèle puis séquentiel → reduce pattern
```

**Approche SYCL-like mais émergente** : en SYCL, tu écris `queue.submit([&](handler& h) { ... })` — verbeux, avec keywords. En 342, tu écris `~{ ... }` — le mode est dans le symbole.

**Neuromorphique** : event-driven = `|!` (async fire-and-forget) naturellement. Le compilateur peut cibler un chip neuromorphique pour les patterns async.

→ **D65 : `|` `~` `^` = modes de calcul (séquentiel/parallèle/superposé), PAS hardware spécifique. Le compilateur mappe vers le hardware disponible. Jamais besoin d'ajouter un boson.**

---

## §4 — MODÈLE STANDARD COMPLET : RECLASSIFICATION PROFONDE

### Le vrai Modèle Standard de la physique

**17 particules fondamentales** :

**12 Fermions** (matière, spin ½, obéissent au principe d'exclusion de Pauli) :
- 6 Quarks : up, down, charm, strange, top, bottom (3 générations × 2)
- 6 Leptons : électron, muon, tau, neutrino-e, neutrino-μ, neutrino-τ (3 générations × 2)

**5 Bosons** (forces, spin entier, PAS d'exclusion de Pauli) :
- Photon (γ) : force électromagnétique
- Gluon (g) : force forte (8 types de couleur)
- W± : force faible (changement de saveur)
- Z⁰ : force faible (courant neutre)
- Higgs (H) : donne la masse

**4 Forces fondamentales** :
- Forte : lie les quarks → gluons
- Électromagnétique : charge électrique → photons
- Faible : désintégration → W±, Z⁰
- Gravité : masse → graviton (HYPOTHÉTIQUE, pas encore observé)

### Mapping complet vers 342

#### FERMIONS en 342 = DONNÉES (la matière du programme)

| Physique | 342 | Pourquoi |
|----------|-----|----------|
| **Quarks** (ne peuvent pas exister seuls) | Types primitifs liés (`i32`, `f64`, `bool`) | Comme les quarks confinés dans les hadrons, les primitifs n'ont de sens que dans un contexte |
| **Leptons** (existent seuls) | Valeurs indépendantes, variables | L'électron est libre. Une variable est libre. |
| **3 générations** | 3 niveaux de complexité | Gen 1 (stable) = `i32, f64`. Gen 2 = `struct, enum`. Gen 3 = `trait object, generic` |
| **Principe de Pauli** | **MOVE semantics** | 2 fermions identiques NE PEUVENT PAS être au même endroit → MOVE : une donnée n'existe qu'à un seul endroit. **C'est EXACTEMENT Pauli.** |
| **Antiparticules** | Valeurs complémentaires | `+` a `-`. `true` a `false`. Chaque valeur a son "anti". |
| **Neutrinos** (quasi-invisibles, traversent tout) | **Annotations `@`/`#`** | Les neutrinos interagissent à peine. Les annotations traversent le code sans l'affecter au runtime. |

#### BOSONS en 342 = OPÉRATEURS (les forces du programme)

| Physique | 342 | Pourquoi |
|----------|-----|----------|
| **Photon** (électromagnétisme, transmission) | `\|` CONNECTER | Le photon transmet l'interaction EM. `\|` transmet les données dans un pipe. |
| **Gluons** (force forte, confinement) | `{} () [] "" ''` (nos gluons) | Les gluons confinent les quarks. Les gluons 342 confinent les expressions. **Même nom, même rôle.** |
| **W±** (force faible, transformation) | `:` DÉFINIR + `=` LIER | W change la "saveur" d'un quark. `:` et `=` changent le type/la liaison d'une donnée. |
| **Z⁰** (courant neutre) | `?` MESURER | Z interagit sans changer la saveur. `?` mesure/teste sans modifier. |
| **Higgs** (donne la masse) | `:` système de types | Le Higgs donne la masse aux particules. `:` donne le type (la "masse computationnelle") aux données. |

#### GRAVITÉ en 342 = GRAVITONS (scopes typés)

| Physique | 342 | Pourquoi |
|----------|-----|----------|
| **Graviton** (hypothétique, confine la masse) | `boson + {}` = scope typé | La gravité confine la matière dans des orbites. Le graviton 342 confine le code dans des scopes. **Le nom est PARFAIT.** |
| **Gravité = courbure de l'espace-temps** | Scope = courbure du flux d'exécution | Un `?{}` courbe le flux (branching). Un `\|{}` courbe vers le parallèle. |

### Principe d'exclusion de Pauli = MOVE

C'est la découverte la plus profonde :
- **Pauli** : deux fermions identiques ne peuvent PAS être dans le même état quantique
- **MOVE** : une donnée ne peut exister qu'à UN seul endroit en mémoire
- C'est la MÊME chose. Le MOVE de 342 implémente Pauli.

Les **bosons** en physique N'OBÉISSENT PAS à Pauli (condensat de Bose-Einstein = plein de bosons au même endroit). En 342, les opérateurs (`+`, `|`, `?`...) peuvent être utilisés autant de fois qu'on veut, n'importe où. Cohérent.

→ **D66 : MOVE = Pauli pour les fermions. Les opérateurs (bosons 342) n'ont pas cette restriction. Le parallèle est exact.**

---

## §5 — GARDER LE NOM "GRAVITON" ?

### Argument POUR garder
1. En physique, le graviton est la particule hypothétique qui médierait la **gravité** = la force qui **confine** la matière
2. En 342, un graviton = boson + `{}` = une force qui **confine** du code dans un scope
3. Le parallèle conceptuel est EXACT : gravité confine → scope confine
4. Le graviton en physique est HYPOTHÉTIQUE (pas encore observé) → en 342 aussi, les gravitons ÉMERGENT, ils ne sont pas prédéfinis
5. La gravité est la seule force qui affecte TOUT → les gravitons 342 s'appliquent à tout le code

### Argument CONTRE
1. Confusion possible avec les vrais gravitons (physique)
2. En physique, le graviton est un boson (spin 2), pas un "boson + gluon"

### Verdict
**GARDER.** Les arguments conceptuels sont trop forts. La gravité = confinement = scope. Le fait que le graviton physique soit hypothétique renforce même l'analogie : les gravitons 342 émergent, ils ne sont pas préprogrammés.

Alternative considérée et rejetée : "scope-on" (moche), "containon" (trop littéral), "orbitalon" (trop long).

→ **Le terme GRAVITON est confirmé.**

---

## §6 — GÉOMÉTRIE, ALGÈBRE, ROTATIONS OPTIMISÉES

### État de l'art 2026
- **Aucun langage** n'a l'algèbre géométrique nativement (que des bibliothèques)
- **Rotors** (algèbre géométrique) > quaternions pour les rotations 3D (plus intuitifs, généralisent en N dimensions)
- **Multivecteurs** : scalaires + vecteurs + bivecteurs + trivecteurs en un seul type
- **Conformal GA (CGA)** : rotations, translations, dilatations = MÊME opération (un rotor)

### Comment ça émerge en 342

**Types géométriques via le système de types** :
```
// Multivecteur = type natif possible via `:`
Vec3 : { x: f64; y: f64; z: f64 };
Bivec3 : { xy: f64; xz: f64; yz: f64 };
Rotor3 : { s: f64; xy: f64; xz: f64; yz: f64 };  // scalar + bivector

// Produit géométrique = opérateur `*` surchargé
// a * b pour deux vecteurs = produit géométrique = scalaire + bivecteur
```

**Rotations via rotors** :
```
// Rotation de 90° autour de Z
r : Rotor3 = rotor(z_axis, pi / 2);
// Appliquer : sandwich product
v_rotated = r * v * r.reverse();
// Plus propre que quaternions, généralisable en N dimensions
```

**Avantage du ternaire pour les rotations** :
- Balanced ternary : négation = triviale (flip les trits)
- Rotation 120° en ternaire = opération native (3 états = 3 positions sur un cercle)
- `+` = 0°, `-` = 120°, `_` = 240° → triangle équilatéral naturel
- Matrices de rotation 3×3 en ternaire = moins de carry, plus rapide

**Stream fusion pour l'algèbre linéaire** :
```
// Multiplication matricielle fusionnée
result = matA *{ (row, col) {
    row +{ 0; (acc, pair) { acc + pair.a * pair.b } }
} };
// = map rows × fold products → UNE passe, zéro allocation intermédiaire
```

**Tenseurs via composition fractale** :
```
// Tenseur 3D = struct de structs (fractal)
Tensor3 : { data : [[[f64]]] };
// Contraction via stream fusion sur N dimensions
result = tensorA *{ contract(tensorB, dim: 2) };
```

→ **Q28 : Algèbre géométrique native (rotors, multivecteurs, CGA) — le type `:Multivec` comme citoyen de première classe**

---

## §7 — TERNAIRE ET GÉOMÉTRIE : SYMÉTRIE NATURELLE

### Pourquoi le balanced ternary est naturel pour la géométrie

| Propriété | Binaire | Balanced Ternary |
|-----------|---------|-----------------|
| Négation | Complément + 1 (coûteux) | Flip les trits (gratuit) |
| Arrondi | ≠ troncature | = troncature (Knuth) |
| Carry en addition | Probabilité 50% | Probabilité 30% (8/27) |
| Symétrie | Non (0,1 asymétrique) | Oui (+, 0, -) parfaitement symétrique |
| Rotation 120° | Calcul trigo | Opération native (permutation circulaire) |
| Signe | Bit séparé | Intrinsèque (+ ou -) |

### Mapping ternaire → géométrie en 342

Les 4 spins `+ - # _` en mode trit :
- `+` = positif (sens horaire, accumulation)
- `-` = négatif (sens anti-horaire, réduction)
- `_` = zéro/neutre (invariant)
- `#` = out-of-band (erreur/trap, hors de l'espace géométrique)

**Triangle équilatéral** : les 3 valeurs `+ - _` forment naturellement un triangle. C'est la base de la symétrie ternaire.

**Rotations optimisées** :
```
// Rotation ternaire = permutation circulaire des trits
rotate_120 : (t : trit) : trit = t ? {
    + : -;    // + → -
    - : _;    // - → _
    _ : +;    // _ → +
};
// Gratuit en hardware ternaire. Pas besoin de trigo.
```

→ **Les rotations par 120° sont des opérations NATIVES en ternaire. Pas de sin/cos nécessaire.**

---

## §8 — HARMONIQUES L'HUILLIER + QUANTUM-INSPIRED : ÉTAT 2026

### Anne L'Huillier — Mise à jour post-Nobel (2023→2026)
- **Nobel 2023** (pas 2022) : méthodes expérimentales pour les impulsions attosecondes
- **Recherche actuelle** (Lund University, Wallenberg Scholar) : 4 systèmes de sources attosecondes
- **Nouveau** : source attoseconde basée sur un laser industriel simple (collaboration industrie semiconducteurs)
- **Applications** : imagerie d'électrons en temps réel, réactions chimiques ultra-rapides, contrôle lumière-matière

### Parallèle avec le compilateur 342 (approfondissement)

**HHG (High Harmonic Generation)** : un laser intense frappe un gaz → les électrons oscillent → émettent des harmoniques (multiples de la fréquence fondamentale) → ces harmoniques se combinent en impulsions ultra-courtes.

**Parallèle exact avec les passes d'optimisation** :

| HHG | Compilateur 342 |
|-----|-----------------|
| Laser fondamental | Code source (fréquence de base) |
| Électrons oscillant | Variables/données traversant les passes |
| Harmoniques émises | Chaque passe d'optimisation = une "harmonique" |
| Combinaison → impulsion courte | Fusion des passes → code optimisé minimal |
| Phase matching | Cohérence entre passes (pas de conflit) |

**Ce qui est nouveau et CONFIRMÉ en 2026** :
- Les harmoniques ne sont pas aléatoires : elles suivent un **spectre plateau** (toutes les harmoniques impaires ont la même amplitude jusqu'à un cutoff)
- En compilateur : chaque passe d'optimisation devrait avoir un **impact égal** jusqu'à un point de rendement décroissant
- **Structure** : les passes ne sont pas un pipeline linéaire mais un **spectre de transformations** qui se combinent
- Spectre harmonique → arbre de passes parallèles avec phase matching = **pas de conflit entre optimisations**

→ **Pas trouvé mieux que cette approche en 2026. Les travaux de L'Huillier confirment la structure harmonique. L'idée de structurer les passes compilateur comme un spectre HHG reste originale et pertinente.**

### Quantum-Inspired Algorithms (état 2026)

**Tensor networks** :
- Réseau de tenseurs = compression de données haute dimension
- Applications : ML (80× plus rapide que CPLEX pour l'optimisation), portfolio finance, simulation quantique classique
- **En 342** : stream fusion sur tenseurs + GPU gravitons = tensor network émergent

**Algorithmes quantum-inspired sur hardware classique** :
- QAOA (Quantum Approximate Optimization Algorithm) simulé classiquement → optimisation combinatoire
- VQE (Variational Quantum Eigensolver) → chimie computationnelle
- Quantum annealing classique → recherche d'optimum global

**En 342** : le mode `^{}` peut cibler un simulateur quantum-inspired même sans QPU réel. Le compilateur choisit : QPU si dispo, sinon tensor network classique.

---

## §9 — SUGAR + IA : L'IDÉE EST FORTE

L'idée : une IA peut CRÉER des sugar dictionnaires automatiquement.

**Workflow** :
```
// L'utilisateur écrit en 342 pur
result = data *{ (x) { x * 2 } } -{ (x) { x > 0 } } +{ 0; (acc, x) { acc + x } };

// Il demande à l'IA : "crée un sugar Python-like"
// L'IA génère le dictionnaire :
sugar python {
    "for X in Y:" → "Y *{"
    "if X:" → "? X {"
    "sum(X)" → "X +{ 0; (a, b) { a + b } }"
    "list comprehension [f(x) for x in Y if g(x)]" → "Y -{ g } *{ f }"
}

// Maintenant on peut écrire :
result = [x * 2 for x in data if x > 0] |> sum
// Le compilateur traduit via le sugar → même code 342 sous le capot
```

**Pourquoi c'est fort** :
1. Chaque développeur peut avoir SON sugar (Python-like, C-like, Haskell-like, COBOL-like)
2. Le code 342 sous le capot est toujours le même → même optimisation
3. L'IA peut corriger les erreurs de sugar en temps réel
4. Nouveau langage populaire ? → nouvelle couche sugar, zéro effort
5. Les sugar sont des fichiers simples (dictionnaire de regex → 342), versionnable, partageable

**Pas besoin que ce soit dans le langage** : c'est un outil (IDE/compilateur/IA). Le langage reste pur (0 keywords).

---

## §10 — ÉMERGENCE FRACTALE : TOUT PEUT SE CONSTRUIRE

### Le principe fondamental

La règle `A B = A(B)` + les 18 bosons + 5 gluons + 3 séparateurs = un ALPHABET complet.

Comme les lettres de l'alphabet permettent d'écrire TOUS les mots possibles (même ceux pas encore inventés), les bosons de 342 permettent de construire TOUTES les opérations possibles (même celles pas encore imaginées).

### Preuves par construction

| Domaine niche | Émergence en 342 | Bosons utilisés |
|--------------|-------------------|-----------------|
| Memory fence (MFENCE) | `@@ + #{ ordered: + } + {}` | `@`, `#`, `{}` |
| Photonic compute | `~{ ... }` (parallèle → dispatch photonique) | `~`, `{}` |
| DNA computing | `~{ %{ ... } }` (parallèle + probabiliste) | `~`, `%`, `{}` |
| Autograd (dérivées auto) | `Diffable + *{} +{} .diff()` | `.`, `*`, `+`, `:` |
| Blockchain | `Diffable + #{ hash } + ={}` | `#`, `=`, `{}` |
| Rotation 3D | `Rotor * v * Rotor.rev()` | `*`, `.`, `:` |
| Tensor contraction | `*{ +{ } }` (map de folds = contraction) | `*`, `+`, `{}` |
| Assembleur inline | `:.cpu { MOV eax, 1; }` | `:`, `.`, `{}` |
| Regex | `?{ pattern }` (match avec pattern) | `?`, `{}` |
| SQL-like query | `data -{ filter } *{ select } +{ group }` | `-`, `*`, `+` |
| Audio DSP | `~{ *{ sample } +{ mix } }` (GPU stream) | `~`, `*`, `+` |
| COBOL decimal | `:decimal + %` (type + proportionner) | `:`, `%` |
| Protocol buffer | `:{} + #{} + <>{}` (type + meta + I/O) | `:`, `#`, `<`, `>` |
| UI réactive | `Diffable + ={} + |!` (state + closure + async) | `=`, `!`, `|`, `:` |
| Genetic algorithm | `%{} + *{} + ^{}` (proba + transform + mutation) | `%`, `*`, `^` |

**16 domaines niche, TOUS constructibles avec les 18 bosons existants. Aucun nouveau symbole nécessaire.**

### Niveaux d'émergence

```
Niveau 0 : Bosons seuls        (18) : + - * / : = etc.
Niveau 1 : Boson + Gluon       (18) : +{} -{} *{} etc. = gravitons
Niveau 2 : Boson + Boson collé  (~50): |! |* ^~ etc. = transducteurs
Niveau 3 : Graviton + Graviton  (∞) : *{} -{} +{} etc. = stream fusion
Niveau N : Composition libre    (∞) : ~{ *{ +{} } } etc. = tout
```

L'émergence est **exponentielle** : 18 bosons → 18 gravitons → ~50 transducteurs → ∞ compositions.

---

## §11 — RECLASSIFICATION DES TIERS

Après toute cette analyse, certains bosons changent de tier :

### Avant (CP-3.5)

| Tier | Bosons | Count |
|------|--------|-------|
| 1 (Fondamental) | `: ? \| ~ ^ ! = #` | 8 |
| 2 (Puissant) | `< > . + - *` | 6 |
| 3 (Spécialisé) | `% @ / _` | 4 |

### Après (CP-3.6) — Reclassification par RÔLE PHYSIQUE

**Tier 0 — TRIO COMPUTE (les 3 modes de calcul)** :
`|` SÉQUENTIEL, `~` PARALLÈLE, `^` SUPERPOSÉ
→ Ce sont les 3 forces fondamentales du calcul. Ils méritent leur propre tier.

**Tier 1 — STRUCTURE (définir la matière)** :
`: = . ! #`
→ DÉFINIR, LIER, ACCÉDER, INVERSER, MARQUER = les opérations qui structurent le programme

**Tier 2 — FLUX (transformer la matière)** :
`? < > + - *`
→ MESURER, SORTIR, ENTRER, ACCUMULER, RÉDUIRE, TRANSFORMER = les opérations de flux de données

**Tier 3 — MÉTA (opérations secondaires)** :
`% @ / _`
→ PROPORTIONNER, RÉFÉRENCER, DIVISER, IGNORER = contextes spécifiques

### Nouvelle structure : 3 + 5 + 6 + 4 = 18

| Tier | Nom | Bosons | Rôle physique | Count |
|------|-----|--------|---------------|-------|
| 0 | Compute | `\| ~ ^` | Forces fondamentales du calcul | 3 |
| 1 | Structure | `: = . ! #` | Construire la matière (données) | 5 |
| 2 | Flux | `? < > + - *` | Transformer et déplacer | 6 |
| 3 | Méta | `% @ / _` | Contextes spécialisés | 4 |

**Pourquoi c'est mieux** :
- Le trio compute est isolé → on voit immédiatement les 3 modes
- Structure vs Flux = distinction claire (comme forte vs électromagnétique)
- Méta = les cas niche qui émergent par composition
- Cohérent avec le modèle Standard : 4 forces → 4 tiers

→ **D67 : Reclassification en 4 tiers (0: compute, 1: structure, 2: flux, 3: méta)**

---

## §12 — VISUALISATION : 2 OCTOGONES + 2 CENTRES

Fichier : `342-octogones.html` (interactif, hoverable)

```
         DÉFINIR (:)
        /          \
   MARQUER (#)    MESURER (?)           ← Octogone externe (Tier 0+1+2)
      |              |
  INVERSER (!)    CONNECTER (|)         ← Les 8 bosons les plus utilisés
      |              |
   LIER (=)      DIFFUSER (~)
        \          /
       SUPERPOSER (^)

         SORTIR (<)
        /          \
   RÉFÉRENCER (@)  ENTRER (>)           ← Octogone interne (Tier 2+3)
      |              |
  PROPORTIONNER (%)  ACCÉDER (.)        ← Les 8 restants
      |              |
   IGNORER (_)    ACCUMULER (+)
        \          /
        RÉDUIRE (-)  TRANSFORMER (*)

              / _                        ← 2 centres (DIVISER, IGNORER)
          [neutres]
```

---

## §13 — RÉCAPITULATIF DES NOUVELLES DÉCISIONS

| # | Décision | Résumé | Statut |
|---|----------|--------|--------|
| D64 | Autograd natif via Diffable | Chaque op math implémente Diffable → gradients automatiques. 342 = premier langage avec autograd natif. Killer feature pour IA/physique/finance. | ✓ CONFIRMÉ |
| D65 | `\|` `~` `^` = modes de calcul | Séquentiel/Parallèle/Superposé, PAS CPU/GPU/QPU. Le compilateur mappe vers le hardware disponible. Extensible sans nouveau boson. | ✓ SOLIDE |
| D66 | MOVE = Pauli | Principe d'exclusion : une donnée (fermion) ne peut être qu'à un endroit. Les opérateurs (bosons) peuvent être réutilisés partout. Parallèle exact avec la physique. | ✓ PROFOND |
| D67 | 4 tiers de bosons | Tier 0: Compute (3), Tier 1: Structure (5), Tier 2: Flux (6), Tier 3: Méta (4). Aligné avec les 4 forces de la physique. | ✓ SOLIDE |
| Q28 | Algèbre géométrique native | ✅ RÉSOLU CP-3.9 (D77) : émergente des bosons existants (`*^` wedge, `*.` dot, `.!` reverse). | ✅ FERMÉE |

### Compteurs mis à jour
- **83 décisions** (D1-D81, D65 corrigé, D40 obsolète)
- **18 bosons** (inchangé)
- **5 gluons** (inchangé)
- **3 séparateurs** : `;` `,` espace
- **18 gravitons** (inchangé, nom CONFIRMÉ)
- **4 tiers SOLIDES** (reclassifié : 3+5+6+4)
- **28 questions** (Q1-Q28 **TOUTES RÉSOLUES**)
