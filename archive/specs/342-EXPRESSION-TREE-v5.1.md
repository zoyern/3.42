# 3.42 — Arbre d'Expressions Symboliques v5.1
## "Chaque symbole est un nœud, `{}` est le conteneur universel"

---

## 0. L'IDÉE CENTRALE

Le langage 3.42 n'est **pas** une liste de keywords avec des règles spéciales.
C'est un **arbre de symboles combinables** où :

1. Chaque **symbole** est un nœud (node) avec une sémantique propre
2. Les symboles se **combinent** pour créer des actions plus complexes (émergence)
3. `{}` est le **conteneur universel** — attachable à n'importe quel nœud
4. `()` est le **groupe** — pour les paramètres et l'ordre des opérations
5. Le texte (lettres) existe **uniquement** pour ce que l'humain nomme

C'est comme assembler des mots : chaque symbole est un mot, les combinaisons forment des phrases, les phrases forment des paragraphes.

---

## 1. ATOMES SYMBOLIQUES (les briques de base)

### Direction

| Symbole | Nom | Sémantique |
|---|---|---|
| `<` | back (retour) | Direction arrière, vers le caller |
| `>` | forward (avant) | Direction avant, transformation, cast |

### Mesure & Accès

| Symbole | Nom | Sémantique |
|---|---|---|
| `.` | access (accès) | Toucher sans consommer, lecture faible |
| `?` | measure (mesure) | Observer/effondrer, branchement |
| `!` | not (négation) | Inverser |

### Flux

| Symbole | Nom | Sémantique |
|---|---|---|
| `\|` | pipe (tuyau) | Connecter SPSC, flux séquentiel émergent |
| `~` | wave (onde) | Vibration, mutation, diffusion |

### Liaison

| Symbole | Nom | Sémantique |
|---|---|---|
| `:` | bind (lier) | Type annotation, clé→valeur, pattern→action |
| `=` | assign (nommer) | Donner un nom à une sphère |
| `;` | end (fin) | Terminer une instruction |

### Spins (états fondamentaux)

| Symbole | Nom | Alias sucre | Sémantique |
|---|---|---|---|
| `+` | positive | `true` | Succès, vrai, valide |
| `-` | negative | `false` | Échec, faux, moved |
| `*` | neutral | `void` | Vide, nil, superposition, défaut |
| `#` | error | `err` | Erreur, corruption, décohérence |
| `_` | wildcard | `any` | Attrape-tout |

### Conteneurs

| Symbole | Nom | Sémantique |
|---|---|---|
| `{}` | body (corps) | Contexte, scope, bloc d'exécution, arène |
| `()` | group (groupe) | Paramètres, groupement, génériques |
| `[]` | collection | Liste, map, index, slice |

### Mémoire

| Symbole | Nom | Sémantique |
|---|---|---|
| `@` | borrow-read (emprunt lecture) | Référence sans propriété |
| `@@` | borrow-write (emprunt écriture) | Référence exclusive |

---

## 2. COMBINAISONS (émergence de profondeur 2)

La combinaison de deux atomes crée un **nouveau sens** — ce n'est pas arbitraire, c'est **émergent** de la sémantique des parties.

### Direction × Direction

| Combinaison | Décomposition | Sens émergent |
|---|---|---|
| `<<` | back + back | **Return** — reculer deux fois = retourner au caller |
| `>>` | forward + forward | **Break** — avancer deux fois = sortir de la boucle |

Logique : `<` = un pas en arrière. `<<` = deux pas = retour complet au point d'appel.
`>` = un pas en avant. `>>` = deux pas = sauter hors du contexte actuel.

### Bind × Direction

| Combinaison | Décomposition | Sens émergent |
|---|---|---|
| `:>` | bind + forward | **Cast** — lier un type en avançant = transformer |
| `::` | bind + bind | **Path** — double liaison = chemin dans un namespace |

Logique : `:` = relier. `>` = vers l'avant. `:>` = relier vers un nouveau type.
`::` = double liaison = naviguer dans une structure liée.

### Accès × Mesure

| Combinaison | Décomposition | Sens émergent |
|---|---|---|
| `.?` | access + measure | **Introspect** — accéder puis mesurer = vérifier l'état |
| `??` | measure + measure | **Continuous** — double mesure = observer en boucle |

### Spin × Spin

