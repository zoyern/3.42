# 3.42 — Arbre Symbolique Complet v5.2
## "expr est `*`, chaque chemin dans l'arbre a un nom"

---

## 0. LE DÉCLIC

`expr` dans une grammaire classique = "n'importe quoi peut aller ici". C'est vague, aléatoire, source de bugs.

Dans 3.42 : **`expr` = `*`** = le nœud neutre = "une valeur". C'est un symbole concret. Quand tu combines `*` avec n'importe quel autre symbole, tu obtiens un **nœud nommé** dans l'arbre. Pas de hasard. Chaque chemin a un nom, une sémantique, un type.

Le langage entier est un **arbre de combinaisons déterministes** à partir de la racine `*` (valeur).

---

## 1. LA RACINE : `*` (expr / valeur / sphère)

`*` = une sphère. Toute donnée dans 3.42 est une sphère. La racine de l'arbre est "une sphère quelconque".

À partir de cette racine, chaque symbole est une **branche** qui transforme la sphère :

```
* (sphère = valeur = expr)
│
├─── * {        → SCOPE      (sphère avec corps)
├─── * :        → BIND       (sphère liée à un type/pattern)
├─── * ?        → MEASURE    (sphère mesurée)
├─── * .        → ACCESS     (accès dans la sphère)
├─── * |        → PIPE       (sphère connectée)
├─── * ~        → WAVE       (sphère diffusée)
├─── * @        → BORROW     (sphère empruntée)
├─── * =        → NAME       (sphère nommée)
├─── * + *      → ADD        (deux sphères additionnées)
├─── * - *      → SUB        (deux sphères soustraites)
├─── * * *      → MUL        (deux sphères multipliées)
├─── * << *     → SHIFT      (sphère décalée)
├─── * == *     → COMPARE    (deux sphères comparées)
│
├─── << *       → RETURN     (renvoyer la sphère)
├─── >> ;       → BREAK      (sortir)
├─── ** *       → COPY       (dupliquer la sphère)
├─── ~ * = *    → MUT        (sphère mutable)
└─── :: nom {}  → MODULE     (espace nommé)
```

---

## 2. L'ARBRE COMPLET — NIVEAU PAR NIVEAU

### Niveau 0 : Atomes (symboles seuls)

Chaque atome a **un nom** et **une sémantique** :

```
DIRECTION    < (back)       > (forward)
MESURE       ? (measure)    ! (not)        . (access)
FLUX         | (pipe)       ~ (wave)
LIAISON      : (bind)       = (assign)     ; (end)
SPIN         + (positive)   - (negative)   * (neutral)   # (error)   _ (wild)
MÉMOIRE      @ (borrow)
CONTENEUR    {} (body)      () (group)     [] (collection)
```

Total : **20 atomes**. C'est le vocabulaire complet du système.

### Niveau 1 : Combinaisons binaires (atome + atome)

Chaque combinaison de 2 atomes produit un **nœud nommé** :

```
DIRECTION × DIRECTION
  << = back + back       → RETURN (retour au caller)
  >> = forward + forward → BREAK (sortir du contexte)
  <> = back + forward    → RÉSERVÉ (futur : swap ? ping-pong ?)
  >< = forward + back    → RÉSERVÉ

BIND × DIRECTION
  :: = bind + bind       → PATH (chemin dans namespace)
  :> = bind + forward    → CAST (transformer le type)
  := = bind + assign     → RÉSERVÉ (≠ car = seul suffit)

ACCESS × MESURE
  .? = access + measure  → INTROSPECT (vérifier sans consommer)
  .! = access + not      → RÉSERVÉ (accès inversé ?)

MESURE × MESURE
  ?? = measure + measure → CONTINUOUS (mesure en boucle/stream)
  ?! = measure + not     → RÉSERVÉ (mesure inversée ?)

SPIN × SPIN
  ** = neutral + neutral → COPY (dupliquer)
  ++ = positive + positive → CONCAT (accumuler/joindre)
  -- = negative + negative → RÉSERVÉ (double négation ? décrement ?)
  ## = error + error     → RÉSERVÉ (erreur fatale ? panic ?)

MÉMOIRE × MÉMOIRE
  @@ = borrow + borrow   → BORROW-WRITE (emprunt exclusif)
```

