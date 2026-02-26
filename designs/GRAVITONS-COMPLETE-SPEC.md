# 19 GRAVITONS — Spécification Complète
## Graviton = Boson + Gluon {} = Scope Typé Émergent

---

## PRINCIPE

Un **graviton** = un boson appliqué à un gluon `{}`. Suivant la règle `A B = A(B)` :
- Le boson définit la **force** (ce que fait le scope)
- Le gluon `{}` définit le **corps** (le contenu)
- Le graviton résultant = un **scope typé** avec des invariants spécifiques

Pas une nouvelle catégorie de particule. ÉMERGE de la composition fractale.

---

## CONFLIT RÉSOLU : |! async vs |! NOT gate

### Problème
- D41 : `|! = async (CONNECTER+INVERSER = fire-and-forget)`
- D54 : `|! = NOT, |+ = AND, |- = NAND, |* = XOR`

### Analyse
Les portes quantiques (`^!` = X gate, `^~` = H, `^|` = CNOT) fonctionnent car `^` = SUPERPOSER.
Chaque composition `^+boson` = "superposer + transformation" = opération quantique. C'est **sémantiquement cohérent**.

Mais `|` = CONNECTER = pipe/flux de données. `|!` = "connecter + inverser" ≠ NOT logique.
"Connecter inversement" = **envoyer sans attendre la réponse** = fire-and-forget = async. ✓

Les portes logiques classiques sont des **instructions CPU** (AND, OR, XOR, NOT).
Elles n'émergent pas naturellement du pipe. Elles vivent dans `:.bits` (D48).

### Résolution
- **`|!` = async** (D41 conservé) ✓
- **D54 CORRIGÉ** : les compositions `|+`, `|-`, `|*` deviennent des **transducteurs pipe** (voir ci-dessous)
- **Portes logiques classiques** → `:.bits.{and, or, xor, not, nand, nor}` (D48 conservé)

---

## SYSTÈME SYMÉTRIQUE : TRIO COMPUTE × SUFFIXES

La découverte clé : les **mêmes suffixes** appliqués aux 3 bosons compute (`|`, `~`, `^`) donnent des opérations **sémantiquement cohérentes** dans chaque paradigme.

| Suffixe | `|` (CPU séquentiel) | `~` (GPU parallèle) | `^` (QPU quantique) |
|---------|---------------------|---------------------|---------------------|
| `!` | async (fire-forget) | scatter (diffusion inverse) | X gate (bit flip) |
| `*` | map (transformer) | parallel map (kernel) | rotation gate (Rz) |
| `+` | fold (accumuler) | parallel reduce (sum) | Y gate (spin+) |
| `-` | filter (réduire) | parallel filter | Z gate (spin-) |
| `?` | conditional pipe | predicated exec | measure (collapse) |
| `~` | fanout (1→N) | — (identité) | Hadamard (H) |
| `|` | — (identité) | sync barrier | CNOT (controlled) |

### Pipe transducteurs (CPU) — D54 corrigé
```
data |* (x) { x * 2 };         // map : transformer chaque élément
data |- (x) { x > 0 };         // filter : garder si vrai
data |+ 0 (acc, x) { acc + x }; // fold : accumuler avec valeur initiale
data |! process;                 // async : fire-and-forget
data |? (x) { x.valid };        // conditional : pipe si vrai
data |~ [sink1, sink2];          // fanout : envoyer à N destinations
```

### GPU parallèle
```
~* (x) { x * 2 };   // parallel map (CUDA-like kernel)
~+ (acc, x) { ... }; // parallel reduce (GPU reduction)
~- (x) { x > 0 };   // parallel filter (stream compaction)
```

### QPU quantique (déjà défini D22)
```
^! q;      // X gate (bit flip)
^~ q;      // Hadamard (superposition égale)
^| (a, b); // CNOT (controlled-NOT)
^(π/4) q;  // Rotation Rz(θ)
```

**C'est FRACTAL** : même structure, 3 paradigmes de calcul.

---

## LES 19 GRAVITONS

### Tier 1 — FONDAMENTAUX (déjà validés)

#### 1. `:{} ` — DÉFINIR+CORPS = Définition de type/trait
```
Point : {
    x : f64;
    y : f64;
};
```
- Émergent de D35 : le contenu décide (champs = type, signatures = trait)
- `Name : { ... }` = type. `Name : { .signatures }` = trait.

