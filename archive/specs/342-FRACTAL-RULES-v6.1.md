# 3.42 — Règles Fractales v6.1
## "Zéro hardcodé. Les règles simples produisent tout."

---

## 0. L'ERREUR DU TABLEAU 2D

Le tableau v6.0 avait un défaut : il **hardcodait** les sens. `<<` = return, `><` = break. C'est exactement ce qu'on voulait éviter. C'est des keywords déguisés en symboles.

**La vraie approche** : il n'y a que des **règles de composition**. Les sens émergent. Pas de table de lookup. Un algorithme.

---

## 1. LES RÈGLES DE COMPOSITION (le moteur)

### Règle unique : APPLIQUER

```
A B = "A appliqué à B"
```

C'est tout. Une seule règle. Le sens de la combinaison dépend de :
1. Le sens de A (atome ou sous-expression)
2. Le sens de B (atome ou sous-expression)
3. Le **contexte** (statement, expression, type, pattern)

### Les 16 atomes et leur verbe

Chaque atome est un **verbe** (action), pas un nom :

| Atome | Verbe | Action sur ce qui suit |
|-------|-------|----------------------|
| `<` | SORTIR | Pousser ce qui suit vers l'extérieur |
| `>` | ENTRER | Tirer ce qui suit vers l'intérieur |
| `.` | ACCÉDER | Lire dans ce qui suit |
| `?` | MESURER | Observer/décider sur ce qui suit |
| `!` | INVERSER | Nier ce qui suit |
| `\|` | CONNECTER | Relier ce qui précède à ce qui suit |
| `~` | ONDULER | Propager/vibrer ce qui suit |
| `:` | LIER | Attacher ce qui précède à ce qui suit |
| `=` | NOMMER | Donner un nom à ce qui suit |
| `;` | FINIR | Terminer ce qui précède |
| `+` | AFFIRMER | Ce qui suit est positif |
| `-` | NIER | Ce qui suit est négatif |
| `*` | GÉNÉRALISER | Ce qui suit est tout/any |
| `#` | CORROMPRE | Ce qui suit est en erreur |
| `_` | IGNORER | Ce qui suit est rien/void |
| `@` | EMPRUNTER | Référencer ce qui suit sans posséder |

### Composition : A B = "A appliqué à B"

```
< <   = SORTIR(SORTIR)     = sortir de la sortie     = EXIT COMPLET
> <   = ENTRER(SORTIR)      = entrer dans la sortie   = TRAVERSER/QUITTER LE CONTEXTE
< >   = SORTIR(ENTRER)      = sortir de l'entrée      = ÉCHANGER
> >   = ENTRER(ENTRER)       = entrer dans l'entrée    = PLONGER PROFOND
? ?   = MESURER(MESURER)     = mesurer la mesure        = MESURE CONTINUE
* *   = GÉNÉRALISER(GÉNÉRAL) = généraliser le général   = DUPLIQUER (tout de tout = copie)
+ +   = AFFIRMER(AFFIRMER)   = affirmer l'affirmation   = ACCUMULER
: :   = LIER(LIER)           = lier la liaison          = CHEMIN (lien de lien)
: >   = LIER(ENTRER)         = lier vers l'entrée       = TRANSFORMER (cast)
. ?   = ACCÉDER(MESURER)     = accéder à la mesure      = INTROSPECTER
@ @   = EMPRUNTER(EMPRUNTER) = emprunter l'emprunt      = EMPRUNT EXCLUSIF
! |   = INVERSER(CONNECTER)  = inverser la connexion     = BLOQUER (fence)
```

---

## 2. DIMENSION N : LA COMPOSITION EST RÉCURSIVE

### Profondeur 2

```
< < expr ;    = SORTIR(SORTIR(expr)) FINIR
              = sortir complètement avec expr, fin
              = ce qu'on appelle "return"
```

### Profondeur 3

```
< < + expr ;  = SORTIR(SORTIR(AFFIRMER(expr))) FINIR
              = sortir complètement, l'affirmer, fin
              = "return positif" (succès)

< < # expr ;  = SORTIR(SORTIR(CORROMPRE(expr))) FINIR
              = sortir complètement, c'est corrompu, fin
              = "return erreur"
```