| Combinaison | Décomposition | Sens émergent |
|---|---|---|
| `**` | neutral + neutral | **Copy** — doubler le neutre = dupliquer |
| `++` | positive + positive | **Concat** — doubler le positif = accumuler/joindre |

Logique : `*` = neutre, rien. `**` = doubler le rien = créer une copie (passer de 1 à 2).
`+` = positif, ajouter. `++` = doubler l'ajout = concaténer.

### Wave

| Combinaison | Décomposition | Sens émergent |
|---|---|---|
| `~` (préfixe) | wave + assign | **Mut** — onde devant assignation = variable qui vibre (mutable) |
| `~` (infixe) | expr wave expr | **Broadcast** — onde entre deux = diffusion |

---

## 3. `{}` — LE CONTENEUR UNIVERSEL

### Principe

`{}` est un **corps** (body) qui peut se combiner avec **n'importe quel symbole ou expression**. C'est un conteneur d'instructions qui retourne la valeur de sa dernière expression.

`{}` est une expression à part entière. Il peut apparaître partout où une expression est attendue.

### Combinaisons avec `{}`

| Expression | Sens | Exemple |
|---|---|---|
| `? {}` | Mesurer avec corps de branches | `x ? { + : a; - : b; };` |
| `?? {}` | Mesure continue avec corps | `stream ?? { + val? : process(val); };` |
| `<< {}` | Retourner le résultat d'un bloc | `<< { compute(); transform(); };` |
| `<< + {}` | Retourner spin+ avec bloc | `<< + { heavy_compute() };` |
| `** {}` | Copier le résultat d'un bloc | `clone = ** { build_complex_thing() };` |
| `:: name {}` | Déclarer un module | `::math { pi = 3.14159; };` |
| `x = {}` | Assigner le résultat d'un bloc | `result = { a + b };` |
| `\| {}` | Piper vers un bloc | `data \| { process(it) };` |
| `~ x = {}` | Déclarer mutable avec bloc | `~ state = { init_complex() };` |

### Le bloc est une expression

```
// Un bloc retourne sa dernière expression
result = {
    a = compute();
    b = transform(a);
    a + b                 // ← dernière expression = valeur retournée
};

// Utilisable partout où une expression est attendue
total = (x * {
    factor = get_factor();
    factor + 1
}) + offset;

// Dans un pipe
data | { filter(it) } | { transform(it) } | output;

// Dans un return multi-spin
<< + {
    validated = validate(input);
    process(validated)
};
```

### Pourquoi c'est puissant

Le `{}` comme expression universelle signifie que **n'importe quelle opération atomique peut devenir complexe** sans changer sa sémantique. C'est l'émergence : une action simple (`<< + x;`) et une action complexe (`<< + { ... };`) utilisent la même syntaxe, la même grammaire, le même AST node.

---

## 4. ARBRE DE BRANCHEMENT INFINI

### Le langage est un arbre

```
                    Programme
                       │
              ┌────────┼────────┐
              │        │        │
           Statement Statement Statement
              │        │        │
         ┌────┤   ┌────┤   ┌────┤
         │    │   │    │   │    │
       Symbole Body Symbole Body ...
         │         │
    ┌────┤    ┌────┤
    │    │    │    │
   Atom Atom Atom Atom
```

Chaque nœud est un symbole. Chaque symbole peut avoir :
- Des **enfants** (sous-expressions)
- Un **corps** `{}` (contexte d'exécution)
- Des **spins** (état de sortie)

### Pas de crash — par design

Pourquoi le branchement infini ne peut pas crasher :

1. **Chaque nœud a un spin** — même en cas d'erreur, le nœud retourne `#` (pas d'exception non-gérée)
2. **`?` force l'exhaustivité** — le caller DOIT gérer tous les spins possibles (ou `_`)
3. **Les types linéaires empêchent les fuites** — move by default, pas de dangling references
4. **L'arène mémoire** — chaque `{}` peut être une arène isolée, libérée en sortie
5. **La propagation automatique** — un spin non-géré remonte naturellement au parent

```
// Si compute() retourne #, ça propage automatiquement
result = compute();
// ↑ continue seulement si compute retourne +
// si - ou # → propage au caller sans crash

// Explicitement :
compute() ? {
    + val? : use(val);
    - : << - "échec de compute";    // propagation explicite
    # err? : << # err;               // propagation explicite
    * : << *;                         // propagation void
};
```

---

