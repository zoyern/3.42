# CP-3.9 — CONVERGENCE : TYPES, DÉCIMAUX, GÉOMÉTRIE, STACK
## Auto-optimisation numérique, range analysis, algèbre géométrique émergente, stack technique
### Date : 01/03/2026

---

## §1 — NOMBRES : UN TYPE, OPTIMISÉ AUTOMATIQUEMENT

### Le problème que tu soulèves
`byte`, `int`, `long` = **la même chose** conceptuellement (un entier). `float`, `double`, `decimal` = **la même chose** (un nombre à virgule). Pourquoi le programmeur doit choisir ?

### La vision 342 : le compilateur choisit la représentation optimale

```
// Le programmeur écrit :
x = 42;           // entier — le compilateur SAIT que 42 tient dans 1 octet
y = 3.14;          // décimal — le compilateur SAIT que c'est une constante
z = x * 1000000;   // le compilateur SAIT que le résultat peut dépasser i8

// Le compilateur décide :
// x → i8 (42 tient dans 8 bits) ou registre
// y → rational exact (3.14 = 314/100) ou f64 si non-critique
// z → i32 (42 × 1000000 = 42M, tient dans 32 bits)
```

### Deux modes de typage émergents

**Mode 1 — Implicite (défaut)** : le compilateur infère et optimise
```
a = 42;           // compilateur → i8 (ou registre)
b = a + 100;      // compilateur → i16 (peut dépasser i8)
c = b * b;        // compilateur → i32 (10000+ possibles)
ratio = a / b;    // compilateur → EXACT (rationnel) ou f64 si perf critique
```

**Mode 2 — Explicite (contrainte)** : le programmeur fixe une borne
```
a : i32 = 42;     // forcé i32 même si i8 suffirait
b : f64 = 3.14;   // forcé IEEE 754 double (perte de précision acceptée)
```

### Les types ne sont pas des catégories — ce sont des BORNES

```
// En 342, la hiérarchie est :
// nombre → le compilateur optimise la REPRÉSENTATION
//
// Entiers :    le compilateur choisit i8/i16/i32/i64/i128/bigint
// Décimaux :   le compilateur choisit rational/decimal64/decimal128/f64
// Complexes :  paire de nombres
//
// Le TYPE du programmeur = la BORNE (pas la représentation)
```

→ **D74 : Typage numérique à optimisation automatique. `byte/int/long` = même concept, le compilateur choisit la représentation minimale. Pas de perte de précision par défaut.**

---

## §2 — DÉCIMAUX : ZÉRO PERTE DE PRÉCISION (DÉFAUT)

### Le problème fondamental
```
// En C, Python, JavaScript, Rust, Go... :
0.1 + 0.2 = 0.30000000000000004    // FAUX

// Pourquoi ? IEEE 754 (float/double) encode en BINAIRE.
// 0.1 en binaire = 0.0001100110011... (infini, tronqué)
// La troncature accumule des erreurs.
```

### La solution 342 : décimal EXACT par défaut

**Principe** : si le programmeur écrit `0.1`, il veut `0.1`, pas `0.09999999...`

```
// 342 par défaut :
x = 0.1 + 0.2;     // = 0.3 EXACTEMENT
// Le compilateur utilise :
// 1. Arithmétique rationnelle : 1/10 + 2/10 = 3/10 (exact)
// 2. Ou decimal128 (IEEE 754-2008) si rationnel trop lourd
// 3. Ou bigdecimal si aucune borne ne suffit

// Si le programmeur VEUT du float rapide (GPU, graphisme) :
x : ~f64 = 0.1;    // ~ = mode parallèle/GPU → float accepté
// OU
#{ precision: fast }
x = 0.1;            // annotation = float autorisé ici
```

### Range analysis à pre-compile time (ton idée)

C'est la killer feature. Le compilateur + IDE analysent les BORNES de chaque variable :

```
// Le programmeur écrit :
f : (x : number) : number = {
    << x * x + 3 * x - 7;
};

// Le compilateur calcule automatiquement :
// Si x ∈ [-100, 100] (inféré du contexte d'appel)
// → x*x ∈ [0, 10000]
// → 3*x ∈ [-300, 300]
// → résultat ∈ [-307, 10293]
// → tient dans i16 (bornes : -32768 à 32767) ✓
// → PAS besoin de i32, PAS de risque d'overflow

// Si x est un float :
// → résultat ∈ [-307.0, 10293.0]
// → f64 suffit, PAS besoin de decimal
// → MAIS si précision exacte requise → decimal128 ou rationnel
```