#### 2. `?{}` — MESURER+CORPS = Match/Switch (pattern matching)
```
value ? {
    + : handle_success;
    - : handle_failure;
    # : handle_error;
    _ : handle_void;
};
```
- Le cœur du contrôle de flux. D7 : `?` = if, `??` = while.

#### 3. `|{}` — CONNECTER+CORPS = Tâche CPU (concurrence structurée)
```
|{ download(url1); } | |{ download(url2); };
// Les deux tâches en parallèle via pipe
// Scope = lifetime. Auto-join à la sortie.
```
- Inspiré Kotlin `coroutineScope {}`, Swift `withTaskGroup {}`
- MOVE garantit thread-safety (pas de data race)
- La tâche ne peut pas survivre à son scope (structured concurrency)

#### 4. `~{}` — DIFFUSER+CORPS = Scope GPU (compute parallèle)
```
~{
    data ~* (x) { x * 2 };   // parallel map
    result ~+ (a, b) { a + b }; // parallel reduce
};
```
- Équivalent de `<<<blocks, threads>>>` en CUDA mais scopé
- Mémoire GPU allouée dans l'arène du scope, libérée en O(1) à la sortie

#### 5. `^{}` — SUPERPOSER+CORPS = Scope QPU (circuit quantique)
```
^{
    q = ^~(0.5, 0.5);  // Hadamard
    r = ^|(q, ancilla); // CNOT
    result = ^^r;       // Mesure
};
```
- Déjà défini D15. Simulation classique ou QPU réel.

#### 6. `!{}` — INVERSER+CORPS = Defer (exécution inversée)
```
f = open("data.txt");
!{ close(f); };  // Exécuté à la sortie du scope (LIFO)
// ... utiliser f ...
// close(f) appelé automatiquement ici
```
- Plus propre que `!?` (qui reste comme sugar/alias)
- `!?` = lazy evaluation. `!{}` = defer block. Subtilité :
  - `!? expr` = ne pas évaluer maintenant (lazy)
  - `!{ block }` = exécuter à la sortie (defer)

#### 7. `={}` — LIER+CORPS = Closure (liaison de scope)
```
counter = ={
    n = 0;
    () { n += 1; << n; };
};
```
- LIER+CORPS = capturer l'environnement dans un bloc
- Explicite sur ce qui est capturé (tout le bloc)
- Équivalent Rust `move || {}` mais plus clair

#### 8. `#{}` — MARQUER+CORPS = Métadonnées/Annotations
```
#{ version: 1; author: "zoyern"; pure: +; }
fib : (n : i64) : i64 = { ... };
```
- Compile-time uniquement, pas de code généré
- Remplace `#[derive(...)]` de Rust, `@Annotation` de Java
- Introspectable par le compilateur et les outils

---

### Tier 2 — PUISSANTS (nouvelles découvertes)

#### 9. `<{}` — SORTIR+CORPS = Bloc de sortie (générateur/writer)
```
html = <{
    "<html>";
    "<body>";
    content;
    "</body>";
    "</html>";
};
```
- Tout à l'intérieur est **streamé vers la sortie**
- Comme Kotlin `buildString {}`, Python `yield`
- Générateur : chaque expression = un élément produit
- IO optimisé : buffered write, pas de concat string

#### 10. `>{}` — ENTRER+CORPS = Bloc d'entrée (lecteur/allocateur)
```
>{
    buf = > 4096;     // alloc 4Ko dans cette arène
    header = > 64;    // alloc 64 octets
    process(@buf);
};
// Tout libéré en O(1)
```
- Scope d'allocation explicite
- Ou : scope de lecture (stdin/file input)
- L'arène interne est automatique, reset à la sortie

#### 11. `.{}` — ACCÉDER+CORPS = Destructuration (with-block)
```
point .{
    // x, y, z accessibles directement
    distance = (x * x + y * y + z * z) ^ 0.5;
};
```
- Comme Kotlin `with(obj) {}` ou JS `with`
- Ouvre les champs d'un objet dans le scope
- Immutable par défaut (@ implicite sur les champs)

#### 12. `+{}` — ACCUMULER+CORPS = Fold/Accumulate
```
total = items +{
    init: 0;
    (acc, item) { acc + item.price };
};
```
- Transducteur : fold avec valeur initiale
- Peut fusionner avec `*{}` et `-{}` (stream fusion)
- Equivalent Haskell `foldl`, Clojure `reduce`

