# 3.42 — Paradigme Symbol-Only v5.0
## "Le système parle en symboles, l'humain nomme ce qu'il crée"

---

## 0. PRINCIPE FONDATEUR

**Séparation totale** :
- Le **système** (langage, opérateurs, contrôle de flux) = **symboles uniquement**
- Le **développeur** (variables, types custom, modules) = **noms libres**
- Le **texte** n'existe dans la syntaxe QUE pour ce que l'humain nomme

**Pourquoi ?**
- Un mathématicien voit `f = (x) { << + x * 2; };` et comprend sans connaître l'anglais
- Un physicien écrit `ψ = *(); ψ ? { + : mesure_up; - : mesure_down; };` — c'est SA notation
- Les symboles se combinent comme des compétences : `.?` = accéder + interroger = introspection
- Combinaison à profondeur N : plus on combine, plus l'action est complexe, sans mot-clé

---

## 1. TABLE DE CONVERSION v4.1 → v5.0

| v4.1 (keyword) | v5.0 (symbole) | Mnémonique | Conflit ? | Résolution |
|---|---|---|---|---|
| `module name {}` | `::name {}` | `::` = namespace, déjà utilisé pour les chemins | Aucun | `::` en début de statement = déclaration module. `a::b` en expression = chemin. |
| `return expr;` | `<< expr;` | `<<` = "pousser vers la sortie" (flèche gauche) | `<<` = shift left | **Contexte** : début de statement = return. Entre expressions = shift. PEG ordered choice résout. |
| `return + expr;` | `<< + expr;` | `<<` + spin = return avec spin | Idem | Idem |
| `break;` | `>>;` | `>>` = "sortir vers la droite" (flèche droite) | `>>` = shift right | **Contexte** : début de statement = break. Entre expressions = shift. |
| `mut x = 5;` | `~ x = 5;` | `~` = "vibration, changement" | `~` = broadcast SPMC | **Contexte** : `~` en début de statement suivi de `ident =` → déclaration mutable. `expr ~ expr` en expression → broadcast. |
| `copy(expr)` | `** expr` | `**` = "dupliquer" (double étoile) | Aucun | Nouveau opérateur préfixe. `*` seul ≠ `**`. |
| `as` | `:>` | `:` (type) + `>` (transformer vers) | Aucun | Nouveau symbole composé. `x :> i32` = cast. |
| `true` | `+` (SpinLiteral) | `+` = positif = vrai | `+` = addition, unaire | **Contexte** : en position primaire sans opérande suivant = SpinLiteral. Sinon = opérateur. |
| `false` | `-` (SpinLiteral) | `-` = négatif = faux | `-` = soustraction, unaire | Idem. |
| `qubit()` | `quantum::new()` | Plus un keyword — c'est un type dans le module quantum | — | `qubit` supprimé des keywords. Devient un type standard. |
| `type` | `type` | **CONSERVÉ** — concept mathématique universel | — | 1 des 2 seuls mots restants. |
| `trait` | `trait` | **CONSERVÉ** — concept mathématique (propriété/contrat) | — | 1 des 2 seuls mots restants. |

**Résultat : 11 keywords → 2 (`type`, `trait`)**

---

## 2. DÉFINITION DE `*` SEUL

### Le système de valeurs spin

| Symbole seul | Signification | Équivalent classique | Usage |
|---|---|---|---|
| `+` | spin positif | `true`, `Some`, `Ok` | Succès, vrai, valide |
| `-` | spin négatif | `false`, `None`, `Err` | Échec, faux, moved |
| `*` | spin neutre | `null`, `void`, `None`, `nil` | Vide, non-initialisé, superposition, default |
| `#` | spin erreur | `Error`, `panic`, `throw` | Erreur critique, corruption, décohérence |

### `*` seul = la valeur neutre

`*` est le **zéro du système de spin**. C'est l'absence de valeur, l'état par défaut, le vide.