### Profondeur 4

```
< < + { stmts; expr } ;
= SORTIR(SORTIR(AFFIRMER(CORPS(stmts → expr)))) FINIR
= sortir complètement, affirmer le résultat d'un bloc
= "return positif avec computation"
```

### Profondeur N

```
Chaque niveau ajoute un verbe.
Chaque verbe modifie le sens de ce qui suit.
Il n'y a pas de limite.
Le sens émerge à chaque profondeur.
```

---

## 3. `a >< b` EN CONTEXTE EXPRESSION

### Application de la règle

```
a >< b
= a ENTRER(SORTIR) b
= a entre dans la sortie de b
= a traverse b
= a passe à travers b et ressort transformé
```

C'est un **pass-through** : a entre dans b, b le traite, le résultat sort. Comme un filtre.

### Comparaison avec `|`

```
a | b     = a CONNECTER b     = a connecté à b (pipe, async, buffer)
a >< b    = a ENTRER(SORTIR) b = a traverse b (sync, direct, transform)
```

`|` connecte avec un buffer (async, SPSC ring buffer).
`><` traverse directement (sync, pas de buffer, transformation immédiate).

C'est la différence entre un **pipe** (asynchrone, buffered) et un **call** (synchrone, direct).

### Exemples

```
// Pipe (async, buffered)
data | transform | output;

// Pass-through (sync, direct)
data >< transform >< output;

// La différence :
// | crée des green threads avec buffers entre chaque étape
// >< appelle directement, dans le même thread, sans overhead
```

### Autres contextes de `><`

```
// En statement seul
>< ;
= ENTRER(SORTIR) FINIR
= entrer dans la sortie, fin
= quitter le contexte actuel
= ce qu'on appelle "break" (mais c'est émergent, pas hardcodé)

// Avec un spin
>< # "erreur" ;
= ENTRER(SORTIR) CORROMPRE("erreur") FINIR
= quitter le contexte avec une erreur

// Avec une expression
>< + result ;
= ENTRER(SORTIR) AFFIRMER(result) FINIR
= quitter le contexte avec un résultat positif
= "break avec valeur" (comme break avec label en Rust)
```

---

## 4. TOUTES LES COMBINAISONS 3-ATOMES (émergentes)

On ne les liste pas comme un dictionnaire. On **applique la règle** :

```
A B C = A(B(C))

< < <   = SORTIR(SORTIR(SORTIR))   = triple sortie = exit profond (process exit ?)
< < >   = SORTIR(SORTIR(ENTRER))   = exit puis re-enter = restart ?
< > <   = SORTIR(ENTRER(SORTIR))   = swap-exit = yield ?
> < >   = ENTRER(SORTIR(ENTRER))   = traverse-enter = callback ?
> > <   = ENTRER(ENTRER(SORTIR))   = dive-exit = unwrap ?
> > >   = ENTRER(ENTRER(ENTRER))   = triple entrée = scope très profond

? ? ?   = MESURER(MESURER(MESURER)) = mesure de mesure de mesure = ???
                                     = probablement pas utile (sur-mesure)

? ! ?   = MESURER(INVERSER(MESURER)) = mesurer le contraire de la mesure = ASSERT
                                       = vérifier que la mesure N'EST PAS quelque chose

| ~ |   = CONNECTER(ONDULER(CONNECTER)) = pipe-broadcast-pipe = fan-out-fan-in
                                          = map-reduce émergent !
```

### Le moteur de prédiction

Pour N'IMPORTE quelle séquence de symboles, applique :

```
sens(S₁ S₂ ... Sₙ) = S₁(S₂(...(Sₙ)...))
```

Le compilateur n'a pas besoin de connaître chaque combinaison. Il applique la règle récursivement. Le sens **émerge**.

---

## 5. CONTEXTE : LE MÊME SYMBOLE, UN SENS DIFFÉRENT SELON LA POSITION

### Les 4 contextes

```
STATEMENT  : début de ligne / après ;     → action sur le programme
EXPRESSION : entre deux valeurs           → transformation de données
TYPE       : après : dans une déclaration → contrainte sur la sphère
PATTERN    : dans un ? {} block           → filtrage/matching
```