### Range analysis à 3 NIVEAUX (pas juste compile-time)

**Niveau 1 — IDE realtime (LSP)** : pendant que tu tapes, l'IDE calcule et affiche les bornes en ghost text :
```
x : 0..100 = input();     // IDE: [u8, 1 byte]
y = x * 3;                // IDE: [0..300, u16, 2 bytes]
z = y - 50;               // IDE: [-50..250, i16, 2 bytes]
overflow_risk = z * z;     // IDE: [2500..62500, u16, ⚠️ overflow si z > 255]
```
Le LSP (Language Server Protocol — serveur d'analyse intégré) recalcule en continu. Comme Rust Analyzer montre les types inférés, mais en plus puissant : **bornes + taille mémoire + bugs potentiels**.

**Niveau 2 — Compile-time** : vérification formelle, choix optimal de représentation, erreur si overflow non géré.

**Niveau 3 — Runtime (debug)** : le visualiseur montre les valeurs réelles superposées aux bornes théoriques.

### Ce que l'IDE montre (en realtime) :

```
f : (x : number) : number = {
    << x * x + 3 * x - 7;
};
//  ├── x       : [-∞, +∞] → ATTENTION: bornes non connues
//  ├── x*x     : [0, +∞] → overflow possible si x > 2^31
//  ├── 3*x     : [-∞, +∞]
//  └── résultat : [-∞, +∞] → type = bigint (coûteux)
//
// SUGGESTION: ajouter une borne sur x pour optimiser
// f : (x : -1000..1000) → résultat tient dans i32
```

### Range comme type natif

```
// Les bornes SONT le type :
age : 0..150;              // entier entre 0 et 150 → compilateur → u8
temperature : -273.15..+∞; // décimal avec borne min → rational ou f64
pourcentage : 0.0..1.0;    // décimal borné → fixed-point optimal
pixel : 0..255;            // u8 garanti, overflow = erreur compile
```

### Interaction avec les erreurs (#)

```
// Si overflow détecté à compile-time → erreur
// Si overflow possible à compile-time → warning + suggestion de borne
// Si overflow à runtime (cas non-prévu) → # (out-of-band, type erreur)

result : i32 | # = risky_calc(x);
result ? {
    + : use_value;     // valeur positive ok
    - : use_negative;  // valeur négative ok
    # : handle_overflow; // overflow capturé proprement
};
```

### Comparaison : aucun langage ne fait ça

| Langage | Décimal exact | Range analysis | Auto-type | IDE bornes |
|---------|---------------|----------------|-----------|------------|
| C/C++ | ❌ (IEEE 754) | ❌ | ❌ | ❌ |
| Rust | ❌ (IEEE 754) | ❌ | ❌ | ❌ |
| Python | ❌ (lib Decimal) | ❌ | ❌ | ❌ |
| Ada | ✅ (delta types) | ✅ (partiel) | ❌ | Partiel |
| COBOL | ✅ (PICTURE) | ❌ | ❌ | ❌ |
| **342** | ✅ (défaut) | ✅ (compile+IDE) | ✅ (auto) | ✅ (natif) |

Ada est le plus proche, mais 342 va plus loin : Ada demande des annotations explicites (`type F is delta 0.005 range -50.0..50.0`), 342 INFÈRE les bornes automatiquement et montre dans l'IDE.

→ **D75 : Décimal exact par défaut. `0.1 + 0.2 = 0.3` garanti. Float = opt-in via `~f64` ou `#{ precision: fast }`. Range analysis native = 3 niveaux (IDE realtime + compile-time + debug runtime). Overflow impossible si bornes connues.**

→ **D76 : Range types natifs. `age : 0..150` = le type EST la borne. Le compilateur choisit la représentation minimale (u8, i16, etc.). Overflow vérifié statiquement. Pas de clamp implicite — le développeur gère explicitement les cas hors-borne.**

---

## §3 — CONFLIT `.diff` : VÉRIFICATION

### Ton pseudo-code analysé

```
// Ta question :
result() { diff; << diff }

func() {
    diff = result();   // "diff" = fermion local
    .diff              // ".diff" = accéder au champ
}
```

### Pourquoi il n'y a PAS de conflit

C'est la même règle qu'en C :
```c
int x = 1;        // variable locale "x"
point.x = 2;      // champ "x" de point
// Pas de conflit : "x" seul ≠ "point.x"
```

En 342 :
```
diff = 42;              // fermion "diff" (spin ½, donnée locale)
obj.diff();             // boson "." (spin 1) + fermion "diff"
// Le PARSER distingue :
// "diff" nu = fermion standalone → variable locale
// ".diff" = boson + fermion → accès champ
// La grammaire PEG résout ça sans ambiguïté :
// Expr = Fermion / (Expr "." Fermion) / ...
```

Même avec D70 (`.~` = dérivée) :
```
tilde = 42;            // fermion "tilde"
result = f .~(x);      // boson "." + boson "~" + gluon "()" → AUTOGRAD
// Pas de conflit : "tilde" ≠ ".~"
// "." est un BOSON, pas un caractère de nom
```

### Le vrai test : ton cas extrême

```
diff : () : i64 = { << 42 };     // fonction nommée "diff"
obj : { .diff : () : i64 };      // type avec champ ".diff"

x = diff();        // appel de la FONCTION "diff" → 42
y = obj.diff();    // accès au CHAMP "diff" de obj → autre chose

// Le parser voit :
// "diff()" = fermion + gluon () → appel de fonction
// "obj.diff()" = fermion + boson + fermion + gluon → accès champ + appel
// ZÉRO ambiguïté. La grammaire PEG est LL(k) : elle regarde le contexte.
```

→ **Confirmé : AUCUN conflit. Le parser distingue `diff` (fermion) de `.diff` (boson + fermion) grâce à la grammaire. C'est exactement comme `x` vs `point.x` en C.**

---

## §4 — ALGÈBRE GÉOMÉTRIQUE ÉMERGENTE

### La question : 120° ou configurable ?

**Réponse : c'est 360°/N, et N dépend du contexte.**

```
// En binaire (N=2) : 360°/2 = 180° entre les 2 états
// En ternaire (N=3) : 360°/3 = 120° entre les 3 états
// En base 5 : 360°/5 = 72°
// En base N : 360°/N

// En 342, grâce à l'agnosticisme D28 :
// Le compilateur SAIT la base cible et adapte l'angle
```

### Algèbre géométrique = ÉMERGENTE des bosons

Les multivecteurs ne sont PAS un nouveau type hardcodé. Ils ÉMERGENT :

```
// Scalaire = nombre simple
s = 42;

// Vecteur = type composé via ":"
v : { x: f64; y: f64; z: f64 };

// Bivecteur = produit extérieur de 2 vecteurs
// Le produit extérieur ÉMERGE de "*" (TRANSFORMER)
bv = v1 *^ v2;    // TRANSFORMER+SUPERPOSER = produit extérieur
// *^ = "transformer en superposant" = wedge product

// Rotor = scalaire + bivecteur
// ÉMERGE de la composition type
Rotor : { s: f64; bv: Bivec3 };

// Rotation = sandwich product : r * v * r.reverse()
// ÉMERGE de "*" et "." (.reverse = .! = ACCÉDER+INVERSER)
v_rotated = r * v * r.!();
// r.!() = snapshot inversé = reverse du rotor
```

### Les opérations géométriques en bosons purs

| Opération géométrique | 342 | Bosons utilisés |
|----------------------|-----|-----------------|
| Produit géométrique | `a * b` | `*` (TRANSFORMER) |
| Produit extérieur (wedge) | `a *^ b` | `*` + `^` |
| Produit intérieur (dot) | `a *. b` | `*` + `.` |
| Reverse | `r.!()` | `.` + `!` (ACCÉDER+INVERSER) |
| Dual | `a */()` | `*` + `/` (TRANSFORMER+DIVISER) |
| Rotation | `r * v * r.!()` | `*` × 2 + `.!` |
| Grade extraction | `mv.grade(k)` | `.` (ACCÉDER) |
| Norme | `v *. v` puis `.~(0.5)` | `*.` + `.~` |

**AUCUN nouveau boson nécessaire.** Tout émerge de `*` + `.` + `!` + `^` + `/`.

### Ton tableau de phases φ1 × φ2

Le tableau que tu as envoyé montre des **interférences de phases** — c'est exactement comment les compositions de bosons interagissent :

```
// Phase φ1=+1, φ2=+1 → Constructif (résultat +1)
// En 342 : boson + boson de même signe = renforcement
// Exemple : + + = ++ (double accumulation)

// Phase φ1=+1, φ2=-1 → Destructif (résultat -1)
// En 342 : boson + anti-boson = annulation partielle
// Exemple : + - = +- (accumulation puis réduction)

// Phase φ1=0, φ2=x → Neutre (résultat 0)
// En 342 : séparateur + quoi que ce soit = neutre
// Les séparateurs (spin 0) n'interfèrent avec rien

// Phase φ1=0.5 → Semi-constructif/destructif
// En 342 : compositions partielles (profondeur fractale)
// Exemple : un graviton (*{}) a un "poids" intermédiaire
```

**C'est profond** : les phases correspondent aux spins. Spin 0 (séparateurs) = phase neutre. Spin 1 (bosons) = phase +1/-1. Les compositions fractales = phases intermédiaires (0.5 etc.).

→ **D77 : Algèbre géométrique émergente. Rotors, multivecteurs, CGA = compositions de bosons existants (`*`, `*^`, `*.`, `.!`). 360°/N s'adapte à la base (D28). Aucun boson supplémentaire. Q28 FERMÉE.**

---

## §5 — STACK TECHNIQUE : SDL3 + wgpu + Cranelift

### Recherche 2026

**SDL3** (v3.4.2, février 2026) :
- Stable et en production
- GPU API native, support Vulkan/Metal/D3D12
- Camera, Pen, Process APIs
- Cross-platform : Windows, macOS, Linux, Android, iOS
- C natif → interfaçable directement avec 342

**wgpu** (Rust, pré-1.0 mais production-ready) :
- Backend : Vulkan, Metal, D3D12, OpenGL ES, **WebGPU**
- Écrit en Rust → même stack que 342 (Cranelift en Rust)
- Production-ready pour desktop/mobile/web
- Le **seul** qui couvre natif + web en un seul API

**Cranelift** (production) :
- Utilisé en production par Wasmtime
- x86-64, aarch64, s390x, riscv64
- ~2% plus lent que V8 TurboFan, compilation 10× plus rapide que LLVM
- WASM support natif (c'est son origin : Wasmtime)

### La stack 342 optimale

```
Couche graphique :
├── wgpu (GPU cross-platform + web via WebGPU)    ← RECOMMANDÉ
│   └── Vulkan / Metal / D3D12 / OpenGL / WebGPU
├── SDL3 (fenêtrage, input, audio, caméra)         ← COMPLÉMENTAIRE
│   └── Ne fait PAS le rendu GPU, mais tout le reste
└── Combinaison : SDL3 (fenêtre+input) + wgpu (rendu) = OPTIMAL

Couche compilation :
├── Cranelift (prototype + WASM)    ← D43 confirmé
├── LLVM (production optimisée)     ← futur
└── WASM = quasi-gratuit via Cranelift

Couche intégration web (futur) :
├── Cranelift → WASM → navigateur via WebGPU
├── OU natif : app kernel → wgpu → Vulkan
└── Le CODE est le même grâce à ~{} (modes, pas hardware)
```

### Pourquoi SDL3 + wgpu ensemble (pas l'un OU l'autre)

- **SDL3** = fenêtrage, audio, input, gamepad, caméra, filesystem. Il ne fait PAS de rendu GPU avancé.
- **wgpu** = rendu GPU. Il ne fait PAS de fenêtrage.
- **Ensemble** = couverture complète : SDL3 crée la fenêtre, wgpu dessine dedans.
- **Alternative** : winit (fenêtrage Rust pur) + wgpu. Mais SDL3 a plus de features (audio, gamepad, caméra).

→ **D78 : Stack graphique = SDL3 (fenêtrage/input/audio) + wgpu (GPU cross-platform + WebGPU). Cranelift = compilateur + WASM. LLVM = production future. Tout en Rust (même écosystème).**

→ **Q27 FERMÉE : WASM est quasi-gratuit via Cranelift. Priorité desktop/embarqué d'abord, web via WASM+WebGPU ensuite.**

---

## §6 — D67/D69 : TIERS = ORBITALES → SOLIDE

### Pourquoi passer en ✓ SOLIDE

Les tiers ne sont pas qu'une analogie. Ils ont une **conséquence technique réelle** :

1. **Ordre d'apprentissage** = ordre de remplissage orbital (s→p→d→f)
   - Tu apprends d'abord `~ ^` (compute)
   - Puis `: = . ! #` (structure)
   - Puis `? < > + - *` (flux)
   - Enfin `% @ / _` (méta)
   - C'est un CURRICULUM naturel

2. **Fréquence d'utilisation** suit les tiers
   - Tier 0 : dans presque chaque programme
   - Tier 3 : seulement dans des contextes spécifiques
   - Comme les orbitales : s = toujours rempli, f = rarement

3. **L'image de la visualisation** le confirme : les orbitales concentriques MONTRENT la structure. Si on enlève les tiers, on perd la lisibilité.

4. **Le tableau de phases** que tu as envoyé montre que les interactions entre bosons suivent des règles d'interférence → les tiers organisent QUELS bosons interagissent le plus souvent

→ **D67 : ✓ SOLIDE. D69 : ✓ SOLIDE. Les tiers ne sont pas optionnels — ils structurent l'apprentissage, l'usage, et la visualisation.**

---

## §7 — TYPES UNIFIÉS : LA GRANDE SIMPLIFICATION

### Le principe : le TYPE = la CONTRAINTE, pas la REPRÉSENTATION

```
// Aujourd'hui en C/Rust/Go :
// Le programmeur choisit la REPRÉSENTATION :
int8_t x = 42;    // "je veux 8 bits"
float y = 3.14;   // "je veux IEEE 754 32 bits"
// Pourquoi le programmeur devrait savoir combien de bits il faut ?

// En 342 :
// Le programmeur donne la CONTRAINTE (optionnelle) :
x = 42;            // "c'est un entier" → compilateur choisit
y = 3.14;          // "c'est un décimal" → compilateur choisit exact
z : 0..100 = 42;   // "c'est entre 0 et 100" → compilateur → u7 ou u8
```

### Hiérarchie unifiée

```
number (racine)
├── integer (pas de virgule)
│   ├── auto-sized : compilateur choisit i8/i16/i32/i64/i128/bigint
│   └── ranged : 0..255 → u8, -1000..1000 → i16
├── rational (fraction exacte, DÉFAUT pour décimaux)
│   ├── 1/3 = exact (pas 0.333...)
│   ├── 0.1 = 1/10 (exact)
│   └── si trop coûteux → decimal128
├── decimal (IEEE 754-2008, exact à N chiffres)
│   └── pour finance : #{ precision: 2 } → centimes
├── float (IEEE 754, PERTE acceptée, opt-in)
│   └── ~f32, ~f64 = GPU mode, graphisme
└── complex (paire de nombres)
    └── émerge de la composition : { re: number; im: number }
```

### Interaction avec les modes de calcul

```
// Mode séquentiel (défaut) → rational/decimal exact
x = 0.1 + 0.2;     // = 0.3 exact (rationnel)

// Mode parallèle (~) → float accepté
~{
    x = 0.1 + 0.2;  // = 0.30000000000000004 (f64, GPU-friendly)
    // Le compilateur PRÉVIENT dans l'IDE :
    // "⚠️ mode ~: précision float, 0.1+0.2 ≠ 0.3 exact"
};

// Annotation explicite
#{ precision: exact }
x = 0.1 + 0.2;     // = 0.3 garanti même en ~{}
```

→ **D74 confirmé + étendu : la hiérarchie number→integer/rational/decimal/float est ÉMERGENTE du contexte. Le programmeur n'a pas besoin de spécifier i32 vs i64 sauf s'il veut une borne. Le compilateur optimise.**

---

## §8 — VISION GLOBALE : ZÉRO CRASH, ZÉRO FUITE, ZÉRO PERTE

### Le contrat 342

| Garantie | Mécanisme | Décisions |
|----------|-----------|-----------|
| Zéro crash mémoire | MOVE + arènes O(1) + warnings | D5, D4, D38, D42 |
| Zéro data race | MOVE + @@ séquentiel + `|{}` structuré | D5, D37, D41 |
| Zéro perte de précision | Rational défaut + range analysis | D75, D76 |
| Zéro overflow silencieux | Bornes statiques + `#` runtime | D76 |
| Zéro fuite mémoire | Arènes auto-free + escape analysis | D4, D21 |
| Zéro keyword | Tout émerge des 18 bosons | D35, D70 |
| Zéro dépendance hardware | Modes `~ ^` = compilateur choisit | D65 corrigé |
| Debug total | Diffable + énergie + range + sphère | D62, D75 |
| Portabilité absolue | Cranelift + LLVM + wgpu + SDL3 | D43, D78 |
| Versioning natif | Diffable (D20) = time-travel | D20, D62 |

### L'idée kernel : le code se maintient lui-même

```
// Grâce à :
// 1. Range analysis → pas de surprise numérique
// 2. MOVE → pas de surprise mémoire
// 3. Diffable → undo/redo/branching/merge natif
// 4. #{ annotations } → metadata versionnée
// 5. Sugar → compatible avec tout développeur
// 6. Bornes statiques → garanti même sur "minitel"

// Le compilateur REFUSE le code dangereux :
// - Overflow non-géré → erreur compile
// - Data race possible → erreur compile
// - Mémoire non-libérée → erreur compile (arènes)
// - Float non-exact en mode non-~ → warning
```

---

## §9 — FERMETURE DES QUESTIONS

| Q | Décision | Résolution |
|---|----------|------------|
| Q25 | HKT émergent + autograd | HKT émergent via `:` + composition. Autograd = D70 (.~ .! .=). Tenseurs = stream fusion `*{} +{}`. **FERMÉE** |
| Q26 | COBOL decimal | D75 : décimal exact par défaut (rational). Float = opt-in. Range types (D76). **FERMÉE** |
| Q27 | WASM | Cranelift → WASM quasi-gratuit. Desktop/embedded prioritaire. Web via WASM+WebGPU futur. **FERMÉE** |
| Q28 | Algèbre géométrique | D77 : émergente (`*^` wedge, `*.` dot, `.!` reverse). 360°/N adaptatif. **FERMÉE** |

---

## §10 — RÉCAPITULATIF DÉCISIONS

| # | Décision | Résumé | Statut |
|---|----------|--------|--------|
| D67 | 4 tiers = orbitales atomiques | ✓ Structure l'apprentissage, l'usage, la visualisation. | ✓ SOLIDE |
| D69 | Tiers = remplissage orbital | ✓ s→p→d→f = curriculum naturel du langage. | ✓ SOLIDE |
| D74 | Typage numérique auto-optimisé | `byte/int/long` = même concept. Le compilateur choisit la représentation minimale. | ✓ ÉLÉGANT |
| D75 | Décimal exact par défaut | `0.1 + 0.2 = 0.3` garanti (rational). Float = opt-in via `~f64`. Range analysis native dans IDE + compile-time. | ✓ SOLIDE |
| D76 | Range types natifs | `age : 0..150` = le type est la borne. Overflow vérifié statiquement. Représentation auto (u8, i16, etc.). | ✓ SOLIDE |
| D77 | Algèbre géométrique émergente | Rotors, multivecteurs, CGA = `*` `*^` `*.` `.!`. 360°/N adaptatif (D28). Aucun boson ajouté. Q28 fermée. | ✓ ÉMERGENT |
| D78 | Stack : SDL3 + wgpu + Cranelift | SDL3 = fenêtrage/input. wgpu = GPU cross-platform + WebGPU. Cranelift = proto + WASM. LLVM = prod. | ✓ SOLIDE |

| D79 | Accessibilité par construction | IDE (range realtime) + compilateur (refuse code dangereux) + sugar (mots familiers) = barrière d'entrée basse + sécurité haute. Un débutant produit du code aussi sûr qu'un expert. | ✓ SOLIDE |

### §11 — MULTI-FICHIER / POO ÉMERGENTE

Le système OOP (programmation orientée objet) émerge de `:` + `.` + `@` + `|>` :

```
// === types/animal.342 ===
Animal : {
    name : str; age : 0..50;
    .new : (name: str, age: 0..50) : @ = { @ = { name, age } };
    .speak : () : str;                   // méthode abstraite
    .info : () : str = { << "$(@.name), $(@.age) ans" };
};

// === types/dog.342 ===
|> types/animal;                         // |> = import (CONNECTER+DIRIGER)
Dog : Animal {                           // héritage via :
    breed : str;
    .new : (name, age, breed) : @ = { @ = Animal.new(name, age) + { breed } };
    .speak : () : str = { << "Woof!" };
};

// === main.342 ===
|> types/dog;
main : () = {
    pets = [ Dog.new("Rex", 5, "Berger") ];
    pets |* { pet |> pet.speak() |> print };   // polymorphisme
};
```

Pas de `class`, `extends`, `implements` — tout émerge de `:` (héritage = typage), `.` (méthodes), `@` (self).

### §12 — ANNOTATIONS `#{}` : COMMENT ÇA SAIT

L'annotation `#{}` (boson CONFIGURER) est **scopée** — elle s'attache au contexte immédiat :

```
// Sur une variable → configure CETTE variable
x : rational #{ precision: fast } = 0.1;

// Sur un bloc → configure TOUT le bloc
#{ precision: fast } { a = 0.1 + 0.2; b = sin(x); };

// Sur une fonction → configure la fonction entière
calc : (x: f64) : f64 #{ precision: exact } = { ... };

// Sur un fichier (en haut) → configure tout le fichier
#{ precision: fast, target: gpu }
```

Le compilateur a un **registre de clés connues** :
- `precision` → types numériques (rational/float)
- `overflow` → opérations arithmétiques (saturate/wrap/trap)
- `target` → compilation (CPU/GPU/QPU)
- `arena` → allocation mémoire
- Clé inconnue → erreur compile

### §13 — OVERFLOW : PAS DE CLAMP IMPLICITE

```
// Range overflow → le développeur CHOISIT :
x : -1000..1000 = get_value();

// Statique → erreur compile
x = 1500;                          // ❌ COMPILE ERROR

// Dynamique → ? (boson TESTER) gère
x = external_input();              // type = i32 | #
x ? { + : use; - : use; # : handle_overflow };

// Clamp explicite si voulu
x = external_input() |> clamp(-1000, 1000);

// Saturation via annotation
x : -1000..1000 #{ overflow: saturate } = calc();  // → clamp auto
```

Philosophie : **ne jamais perdre d'information silencieusement.**

### §14 — DIVISION PAR ZÉRO = PROJECTION À L'INFINI

La sphère de Riemann a 2 pôles : 0 (sud) et ∞ (nord). `* 0` = collapse vers 0. `/ 0` = projection vers ∞. Ce sont des transformations géométriques, pas des erreurs.

```
x = 42 / 0;        // = ∞ (fermion valide, PAS une erreur)
x : number | ∞;     // ∞ est dans le système de types

// Arithmétique cohérente :
∞ + 1 = ∞;          // absorbant
∞ * 0 = #;          // indéterminé → # (out-of-band)
1 / ∞ = 0;          // retour à l'origine
-1 / 0 = -∞;        // infini signé

// Range analysis :
y = 42 / z;
// z : 1..100 → type = number (pas de /0)
// z : 0..100 → type = number | ∞ (IDE: "⚠️ z peut être 0")

// Opt-out vers erreur :
y : number !∞ = 42 / z;              // !∞ = erreur si /0
#{ division_zero: trap } { y = 42 / z; };  // idem via annotation
```

`∞` est un **fermion** (valeur/donnée, spin ½), PAS un boson. Comme `0`, `+` (true), `-` (false), `#` (erreur). 18 bosons inchangés.

→ **D80 : Division par zéro = ∞ (Riemann/CGA). Fermion valide. `∞ * 0 = #`. Opt-out : `!∞` ou `#{ division_zero: trap }`.**

### §15 — `#key` ÉMERGENT (PAS HARDCODÉ)

Le boson `#` est natif. Les clés dans `#{}` sont des **types définis par le langage** :

```
// Dans prelude.342 (standard, pas hardcodé) :
precision : #key : { fast, exact, fixed(n: 0..128) };
overflow  : #key : { trap, saturate, wrap };
target    : #key : { cpu, gpu, qpu, auto };

// #key = trait "ce qui peut aller dans #{}"
// Le dev peut créer ses propres clés :
logging : #key : { verbose, quiet, off };
#{ logging: verbose } { my_function(); };

// Backend binding (liaison) :
target.gpu  |> backend.wgpu;    // le compilateur dispatch
target.qpu  |> backend.qiskit;
// Chaque variante implémente .compile() et .execute()
```

→ **D81 : `#key` = trait émergent. `#{}` natif, clés = types utilisateur. Extensible sans modifier le compilateur.**

### §16 — PRELUDE : `str` ET TYPES DE BASE

Tout émerge des bosons, mais il faut un socle. Le **prelude** est un fichier .342 normal chargé implicitement :

```
// prelude.342 — PAS du hardcoding, réécritable
char : 0..1_114_111;          // point Unicode → u32 (range type)
str  : [char];                // liste de chars (gluon [] + contenu)
bool : { + | - };             // spin positif ou négatif
byte : 0..255;                // u8

// "" est le gluon qui crée un str :
name = "Rex";                 // = [82, 101, 120] : str

// StringBuilder émerge aussi :
builder = [];                 // [char] vide
@@builder = builder + "hello";  // concaténation
@@builder = builder + " world";
result = builder |> str;        // conversion → "hello world"

// Ou via pipe (stream) :
result = ["hello", " ", "world"] +{ ""; (acc, s) { acc + s } };
```

### §17 — DIAMOND PROBLEM : RÉSOLUTION PAR `+` EXPLICITE

```
DiamondTrap : ScavTrap + FragTrap {
    // "+" = union. Le compilateur REFUSE si conflit non résolu :
    // ❌ ".attack existe dans ScavTrap ET FragTrap"

    .attack = ScavTrap.attack;     // résolution explicite
    // "..Parent.new()" = spread (copie les champs, pas de duplication)
};
// Pas de virtual inheritance, pas de vtable cachée.
// Le dev choisit, le compilateur vérifie.
```

### §18 — VALIDATION : PROJETS 42 EN 342

| Projet 42 | Concept testé | Émergence 342 |
|-----------|---------------|----------------|
| libft | Types de base, strings, listes | `str : [char]`, `[T]`, `|*` map, `+{}` fold |
| ft_printf | Format strings, variadics | `%"fmt"` (boson PROPORTIONNER + gluon "") |
| push_swap | Algo, piles, complexité | `[T]`, `+{}` fold, range analysis optimise |
| pipex | Pipes, fork, exec, fd | `\|` pipe natif, `\|{}` process scope |
| minitalk | Signaux, bits, protocoles | `:.bits`, `\|!` async, `#{}` protocole |
| so_long | Graphique 2D, events | SDL3 + wgpu, `~{}` rendu GPU |
| philosophers | Threads, mutex, deadlock | `\|{} \| \|{}` threads, `@@` séquentiel (zéro deadlock) |
| cub3d | Raycasting, maths 3D | `*^` wedge, rotors GA, `~{}` GPU |
| ft_irc | Réseau, protocoles, multi-client | `\|!` async, `<>{}` I/O, `#{} ` protocole |
| webserv | HTTP, parsing, concurrence | `\|{}` workers, `?{}` routing, `str` parsing |
| inception | Containers, isolation | `@{}` scope isolation, `#{ target }` dispatch |
| CPP modules | OOP, templates, diamond | `:` héritage, `+` union, `()` génériques |

Chaque projet 42 est constructible avec les 18 bosons. Le langage se valide par les exercices.

### Compteurs
- **83 décisions** (D1-D83)
- **18 bosons** | **5 gluons** | **3 séparateurs** | **18 gravitons**
- **28 questions** (Q1-Q28 **TOUTES RÉSOLUES**)
- **4 tiers SOLIDES** (s,p,d,f)

---

## SOURCES

| Source | URL | Pertinence |
|--------|-----|------------|
| SDL3 3.4.2 | github.com/libsdl-org/SDL/releases | Stable, production |
| wgpu | wgpu.rs | Rust GPU cross-platform, pré-1.0 production |
| Cranelift | cranelift.dev | Production via Wasmtime, WASM natif |
| IEEE 754-2008 | ieee.org | Standard decimal128 |
| Ada delta types | learn.adacore.com | Précédent range types |
| CliffordNet 2026 | arxiv.org/pdf/2601.06793 | GA neural networks |
| GA-FuL C# | github.com/ga-explorer | Multivectors library |
| Multivectors.jl | github.com/digitaldomain | Julia GA library |