```
// Déclarer une variable sans valeur (neutre/void)
x = *;

// Retourner rien (void return)
<< *;

// Tester si quelque chose est neutre
x ? {
    + val? : utiliser(val);    // x a une valeur positive
    * : skip();                // x est neutre/vide
    _ : erreur();              // autre
};

// Valeur par défaut dans un type
type Option(T) = + T | - | *;

// * dans une liste = placeholder
items = [1, *, 3, *, 5];  // trous neutres
```

### `*` vs `**` — deux concepts séparés

| Symbole | Catégorie | Signification | Exemple |
|---|---|---|---|
| `*` seul | SpinLiteral (valeur) | Neutre, void, nil | `x = *;` |
| `** expr` | CopyExpr (opérateur préfixe) | Deep copy (clone profond) | `y = ** x;` |
| `a * b` | MulOp (opérateur binaire) | Multiplication | `c = a * b;` |
| `* Leaf` | SpinExpr (constructeur variant) | Variant avec spin neutre | `node = * Leaf;` |

### Comment le parser distingue

Le PEG utilise l'**ordered choice** (choix ordonné) et le **contexte de position** :

1. `** x` → CopyExpr (essayé en premier car `'**'` est plus long)
2. `* Leaf` → SpinExpr (essayé après, car `*` suivi d'expression = constructeur variant)
3. `a * b` → MulOp (entre deux expressions au niveau MultiplicativeExpr)
4. `*;` ou `*, ` ou `*)` → SpinLiteral (aucune des alternatives ci-dessus ne match car `*` n'est suivi d'aucune expression)

**Zéro ambiguïté. Le PEG est déterministe.**

---

## 3. MULTI-SPIN RETURN

### Concept

Une fonction peut retourner avec **différents spins** selon le chemin d'exécution. Le type de retour déclare les spins possibles. L'appelant **mesure** le résultat pour gérer chaque cas.

### Syntaxe

```
// Fonction qui peut retourner 3 spins différents
validate = (data: string) : +Parsed | -string | #string {
    data.empty() ? {
        + : << # "données vides";       // erreur critique
    };

    result = parser::parse(data) ? {
        + parsed? : parsed;
        - err? : << - "parse échoué: {err}";  // échec
    };

    << + result;                                // succès
};

// L'appelant mesure le résultat
validate("hello") ? {
    + parsed? : utiliser(parsed);        // spin positif = succès
    - msg? : log::warn(msg);             // spin négatif = échec
    # err? : panic(err);                 // spin erreur = critique
};
```

### Règles

1. **`<<` prend un spin optionnel** : `<< + expr;`, `<< - expr;`, `<< * ;`, `<< # expr;`
2. **Sans spin** : `<< expr;` → spin déduit du contexte (+ par défaut)
3. **Le premier `<<` atteint sort la fonction** — PAS d'exécution multi-path
4. **Le type retour déclare les spins possibles** : `+T | -E | #C`
5. **Le `?` de l'appelant DOIT être exhaustif** — tous les spins déclarés doivent être gérés (ou `_` wildcard)
6. **Sans `?`** : `result = validate("x");` → spin + implicite. Si la fonction retourne - ou #, **propagation automatique** (comme `?` en Rust)

### Propagation automatique

```
// Si validate retourne -, le - se propage automatiquement au caller
process = (data: string) : +Result | -string | #string {
    parsed = validate(data);     // si - ou # → propage immédiatement
    transform(parsed);           // exécuté seulement si validate retourne +
    << + transform(parsed);
};
```

C'est l'équivalent du `?` operator de Rust, mais **implicite et universel**. Pas besoin d'opérateur supplémentaire — la nature spin du retour fait le travail.

---

## 4. COMBINAISONS SYMBOLIQUES ÉMERGENTES

### Principe : chaque symbole est une compétence

Les symboles se combinent comme des compétences humaines. Plus on combine, plus l'action est précise et puissante.

| Combinaison | Décomposition | Signification émergente |
|---|---|---|
| `.?` | accès `.` + mesure `?` | Introspection (vérifier état sans consommer) |
| `.??` | accès `.` + mesure continue `??` | Monitoring (observer en continu) |
| `\|?` | pipe `\|` + mesure `?` | Filtrer un flux (pipe conditionnel) |
| `~?` | broadcast `~` + mesure `?` | Diffuser et vérifier réception |
| `<<+` | return `<<` + spin `+` | Retourner avec succès |
| `<<-` | return `<<` + spin `-` | Retourner avec échec |
| `<<#` | return `<<` + spin `#` | Retourner erreur critique |
| `<<*` | return `<<` + spin `*` | Retourner void/rien |
| `~x=` | mut `~` + binding | Déclarer mutable |
| `**` | étoile + étoile | Dupliquer (copie profonde) |
| `:>` | type `:` + direction `>` | Cast (transformer type) |
| `::` | binding `:` + binding `:` | Chemin dans namespace |
| `::{}` | namespace + contexte | Déclarer un module |
| `@` | méta (prefix) / emprunt lecture (postfix) | Selon position |
| `@@` | double emprunt | Emprunt écriture exclusif |

### Profondeur N

```
// Profondeur 1 : mesure simple
x ?;

// Profondeur 2 : mesure avec branchement
x ? { + : a; - : b; };

// Profondeur 3 : mesure + destructure + filtre
x ? { + Data(val?) (val > 0) : process(val); };

// Profondeur 4 : pipe + mesure + destructure + filtre
source | transform | (x) { x ? { + Valid(v?) (v.len() > 0) : << + v; }; };
```

---

## 5. ÉCOSYSTÈME "TOUT EST LIB"

### Principe fondateur

> Tout code créé dans 3.42 est automatiquement une bibliothèque réutilisable.
> Le langage lui-même est une bibliothèque modifiable.

### Architecture

```
┌─────────────────────────────────────────────┐
│                 REGISTRE GLOBAL              │
│  (blockchain décentralisée, content-addressed)│
├─────────────────────────────────────────────┤
│  Chaque module = un nœud dans le registre    │
│  Hash du contenu = identité unique           │
│  Auteur = signature cryptographique          │
│  Dépendances = liens vers autres hashes      │
│  Historique = chaîne de diffs                │
└─────────────────────────────────────────────┘
```

### Cycle de vie d'un module

```
1. CRÉATION
   ::math_utils {
       fibonacci = (n: i32) : i32 { ... };
   };
   → Hash calculé automatiquement
   → Signature de l'auteur ajoutée
   → Publié dans le registre local

2. PUBLICATION
   $ 342 publish math_utils
   → Vérifie que le hash n'existe pas déjà
   → Si code identique existe → lien vers l'existant (déduplication)
   → Si nouveau → ajout au registre global
   → Certification blockchain : auteur + timestamp + hash

3. UTILISATION
   // Un autre développeur
   fib = math_utils::fibonacci;
   // Résolu par hash, pas par nom (content-addressed)
   // Version exacte garantie

4. MISE À JOUR (diff-based)
   → L'auteur modifie fibonacci (optimisation)
   → Diff calculé automatiquement
   → Nouvelle version = ancien hash + diff
   → Les utilisateurs de l'ancienne version NE SONT PAS affectés
   → Opt-in pour la mise à jour

5. AUTO-REVERT
   → Si un update casse les tests d'un dépendant :
     → Notification automatique
     → Revert proposé (pas forcé)
     → L'auteur de l'update est notifié du problème
```

### Le langage est une lib

```
// Le module `core` EST le langage
// Il contient : types primitifs, opérateurs, runtime

// Un utilisateur peut ÉTENDRE le core
@extend core::operators {
    // Ajouter un opérateur custom
    (<>) = (a: T, b: T) : T { ... };  // opérateur diamant
};

// Un utilisateur peut REMPLACER une implémentation
@extend core::i32 {
    // Remplacer l'addition pour i32 (dangereux mais possible)
    (+) = (self@, other: i32@) : i32 { ... };
};

// Le système trace tout :
// - Qui a modifié quoi
// - Quelle version du core est utilisée
// - Rollback possible à tout moment
```

### Déduplication globale

```
// Développeur A écrit :
sort = (list: [i32]) : [i32] { /* quicksort */ };

// Développeur B écrit (indépendamment) :
tri = (tableau: [i32]) : [i32] { /* quicksort identique */ };

// Le registre détecte :
// → hash(sort.body) == hash(tri.body)
// → Même implémentation, noms différents
// → Proposition : "Votre code est identique à sort de DevA. Lier ?"
// → Si oui : un seul nœud dans le registre, deux noms
// → Optimisations futures bénéficient aux deux
```

### Sécurité

- **Signature cryptographique** : chaque publication est signée par l'auteur
- **Immutabilité** : un hash publié ne change JAMAIS (les updates créent de nouveaux hashes)
- **Audit trail** : blockchain trace toutes les modifications
- **Sandboxing** : le code d'un module tourne dans son arène mémoire isolée
- **Permissions** : un module déclare ce dont il a besoin (`io`, `network`, `fs`)

---

## 6. VÉRIFICATION BLUEPRINT — RÉSULTATS

### Score de cohérence global : 42/100

### Contradictions critiques trouvées

**1. Undo universel vs permanence des contrats**
- PILIER 2 dit "toujours pouvoir annuler"
- Les smart contracts sont irréversibles une fois signés
- **Résolution proposée** : l'undo est universel pour les actions PERSONNELLES. Les contrats multi-parties nécessitent le consentement de TOUTES les parties pour un revert. C'est un undo CONDITIONNEL, pas absolu.

**2. Liberté de création vs plafond économique**
- "Pas de limite sur la création" + "plafond raisonnable"
- **Résolution proposée** : le plafond s'applique à l'accumulation PASSIVE (spéculation, rente), pas à la création ACTIVE. Un créateur qui produit continuellement n'est pas plafonné — c'est l'inactivité qui est taxée.

**3. 1 humain = 1 token vs méritocratie**
- Baseline égalitaire + récompenses au mérite = inégalité recréée
- **Résolution proposée** : le token de base est un MINIMUM garanti (comme un revenu universel). Le mérite ajoute par-dessus. Le plafond empêche les extrêmes. C'est un compromis explicite, pas une contradiction — mais il faut le documenter clairement.

**4. Ternaire = décision irréversible vs modularité**
- Si le ternaire s'avère non-optimal, migration "facile" est fausse
- **Résolution proposée** : accepter que le ternaire EST une décision irréversible. Le documenter comme tel. Ne pas prétendre que la migration est facile — elle ne l'est pas.

### Dépendance circulaire critique

```
IA → nécessite → Blockchain (pour les records immuables)
Blockchain → nécessite → Proof of Personhood (pour les poids de vote)
Proof of Personhood → nécessite → IA (pour détecter les comptes dupliqués)
```

**Résolution proposée** : bootstrap séquentiel.
- Phase 1 : Blockchain minimale avec proof-of-work classique (pas de personhood)
- Phase 2 : IA locale basique (détection de patterns simples)
- Phase 3 : Proof of Personhood avec IA + social graph
- Phase 4 : Migration vers blockchain with personhood

### Failles de sécurité identifiées

1. **IA comme point unique de défaillance** → ajouter arbitrage multi-parties + appel humain
2. **Crypto post-quantique non spécifiée** → fixer SPHINCS+ (signatures) + Kyber (échange clés)
3. **Révocation sur réseau décentralisé** → impossible à 100%, prévoir deletion best-effort + chiffrement avec clé révocable
4. **Hash27 pas résistant quantique** → prévoir migration vers hash plus long ou algorithme PQ
5. **Identité pseudonyme vs analyse réseau** → intégrer Tor/mixnet au niveau transport

---

## 7. PEG v5.0 — CHANGEMENTS PAR RAPPORT À v4.1

### Statements modifiés

```peg
# v4.1 → v5.0
ModuleDecl    <- '::' Spacing Identifier Spacing '{' Spacing (Statement Spacing)* '}'
#              (était: 'module' Spacing Identifier ...)

MutDecl       <- '~' Spacing Assignment
#              (était: 'mut' Spacing Assignment)

ReturnStmt    <- '<<' Spacing SpinReturn? Spacing Expression? Semicolon
#              (était: 'return' Spacing ...)

BreakStmt     <- '>>' Semicolon
#              (était: 'break' Semicolon)
```

### Expressions modifiées

```peg
# CopyExpr remplace copy()
CopyExpr      <- '**' Spacing UnaryExpr
#              (était: 'copy' '(' Spacing Expression Spacing ')')

# CastExpr utilise :> au lieu de 'as'
CastExpr      <- Spacing ':>' Spacing TypeExpr
#              (était: Spacing 'as' Spacing TypeExpr)

# SpinLiteral ajouté dans PrimaryExpr
SpinLiteral   <- '+' !'+'  / '-' !'-' / '*' !'*' / '#'
#              (remplace BoolLiteral: 'true' / 'false')

# QubitLiteral supprimé (qubit est un type, pas un keyword)
# Devient: quantum::new() ou un constructeur de type
```

### Opérateurs modifiés

```peg
# MulOp : éviter conflit avec ** (copy)
MulOp         <- '*' !'=' !'*' / '/' / '%'

# ShiftOp : INCHANGÉ (<<, >> restent des opérateurs shift en contexte expression)
ShiftOp       <- '<<' / '>>'
# La disambiguation return/shift est faite au niveau Statement vs Expression
```

### Keywords v5.0

```peg
Keyword       <- ('type' / 'trait') ![a-zA-Z0-9_]
#              (était: 11 keywords, maintenant 2)
```

---

## 8. EXEMPLES COMPARATIFS v4.1 → v5.0

### Hello World

```
// v4.1
module hello {
    main = () { print("Hello, 3.42!"); };
};

// v5.0
::hello {
    main = () { print("Hello, 3.42!"); };
};
```

### Variable mutable

```
// v4.1
mut counter = 0;
counter += 1;

// v5.0
~ counter = 0;
counter += 1;
```

### Fonction avec return typé

```
// v4.1
divide = (a: f64, b: f64) : f64 {
    (b == 0.0) ? { true : return - "division par zéro"; };
    return + a / b;
};

// v5.0
divide = (a: f64, b: f64) : +f64 | -string {
    (b == 0.0) ? { + : << - "division par zéro"; };
    << + a / b;
};
```

### Pattern matching avec destructuring

```
// v4.1
result ? {
    + Authenticated(session?) : handle(session);
    - Denied(reason?) : log(reason);
    # Error(msg?) : panic(msg);
    _ : fallback();
};

// v5.0 (IDENTIQUE — pas de changement dans le ? block)
result ? {
    + Authenticated(session?) : handle(session);
    - Denied(reason?) : log(reason);
    # Error(msg?) : panic(msg);
    _ : fallback();
};
```

### Copy et cast

```
// v4.1
original = Point { x: 1.0, y: 2.0 };
clone = copy(original);
value = number as f64;

// v5.0
original = Point { x: 1.0, y: 2.0 };
clone = ** original;
value = number :> f64;
```

### Module avec traits

```
// v4.1
module geometry {
    type Circle { radius: f64; };

    trait Area {
        area = () : f64;
    };

    Circle : Area {
        area = () : f64 { return + 3.14159 * self.radius * self.radius; };
    };
};

// v5.0
::geometry {
    type Circle { radius: f64; };

    trait Area {
        area = () : f64;
    };

    Circle : Area {
        area = () : f64 { << + 3.14159 * self.radius * self.radius; };
    };
};
```

### Booleans / Spins

```
// v4.1
active = true;
done = false;

// v5.0
active = +;
done = -;

// Conditionnel
active ? {
    + : run();       // si positif (vrai)
    - : skip();      // si négatif (faux)
    * : init();      // si neutre (non-initialisé)
};
```

### Pipe avec broadcast et shift

```
// v5.0 — pas de conflit entre << (return) et << (shift)
// << en début de statement = return
// << entre expressions = shift

bits_shifted = value << 3;           // shift left (expression)
<< + bits_shifted;                   // return (statement)

// >> pareil
extracted = data >> 8;               // shift right (expression)
>>;                                  // break (statement)
```

### Multi-spin return complet

```
// v5.0
http_request = (url: string) : +Response | -string | #string {
    connection = net::connect(url) ? {
        + conn? : conn;
        - err? : << - "connexion échouée: {err}";
        # timeout? : << # "timeout critique: {timeout}";
    };

    response = connection.send() ? {
        + resp? : resp;
        - err? : << - "envoi échoué: {err}";
    };

    << + response;
};

// Appelant — mesure exhaustive
http_request("https://api.example.com") ? {
    + resp? : process(resp);
    - msg? : retry(msg);
    # critical? : {
        log::error(critical);
        alert::admin(critical);
    };
};

// Appelant — propagation automatique (sans ?)
// Si http_request retourne - ou #, ça propage au caller automatiquement
data = http_request("https://api.example.com");
// ^ ne continue que si + ; sinon propage le spin au parent
```

---

## 9. ANALYSE DE CONFLITS EXHAUSTIVE

### `<<` : return vs shift left

| Contexte | Parse | Exemple |
|---|---|---|
| Début de statement | ReturnStmt | `<< + result;` |
| Entre expressions | ShiftOp | `x = a << 3;` |
| Après `=` | ShiftOp (dans expression) | `y = data << bits;` |
| Dans ArgList | ShiftOp (dans expression) | `foo(a << 2);` |

**Mécanisme PEG** : `Statement` essaie `ReturnStmt <- '<<' ...` AVANT `ExpressionStmt`. Si `<<` est en début de statement, ReturnStmt matche. Sinon, l'expression contient `<<` comme ShiftOp.

**Edge case** : `<< a << b;` → ReturnStmt(`<<`, no spin, Expression(`a << b`)). Retourne le résultat de `a << b`. Correct.

### `>>` : break vs shift right

| Contexte | Parse | Exemple |
|---|---|---|
| Début de statement + `;` | BreakStmt | `>>;` |
| Entre expressions | ShiftOp | `x = a >> 3;` |

**Mécanisme PEG** : `BreakStmt <- '>>' Semicolon`. Ne matche QUE si `>>` est suivi directement de `;`. `>> expr;` ne matche PAS BreakStmt → tombe dans ExpressionStmt. Aucune ambiguïté.

### `~` : mut vs broadcast

| Contexte | Parse | Exemple |
|---|---|---|
| `~ ident = expr;` | MutDecl | `~ x = 5;` |
| `~ ident : type = expr;` | MutDecl | `~ x: i32 = 5;` |
| `expr ~ expr` | BroadcastExpr | `source ~ handler;` |

**Mécanisme PEG** : `MutDecl <- '~' Spacing Assignment` essayé avant `ExpressionStmt`. Si le pattern `~ identifier (: type)? = expr;` matche → MutDecl. Sinon → expression broadcast.

**Edge case** : `~ x;` (pas d'assignation) → MutDecl échoue → ExpressionStmt → `~ x` → broadcast avec `~` en préfixe → échoue (broadcast est infixe). **Erreur de syntaxe** — comportement correct.

### `+` / `-` : spin literal vs opérateurs

| Contexte | Parse | Exemple |
|---|---|---|
| Position primaire, seul | SpinLiteral | `x = +;`, `[+, -, *]` |
| Préfixe + expression | UnaryOp | `x = + 5;` (= +5) |
| Infixe | AddOp | `x = a + b;` |
| Dans MeasureBlock | SpinPattern | `+ val? : action;` |

**Mécanisme PEG** : UnaryExpr essaie `UnaryOp Spacing UnaryExpr` d'abord. Si `+` est suivi d'une expression → UnaryOp. Si `+` est suivi de `;`, `,`, `)` → UnaryOp échoue → fallback → PrimaryExpr → SpinLiteral.

### `*` : neutre vs multiplication vs copy vs variant

| Contexte | Parse | Exemple |
|---|---|---|
| `** expr` | CopyExpr | `y = ** x;` |
| `* Ident` (spin context) | SpinExpr | `* Leaf` |
| `expr * expr` | MulOp | `c = a * b;` |
| Seul (position primaire) | SpinLiteral | `x = *;` |
| `*=` | CompoundAssign | `x *= 2;` |

**Mécanisme PEG** :
1. `'**'` testé en premier (CopyExpr) — consomme les deux `*`
2. `'*' !'=' !'*'` pour MulOp — ne matche pas `**` ni `*=`
3. `('*' !'=' / '#')` pour SpinExpr — matche `*` suivi d'une expression
4. SpinLiteral en dernier recours dans PrimaryExpr

**Zéro conflit.**

### `:>` : cast

| Contexte | Parse | Exemple |
|---|---|---|
| Postfixe après expression | CastExpr | `x :> i32` |

Pas de conflit : aucun autre opérateur ne commence par `:>`. Le `:` seul est utilisé pour type annotations et key:value, mais toujours suivi d'un espace + type/expression, jamais de `>`.

---

## 10. INVENTAIRE FINAL v5.0

### Symboles du système (ce que le langage utilise)

```
QUARKS (7) :  .  ?  ??  |  ~  {}  :
SPINS (5) :   +  -  *  #  _
GLUONS (8) :  =  ;  ()  []  ..  ::  @  @@
CONTRÔLE :    <<  >>
MÉMOIRE :     **  :>  ~
LOGIQUE :     &&  ||  !
BITS :        &  ^  <<  >>
COMPARE :     ==  !=  <  >  <=  >=
ARITHM :      +  -  *  /  %  ++
ASSIGN :      +=  -=  *=  /=  %=  &=  ^=  <<=  >>=
MOTS (2) :    type  trait
```

### Ce que le développeur nomme (texte libre)

```
- Variables : counter, data, result, ψ, α, 変数
- Types custom : Point, Circle, HttpResponse, 座標
- Modules : geometry, network, mon_module
- Traits custom : Drawable, Serializable
- Fonctions : calculate, transform, 計算
```

### Total

- **~25 symboles uniques** (vs 15 en v4.1)
- **2 mots réservés** (vs 11 en v4.1, vs 38 en C)
- **0 dépendance à l'anglais** pour le système
- **100% des mots** dans le code = choix du développeur

---

## 11. DÉCISION : POURQUOI C'EST LE BON CHOIX

### Arguments POUR

1. **Universalité** : un développeur japonais, français, ou arabe lit le même code système
2. **Émergence** : les symboles se combinent naturellement — pas besoin de mémoriser des mots
3. **Cohérence avec la vision** : "zéro-parsing" + "tout est sphère" → les symboles sont plus proches de l'AST que les mots
4. **Séparation nette** : système vs utilisateur — aucune confusion possible
5. **Compacité** : `<< + x;` est plus court que `return + x;` (6 vs 13 caractères)
6. **Combinatoire** : `.?`, `<<+`, `~x=` — chaque combinaison a un sens émergent

### Arguments CONTRE (et réponses)

1. **"C'est illisible comme APL"** → Non, car les symboles sont peu nombreux (~25), ont une sémantique claire, et le texte reste pour les noms. APL avait 100+ symboles sans logique combinatoire.
2. **"Courbe d'apprentissage"** → Identique à apprendre des keywords anglais pour un non-anglophone. Un tableau de 25 symboles se mémorise en 1 jour.
3. **"Les erreurs du compilateur seront cryptiques"** → Les messages d'erreur sont en texte naturel (dans la langue de l'utilisateur). Seul le CODE est en symboles.

---

## APPENDICE A : GRAMMAIRE PEG v5.0 COMPLÈTE

Voir fichier séparé : `grammar/342-v5.0.peg`