## 5. SUCRE SYNTAXIQUE (couche optionnelle)

### Principe : le sucre est un ALIAS, pas une feature

Le sucre syntaxique est une **couche configurable** au-dessus de la grammaire pure. Un développeur peut choisir :
- **Mode pur** : uniquement des symboles
- **Mode sucré** (défaut) : symboles + alias lisibles
- **Mode custom** : définir ses propres alias

### Table des alias par défaut

| Symbole (base) | Alias (sucre) | Contexte |
|---|---|---|
| `+` | `true` | Valeur booléenne |
| `-` | `false` | Valeur booléenne |
| `*` | `void` | Valeur vide |
| `#` | `err` | Valeur erreur |
| `_` | `any` | Pattern wildcard |
| `<<` | `return` | Retour de fonction |
| `>>` | `break` | Sortie de boucle |
| `~` | `mut` | Déclaration mutable |
| `**` | `clone` | Copie profonde |
| `:>` | `as` | Cast de type |

### Exemples comparatifs

```
// MODE PUR (symboles uniquement)
::math {
    divide = (a: f64, b: f64) : +f64 | -string {
        (b == 0.0) ? { + : << - "div/0"; };
        << + a / b;
    };
};

// MODE SUCRÉ (avec alias)
::math {
    divide = (a: f64, b: f64) : +f64 | -string {
        (b == 0.0) ? { true : return false "div/0"; };
        return true a / b;
    };
};
```

### Implémentation

Le sucre est géré par un **preprocesseur** (désucrage / desugaring) qui :
1. Parse le fichier source
2. Remplace les alias par les symboles correspondants
3. Passe le résultat au parser principal (grammaire pure)

Le preprocesseur est **lui-même un module** (`core::sugar`) — modifiable et extensible.

---

## 6. COMBINAISONS FUTURES (profondeur N)

### Le principe d'extension

Les utilisateurs de demain pourront **définir de nouvelles combinaisons** de symboles. Le langage évolue par **adoption**, pas par obligation.

```
// Exemple : un utilisateur définit une nouvelle combinaison
// ?~ = mesure + broadcast = "observer et diffuser le résultat"
@extend core::operators {
    (?~) = (source, handlers: [handler]) {
        result = source ?;
        result ~ handlers;
    };
};

// Utilisable ensuite :
sensor_data ?~ [logger, display, storage];
```

### Règles d'extension

1. **Pas de conflit** : une nouvelle combinaison ne peut pas écraser une existante
2. **Adoption** : la combinaison est disponible dans le registre global
3. **Portabilité** : elle est définie en termes de combinaisons existantes (pas de magie)
4. **Réversibilité** : comme un diff git — si ça casse quelque chose, rollback

### Profondeur 3+

```
// Profondeur 1 : atome
x ?

// Profondeur 2 : combinaison
x ? {}

// Profondeur 3 : combinaison dans combinaison
x ? { + Data(val?) : val | transform | output; }

// Profondeur 4 : imbrication
x ? {
    + Data(val?) (val > 0) : {
        val | validate | (v) { v ? {
            + : << + v;
            - err? : << - err;
        }};
    };
};

// Profondeur N : pas de limite, pas de crash
// Chaque niveau est un nœud dans l'arbre
// Chaque nœud a un spin de sortie
// L'arbre se résout de bas en haut
```

---

## 7. PORTES FFI (Foreign Function Interface)

### Concept : les portes

Une **porte** (gate) est un module qui traduit les symboles 3.42 vers un autre langage. C'est un adaptateur bidirectionnel.

```
┌──────────┐    ┌──────────┐    ┌──────────┐
│  3.42    │───>│  PORTE   │───>│  C/Rust  │
│ symbols  │<───│  gate    │<───│  code    │
└──────────┘    └──────────┘    └──────────┘
```

### Table de mapping

| 3.42 | C | Python | Rust | JS |
|---|---|---|---|---|
| `<<` | `return` | `return` | `return` | `return` |
| `>>` | `break` | `break` | `break` | `break` |
| `~` | mutable var | (default) | `let mut` | `let` |
| `?` | `switch` | `match` | `match` | `switch` |
| `??` | `while` | `while` | `loop` | `while` |
| `\|` | pipe (custom) | `\|` (shell) | `.pipe()` | `.pipe()` |
| `+`/`-` | `1`/`0` | `True`/`False` | `true`/`false` | `true`/`false` |
| `**` | `memcpy` | `copy.deepcopy` | `.clone()` | `structuredClone` |
| `:>` | `(type)` cast | `type()` | `as` | `as` |
| `@`/`@@` | `const*/` `*` | N/A | `&`/`&mut` | N/A |
| `::` | `_` (naming) | `.` | `::` | `.` |