### Exemple : `><`

```
STATEMENT  : >< ;         → break (quitter le contexte)
EXPRESSION : a >< b       → pass-through (a traverse b)
TYPE       : (pas de sens en type — erreur de contexte)
PATTERN    : (pas de sens en pattern — erreur de contexte)
```

### Exemple : `<<`

```
STATEMENT  : << expr ;    → return (sortir avec valeur)
EXPRESSION : a << b       → shift left (sortir les bits de a vers la gauche de b positions)
TYPE       : (pas de sens en type)
PATTERN    : (pas de sens en pattern)
```

### Exemple : `?`

```
STATEMENT  : (toujours partie d'une expression)
EXPRESSION : expr ?       → mesurer expr (collapse)
           : expr ? {}    → mesurer expr avec branches
           : expr ??      → mesurer expr en continu (boucle)
TYPE       : T?           → type optionnel (la sphère peut être void)
PATTERN    : name?        → binding (capturer dans un nom)
```

Le contexte n'est PAS une règle supplémentaire. C'est la **position dans l'arbre AST** qui détermine l'interprétation. L'arbre est la seule vérité.

---

## 6. POURQUOI C'EST FRACTAL

### Définition

Un fractal = une structure où chaque partie ressemble au tout. Dans 3.42 :

```
Programme = Sphère
  contient des Statements = Sphères
    contiennent des Expressions = Sphères
      contiennent des Sous-expressions = Sphères
        contiennent des Atomes = Sphères (les plus petites)
```

Chaque niveau a la **même structure** : une sphère avec un spin, transformable par les mêmes symboles.

### Auto-similarité

```
// Niveau programme : un module est une sphère
::app {
    // Niveau function : une fonction est une sphère
    compute = (x) {
        // Niveau expression : une mesure est une sphère
        x ? {
            // Niveau branch : un branchement est une sphère
            + val? : {
                // Niveau bloc : un bloc est une sphère
                val + 1
            };
        }
    };
};

// Chaque {} est une sphère.
// Chaque sphère se compose de la même façon.
// Les mêmes 16 atomes s'appliquent à chaque niveau.
// C'est fractal.
```

### Scalabilité infinie

Parce que c'est fractal :
- Pas besoin de nouvelles règles pour la profondeur N+1
- Le compilateur est récursif (descente récursive PEG = naturellement fractal)
- L'optimisation est locale (chaque sphère s'optimise indépendamment)
- Le debugging est zoomable (zoom sur n'importe quel niveau de l'arbre)

---

## 7. L'AST COMME LANGAGE UNIVERSEL

### Chaque nœud AST = application d'un verbe

```
AST Node = Verbe(Enfants...)

ReturnNode    = SORTIR(SORTIR(spin?, valeur?))     ← pas hardcodé, composé
BreakNode     = ENTRER(SORTIR(spin?, valeur?))      ← pas hardcodé, composé
MeasureNode   = MESURER(expression, branches?)       ← pas hardcodé, composé
LoopNode      = MESURER(MESURER(expression, body))   ← pas hardcodé, composé
PipeNode      = CONNECTER(gauche, droite)            ← pas hardcodé, composé
CopyNode      = GÉNÉRALISER(GÉNÉRALISER(expression)) ← pas hardcodé, composé
```

L'AST n'a pas de "ReturnNode" ou "BreakNode" spéciaux. Il n'a que des **CompositionNodes** :

```
type ASTNode =
    | Atom(symbol)                    // un seul atome
    | Compose(ASTNode, ASTNode)       // A appliqué à B
    | Sphere(spin, value, children)   // une sphère avec état
```

3 types de nœuds. C'est tout. Le reste émerge.

---

## 8. TABLEAU FRACTAL : RÈGLES DE COMPOSITION

Au lieu d'un tableau 2D de résultats hardcodés, voici les **règles** qui permettent de prédire N'IMPORTE quelle combinaison :

### Règle 1 : Direction

