# 3.42 — Spécification Syntaxique v3
## "Tout est Sphère, ? est Mesure, le Spin détermine tout"
## TOUT EST NATIF — Pas de stdlib — C = export/linkage uniquement

---

## 1. QUARKS (primitives fondamentales)

### Sphère
Toute valeur est une sphère : contenu + spin (état trit).

### Spin (4 états)
| Pattern | Trit | Signification | Sphère | Quantique |
|---------|------|---------------|--------|-----------|
| `+` | 01 | positif, succès, vrai | pôle nord | spin up |
| `-` | 11 | négatif, échec, faux | pôle sud | spin down |
| `*` | 00 | neutre, vide, superposition | centre | indéterminé |
| `#` | 10 | erreur, corrompu, décohérence | invalide | décohérent |

### Wildcard
| Pattern | Signification |
|---------|---------------|
| `_` | attrape-tout (match n'importe quel spin) |

### Mesure
| Opérateur | Effet |
|-----------|-------|
| `?` | mesure unique (if/match/foreach/collapse selon le type) |
| `??` | mesure continue (while/loop/stream) |

### Accès
| Opérateur | Effet |
|-----------|-------|
| `.` | accès membre + auto-propagation du spin |
| `.?` | point de contrôle explicite (intercepter le spin) |

---

## 2. GLUONS (ce qui lie)

| Symbole | Rôle |
|---------|------|
| `=` | nommer une sphère |
| `:` | lier pattern → action, annotation de type, clé → valeur (map) |
| `;` | terminer une instruction |
| `{}` | contexte structuré (mesure ou corps de fonction) |
| `()` | groupement, paramètres, generics |
| `[]` | liste, map, index, slice |
| `..` | range (sphère itérable native) |
| `::` | chemin dans les modules |
| `@` | emprunt lecture (postfix) / annotation (prefix) |
| `@@` | emprunt écriture exclusif (postfix) |

---

## 3. OPÉRATEURS

### Arithmétique
`+  -  *  /  %`

### Bitwise (fonctionne sur int ET trit)
`&  |  ^  <<  >>`

### Logique
`&&  ||  !`

### Comparaison
`==  !=  <  >  <=  >=`

### Spécifiques 3.42
| Opérateur | Rôle |
|-----------|------|
| `!` | NOT unifié (binaire/ternaire/quantique — déduit du type) |
| `~` | RÉSERVÉ (libre pour futur concept) |
| `identifier?` | binding (capture d'une valeur dans un nom) |
| `.?` | point de contrôle explicite |

---

## 4. MÉMOIRE

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

---

## 5. ANNOTATIONS

| Annotation | Effet |
|------------|-------|
| `@parallel` / `@parallel(N)` | exécution parallèle CPU (N threads max) |
| `@gpu` / `@gpu("compute")` | compute shader GPU |
| `@async` | I/O non-bloquant |
| `@qpu` / `@qpu("backend")` | exécuter sur processeur quantique |
| `@extend` | définir un nouveau domaine (macros hygiéniques) |
| `@comptime` | exécution à la compilation |
| `@public` | sphère accessible inter-processus |

**Règle** : `@` en PREFIX = annotation. `@`/`@@` en POSTFIX = emprunt.
**Règle d'or** : les annotations optimisent ou ciblent un backend, elles ne changent pas la sémantique.

---

## 6. ÉMERGENCE — CE QUI N'EST PAS DÉFINI MAIS EXISTE

| Construction | Émerge de | Syntaxe |
|-------------|-----------|---------|
| if/else | `?` sur valeur unique | `(cond) ? { + : a; - : b; }` |
| switch/match | `?` avec conditions | `val ? { (c1) : a; (c2) : b; _ : c; }` |
| foreach | `?` sur collection/range | `0..100 ? { i? : action(i); }` |
| while | `??` sur condition mutable | `(cond) ?? { + : body; }` |
| loop | `??` sur true | `(true) ?? { + : body; }` |
| try/catch | `?` après opération faillible | `op() { result? : ok; - err? : handle; }` |
| scan | `?` multi-spin sur collection | `list ? { item? : a; - e? : b; * : c; }` |
| pipeline | chaînage UFCS + `{}` | `a.b().c() { r? : ok; - : err; }` |
| stream | `??` sur flux infini | `pi ?? { digit? : process(digit); }` |
| coin flip | `?` sur probabilité | `0.5p ? { + : "pile"; - : "face"; }` |
| quantum measure | `?` sur qubit | `q ? { + : "up"; - : "down"; }` |

---

## 7. TYPES NATIFS

### Primitifs (built-in, pas de stdlib)
| Type | Description |
|------|-------------|
| `int` | entier signé (taille adaptée au contexte) |
| `float` | flottant (32 ou 64 bits selon contexte) |
| `bool` | booléen (true/false) |
| `trit` | ternaire (2 bits : +, -, *, #) |
| `string` | chaîne UTF-8 |
| `prob` | probabilité (0.0 à 1.0, littéral avec `p`) |
| `qubit` | sphère en superposition quantique |

### Composés (built-in)
| Type | Description |
|------|-------------|
| `list(T)` | liste dynamique |
| `(T, U)` | tuple |
| `[K : V]` | map (table de hachage) |

### Constantes streams (built-in, itérables infinis)
| Constante | Valeur |
|-----------|--------|
| `pi` | 3.14159... (stream de décimales) |
| `e` | 2.71828... (stream de décimales) |
| `phi` | 1.61803... (stream de décimales) |

### Probabilité native
```
// Littéral avec suffixe p
coin = 0.5p;
coin ? { + : "pile"; - : "face"; };

// Opérations
both = p1 && p2;        // intersection (ET probabiliste)
either = p1 || p2;      // union (OU probabiliste)
not_p = !p1;            // complémentaire

// Distribution (built-in functions, pas de stdlib)
dice = uniform(1, 6);
height = normal(175.0, 10.0);
```

### Quantique natif
```
// Créer un qubit (spin * = superposition)
q = qubit();

// Portes quantiques (built-in, pas de stdlib)
q = H(q);              // Hadamard → superposition 50/50
q = X(q);              // Pauli-X → inversion (NOT quantique)
q = Z(q);              // Pauli-Z → phase flip
CNOT(control, target); // Porte contrôlée (intrication)
SWAP(q1, q2);          // Échange de qubits

// Mesure = ? (comme toujours — le paradigme EST quantique)
q ? {
    + : "spin up";
    - : "spin down";
};

// Intrication (entanglement)
q1 = qubit();
q2 = qubit();
q1 = H(q1);
CNOT(q1, q2);           // q1 et q2 sont maintenant intriqués
q1 ? {
    + : q2;              // q2 est AUSSI + (corrélé)
    - : q2;              // q2 est AUSSI - (corrélé)
};

// QPU distant
@qpu("ibm_eagle")
quantum_search = () {
    // Ce bloc s'exécute sur le processeur quantique distant
    q = qubit();
    q = H(q);
    q ?;  // mesure et retourne le résultat classique
};

// Sans @qpu → simulation locale automatique (jusqu'à ~30 qubits)

// Uncomputation automatique :
// Les qubits temporaires sont nettoyés par le compilateur
// Si impossible → erreur compile-time (pas de fuite quantique)
```

---

## 8. ARCHITECTURE COMPILATION

```
Code 3.42
    ↓
PEG Grammar (formalise comment lire le code — lexer+parser combiné)
    ↓ produit
AST (unique nœud de contrôle : QueryNode)
    ↓
C11 freestanding (pas de libc, pas de syscalls)
    ↓
├── Mode natif (kernel 3.42) → scheduler intégré, zéro syscall
├── Mode hosted (Linux/Win/Mac) → HAL mince (~20 fonctions)
└── Mode quantique → compile vers OpenQASM 3 pour QPU
    ↓
Linkage externe : SDL3 (graphique) + WGPU (GPU compute)
    ↓
Hardware (CPU/GPU/QPU)
```

**Le C n'est PAS une stdlib.** C'est un format d'export pour la portabilité. Le compilateur 3.42 génère du C11 freestanding qui se compile partout. Les seuls liens externes sont SDL3 et WGPU, inclus via le système de modules natif.

---

## 9. RUNTIME INTÉGRÉ

Tout ce qui suit est natif (compilé dans le binaire, pas de bibliothèque) :

- Green threads (2KB par thread, ~200ns context switch)
- Arena allocation (64MB chunks, libération en bloc)
- SLAB pools (objets taille fixe, O(1))
- Lock-free SPSC ring buffers (scheduler)
- io_uring/kqueue/IOCP (I/O batché — via HAL)
- Scheduler adaptatif (décide CPU/GPU/QPU/séquentiel automatiquement)
- CAS atomique sur 2 bits pour les spins (~5ns vs ~25ns mutex)
- Move semantics (copie du pointeur, ~1ns, NRVO élimine la majorité)
- Simulateur quantique intégré (jusqu'à ~30 qubits)
- Générateur de nombres aléatoires hardware (RDRAND/RDSEED quand disponible)

---

## 10. INVENTAIRE DES SYMBOLES

```
Spins :      + - * #                    (4)
Wildcard :   _                          (1)
Mesure :     ? ??                       (2)
Contrôle :   .? identifier?             (2)
Range :      ..                         (1)
Prob :       p (suffixe)                (1)
Logique :    ! && ||                    (3)
Bitwise :    & | ^ << >>               (5)
Arithm :     + - * / %                  (5 — contexte distingue des spins)
Comparaison: == != < > <= >=            (6)
Structure :  { } ( ) [ ] ; : = . ::     (11)
Emprunt :    @ @@                       (2 — postfix)
Réservé :    ~                          (1)

Mot-clés :   mut return break copy      (4)
             module type trait as        (4)
             true false                  (2)
             qubit self                  (2)

Total : ~15 symboles uniques + 12 mot-clés
(vs C : 38 mots-clés + 45 opérateurs)
(vs Rust : 52 mots-clés + ~40 opérateurs)
```

---

## 11. BUILT-IN FUNCTIONS (natives, pas de stdlib)

```
// I/O
print(args...)           // sortie standard
read()                   // entrée standard

// Collections
len(collection)          // taille
push(list, item)         // ajouter
pop(list)                // retirer dernier

// Mémoire
copy(sphere)             // copie explicite

// Math
abs(n)  sqrt(n)  pow(a, b)  min(a, b)  max(a, b)

// Probabilité
uniform(min, max)        // distribution uniforme
normal(mean, stddev)     // distribution gaussienne

// Quantique
H(q)                     // Hadamard
X(q)  Y(q)  Z(q)        // Pauli gates
CNOT(control, target)    // Controlled NOT
SWAP(q1, q2)             // Échange
T(q)  S(q)               // Phase gates
Rx(q, angle)  Ry(q, angle)  Rz(q, angle)  // Rotations

// Temps
now()                    // timestamp nanoseconde
```

Tout le reste (sin, cos, HTTP, file I/O, etc.) vit dans des **modules** écrits en 3.42 lui-même ou linkés depuis du C.

---

## 12. RETURN AVEC SPIN

```
// Retourne spin positif (implicite sans préfixe spin)
return data;             // équivalent à return + data;

// Retourne spin négatif (échec)
return - "fichier non trouvé";

// Retourne spin erreur (fatal)
return # "corruption détectée";

// Retourne spin neutre (rien)
return *;

// Le spin fait partie de la valeur retournée
// L'appelant le mesure avec ? comme d'habitude
```
