# 3.42 — Spécification Syntaxique v4.1 (post-audit)
## "Tout est Sphère, ? est Mesure, le Spin détermine tout"
## ÉMERGENCE TOTALE — Règles minimales → comportement complexe

---

## 1. QUARKS (7 opérateurs fondamentaux)

Ce sont les briques atomiques. Tout le reste ÉMERGE de leur combinaison.

| Quark | Nom | Rôle |
|-------|-----|------|
| `.` | accès faible | lecture sans consommer (weak measurement) + auto-propagation spin |
| `?` | mesure forte | collapse du spin, branchement, consommation possible |
| `??` | mesure continue | boucle/stream — mesure répétée tant que le flux produit |
| `|` | pipe SPSC | connecte deux sphères via ring buffer (parallélisme émergent) |
| `~` | broadcast SPMC | diffuse une sphère vers N consommateurs |
| `{}` | contexte | scope, corps de fonction, bloc de mesure, arène mémoire |
| `:` | liaison | pattern → action, type annotation, clé → valeur |

### Pourquoi 7 ?
Chaque quark est irréductible. Aucun ne peut être exprimé par les autres. Ensemble, ils font ÉMERGER : if/else, switch/match, foreach, while, loop, try/catch, pipeline, stream, scan, parallélisme, diffusion, introspection.

---

## 2. SPINS (5 états)

Toute sphère porte un spin. C'est son état fondamental.

| Spin | Trit | Signification | Mémoire | Quantique |
|------|------|---------------|---------|-----------|
| `+` | 01 | positif, succès, vrai | valide, propriétaire actif | spin up |
| `-` | 11 | négatif, échec, faux | déplacée (moved) | spin down |
| `*` | 00 | neutre, vide, superposition | non initialisée | indéterminé |
| `#` | 10 | erreur, corrompu, décohérence | corrompue | décohérent |
| `_` | — | wildcard (attrape-tout) | — | — |

`_` n'est pas un état réel — c'est un pattern qui match n'importe quel spin.

---

## 3. GLUONS (ce qui lie les sphères)

| Symbole | Rôle |
|---------|------|
| `=` | nommer une sphère |
| `;` | terminer une instruction |
| `()` | groupement, paramètres, generics |
| `[]` | liste, map, index, slice |
| `..` | range (sphère itérable native) |
| `::` | chemin dans les modules |
| `@` | emprunt lecture (postfix) / annotation méta (prefix) |
| `@@` | emprunt écriture exclusif (postfix) |

**Règle** : `@` en PREFIX = annotation méta. `@`/`@@` en POSTFIX = emprunt.

---

## 4. OPÉRATEURS

### Arithmétique
`+  -  *  /  %`

### Bitwise (sur int et trit)
`&  ^  <<  >>`
Note v4 : `|` n'est plus bitwise OR — c'est le pipe. Bitwise OR via `bits::or()`.

### Logique
`&&  ||  !`

### Comparaison
`==  !=  <  >  <=  >=`