### Auto-génération de portes

```
// 3.42 → C header (automatique)
// Source 3.42 :
::math {
    add = (a: i32, b: i32) : +i32 { << + a + b; };
};

// Porte C générée :
// math.h
int32_t math_add(int32_t a, int32_t b);

// math.c
int32_t math_add(int32_t a, int32_t b) {
    return a + b;
}
```

```
// 3.42 → Python binding (automatique)
# math_342.py
def add(a: int, b: int) -> int:
    return a + b
```

### Porte inverse (C → 3.42)

```
// Importer du C dans 3.42
@gate("c", "libpng.h")
::png {
    // Les fonctions C sont disponibles comme symboles 3.42
    // Les types C sont mappés aux types 3.42
    // Le spin de retour est déduit :
    //   - pointeur NULL → spin -
    //   - valeur valide → spin +
    //   - errno set → spin #
};

// Utilisation :
image = png::read("photo.png") ? {
    + img? : process(img);
    - : << - "fichier non trouvé";
    # err? : << # "erreur PNG: {err}";
};
```

### Sécurité des portes

1. **Sandboxing** : le code étranger tourne dans une arène isolée
2. **Type checking** : les types sont vérifiés aux frontières de la porte
3. **Spin wrapping** : tout retour étranger est wrappé dans un spin (pas de crash non-géré)
4. **Permissions** : une porte déclare ses besoins (`@gate` spécifie les permissions)

---

## 8. RÉSOLUTION DES INCOHÉRENCES DU BLUEPRINT

### Principe : tout est un branchement (comme git)

L'émergence résout les contradictions en les transformant en **branches parallèles** qui coexistent. Pas de choix binaire — les branches divergent et convergent naturellement.

```
     main (état initial)
       │
       ├── branch: contrat signé (immuable)
       │     │
       │     ├── branch: exécution normale
       │     └── branch: revert (si les deux parties acceptent)
       │
       ├── branch: création libre (pas de limite)
       │     │
       │     ├── branch: accumulation passive (coût croissant)
       │     └── branch: création active (pas de plafond)
       │
       └── branch: token de base (garanti)
             │
             ├── branch: mérite (tokens supplémentaires)
             └── branch: plafond (friction progressive, pas mur)
```

### Contradiction 1 : Undo universel vs contrats

**Avant** : "undo universel" contredit "contrats permanents"

**Résolution émergente** : tout est un **branchement**, pas un effacement.

- `undo` = créer un **revert branch** (comme `git revert`, pas `git reset --hard`)
- Le contrat original **existe toujours** sur sa branche (immuable)
- Le revert est une **nouvelle branche** qui annule les effets
- Pour un contrat multi-parties : revert nécessite un **merge** (accord des parties)
- Si une partie refuse → le revert branch existe mais n'est pas mergé → la branche originale prévaut

```
contrat_v1 (signé par A et B)
    │
    ├── exécution normale...
    │
    └── revert_request (par A)
          │
          ├── B accepte → merge → contrat_annulé
          └── B refuse → revert reste en attente (pas de force)
```

**Pas de contradiction** : l'undo existe toujours (on peut toujours PROPOSER un revert). Mais il n'est pas forcé — il nécessite le consensus des parties concernées. C'est émergent.

### Contradiction 2 : Liberté de création vs plafond

**Avant** : "pas de limite" contredit "plafond raisonnable"

**Résolution émergente** : pas de mur, mais une **friction progressive** (comme la gravité).