### Niveau 2 : expr + atome (sphère transformée)

Ici `*` = n'importe quelle sphère concrète (variable, littéral, bloc...) :

```
POSTFIXE (symbole APRÈS la sphère)
  * {}        → SCOPE      (exécuter un corps avec cette sphère comme contexte)
  * ?         → MEASURE    (mesurer/effondrer cette sphère)
  * ? {}      → BRANCH     (mesurer avec branchement sur les spins)
  * ?? {}     → STREAM     (mesurer en continu, boucle)
  * .nom      → FIELD      (accéder à un champ de la sphère)
  * .? {}     → INSPECT    (introspection avec branches)
  * @         → REF-READ   (emprunter en lecture)
  * @@        → REF-WRITE  (emprunter en écriture)
  * :> type   → CAST       (transformer le type de la sphère)
  * ()        → CALL       (appeler la sphère comme fonction)
  * []        → INDEX      (indexer dans la sphère)
  * [..]      → SLICE      (découper la sphère)

PRÉFIXE (symbole AVANT la sphère)
  << *        → RETURN     (renvoyer la sphère au caller)
  ** *        → COPY       (dupliquer la sphère)
  ! *         → NOT        (inverser la sphère)
  + *         → POS        (sphère positive / unaire +)
  - *         → NEG        (sphère négative / unaire -)
  * NOM       → VARIANT    (constructeur de variante avec spin)
  # NOM       → ERROR-VAR  (constructeur de variante erreur)

INFIXE (symbole ENTRE deux sphères)
  * | *       → PIPE       (connecter via ring buffer SPSC)
  * ~ *       → BROADCAST  (diffuser 1→N via SPMC)
  * + *       → ADD        (addition)
  * - *       → SUB        (soustraction)
  * * *       → MUL        (multiplication)
  * / *       → DIV        (division)
  * % *       → MOD        (modulo)
  * ++ *      → CONCAT     (concaténation)
  * << *      → SHL        (shift left — en contexte expression)
  * >> *      → SHR        (shift right — en contexte expression)
  * == *      → EQ         (égalité)
  * != *      → NEQ        (inégalité)
  * < *       → LT         (inférieur)
  * > *       → GT         (supérieur)
  * <= *      → LTE        (inférieur ou égal)
  * >= *      → GTE        (supérieur ou égal)
  * && *      → AND        (et logique)
  * || *      → OR         (ou logique)
  * & *       → BIT-AND    (et bitwise)
  * ^ *       → BIT-XOR    (xor bitwise)

STATEMENT (en début de ligne)
  << *        → RETURN     (retourner au caller)
  << spin *   → SPIN-RET   (retourner avec spin explicite)
  >> ;        → BREAK      (sortir de la boucle)
  ~ nom = *   → MUT-DECL   (déclarer mutable)
  nom = *     → ASSIGN     (nommer une sphère)
  :: nom {}   → MODULE     (déclarer un namespace)
  type nom {} → TYPE-DECL  (déclarer un type)
  trait nom {} → TRAIT-DECL (déclarer un contrat)
```

### Niveau 3 : Combinaisons profondes

Chaque nœud de niveau 2 peut lui-même se combiner :

```
RETURN + SPIN + BODY
  << + { compute() }     → RETURN-POS-BODY   (retourner succès avec bloc)
  << - "erreur"           → RETURN-NEG-VALUE  (retourner échec avec message)
  << * ;                  → RETURN-VOID       (retourner rien)
  << # { handle_err() }  → RETURN-ERR-BODY   (retourner erreur avec bloc)

MEASURE + BODY + SPIN-BRANCHES
  * ? {                   → MEASURE-BRANCH
    + val? : action;        → BRANCH-POS-BIND (spin+, capturer, agir)
    - err? : handle;        → BRANCH-NEG-BIND (spin-, capturer, gérer)
    # : panic;              → BRANCH-ERR      (spin#, paniquer)
    _ : default;            → BRANCH-WILD     (wildcard, défaut)
  }

MEASURE + BODY + VARIANT-DESTRUCT
  * ? {                   → MEASURE-VARIANT
    + Data(x?, y?) : use(x, y);   → BRANCH-DESTRUCT (extraire les champs)
    + Data(x?) (x > 0) : ok;      → BRANCH-FILTER   (extraire + filtrer)
  }

PIPE + CHAIN
  * | * | * | *           → PIPELINE  (chaîne de transformations)

PIPE + BODY
  * | { transform(it) }  → PIPE-BODY (piper vers un bloc)

COPY + BODY
  ** { build() }          → COPY-BODY (copier le résultat d'un bloc)

MUT + TYPE + BODY
  ~ x: i32 = { calc() }  → MUT-TYPED-BODY (mutable typé avec bloc init)
```

