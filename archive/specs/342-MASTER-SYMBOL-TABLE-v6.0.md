# 3.42 — Table Maître des Symboles v6.0
## "Chaque symbole a un sens réel, chaque combinaison est prédictible"

---

## 0. RÈGLE FONDAMENTALE

Chaque symbole a un **sens lié au processeur/à la logique réelle**. Pas d'arbitraire.
Les combinaisons sont **prédictibles** : si tu connais le sens de A et de B, tu peux deviner A+B.

---

## 1. LES 16 ATOMES

### Sémantique ancrée dans le réel

| # | Symbole | Nom FR | Nom EN | CPU / Logique réelle | Sens dans 3.42 |
|---|---------|--------|--------|---------------------|-----------------|
| 1 | `<` | sortir | out | CPU: STORE (écrire vers l'extérieur) | Pousser vers l'extérieur, sortir du scope |
| 2 | `>` | entrer | in | CPU: LOAD (lire depuis l'extérieur) | Tirer vers l'intérieur, entrer dans le scope |
| 3 | `.` | accéder | access | CPU: LOAD offset (accès mémoire relatif) | Accéder à un champ, lire sans consommer |
| 4 | `?` | mesurer | measure | CPU: CMP + BRANCH (comparer puis brancher) | Observer, effondrer, décider |
| 5 | `!` | inverser | not | CPU: NOT (inversion bits) | Nier, inverser le spin |
| 6 | `\|` | connecter | pipe | CPU: registre → registre (data path) | Connecter deux exécutions (SPSC) |
| 7 | `~` | ondoyer | wave | GPU: SIMT broadcast (1 instruction → N threads) | Diffuser, muter, vibrer |
| 8 | `:` | lier | bind | CPU: MOV dest, src (lier adresse à valeur) | Type, pattern→action, clé→valeur |
| 9 | `=` | nommer | name | CPU: STORE reg (nommer un registre) | Assigner un nom à une sphère |
| 10 | `;` | finir | end | CPU: fin d'instruction (cycle clock) | Terminer une instruction |
| 11 | `+` | positif | pos | CPU: flag POSITIVE / carry=0 | Vrai, succès, ajouter |
| 12 | `-` | négatif | neg | CPU: flag NEGATIVE / sign bit | Faux, échec, soustraire |
| 13 | `*` | tout | any | CPU: wildcard / don't care bits | Any, everything, multiply |
| 14 | `#` | corrompu | error | CPU: flag OVERFLOW / exception | Erreur, corruption |
| 15 | `_` | vide | void | CPU: NOP / zero register | Rien, ignorer, placeholder |
| 16 | `@` | emprunter | borrow | CPU: pointeur indirect (adresse de) | Référence sans propriété |

### Conteneurs (groupement)

| Symbole | Nom | CPU | Sens |
|---------|-----|-----|------|
| `{}` | corps | CPU: stack frame / scope | Bloc d'exécution, arène mémoire |
| `()` | groupe | CPU: registre group / operand | Paramètres, groupement, priorité |
| `[]` | collection | CPU: array / base+index | Liste, map, index |
| `""` | texte | CPU: string data segment | Chaîne de caractères |

---

## 2. TABLEAU DES COMBINAISONS NIVEAU 2

### Lecture du tableau : A (colonne) + B (ligne) = sens émergent

|  | `<` out | `>` in | `.` access | `?` measure | `!` not | `\|` pipe | `~` wave | `:` bind | `=` name | `+` pos | `-` neg | `*` any | `#` err | `_` void | `@` borrow |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| **`<` out** | `<<` **EXIT** (return) | `<>` *SWAP* (échange) | `<.` *remonter* | `<?` *exit conditionnel* | `<!` *exit forcé* | `<\|` *fermer pipe* | `<~` *yield* (céder) | `<:` *export* | `<=` *inférieur-égal* | `<+` — | `<-` *retrait* | `<*` — | `<#` *exit erreur* | `<_` — | `<@` *libérer emprunt* |
| **`>` in** | `><` **BREAK** (entrer dans sortir) | `>>` **ENTER** (scope profond) | `>.` *descendre* | `>?` *import conditionnel* | `>!` *forcer entrée* | `>\|` *ouvrir pipe* | `>~` *absorber* | `>:` *import* | `>=` *supérieur-égal* | `>+` — | `>-` — | `>*` — | `>#` — | `>_` — | `>@` *prendre emprunt* |
| **`.` access** | `.<` — | `.>` — | `..` **UP** (parent scope) | `.?` **INTROSPECT** | `.!` *accès forcé* | `.\|` — | `.~` *observer onde* | `.:` — | `.=` — | `.+` — | `.-` — | `.*` *accès wildcard* | `.#` — | `._` — | `.@` — |
| **`?` measure** | `?<` — | `?>` — | `?.` — | `??` **LOOP** (mesure-remesure) | `?!` **ASSERT** | `?\|` *filtrer pipe* | `?~` *observer broadcast* | `?:` *ternaire* | `?=` **CAS** (compare-and-swap) | `?+` *est positif?* | `?-` *est négatif?* | `?*` *est any?* | `?#` *est erreur?* | `?_` *est vide?* | `?@` *est emprunté?* |
| **`!` not** | `!<` — | `!>` — | `!.` — | `!?` *ne pas mesurer* | `!!` **FORCE** (double négation=affirm) | `!\|` **FENCE** (barrière) | `!~` *bloquer onde* | `!:` *délier* | `!=` **NOT-EQUAL** | `!+` *not true* | `!-` *not false* | `!*` — | `!#` *ignorer erreur* | `!_` — | `!@` *interdit emprunt* |
| **`\|` pipe** | `\|<` — | `\|>` — | `\|.` — | `\|?` *pipe conditionnel* | `\|!` *pipe forcé* | `\|\|` **OR** (logique) | `\|~` **FANOUT** (pipe→broadcast) | `\|:` — | `\|=` — | `\|+` — | `\|-` — | `\|*` — | `\|#` — | `\|_` — | `\|@` — |
| **`~` wave** | `~<` *yield* | `~>` *absorber* | `~.` *observer* | `~?` *sync* | `~!` *bloquer* | `~\|` **FANIN** (broadcast→pipe) | `~~` **PARALLEL** (multi-wave) | `~:` — | `~=` **MUT-ASSIGN** | `~+` — | `~-` — | `~*` — | `~#` — | `~_` — | `~@` — |
| **`:` bind** | `:<` — | `:>` **CAST** | `:.` — | `:?` — | `:!` — | `:\|` — | `:~` — | `::` **PATH** (namespace) | `:=` *déf. constante* | `:+` — | `:-` — | `:*` — | `:#` — | `:_` — | `:@` — |
| **`+` pos** | — | — | — | — | — | — | — | — | `+=` *incr.* | `++` **CONCAT** | `+-` — | `+*` — | `+#` — | `+_` — | — |
| **`-` neg** | — | — | — | — | — | — | — | — | `-=` *décr.* | `-+` — | `--` *réservé* | `-*` — | `-#` — | `-_` — | — |
| **`*` any** | — | — | — | — | — | — | — | — | `*=` *mul-assign* | `*+` — | `*-` — | `**` **COPY** | *`*#`* — | `*_` — | — |
| **`#` err** | — | — | — | — | — | — | — | — | — | — | — | — | `##` **PANIC** | `#_` — | — |
| **`@` borrow** | — | — | — | — | — | — | — | — | — | — | — | — | — | — | `@@` **BORROW-WRITE** |

### Légende

- **GRAS** = combinaison active dans v6.0
- *Italique* = combinaison réservée (sens logique mais pas encore implémentée)
- `—` = combinaison sans sens évident (disponible pour le futur)

---

## 3. SÉMANTIQUE RÉVISÉE : `<` et `>`

### Le vrai sens

```
< = SORTIR (out) — pousser vers l'extérieur du scope actuel
> = ENTRER (in) — tirer vers l'intérieur du scope actuel
```

### Combinaisons directionnelles

| Symbole | Décomposition | Logique | Action |
|---------|---------------|---------|--------|
| `<<` | sortir + sortir | Sortir complètement = quitter la fonction | **RETURN** (exit au caller) |
| `><` | entrer + sortir | Entrer dans la sortie = quitter la boucle | **BREAK** (exit du loop) |
| `>>` | entrer + entrer | Entrer profondément = plonger dans un scope | **ENTER** (scope imbriqué) réservé |
| `<>` | sortir + entrer | Sortir puis entrer = échanger | **SWAP** réservé |

### Pourquoi `><` pour break (pas `>>`)

Le break c'est : "je suis DANS une boucle (`>`), et je veux en SORTIR (`<`)". Donc `><` = "entrer dans sortir" = break.

`>>` serait "entrer profondément" — ce qui aurait plus de sens pour un `continue` (re-entrer dans la boucle) ou un scope imbriqué.

### Exemples

```
// Return
<< + result;                    // sortir + sortir + spin + valeur

// Break
counter ?? {
    (counter > 100) : ><;       // entrer-sortir = break
    _ : counter += 1;
};

// L'ancien >> devient ><
// >> est libéré pour un usage futur (enter deep / continue ?)
```

### Impact : `<<` et `>>` comme opérateurs shift

- `<<` en statement = RETURN (inchangé)
- `<<` en expression = shift left (inchangé)
- `><` en statement = BREAK (nouveau)
- `>>` en expression = shift right (inchangé)
- `>>` en statement = **RÉSERVÉ** (futur : continue ? enter scope ? dive ?)

`><` n'a **aucun conflit** avec les opérateurs existants :
- `>` suivi de `<` n'apparaît dans aucun opérateur existant
- En expression `a >< b` ne matche rien → erreur de syntaxe (correct)
- En statement `><;` = break (unique et clair)

---

## 4. `??` — MESURE-REMESURE = BOUCLE

### Pourquoi "mesure + mesure" = boucle

```
?  = mesurer UNE fois → décider → fin
?? = mesurer → décider → RE-mesurer → décider → RE-mesurer → ...
```

C'est exactement ce que fait un `while` :
1. Mesurer la condition (CMP)
2. Si vrai → exécuter le corps
3. Revenir à 1 (re-mesurer)
4. Si faux → sortir

`??` = la boucle de mesure. Le `?` mesure une fois (if). Le `??` re-mesure indéfiniment (while/loop).

### Structure

```
// while(condition) { body }
expr ?? {
    + : { body };              // tant que positif → exécuter
    - : ><;                    // négatif → break
};

// loop infini (pas de condition, mesure le flux)
stream ?? {
    + val? : process(val);     // chaque valeur positive du stream
    - : ><;                    // stream terminé → break
    # err? : log(err);         // erreur → logger
};

// for (via range + mesure continue)
(0..10) ?? {
    + i? : print(i);           // chaque itération
};
```

---

## 5. BUBBLE-UP : ACCÈS AUX VARIABLES INTERNES VIA `.`

### Le concept

Quand une fonction retourne, ses variables internes **ne sont pas immédiatement libérées**. Elles restent accessibles via `.` sur la valeur de retour. Si personne ne les accède, elles sont libérées à la fermeture de l'arène.

### Comment ça marche

```
compute = (x: i32) {
    intermediate = x * 2;          // variable interne
    debug_info = "step1 ok";       // variable interne
    validation = x > 0;            // variable interne
    << + intermediate;             // retourne seulement intermediate
};

// Le caller récupère la valeur de retour
result = compute(5);               // result = 10

// MAIS peut aussi accéder aux variables internes via .
result.debug_info;                  // "step1 ok" → MOVE vers le scope du caller
result.validation;                  // + (true) → MOVE vers le scope du caller

// Ce qui n'est PAS accédé est FREE quand l'arène de compute est libérée
// = quand le scope du caller se ferme OU quand on appelle explicitement le free
```

### Sécurité (types linéaires + arènes)

1. L'arène de `compute` est marquée **"pending"** après return (pas encore free)
2. `.` sur le résultat fait un **MOVE** (pas une copie) — types linéaires
3. Une fois moved, la variable n'est plus dans l'arène de compute
4. À la fin du scope du caller, toutes les arènes pending sont libérées
5. **Aucune référence pendante** possible — le move est la seule façon d'accéder

### Chaînage

```
// On peut chaîner les accès
result = a() | b() | c();
result.a.debug;                    // accéder au debug de a() dans la chaîne
result.b.intermediate;             // accéder à l'intermediate de b()

// Si on ne s'en sert pas → free automatique
// Si on s'en sert → move vers notre scope
```

### Pourquoi c'est puissant

- **Zéro copy** : pas besoin de retourner un struct avec tous les champs, on accède directement
- **Opt-in** : si tu ne fais pas `.`, tu ne paies rien
- **Debug naturel** : les variables de debug restent accessibles sans les retourner
- **Sécurité** : les types linéaires garantissent pas de double accès
- **Mémoire** : les variables non-accédées sont free à la fin du scope
- **Pas de struct wrapper** : au lieu de `return Result { value, debug, error }`, on fait juste `<< + value;` et le reste est accessible via `.`

### Lien avec le processeur

CPU : quand une fonction retourne, le **stack frame** est "logiquement libéré" mais les données sont toujours en mémoire (juste le stack pointer est décrémenté). Le bubble-up exploite cette réalité : on **retarde** la libération du frame jusqu'à ce que le caller ait fini d'accéder.

Avec les arènes : l'arène de la fonction n'est pas un stack frame classique, c'est un chunk de mémoire. Le "pending" signifie juste qu'on ne remet pas le chunk dans le pool. Coût : presque zéro (un bit de flag).

---

## 6. LA SPHÈRE COMME TYPE UNIVERSEL

### Principe

Toute variable est une **sphère** sur laquelle on applique une **range** (contrainte).

```
// Pas de type explicite — la sphère est infinie
x = 42;                        // sphère avec valeur 42

// Un type = une contrainte de range sur la sphère
x: i32 = 42;                   // sphère contrainte à [-2³¹, 2³¹-1]
c: char = 'A';                 // sphère contrainte à [0, 127]
color: rgb = (255, 128, 0);    // sphère contrainte à [0,255]³

// Cast = changer la range (renormaliser sur la sphère)
x: i8 = 200;                   // 200 est hors range i8 [-128, 127]
y = x :> u8;                   // renormalise : 200 est dans [0, 255] → ok
z = x :> i8;                   // renormalise : 200 → wrap around → -56

// La sphère de Riemann : division par * (neutre/zéro)
a = 1 / *;                     // sur la sphère de Riemann : 1/0 = ∞ (point à l'infini)
                                // pas une erreur — un pôle sur la sphère
                                // le résultat a un spin # (attention, pôle)
                                // mais le programme continue
```

### Harmoniques et calcul

Sur la sphère, les opérations sont des **transformations harmoniques** :
- Addition = superposition d'ondes
- Multiplication = modulation (changement de fréquence)
- Division = inversion de fréquence (pôle si diviseur = 0)
- Cast = changement de base harmonique (re-sampling)

```
// Concaténation int + string : les deux sont des sphères
result = 42 ++ " réponses";     // les sphères se combinent
// 42 est une sphère numérique
// " réponses" est une sphère textuelle
// ++ les met au même dénominateur (string) → "42 réponses"
```

### Tout est combinable, rien n'est une erreur

```
// Même une "erreur" produit un résultat sur la sphère
x = "hello" + 5;
// string + int → pas une erreur de type
// les deux sont des sphères → superposition
// le compilateur PEUT avertir (ce n'est peut-être pas intentionnel)
// mais le code COMPILE et produit un résultat défini
// (par ex: concaténation → "hello5", ou erreur de range → spin #)
// le développeur choisit le comportement via la config du module
```

---

## 7. THREAD / GPU / CONCURRENCE — ÉMERGENT DES SYMBOLES

### Pas de nouveaux symboles — tout émerge de `|` et `~`

| Concept | Symboles | Émergence | CPU/GPU |
|---------|----------|-----------|---------|
| **Green thread** | `\| {}` | Pipe + Body = spawn | CPU: créer un fiber/coroutine |
| **GPU kernel** | `~ {}` | Wave + Body = broadcast exec | GPU: lancer N threads SIMT |
| **Join (attendre)** | `\| ?` | Pipe + Measure = attendre résultat | CPU: join/await |
| **Sync broadcast** | `~ ?` | Wave + Measure = synchroniser | GPU: barrier |
| **Yield** | `<~` | Out + Wave = céder le contrôle | CPU: yield/suspend |
| **Resume** | `>~` | In + Wave = reprendre | CPU: resume coroutine |
| **Fence** | `!\|` | Not + Pipe = bloquer le passage | CPU: memory fence |
| **CAS** | `?=` | Measure + Assign = compare-and-swap | CPU: atomic CAS |
| **Pipeline** | `\| \| \|` | Pipe chain = pipeline | CPU: instruction pipeline |
| **Fan-out** | `\|~` | Pipe → Wave = un vers plusieurs | GPU: scatter |
| **Fan-in** | `~\|` | Wave → Pipe = plusieurs vers un | GPU: reduce/gather |
| **Parallel map** | `~ (f) []` | Wave + Function + Collection | GPU: parallel map |

### Exemples

```
// Spawn un green thread
handle = | {
    heavy_compute()
};

// Attendre le résultat
result = handle | ?;                   // pipe + measure = join

// Lancer un calcul GPU (broadcast sur N threads)
results = data ~ (item) {             // wave + function + body
    transform(item)                    // chaque item en parallèle
};

// Pipeline : 3 étapes en parallèle
data | parse | validate | store;

// Fan-out : un producteur, N consommateurs
source |~ [consumer1, consumer2, consumer3];

// Fan-in : N producteurs, un résultat
[sensor1, sensor2, sensor3] ~| aggregate;

// Memory fence
!|;                                    // fence : tout ce qui est avant
                                       // est visible par tout ce qui est après

// Compare-and-swap atomique
old_val ?= (expected, new_val);        // CAS : si old == expected → new
```

---

## 8. `..` = ACCÈS PARENT (COMME cd ..)

| Symbole | Décomposition | Sens |
|---------|---------------|------|
| `.` | accéder | Descendre d'un niveau (accéder à un champ) |
| `..` | accéder + accéder | Remonter d'un niveau (accéder au parent) |
| `...` | accéder × 3 | Rest/spread (capturer le reste) |

```
// . = descendre
obj.field.subfield;              // descendre dans obj → field → subfield

// .. = remonter
{
    inner = {
        x = 5;
        ..y;                     // accéder à y dans le scope parent
    };
};

// ... = rest (spread)
[first, second, ...rest] = list;  // capturer le reste (déjà en v4.1)

// Combinaison avec :: (path)
::module..;                       // remonter au module parent
```

---

## 9. `><` — BREAK RÉVISÉ

### PEG update

```peg
# v6.0 : break = >< (entrer dans sortir)
# >> libéré pour usage futur
BreakStmt        <- '><' Semicolon

# Note : >< ne conflit avec rien
# > suivi de < n'est pas un opérateur existant
# >= existe mais c'est différent (>= vs ><)
```

### Exemples avancés : `}:{}` (mesurer la sortie d'un bloc)

```
// Boucle avec récupération de l'état de sortie
counter = 0;
loop_result = counter ?? {
    (counter > 100) : ><;           // break (entrer dans sortir)
    # err? : >< # err;              // break avec erreur
    _ : { counter += 1; };
} ? {                                // ← mesurer COMMENT la boucle s'est terminée
    + : print("boucle terminée normalement");
    # err? : print("boucle terminée en erreur: {err}");
};

// Le ?? retourne un spin selon comment il s'est terminé
// >< seul → spin + (sortie normale)
// >< # expr → spin # (sortie erreur)
// Pas de >< → le stream s'est épuisé → spin - (fini naturellement)
```

---

## 10. SUCRE SYNTAXIQUE — TABLE COMPLÈTE

| Symbole | Sucre | Catégorie |
|---------|-------|-----------|
| `+` | `true` | Spin |
| `-` | `false` | Spin |
| `*` | `any` | Spin |
| `_` | `void` | Spin |
| `#` | `err` | Spin |
| `<<` | `return` | Direction |
| `><` | `break` | Direction |
| `~` (préfixe) | `mut` | Déclaration |
| `**` | `clone` | Mémoire |
| `:>` | `as` | Type |
| `??` | `loop` | Contrôle |
| `.?` | `inspect` | Accès |
| `type` | — | Mot (irremplaçable) |
| `trait` | — | Mot (irremplaçable) |

---

## 11. TABLEAU DE PRÉDICTION (pour théoriser)

### Comment prédire une nouvelle combinaison

```
Règle : sens(A + B) = action(A) appliquée_à action(B)
```

### Combinaisons réservées et leur sens prédit

| Combinaison | Prédiction | Sens logique | Utile ? |
|---|---|---|---|
| `<>` | out + in | Sortir puis entrer = échanger / swap | ⚠️ Peut-être |
| `>>` | in + in | Entrer profond = scope imbriqué / continue | ⚠️ Peut-être |
| `--` | neg + neg | Double négatif = affirmer ? décrémenter ? | ⚠️ Ambigu |
| `##` | err + err | Double erreur = panic / unrecoverable | ✓ Probable |
| `.!` | access + not | Accès inversé = supprimer un champ ? | ⚠️ Peut-être |
| `?!` | measure + not | Mesurer le contraire = assert (vérifier que c'est PAS) | ✓ Probable |
| `!\|` | not + pipe | Bloquer le pipe = fence / barrière | ✓ Probable |
| `!~` | not + wave | Bloquer l'onde = cancel broadcast | ⚠️ Peut-être |
| `!?` | not + measure | Ne pas mesurer = lazy / defer | ⚠️ Peut-être |
| `?:` | measure + bind | Mesure liée = ternaire compact | ✓ Probable |
| `~\|` | wave + pipe | Broadcast → Pipe = reduce / gather | ✓ Probable |
| `\|~` | pipe + wave | Pipe → Broadcast = scatter / fan-out | ✓ Probable |
| `<~` | out + wave | Sortir l'onde = yield | ✓ Probable |
| `>~` | in + wave | Entrer l'onde = resume | ✓ Probable |
| `?=` | measure + assign | Comparer et assigner = CAS atomique | ✓ Probable |
| `<.` | out + access | Sortir par accès = remonter (parent) | ⚠️ Redondant avec `..` |
| `::()` | path + group | Chemin avec paramètres = générique | ✓ Déjà utilisé |
| `::{}`×N | N niveaux de namespace | Accès profond | ✓ Émerge naturellement |

### Template pour théoriser

```
Combinaison : ____
Atome 1     : ____ (sens : ____)
Atome 2     : ____ (sens : ____)
Prédiction  : ____ appliqué à ____
Sens logique: ____
Conflit ?   : avec ____ ? (oui/non)
Utile ?     : (oui / peut-être / non)
Priorité    : (v6 / futur / jamais)
```

---

## 12. COMPLÉTION AUTOMATIQUE ET AST SANS ERREUR

### Principe : chaque combinaison de symboles est valide

L'AST ne produit JAMAIS d'erreur de syntaxe — seulement des erreurs de **contexte** :

```
// "erreur" classique : diviser un string par un int
result = "hello" / 5;

// Dans 3.42 : PAS une erreur de syntaxe
// L'AST est valide : BinaryNode(StringLiteral, DIV, IntLiteral)
// Le compilateur AVERTIT : "les sphères ont des ranges incompatibles"
// Mais le code compile → le résultat est un spin # (erreur de contexte)
// Le développeur peut mesurer : result ? { # err? : handle; }
```

### Complétion

```
// Le dev tape : x ?
// L'autocomplétion propose :
//   x ? { }     → MEASURE avec branches (le plus fréquent)
//   x ? ;       → MEASURE anonyme (collapse simple)
//   x ?? { }    → LOOP (mesure continue)
//   x ?.        → INTROSPECT

// Le dev tape : <<
// L'autocomplétion propose :
//   << + expr ;  → RETURN positif
//   << - expr ;  → RETURN négatif
//   << * ;       → RETURN void
//   << # expr ;  → RETURN erreur
//   << { } ;     → RETURN bloc

// Le dev tape : |
// L'autocomplétion propose :
//   | expr       → PIPE vers expression
//   | { }        → PIPE vers bloc (green thread)
//   |~           → FAN-OUT
//   |?           → PIPE conditionnel
```

### Visualisation AST en temps réel

Chaque caractère tapé met à jour l'arbre AST visible en graphe. Les nœuds ont les noms de la table (RETURN, MEASURE, PIPE...). Les erreurs de contexte sont visibles comme des nœuds `#` dans l'arbre. Le diff entre deux versions du code = diff entre deux arbres = visible comme un git diff mais sur un graphe.

---

## 13. VÉRIFICATION D'INCOHÉRENCES

### `><` comme break — impact

| Vérifié | Résultat |
|---------|----------|
| Conflit avec `>=` | ✓ Non : `>=` est CmpOp, `><` est BreakStmt (contexte différent) |
| Conflit avec `>` puis `<` dans une expression | ✓ Non : `a > < b` n'est pas valide (< après > sans opérande entre les deux) |
| PEG disambiguation | ✓ `BreakStmt <- '><' Semicolon` — ne matche QUE `><;` en début de statement |
| Lisibilité | ✓ `><` se lit "entrer dans sortir" → intuitif une fois appris |

### Sphère de Riemann pour division par zéro

| Vérifié | Résultat |
|---------|----------|
| Mathématiquement cohérent | ✓ Oui : 1/0 = ∞ sur la sphère de Riemann (géométrie projective) |
| Perte d'axiomes | ⚠️ ∞ n'a pas d'inverse additif (∞ + ∞ ≠ défini). Les nombres étendus ne forment pas un corps (field) |
| Impact pratique | ✓ Le résultat est un pôle → spin `#` avec valeur `∞` → le dev DOIT mesurer le spin → pas de crash silencieux |
| Implémentation | Utiliser IEEE 754 `Inf` pour les floats, valeur spéciale `POLE` pour les ints |

### Bubble-up et sécurité mémoire

| Vérifié | Résultat |
|---------|----------|
| Dangling reference | ✓ Impossible : MOVE sémantique (pas de copie, pas de ref) |
| Use-after-free | ✓ Impossible : une fois moved, la variable n'existe plus dans l'arène source |
| Memory leak | ✓ Impossible : l'arène pending est free à la fin du scope caller |
| Performance | ⚠️ Coût : un bit de flag par arène + delayed free. Négligeable. |
| Thread safety | ⚠️ Si le résultat est passé à un autre thread via `\|`, l'arène pending doit survivre jusqu'au `.` access du thread cible. Résolu : l'arène est attachée à la sphère résultat, pas au scope. |

### `*` = any (pas void)

| Ancien sens | Nouveau sens | Impact |
|---|---|---|
| `*` = neutre/void | `*` = any/tout | `_` devient void à la place |
| `*` seul = nil | `*` seul = wildcard/any match | Plus cohérent avec `*` en glob, regex, etc. |
| `_ ` = wildcard | `_` = void/rien/ignorer | Separation nette : `*` = quelque chose, `_` = rien |

Changement dans le sucre :
- `*` → sucre `any` (était `void`)
- `_` → sucre `void` (était `any`)
- Les rôles s'inversent. C'est PLUS logique car `*` en informatique = wildcard universellement.

---

## 14. RÉCAPITULATIF v6.0 vs v5.1

| Aspect | v5.1 | v6.0 |
|--------|------|------|
| Break | `>>` | `><` (entrer dans sortir) |
| `>>` | break | **réservé** (futur: continue/dive) |
| `*` sens | neutre/void | **any/tout** (wildcard) |
| `_` sens | wildcard | **void/rien** |
| Variables internes | inaccessibles après return | **bubble-up via `.`** |
| Division par 0 | erreur | **pôle sur sphère de Riemann** (spin #) |
| Types | explicites | **sphère + range** (tout est sphère) |
| Thread/GPU | `\|` pipe, `~` broadcast | + `\|{}` spawn, `~{}` kernel, `<~` yield, `>~` resume, `!\|` fence, `?=` CAS |
| `..` | range (2..10) | **double sens** : range (2..10) ET parent access (..field) |
| Erreur syntaxe | possible | **impossible** — tout est AST valide, erreurs = contexte |

---

## ANNEXE : TABLEAU VIERGE POUR THÉORISER

Copie ce template pour explorer de nouvelles combinaisons :

```
┌─────────────────────────────────────────────────────┐
│ COMBINAISON : ___  +  ___  =  ___                   │
├─────────────────────────────────────────────────────┤
│ Atome 1 : ___ (sens: _____________)                 │
│ Atome 2 : ___ (sens: _____________)                 │
│ Prédiction : _______________ appliqué à ____________ │
│ Sens émergent : ________________________________     │
│ Conflit avec : _________ ? □ oui  □ non             │
│ Implémentable ? □ oui  □ plus tard  □ non           │
│ Utile ?        □ v6  □ futur  □ jamais              │
│ Lien processeur : ________________________________  │
│ Exemple de code :                                    │
│   ________________________________________________  │
│   ________________________________________________  │
│ Notes : _________________________________________    │
└─────────────────────────────────────────────────────┘
```