### Spécifiques 3.42
| Opérateur | Rôle |
|-----------|------|
| `!` | NOT unifié (binaire/ternaire/quantique — déduit du type) |
| `identifier?` | binding (capture d'une valeur dans un nom) |
| `.?` | point de contrôle explicite / introspection |

### Concaténation (v4.1)
`++` pour listes et strings : `[1, 2] ++ [3, 4]` → `[1, 2, 3, 4]`

### Précédence (du plus bas au plus haut)
```
|           pipe SPSC
~           broadcast SPMC
? ??        mesure (forte / continue)
||          OU logique
&&          ET logique
^           XOR bitwise
&           ET bitwise
== !=       égalité
< > <= >=   comparaison
<< >>       décalage
++ + -      concaténation, addition
* / %       multiplication
! - + * #   unaire, spin prefix
. () [] {}  postfix
```

---

## 5. MÉMOIRE

| Mécanisme | Syntaxe | Effet |
|-----------|---------|-------|
| Move (défaut) | `send(data)` | data déplacée, n'existe plus |
| Emprunt lecture | `print(data@)` | emprunt temporaire, data reste |
| Emprunt écriture | `modify(data@@)` | emprunt exclusif temporaire |
| Copie explicite | `backup = copy(data)` | duplication volontaire |
| Mutable | `mut x = 5` | sphère modifiable |

### Spin de propriété
| Spin | État mémoire |
|------|-------------|
| `+` | valeur valide, propriétaire actif |
| `-` | valeur déplacée (moved), inaccessible |
| `*` | pas encore initialisée |
| `#` | corrompue (ne devrait jamais arriver) |

### Règles d'emprunt (vérifiées au compile-time)
- Un seul `@@` (écriture) OU N `@` (lecture) à la fois — jamais les deux
- L'emprunt ne peut pas survivre au scope qui l'a créé
- Le compilateur vérifie statiquement — zéro overhead runtime
- Pas de cycles de référence possibles (emprunts forment un DAG)

---

## 6. ÉMERGENCE — CE QUI N'EST PAS DÉFINI MAIS EXISTE

| Construction | Émerge de | Syntaxe |
|-------------|-----------|---------|
| if/else | `?` sur valeur unique | `(cond) ? { + : a; - : b; }` |
| switch/match | `?` avec conditions | `val ? { (c1) : a; (c2) : b; _ : c; }` |
| foreach | `?` sur collection/range | `0..100 ? { i? : action(i); }` |
| while | `??` sur condition mutable | `(cond) ?? { + : body; }` |
| loop | `??` sur true | `(true) ?? { + : body; }` |
| try/catch | `?` après opération faillible | `op() { r? : ok; - e? : handle; }` |
| scan | `?` multi-spin sur collection | `list ? { i? : a; - e? : b; * : c; }` |
| pipeline séq. | chaînage UFCS `.` | `a.b().c() { r? : ok; }` |
| pipeline par. | chaînage `|` | `a | b | c` |
| stream | `??` sur flux infini | `source ?? { item? : process; }` |
| diffusion | `~` sur flux | `source ~ [a, b, c]` |
| coin flip | `?` sur probabilité | `0.5p ? { + : "pile"; - : "face"; }` |
| quantum | `?` sur qubit | `q ? { + : "up"; - : "down"; }` |
| parallélisme | `|` entre fonctions | scheduler auto-décide threads/GPU/QPU |
| async I/O | `??` + I/O dans le body | scheduler détecte → non-blocking auto |
| GPU compute | `|` sur collection homogène | scheduler détecte → batch GPU auto |
| QPU route | `|` avec types qubit | compilateur détecte → route QPU auto |

**Principe** : ZÉRO annotation d'exécution. Le programmeur dit QUOI. Le scheduler/compilateur décide COMMENT.

---

## 7. PIPE `|` — PARALLÉLISME ÉMERGENT

### Sémantique
`a | b` crée deux green threads connectés par un ring buffer SPSC lock-free.
- `a` produit des valeurs
- `b` consomme ces valeurs
- Le buffer découple les vitesses

### Comportement adaptatif du scheduler
| Contexte détecté | Décision du scheduler |
|------------------|-----------------------|
| Volume petit (<1K éléments) | Exécution séquentielle (zéro overhead) |
| Volume grand + fonction pure | Parallélise sur N threads CPU |
| Collection homogène + opération uniforme | Batch vers GPU compute |
| Types qubit détectés | Route vers QPU (ou simule si absent) |
| I/O dans le pipeline | io_uring/kqueue non-blocking |
| Pipeline trivial (identité) | Éliminé à la compilation |

### Exemples
```
// Pipeline CPU parallèle
data | filter(x: x > 0) | map(x: x * 2) | save;

// Le scheduler voit : grande collection + fonctions pures → N threads

// Pipeline GPU émergent
pixels | shade | composite;
// Le scheduler voit : collection uniforme + opération homogène → GPU

// Pipeline quantique émergent
qubits | quantum::H | measure;
// Le compilateur voit : type qubit → route QPU

// Async émergent
server ?? { req? : req; } | handle | respond;
// Le scheduler voit : I/O dans handle → non-blocking
```

---

## 8. BROADCAST `~` — DIFFUSION SPMC

### Sémantique
`source ~ targets` crée un canal SPMC : chaque valeur produite est envoyée à tous les consommateurs.

```
// Un producteur, trois consommateurs
event_stream ~ [logger, analytics, alerter];

// Avec transformation par branche
events ~ [
    filter(e: e.level == "error") | alert,
    filter(e: e.type == "metric") | dashboard,
    archive
];
```

---

## 9. TYPES NATIFS

### Primitifs (built-in, compilés dans le binaire)
| Type | Description |
|------|-------------|
| `int` | entier signé (taille adaptée au contexte) |
| `float` | flottant (32 ou 64 bits selon contexte) |
| `bool` | booléen (true/false) |
| `trit` | ternaire (2 bits : +, -, *, #) |
| `string` | chaîne UTF-8 |
| `prob` | probabilité (0.0 à 1.0, littéral avec suffixe `p`) |
| `qubit` | sphère en superposition quantique |

### Composés (built-in)
| Type | Description |
|------|-------------|
| `list(T)` | liste dynamique |
| `(T, U)` | tuple |
| `[K : V]` | map (table de hachage) |

### Littéral probabilité (natif)
```
coin = 0.5p;                    // prob = 50%
coin ? { + : "pile"; - : "face"; };

both = p1 && p2;                // intersection (ET probabiliste)
either = p1 || p2;              // union (OU probabiliste)
not_p = !p1;                    // complémentaire
```

### Quantique natif
```
q = qubit();                    // sphère en superposition (spin *)
q ? { + : "up"; - : "down"; }; // mesure = collapse

// Intrication (via module quantum)
use quantum;
q1 = quantum::H(qubit());
quantum::CNOT(q1, q2);         // q1 et q2 intriqués
q1 ? { + : "corrélé +"; - : "corrélé -"; };
```

---

## 10. MODULES FONDAMENTAUX

Les modules sont livrés avec le compilateur mais ne sont PAS des built-in. Ils sont écrits en 3.42 (ou linkés depuis C pour le hardware).

### module quantum
```
module quantum {
    H = (q: qubit) : qubit;         // Hadamard
    X = (q: qubit) : qubit;         // Pauli-X (NOT)
    Y = (q: qubit) : qubit;         // Pauli-Y
    Z = (q: qubit) : qubit;         // Pauli-Z (phase flip)
    CNOT = (ctrl: qubit, tgt: qubit@@);  // Controlled NOT
    SWAP = (a: qubit@@, b: qubit@@);     // Échange
    T = (q: qubit) : qubit;         // π/8 gate
    S = (q: qubit) : qubit;         // π/4 gate
    Rx = (q: qubit, angle: float) : qubit;
    Ry = (q: qubit, angle: float) : qubit;
    Rz = (q: qubit, angle: float) : qubit;
}
```

### module math
```
module math {
    pi;                              // stream infini de décimales
    e;                               // stream infini
    phi;                             // stream infini

    abs = (n: T) : T;
    sqrt = (n: float) : float;
    pow = (base: float, exp: float) : float;
    min = (a: T, b: T) : T;
    max = (a: T, b: T) : T;
    sin = (x: float) : float;
    cos = (x: float) : float;

    uniform = (min: float, max: float) : prob;
    normal = (mean: float, stddev: float) : prob;
}
```

---

## 11. BUILT-IN FUNCTIONS (irréductibles, dans le binaire)

Seules les fonctions qui ne PEUVENT PAS être écrites en 3.42 sont built-in :

```
// I/O (nécessite HAL)
print(args...)
read()

// Collections (intrinsèques au type system)
len(collection)
push(list, item)
pop(list)

// Mémoire (intrinsèque au compilateur)
copy(sphere)

// Temps (nécessite HAL)
now()
```

**Tout le reste** vit dans des modules : `math`, `quantum`, `bits`, `runtime`, etc.

---

## 12. STRUCT LITERAL (v4.1)

```
p = Point { x: 1.0, y: 2.0 };
s = Session { user_id: 42, token: "abc", expires: 3600 };
```

Résolution d'ambiguïté avec MeasureBlock : StructLiteral utilise `,` entre les champs, MeasureBlock utilise `;` dans les branches. Le parser essaie StructLiteral en premier.

---

## 13. VARIANT CONSTRUCTOR (v4.1)

Les variantes d'enum se construisent avec un spin préfixe :
```
c = + Red;                              // variante positive
err = - Denied("accès refusé");         // variante négative
nothing = * Empty;                      // variante neutre
critical = # Corrupted("données");      // variante erreur
```

Le `+` et `-` sont parsés comme unaire (le type system distingue spin vs arithmétique). Le `*` et `#` sont parsés par SpinExpr dédié.

### Destructuring dans pattern matching :
```
result ? {
    + Authenticated(session?) : use(session);
    - Denied(msg?) : print(msg);
    # Locked(reason?) : alert(reason);
    _ : default_action();
};
```

---

## 14. MESURE ANONYME (v4.1)

`?` sans bloc de mesure — collapse et retourne la valeur :
```
result = q ?;                           // qubit → 0 ou 1
bit = coin ?;                           // prob → + ou -
```

Utile quand on veut juste la valeur sans brancher.

---

## 15. CONCATÉNATION `++` (v4.1)

```
merged = [1, 2] ++ [3, 4];            // → [1, 2, 3, 4]
full = "hello" ++ " world";            // → "hello world"
tree = left_nodes ++ [root] ++ right_nodes;
```

Même précédence que `+`. Associativité gauche.

---

## 16. ARGUMENTS NOMMÉS (v4.1)

```
runtime::config(threads: 4, buffer: 8192);
create_user(name: "Alice", age: 25);
```

Arguments positionnels et nommés peuvent coexister : `foo(42, name: "test")`.

---

## 17. MÉTHODES INTRINSÈQUES (v4.1)

Résolues par UFCS : `obj.method(args)` → `method(obj, args)`. Pas de règle PEG spéciale.

### list(T)
`filter`, `map`, `collect`, `take`, `skip`, `sort`, `reverse`, `find`, `contains`, `push`, `pop`, `len`

### string
`split`, `join`, `trim`, `contains`, `starts_with`, `ends_with`, `replace`, `len`

### Iterator(T)
`collect`, `take`, `skip`, `count`, `fold`

---

## 18. MODULE RUNTIME (v4.1)

Contrôle fin du scheduler quand l'émergence ne suffit pas :

```
module runtime {
    config = (threads: int, buffer: int);  // configurer le pipe
    config = ();                           // reset auto
    sleep = (ms: int);                     // pause
    threads = () : int;                    // nombre de threads actifs
    uptime = () : int;                     // temps depuis le démarrage
}
```

### 3 niveaux de contrôle
| Niveau | Syntaxe | Contrôle |
|--------|---------|----------|
| Séquentiel garanti | `data.process().save()` (UFCS `.`) | Total |
| Parallèle émergent | `data | process | save` (pipe `|`) | Scheduler décide |
| Parallèle forcé | `runtime::config(threads: 8); data | process;` | Total |

---

## 19. MODULE QUANTUM COMPLET (v4.1)

```
module quantum {
    // Portes à 1 qubit
    H = (q: qubit) : qubit;              // Hadamard
    X = (q: qubit) : qubit;              // Pauli-X (NOT)
    Y = (q: qubit) : qubit;              // Pauli-Y
    Z = (q: qubit) : qubit;              // Pauli-Z (phase flip)
    T = (q: qubit) : qubit;              // π/8 gate
    S = (q: qubit) : qubit;              // π/4 gate
    Rx = (q: qubit, angle: float) : qubit;
    Ry = (q: qubit, angle: float) : qubit;
    Rz = (q: qubit, angle: float) : qubit;

    // Portes à 2+ qubits
    CNOT = (ctrl: qubit, tgt: qubit@@);
    SWAP = (a: qubit@@, b: qubit@@);
    CZ = (a: qubit, b: qubit) : (qubit, qubit);   // Controlled-Z
    controlled_z = (qubits: list(qubit)@@);         // Multi-controlled Z

    // Utilitaires
    barrier = (qubits: list(qubit)@@);              // Synchronisation
    reset = (q: qubit) : qubit;                     // Reset à |0⟩
}
```

---

## 20. ANNOTATIONS MÉTA (uniquement 2)

| Annotation | Effet |
|------------|-------|
| `@extend` | définir un nouveau domaine syntaxique (macros hygiéniques) |
| `@comptime` | forcer l'exécution à la compilation |

**Supprimés en v4** : `@parallel`, `@gpu`, `@async`, `@qpu`, `@public`
Tout ceci ÉMERGE maintenant de `|`, `~`, `?`, `??` et du scheduler adaptatif.

**Règle** : une annotation ne change JAMAIS la sémantique d'exécution. Elle affecte uniquement QUAND (comptime) ou COMMENT le code est interprété (extend).

---

## 21. SELF — BINDING IMPLICITE

`self` n'est PAS un mot-clé. C'est un binding implicite injecté par le compilateur dans les `ImplDecl`.

```
Point : Comparable(Point) {
    cmp = (other: Point@) : int {
        // self est automatiquement disponible ici
        // C'est une référence @ à l'instance courante
        math::sqrt(self.x * self.x + self.y * self.y)
            - math::sqrt(other.x * other.x + other.y * other.y);
    };
}
```

En dehors d'un ImplDecl, `self` est un identifiant normal (peut être utilisé comme variable).

---

## 22. ARCHITECTURE COMPILATION

```
Code 3.42
    ↓
PEG Grammar (lexer+parser combiné)
    ↓ produit
AST (nœud unique de contrôle : QueryNode)
    ↓
Analyse statique (emprunts, types, spins, détection qubit/prob)
    ↓
C11 freestanding (pas de libc, pas de syscalls)
    ↓
├── Mode natif (kernel 3.42) → scheduler intégré, zéro syscall
├── Mode hosted (Linux/Win/Mac) → HAL mince (~20 fonctions)
└── Mode quantique → OpenQASM 3 pour QPU (détecté auto par types qubit)
    ↓
Linkage externe : SDL3 (graphique) + WGPU (GPU compute)
    ↓
Hardware (CPU/GPU/QPU)
```

Le C n'est PAS une stdlib. C'est un format d'export pour la portabilité. Le compilateur 3.42 génère du C11 freestanding. Les seuls liens externes sont SDL3 et WGPU.

Le scheduler adaptatif analyse le code à la compilation ET au runtime pour décider automatiquement : séquentiel, parallèle CPU, GPU compute, QPU, ou I/O non-bloquant.

---

## 23. RUNTIME INTÉGRÉ

Tout ce qui suit est natif (compilé dans le binaire) :

- Green threads (2KB par thread, ~200ns context switch)
- Arena allocation (64MB chunks, libération en bloc)
- SLAB pools (objets taille fixe, O(1))
- Lock-free SPSC ring buffers (pipe `|`)
- Lock-free SPMC ring buffers (broadcast `~`)
- io_uring/kqueue/IOCP (I/O batché — via HAL)
- Scheduler adaptatif (décide CPU/GPU/QPU/séquentiel automatiquement)
- CAS atomique sur 2 bits pour les spins (~5ns vs ~25ns mutex)
- Move semantics (copie du pointeur, ~1ns, NRVO élimine la majorité)
- Simulateur quantique intégré (jusqu'à ~30 qubits)
- PRNG hardware (RDRAND/RDSEED quand disponible)

---

## 24. INVENTAIRE DES SYMBOLES

```
Quarks :     . ? ?? | ~ {} :               (7)
Spins :      + - * # _                     (5)
Gluons :     = ; () [] .. :: @ @@          (8)
Logique :    ! && ||                        (3)
Bitwise :    & ^ << >>                      (4)  — | supprimé (= pipe)
Arithm :     + - * / % ++                   (6)
Comparaison: == != < > <= >=               (6)

Mot-clés :   mut return break copy          (4)
             module type trait as            (4)
             true false qubit               (3)

Total : ~15 symboles uniques + 11 mot-clés
(vs C : 38 mots-clés + 45 opérateurs)
(vs Rust : 52 mots-clés + ~40 opérateurs)
```

---

## 25. RETURN AVEC SPIN

```
return data;             // spin + implicite
return - "erreur";       // spin négatif (échec)
return # "corruption";   // spin erreur (fatal)
return * ;               // spin neutre (vide)
```

Le spin fait partie de la valeur retournée. L'appelant le mesure avec `?`.