### Niveau N : Récursion infinie

```
La beauté : chaque nœud de l'arbre EST une sphère (*).
Donc chaque nœud peut être la racine d'un sous-arbre.
Donc l'arbre est récursif à l'infini.

* ? {
  + : * ? {                          ← mesure dans une mesure
    + : * | * | { * ? { + : *; }; }  ← pipe dans un bloc dans une mesure
  };
};

Chaque * est remplacé par un nom concret à l'exécution.
Pas de randomness. Juste des noms qui s'emboîtent.
```

---

## 3. POURQUOI ÇA ÉLIMINE TOUS LES PROBLÈMES DE COMBINAGE

### Avant (grammaire classique)
```
Expression = ??? (n'importe quoi)
Statement = Expression + ";"  (vague)
ReturnStmt = "return" + Expression (le "return" est un mot magique)
```
Problème : `Expression` est un trou noir. N'importe quoi peut y aller. Les conflits sont inévitables.

### Maintenant (arbre symbolique)
```
* = une sphère (valeur concrète nommée)
<< * ; = RETURN (symbole << + sphère + fin)
* ? {} = MEASURE (sphère + symbole ? + corps)
** * = COPY (symbole ** + sphère)
```
Chaque position dans l'arbre est **un socket typé** : "ici va une sphère". La sphère a un nom, un type, un spin. Le combinage est **déterministe** car :

1. **Chaque symbole est un nœud** — pas un mot-clé magique
2. **Chaque position est nommée** — pas un "Expression" vague
3. **L'arbre est fini** — ~20 atomes × ~40 combinaisons niveau 2 = ~60 nœuds nommés
4. **La récursion est typée** — un nœud `*` dans l'arbre EST une sphère, donc le sous-arbre est valide par construction

### La preuve : chaque nœud AST a un nom