```
SORTIR(x) = pousser x vers le scope parent
ENTRER(x) = tirer x vers le scope enfant
SORTIR(SORTIR(x)) = pousser x vers le scope grand-parent (= exit function)
ENTRER(SORTIR(x)) = entrer dans la sortie de x (= traverser x)
SORTIR(ENTRER(x)) = sortir de l'entrée de x (= échanger avec x)
ENTRER(ENTRER(x)) = entrer dans l'entrée de x (= scope profond)
```

### Règle 2 : Mesure

```
MESURER(x) = observer x une fois, décider du spin
MESURER(MESURER(x)) = observer x, décider, ré-observer (= boucle)
MESURER(INVERSER(MESURER(x))) = observer que x n'est PAS (= assert)
```

### Règle 3 : Connexion

```
CONNECTER(a, b) = créer un canal async entre a et b (pipe)
ONDULER(a, b) = diffuser a vers tous les b (broadcast)
CONNECTER(ONDULER(a)) = pipe le résultat du broadcast (= fan-in)
ONDULER(CONNECTER(a)) = broadcaster le résultat du pipe (= fan-out)
```

### Règle 4 : Spin

```
AFFIRMER(x) = x est positif/vrai/succès
NIER(x) = x est négatif/faux/échec
GÉNÉRALISER(x) = x est tout/n'importe quoi
CORROMPRE(x) = x est en erreur
IGNORER(x) = x est rien/void
```

### Règle 5 : Mémoire

```
EMPRUNTER(x) = référence en lecture sur x
EMPRUNTER(EMPRUNTER(x)) = référence en écriture (exclusif)
GÉNÉRALISER(GÉNÉRALISER(x)) = copie profonde de x (any of any = duplicate)
```

### Règle 6 : Liaison

```
LIER(a, b) = a est de type b / a correspond au pattern b
LIER(LIER(a)) = chemin dans un namespace (a :: b)
LIER(ENTRER(a)) = transformer a vers un nouveau type (cast)
```

### Règle 7 : Corps

```
CORPS(statements, expr?) = exécuter les statements, retourner expr
Attachable à n'importe quel verbe :
  SORTIR(SORTIR(AFFIRMER(CORPS(...)))) = return positif d'un bloc
  MESURER(expr, CORPS(branches))       = if/switch
  CONNECTER(CORPS(a), CORPS(b))        = pipe entre deux blocs
```

---

## 9. EXERCICE : PRÉDIRE DES COMBINAISONS NOUVELLES

### `~ | ~`
```
ONDULER(CONNECTER(ONDULER))
= diffuser une connexion diffusée
= broadcast → pipe → broadcast
= réseau de diffusion (mesh)
= pattern pub/sub émergent !
```

### `? > < ?`
```
MESURER(ENTRER(SORTIR(MESURER)))
= mesurer le résultat de traverser une mesure
= vérifier qu'une transformation préserve la mesure
= test d'idempotence / invariant check
```

### `< ~ >`
```
SORTIR(ONDULER(ENTRER))
= sortir l'onde qui entre
= céder le flux entrant
= yield d'un stream
```

### `# ? ! #`
```
CORROMPRE(MESURER(INVERSER(CORROMPRE)))
= corrompre la mesure de l'inverse d'une corruption
= vérifier qu'une erreur est résolue
= error recovery check
```

### `:: :: ::`
```
LIER(LIER(LIER(LIER(LIER(LIER)))))
= chemin très profond dans le namespace
= a :: b :: c :: d
= navigation multi-niveaux (déjà émergent naturellement)
```

---

## 10. IMPACT SUR LE COMPILATEUR

### L'AST n'a que 3 types de nœuds

```c
typedef enum {
    NODE_ATOM,      // un seul symbole
    NODE_COMPOSE,   // A appliqué à B
    NODE_SPHERE     // valeur avec spin + enfants
} NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        Symbol atom;                         // NODE_ATOM
        struct { ASTNode* verb; ASTNode* arg; }; // NODE_COMPOSE
        struct { Spin spin; Value value; ASTNode** children; }; // NODE_SPHERE
    };
} ASTNode;
```