#### 13. `-{}` — RÉDUIRE+CORPS = Filter/Diff
```
adults = people -{ (p) { p.age >= 18 } };
```
- Filtre : garde les éléments qui passent le test
- Ou : diff block : `-{ old; new }` = calcule la différence
- Cohérent avec `+{}` (si + accumule, - réduit)
- Peut aussi soustraire des éléments d'une collection

#### 14. `*{}` — TRANSFORMER+CORPS = Map/Transform
```
doubled = items *{ (x) { x * 2 } };
```
- Map : transforme chaque élément
- TRANSFORMER = opération la plus universelle
- Fusionne avec `+{}` et `-{}` en pipeline :
```
result = data *{ double } -{ positive } +{ sum };
// = data.map(double).filter(positive).fold(sum)
// Compilé en UNE SEULE passe (stream fusion)
```

---

### Tier 3 — SPÉCIALISÉS (utiles dans des contextes précis)

#### 15. `%{}` — PROPORTIONNER+CORPS = Probabiliste/Pondéré
```
action = %{
    0.7 : common_path;
    0.2 : rare_path;
    0.1 : ultra_rare;
};
```
- Branches pondérées par probabilité
- Cohérent avec `%?` (probabilité) et `%%` (normaliser)
- Utilisation : IA/ML (sampling), jeux (loot tables), tests (fuzzing)
- Le compilateur vérifie que les poids somment à 1.0 (ou normalise)

#### 16. `;{}` — TERMINER+CORPS = Séquence stricte (barrière)
```
;{
    init_hardware();
    configure_registers();
    start_device();
};
// Aucun reordering possible à l'intérieur
```
- Garantie d'ordre : chaque `;` = sequence point dur
- Le compilateur NE PEUT PAS réordonner (contrairement à `{}` normal)
- Usage : I/O hardware, registres mappés mémoire, init sequences
- Équivalent d'une barrière mémoire scopée (`memory_fence`)

#### 17. `@{}` — RÉFÉRENCER+CORPS = Scope partagé immutable
```
@{
    // Tout est @ (lecture seule) à l'intérieur
    // Aucun @@ autorisé
    shared_view(data);
    read_only_access(config);
};
```
- Garantie d'immutabilité dans le scope
- Pas de `@@` possible → pas de data race → thread-safe par construction
- Utile pour les sections critiques en lecture
- Ou : transaction scope (tout est atomique)

#### 18. `/{}` — DIVISER+CORPS = Commentaire bloc (déjà émergé)
```
/* Ceci est déjà un graviton /{}
   DIVISER + gluon = séparer du code */
```
- Déjà émergé comme `/* ... */` (D27)
- En code : pourrait être un **partition block** (diviser une collection)
- `data /{ predicate }` = `(matching, non_matching)`
- Mais usage principal = commentaire bloc

#### 19. `_{}` — IGNORER+CORPS = Sandbox/Discard
```
_{
    expensive_computation();
    // Résultat ignoré, side-effects supprimés
};
```
- Exécute mais ignore le résultat
- Ou : **sandbox** (dry-run) — exécute sans commit les side-effects
- Ou : **suppress** — supprime les warnings à l'intérieur
- Ou : **benchmark** — exécute juste pour mesurer le temps
- Cohérent avec `_` = IGNORER = void = discard

---

## CLASSIFICATION : ; ET ,