| Nœud AST | Symboles | Nom |
|---|---|---|
| `ReturnNode` | `<< spin? expr?` | RETURN |
| `BreakNode` | `>>` | BREAK |
| `MeasureNode` | `expr ? body?` | MEASURE |
| `ContinuousNode` | `expr ?? body` | STREAM |
| `PipeNode` | `expr \| expr` | PIPE |
| `BroadcastNode` | `expr ~ expr` | BROADCAST |
| `AssignNode` | `name = expr` | NAME |
| `MutNode` | `~ name = expr` | MUT |
| `CopyNode` | `** expr` | COPY |
| `CastNode` | `expr :> type` | CAST |
| `AccessNode` | `expr . name` | FIELD |
| `IntrospectNode` | `expr .? body?` | INSPECT |
| `CallNode` | `expr ( args )` | CALL |
| `IndexNode` | `expr [ expr ]` | INDEX |
| `BorrowNode` | `expr @/@@ ` | BORROW |
| `ModuleNode` | `:: name { stmts }` | MODULE |
| `TypeNode` | `type name body` | TYPE |
| `TraitNode` | `trait name { methods }` | TRAIT |
| `BlockNode` | `{ stmts; expr? }` | SCOPE |
| `SpinBranchNode` | `spin match? filter? : body` | BRANCH |
| `BinaryNode` | `expr op expr` | OP(+/-/*/etc) |
| `UnaryNode` | `op expr` | UNARY(!/+/-) |
| `SpinNode` | `+ / - / * / #` | SPIN |
| `LiteralNode` | `42 / "str" / 0.5p` | VALUE |
| `IdentNode` | `nom` | REF |
| `PathNode` | `a :: b :: c` | PATH |
| `ListNode` | `[ exprs ]` | LIST |
| `MapNode` | `[ k:v pairs ]` | MAP |
| `FuncNode` | `( params ) : type? body` | FUNC |
| `StructNode` | `Type { fields }` | STRUCT |
| `VariantNode` | `spin Name(args?)` | VARIANT |

**29 nœuds AST. Chacun a un nom. Chacun est une combinaison de symboles. Aucun n'est aléatoire.**

---

## 4. L'ARBRE VISUEL COMPLET

```
                              * (SPHÈRE)
                              │
        ┌─────────┬───────┬───┴────┬───────┬─────────┐
        │         │       │        │       │         │
    TRANSFORMER MESURER  LIER   CONNECTER STOCKER  CONTENIR
        │         │       │        │       │         │
   ┌────┤    ┌────┤   ┌───┤    ┌───┤   ┌───┤     ┌───┤
   │    │    │    │   │   │    │   │   │   │     │   │
  <<   **   ?    .?  :   =    |   ~   @  @@    {} () []
RETURN COPY  │    │  BIND NAME PIPE WAVE  │   │   │  │  │
             │    │   │       │       BORROW │  │  │
        ┌────┤    │  :>      │         SCOPE │  │
        │    │    │  CAST    │         GROUP  │
       {}   ??   │          │         COLLECTION
     BRANCH STREAM│          │
        │         │          │
   ┌────┼────┐    │     ┌────┤
   │    │    │    │     │    │
   +    -    *    #    expr  expr
  POS  NEG  VOID ERR   │     │
   │    │    │    │    PIPE  PIPE
  val? val? ...  ...   CHAIN CHAIN
  BIND BIND              ...
```

---

## 5. CONSÉQUENCES

### Le langage n'a pas de "syntaxe" — il a un ARBRE

Une syntaxe classique est une liste de règles. L'arbre 3.42 est une **structure de données** :
- Chaque nœud = un symbole
- Chaque arête = une combinaison valide
- Traverser l'arbre = écrire du code
- Le code EST l'arbre (zero-parsing : l'AST est la source de vérité)

### Les nouvelles combinaisons sont des branches

Quand un utilisateur futur définit `?~` (measure + broadcast) :
- Il ajoute une **branche** à l'arbre
- La branche part du nœud `?` et va vers `~`
- Le nom est automatique : MEASURE-WAVE
- Le sens est émergent : "mesurer et diffuser"
- Adoption par la communauté = la branche devient standard

### Le FFI est un mapping d'arbres

Traduire 3.42 → C = mapper l'arbre 3.42 vers l'arbre C :
- `RETURN` → `return`
- `MEASURE` → `switch/case`
- `PIPE` → function composition
- `SCOPE` → `{ }`
- Chaque nœud 3.42 a un nœud C correspondant (ou une émulation)

### Le zero-parsing est naturel

L'AST **EST** l'arbre. Le texte (fichier `.342`) est juste une **projection textuelle** de l'arbre. D'autres projections sont possibles :
- Projection visuelle (comme un graphe de nœuds)
- Projection sonore (chaque symbole = un son)
- Projection tactile (pour l'accessibilité)
- Projection dans un autre langage (FFI)

Le texte n'est qu'UNE vue. L'arbre est la vérité.

---

## 6. IMPACT SUR LA PEG

La PEG v5.1 est déjà **presque** cet arbre. La seule chose qui manque est la **conscience** que chaque `Expression` dans la PEG est un socket pour `*` (sphère).

Changements conceptuels (pas de changement syntaxique majeur) :

```peg
# Avant : Expression = concept vague
Expression <- PipeExpr

# Maintenant : Expression = Sphère (*)
# Chaque Expression dans la grammaire = un slot pour une sphère nommée
# Le parseur produit un AST où chaque nœud a un type de la table §3
Sphere           <- PipeExpr
```

On pourrait renommer `Expression` → `Sphere` dans la PEG pour refléter le paradigme. Mais c'est cosmétique — la structure est identique.

Ce qui compte : **la sémantique**. Chaque nœud AST produit par le parseur a un nom de la table (RETURN, MEASURE, PIPE, COPY...). Pas de nœud "Expression générique". Chaque nœud est typé.

---

## 7. RÉSUMÉ EN UNE PHRASE

> 3.42 n'est pas un langage avec une syntaxe.
> C'est un **arbre de 20 symboles** dont chaque combinaison a un **nom**, un **sens**, et un **type**.
> Le code est un chemin dans cet arbre. Écrire = naviguer. Lire = suivre les branches.
> Pas de hasard. Pas de magie. Que de l'émergence.