- Créer = gratuit, illimité (pas de plafond sur la création)
- Accumuler PASSIVEMENT = coût croissant (comme la résistance de l'air)
- La friction **émerge** de la structure, elle n'est pas imposée par un admin

```
// Modèle de friction
coût_de_holding(tokens) = {
    (tokens <= baseline) : 0;           // base gratuite
    (tokens <= 10x) : tokens * 0.001;   // friction légère
    (tokens <= 100x) : tokens * 0.01;   // friction modérée
    (tokens > 100x) : tokens * 0.1;     // friction forte
};

// La friction est redistribuée au pool commun
// Résultat : accumulation possible mais naturellement limitée
// Comme la gravité : on PEUT monter haut, ça coûte juste plus d'énergie
```

**Pas de mur** : un créateur brillant PEUT accumuler beaucoup — mais le coût de maintenance augmente. Ce n'est pas un plafond, c'est de la physique économique émergente.

### Contradiction 3 : 1 humain = 1 token vs méritocratie

**Résolution émergente** : **branches parallèles** qui coexistent.

```
humain_vérifié
    │
    ├── branche "base" : 1 token garanti (revenu universel)
    │     → Inaliénable, non-transférable
    │     → Suffisant pour les besoins de base
    │
    └── branche "mérite" : tokens supplémentaires
          → Gagnés par contribution
          → Soumis à la friction progressive
          → Transférables, échangeables
```

Les deux branches coexistent. La base est un **filet de sécurité** (pas une égalité forcée). Le mérite est un **incitatif** (pas une inégalité débridée). La friction empêche les extrêmes sans mur dur.

### Contradiction 4 : Dépendance circulaire IA ↔ Blockchain ↔ Personhood

**Résolution émergente** : **bootstrap progressif** (comme git init → premiers commits → repo complet).

```
Phase 0 : "git init" — rien n'existe
    │
    └── Phase 1 : Blockchain minimale
          │   (hash chain simple, proof-of-work, pas de personhood)
          │   (comme le premier commit d'un repo)
          │
          └── Phase 2 : IA basique
                │   (détection de patterns simples, locale)
                │   (utilise la blockchain Phase 1 pour stocker)
                │
                └── Phase 3 : Personhood v1
                      │   (social graph + IA Phase 2 pour vérifier)
                      │   (enregistré sur blockchain Phase 1)
                      │
                      └── Phase 4 : Migration
                            │   (blockchain upgradée avec personhood)
                            │   (IA upgradée avec plus de données)
                            │   (personhood upgradée avec meilleure IA)
                            │
                            └── ... (boucle d'amélioration infinie)
```

Chaque phase est un **commit** qui s'appuie sur les précédents. La dépendance circulaire est résolue en la transformant en **spirale ascendante**. Chaque tour de spirale améliore les trois composants simultanément.

### Contradiction 5 : Ternaire irréversible vs modularité

**Résolution honnête** : le ternaire EST irréversible. C'est le **format interne** (comme le format `.git/objects/`).

```
┌─────────────────────────────┐
│     Couche d'abstraction    │  ← Évolue librement
│  (types, opérateurs, API)   │
├─────────────────────────────┤
│     Couche de stockage      │  ← Évolue avec précaution
│  (sérialisation, hash)      │
├─────────────────────────────┤
│     Couche fondamentale     │  ← IRRÉVERSIBLE
│  (trit, tryte, arène)       │
└─────────────────────────────┘
```

- La couche fondamentale ne change PAS (c'est le "hardware" logiciel)
- La couche d'abstraction évolue librement (c'est le "software")
- Si un jour le ternaire est obsolète → la couche d'abstraction peut émuler un autre système PAR-DESSUS le ternaire (comme Docker émule un OS sur un autre)

### Failles de sécurité — Solutions émergentes

| Faille | Solution émergente |
|---|---|
| IA point unique de défaillance | Multi-IA : plusieurs IA indépendantes votent. Résultat = consensus. Si désaccord → appel humain. Comme un merge conflict git → résolution manuelle. |
| Crypto non spécifiée | SPHINCS+ (signatures, post-quantique NIST 2024). Kyber (échange de clés). Hash → SHAKE-256 sur base ternaire. Tout est un module → migration = changer de branche. |
| Révocation sur réseau décentralisé | Chiffrement avec clé révocable. Révoquer = publier la clé de déchiffrement, puis re-chiffrer avec nouvelle clé. Les copies anciennes deviennent illisibles. Best-effort, pas 100%. |
| Hash27 pas quantum-resistant | Hash27 = hash interne court pour le content-addressing local. Hash long (SHAKE-256 based) pour la blockchain et la sécurité. Deux niveaux de hash. |
| Identité vs analyse réseau | Transport obligatoire via mixnet (comme Tor mais intégré au kernel réseau). Chaque paquet passe par 3+ nœuds avant destination. Coût : latence ~50ms de plus. |

---

## 9. VISUALISATION UNIFIÉE (le git de tout)

### Principe

Tout dans 3.42 est visualisable comme un **arbre git** : code, libs, contrats, économie, identité.

```
// Code = branches de développement
::mon_module {
    v1.0 ──── v1.1 ──── v1.2
                 │
                 └── v2.0-beta ──── v2.0
};

// Libs = branches dans le registre global
registre {
    math::sort     ──── diff ──── diff ──── ...
    math::search   ──── diff ──── ...
    network::http  ──── diff ──── diff ──── diff ──── ...
};

// Contrats = branches avec merge points
contrat_AB {
    proposition ──── signature_A ──── signature_B ──── exécution
                                                        │
                                                   revert_request
                                                        │
                                                   ├── accepté → annulé
                                                   └── refusé → continue
};

// Économie = branches de tokens
token_pool {
    distribution ──── base(1/humain) ──── mérite(contribution)
                                              │
                                         friction(holding_cost)
};
```

### API de visualisation

```
// Voir l'historique d'un module (comme git log)
::mon_module.history() ?? {
    + commit? : display(commit);
};

// Voir les branches actives
::mon_module.branches() | display;

// Voir les diffs entre deux versions
::mon_module.diff("v1.0", "v2.0") | display;

// Merger une branche
::mon_module.merge("v2.0-beta") ? {
    + : << + "merge réussi";
    # conflict? : resolve(conflict);   // comme git merge conflict
};
```

---

## 10. CHANGEMENTS PEG v5.0 → v5.1

### 1. Block comme PrimaryExpr

```peg
# v5.1 : Block ajouté comme expression primaire
# Un bloc retourne la valeur de sa dernière expression
PrimaryExpr      <- BlockExpr          # NOUVEAU — {} comme expression
                  / GroupExpr
                  / FunctionLiteral
                  / ListLiteral
                  / MapLiteral
                  / RangeLiteral
                  / ProbLiteral
                  / NumberLiteral
                  / StringLiteral
                  / StructLiteral
                  / ModulePath
                  / SpinLiteral
                  / Identifier

# v5.1 NOUVEAU — bloc comme expression (retourne la dernière expression)
BlockExpr        <- '{' Spacing (Statement Spacing)* Expression? Spacing '}'
```

### 2. Sucre syntaxique dans SpinLiteral

```peg
# v5.1 : alias optionnels (sucre syntaxique)
# Le preprocesseur les convertit en symboles, mais le parser les accepte aussi
SpinLiteral      <- '+' !'+'
                  / '-' !'-'
                  / '*' !'*'
                  / '#'
                  / 'true' ![a-zA-Z0-9_]     # sucre pour +
                  / 'false' ![a-zA-Z0-9_]    # sucre pour -
                  / 'void' ![a-zA-Z0-9_]     # sucre pour *

# Keyword étendu pour inclure le sucre (évite qu'ils soient des Identifiers)
Keyword          <- ('type' / 'trait' / 'true' / 'false' / 'void') ![a-zA-Z0-9_]
```

Note : `err` n'est PAS un keyword — `#` suffit et `err` reste disponible comme nom de variable. Seuls `true`, `false`, `void` sont du sucre car ce sont les plus fréquents.

### 3. ReturnStmt accepte un BlockExpr

```peg
# Déjà géré car Expression inclut PrimaryExpr qui inclut BlockExpr
# << { compute(); result }; → fonctionne nativement
# << + { heavy_compute() }; → fonctionne nativement
# Pas de changement PEG nécessaire — c'est déjà émergent !
```

---

## RÉSUMÉ DES INVARIANTS

1. **Chaque symbole est un nœud** dans un arbre d'expressions
2. **`{}` est combinable avec tout** — c'est un conteneur universel qui retourne sa dernière expression
3. **Les combinaisons sont émergentes** — `<<` = back+back = return, pas un mot-clé arbitraire
4. **Le sucre est optionnel** — `true`/`false`/`void` sont des alias pour `+`/`-`/`*`
5. **Les contradictions du blueprint sont des branches**, pas des murs — résolubles par consensus émergent
6. **Les portes FFI** mappent les symboles vers n'importe quel langage cible
7. **Tout est visualisable comme git** — code, libs, contrats, économie, identité
8. **Pas de crash possible** — chaque nœud a un spin, la propagation est automatique
9. **Le langage évolue** — les utilisateurs définissent de nouvelles combinaisons, adoption par consensus