### `;` = BOSON ✓ (TERMINER)
- **CPU** : NOP / sequence point / memory barrier
- **Dans la liste des 19** : oui, boson #13
- **Graviton** : `;{}` = séquence stricte (barrière d'ordonnancement)
- **Usage courant** : terminateur de statement (comme en C)
- **Conclusion** : `;` EST un boson ET a un graviton utile

### `,` = SÉPARATEUR SYNTAXIQUE ✗ (pas un boson)
- **CPU** : aucune instruction correspondante
- **Pas dans les 19 bosons** : correct
- **Pas de graviton** : `,{}` n'a pas de sens
- **Rôle** : lie des fermions à l'intérieur d'un gluon
- **Analogue** : comme l'espace (sépare les mots, pas une force)
- **Conclusion** : `,` reste pure ponctuation (D55 confirmé)

---

## RELATION !? vs !{}

| Syntaxe | Composition | Sémantique | Usage |
|---------|-------------|------------|-------|
| `!? expr` | INVERSER+MESURER | Évaluation lazy (ne pas évaluer maintenant) | `!? expensive_calc;` |
| `!{ block }` | INVERSER+CORPS | Defer (exécuter à la sortie du scope) | `!{ close(f); };` |

Les deux coexistent. `!?` = lazy single expression. `!{}` = defer block.
`defer` (keyword) = sugar pour `!{}`.

---

## TABLEAU RÉCAPITULATIF

| # | Graviton | Composition | Sémantique | Tier | Inspiration |
|---|----------|-------------|------------|------|-------------|
| 1 | `:{}`  | DÉFINIR+CORPS | Type/Trait | 1 | Rust struct/trait |
| 2 | `?{}`  | MESURER+CORPS | Match/Switch | 1 | Rust match, Haskell case |
| 3 | `\|{}` | CONNECTER+CORPS | Tâche CPU | 1 | Kotlin coroutineScope |
| 4 | `~{}`  | DIFFUSER+CORPS | Scope GPU | 1 | CUDA kernel launch |
| 5 | `^{}`  | SUPERPOSER+CORPS | Scope QPU | 1 | Qiskit circuit |
| 6 | `!{}`  | INVERSER+CORPS | Defer | 1 | Go defer, Zig defer |
| 7 | `={}`  | LIER+CORPS | Closure | 1 | Rust move closure |
| 8 | `#{}`  | MARQUER+CORPS | Annotations | 1 | Rust #[attr], Java @Ann |
| 9 | `<{}`  | SORTIR+CORPS | Générateur | 2 | Kotlin buildString |
| 10 | `>{}` | ENTRER+CORPS | Alloc scope | 2 | C alloca + scope |
| 11 | `.{}` | ACCÉDER+CORPS | Destructure | 2 | Kotlin with(), JS with |
| 12 | `+{}` | ACCUMULER+CORPS | Fold | 2 | Haskell foldl |
| 13 | `-{}` | RÉDUIRE+CORPS | Filter/Diff | 2 | Clojure filter |
| 14 | `*{}` | TRANSFORMER+CORPS | Map | 2 | Haskell map, .map() |
| 15 | `%{}` | PROPORTIONNER+CORPS | Probabiliste | 3 | Monte Carlo, fuzzing |
| 16 | `;{}` | TERMINER+CORPS | Séquence stricte | 3 | Memory barrier |
| 17 | `@{}` | RÉFÉRENCER+CORPS | Immutable scope | 3 | Rust & lifetime |
| 18 | `/{}` | DIVISER+CORPS | Comment/Partition | 3 | /* */ déjà émergé |
| 19 | `_{}` | IGNORER+CORPS | Sandbox/Discard | 3 | /dev/null, dry-run |

---

## STREAM FUSION : LA KILLER FEATURE

Les gravitons `*{} -{} +{}` composent en **transducteurs** :

```
result = data
    *{ (x) { x * 2 } }          // map : doubler
    -{ (x) { x > 0 } }          // filter : garder positifs
    +{ 0; (acc, x) { acc + x } }; // fold : sommer

// Compilé en UNE SEULE boucle (zéro collection intermédiaire)
// Équivalent :
// acc = 0; for x in data { t = x*2; if t>0 { acc += t; } }
```

**Et c'est FRACTAL sur les 3 paradigmes :**

```
// CPU (séquentiel)
data |* double |- positive |+ sum;

// GPU (parallèle)
data ~* double ~- positive ~+ sum;

// QPU (quantique)
data ^* rotate ^- measure ^+ collapse;
```

Même syntaxe, 3 niveaux de parallélisme. Le compilateur choisit le backend.

---

## DÉCISIONS MISES À JOUR

### D54 CORRIGÉ (anciennement : portes logiques classiques)
**Nouveau** : Transducteurs pipe émergents.
`|*` = map, `|-` = filter, `|+` = fold, `|!` = async, `|?` = conditional pipe, `|~` = fanout.
Portes logiques classiques → `:.bits` module (D48 inchangé).

### D57 (NOUVEAU) : 19 gravitons complets
Chaque boson + {} = graviton avec sémantique émergente.
- Tier 1 (8) : fondamentaux, utilisés partout
- Tier 2 (6) : puissants, map/filter/fold + I/O + destructure
- Tier 3 (5) : spécialisés, contextes précis

### D58 (NOUVEAU) : Stream fusion via transducteurs graviton
`*{} -{} +{}` composent sans collections intermédiaires.
Fractal sur |/~/^ (CPU/GPU/QPU).

### D59 (NOUVEAU) : Trio compute × suffixes = matrice symétrique
Même suffixe (!*+-?~|) appliqué à |/~/^ donne des opérations cohérentes
dans chaque paradigme de calcul.