Le compilateur n'a PAS de cas spéciaux :
- Pas de `case RETURN:` — c'est `Compose(Out, Compose(Out, expr))`
- Pas de `case BREAK:` — c'est `Compose(In, Compose(Out, expr))`
- Pas de `case IF:` — c'est `Compose(Measure, Sphere(branches))`

L'évaluation est **une seule fonction récursive** qui applique les règles de composition.

### Optimisation

Le compilateur reconnaît des **patterns** dans l'arbre de composition et les optimise :

```
// Pattern: SORTIR(SORTIR(expr)) → instruction CPU: RET
// Pattern: MESURER(MESURER(expr, body)) → instruction CPU: CMP + JMP (boucle)
// Pattern: CONNECTER(a, b) → runtime: spawn green thread + ring buffer
// Pattern: ONDULER(a, b) → runtime: SIMT dispatch (si GPU dispo)
```

Les patterns d'optimisation sont des **règles de réécriture**, pas du hardcoding. Ajouter un nouveau pattern = ajouter une règle de réécriture. Le compilateur **évolue** comme le langage.

---

## 11. VÉRIFICATION DE COHÉRENCE

### Est-ce que TOUT est prévisible ?

| Test | Composition | Sens prédit | Logique ? |
|------|------------|-------------|-----------|
| return | `< < + expr ;` | sortir(sortir(affirmer(expr))) finir | ✓ |
| break | `> < ;` | entrer(sortir) finir = quitter contexte | ✓ |
| if | `expr ? { branches }` | mesurer expr, brancher | ✓ |
| while | `expr ?? { body }` | mesurer(mesurer expr), boucler | ✓ |
| pipe | `a \| b` | connecter a et b | ✓ |
| copy | `** expr` | généraliser(généraliser(expr)) = dupliquer | ✓ |
| cast | `expr :> type` | lier(entrer(type)) sur expr | ✓ |
| spawn | `\| { body }` | connecter(corps) = thread | ✓ |
| GPU | `~ { body }` | onduler(corps) = broadcast exec | ✓ |
| yield | `< ~` | sortir(onduler) = céder l'onde | ✓ |
| fence | `! \|` | inverser(connecter) = bloquer | ✓ |
| CAS | `? =` | mesurer(nommer) = compare-and-swap | ✓ |
| assert | `? !` | mesurer(inverser) = vérifier l'inverse | ✓ |
| pass-through | `a >< b` | a entrer(sortir) b = a traverse b | ✓ |
| introspect | `.?` | accéder(mesurer) = lire l'état | ✓ |
| parent | `..` | accéder(accéder) = remonter | ✓ |
| panic | `##` | corrompre(corrompre) = erreur fatale | ✓ |
| namespace | `::` | lier(lier) = chemin | ✓ |
| fan-out | `\|~` | connecter(onduler) = scatter | ✓ |
| fan-in | `~\|` | onduler(connecter) = gather | ✓ |

**20/20 : chaque combinaison est prévisible par les règles.**

### Est-ce qu'il y a des combinaisons absurdes ?

```
; ;    = FINIR(FINIR) = finir la fin = double terminaison = instruction vide (NOP)
; <    = FINIR(SORTIR) = finir de sortir = (pas de sens, ; termine, rien après)
= =    = NOMMER(NOMMER) = nommer le nom = (pas de sens direct, erreur de contexte)
```

Les combinaisons "absurdes" ne sont pas des erreurs de syntaxe. Elles sont des **NOP** ou des **erreurs de contexte**. L'AST reste valide. Le compilateur avertit. Le programme ne crash pas.

---

## CONCLUSION

```
Le langage 3.42 n'a PAS de syntaxe.
Il a 16 verbes et UNE règle : A(B).

Tout le reste — return, break, if, while, pipe, thread, GPU,
copy, cast, assert, yield, fence, panic — ÉMERGE.

C'est fractal : chaque niveau est identique.
C'est scalable : profondeur N sans nouvelles règles.
C'est prédictible : si tu connais les 16 verbes, tu prédis tout.
C'est évolutif : une nouvelle combinaison = un nouveau sens, pas un nouveau keyword.

Le compilateur a 3 types de nœuds et 7 règles de composition.
Le reste est de l'optimisation pattern-matching.
```
