# 3.42 — CHEAT SHEET FRACTAL v7.0
## "16 bosons. Fermions + Gluons. 1 règle. Émergence réelle."

---

## RÈGLE UNIQUE

```
A B = A appliqué à B
A B C = A(B(C))
A B C D = A(B(C(D)))
...profondeur infinie, même règle.
```

---

## NIVEAU 0 — LES 16 QUARKS (instructions CPU élémentaires)

Chaque quark est **ancré** dans une opération processeur réelle.
Le mapping CPU est une **intuition directrice**, pas un ISA littéral.
La composition des verbes produit des **patterns** que le compilateur
reconnaît et traduit en instructions optimales.

```
SYMBOLE  VERBE       CPU                  SENS HUMAIN
───────  ──────────  ───────────────────  ─────────────────────
  <      SORTIR      STORE (écrire)       pousser vers l'extérieur
  >      ENTRER      LOAD (lire)          tirer vers l'intérieur
  .      ACCÉDER     LOAD offset          lire un champ
  ?      MESURER     CMP + BRANCH         observer, décider
  !      INVERSER    NOT (flip bits)      nier, bloquer
  |      CONNECTER   reg → reg (bus)      relier, transférer
  ~      ONDULER     SIMT broadcast       diffuser, muter
  :      LIER        MOV dest,src         attacher, typer
  =      NOMMER      STORE reg            assigner un nom
  ;      FINIR       fin cycle clock      terminer l'instruction
  +      AFFIRMER    flag POSITIVE         vrai, succès
  -      NIER        flag NEGATIVE         faux, échec
  *      GÉNÉRALISER don't care bits      tout, any, wildcard
  #      CORROMPRE   flag OVERFLOW         erreur, corruption
  _      IGNORER     NOP / zero reg       rien, void
  @      EMPRUNTER   indirect (ptr de)    référence sans posséder
```

**Conteneurs (gluons)** — groupement mémoire :

```
  {}     CORPS       stack frame          bloc, scope, arène
  ()     GROUPE      registre group       paramètres, priorité
  []     COLLECTION  base + index         liste, map
  ""     TEXTE       data segment         chaîne interpolée ({expr}, \n)
  ''     BRUT        data segment         texte brut (pas d'expansion)
```

---

## NIVEAU 1 — COMBINAISONS DOUBLES (2 quarks)

### Direction : < et >

```
COMBO   COMPOSITION              CPU ÉMIS             RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 <<     SORTIR(SORTIR)           RET                  exit function     return
 ><     ENTRER(SORTIR)           JMP (out of scope)   quitter contexte  break
 >>     ENTRER(ENTRER)           JMP LOOP_START       re-entrer boucle  continue
 <>     SORTIR(ENTRER)           XCHG                 échanger          (réservé)

 Intuition : < = pousser dehors. << = pousser dehors × 2 = sortir complètement.
             > = tirer dedans. >< = tirer puis pousser = entrer pour sortir = quitter.
             >> = entrer × 2 = re-entrer = revenir au début = continue.
             Le compilateur pattern-match << → RET, >< → JMP out, >> → JMP back.
```

### Mesure : ?

```
COMBO   COMPOSITION              CPU ÉMIS             RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 ?{}    MESURER(CORPS)           CMP + JNE/JE         if/switch         —
 ??     MESURER(MESURER)         CMP + JNE + JMP back boucle while      loop
 ?!     MESURER(INVERSER)        CMP + JE → TRAP      assert (doit!)    assert
 ?=     MESURER(NOMMER)          CMPXCHG (atomique)   compare-and-swap  cas
 ?+     MESURER(AFFIRMER)        TEST + JNZ           est positif ?     —
 ?-     MESURER(NIER)            TEST + JS            est négatif ?     —
 ?#     MESURER(CORROMPRE)       TEST + JO            est erreur ?      —
 ?_     MESURER(IGNORER)         TEST + JZ            est void ?        —
 ?@     MESURER(EMPRUNTER)       TEST ptr             est emprunté ?    —

 Intuition : ? = observer une fois. ?? = observer en boucle (re-mesurer).
             ?! = observer l'inverse = "si c'est PAS ça → erreur" = assert.
             Le compilateur pattern-match ?? → boucle CMP+JMP backward.
```

### Connexion : | et ~

```
COMBO   COMPOSITION              CPU ÉMIS / RUNTIME   RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 |{}    CONNECTER(CORPS)         spawn fiber          green thread      —
 ~{}    ONDULER(CORPS)           SIMT dispatch        kernel GPU        —
 ||     CONNECTER(CONNECTER)     OR (si l'un passe)   ou logique        or

 Note : || n'est PAS MOV(MOV). C'est un pattern émergent :
        connecter DEUX connexions = "l'une OU l'autre passe" = OR.
        Le compilateur émet directement : OR / conditional JMP.
 ~~     ONDULER(ONDULER)         multi-broadcast      parallèle pur     —
 |~     CONNECTER(ONDULER)       scatter              fan-out (1→N)     —
 ~|     ONDULER(CONNECTER)       gather               fan-in (N→1)     —
 |?     CONNECTER(MESURER)       futex/wait+CMP       attendre résultat —
 ~?     ONDULER(MESURER)         __syncthreads()      sync broadcast    —
```

### Mémoire : @ et **

```
COMBO   COMPOSITION              CPU                  RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 @      EMPRUNTER                LEA (load addr)      ref en lecture     —
 @@     EMPRUNTER(EMPRUNTER)     exclusive lock       ref en écriture    —
 **     GÉNÉRALISER(GÉNÉRALISER) memcpy               copie profonde    clone
```

### Accès : . et ..

```
COMBO   COMPOSITION              CPU                  RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 .x     ACCÉDER(x)               LOAD base+offset    lire un champ     —
 ..     ACCÉDER(ACCÉDER)         LOAD [base+offset]  remonter (parent)  —
 ...    (destructuring only)     multi-load           rest/spread        —
 .?     ACCÉDER(MESURER)         LOAD + CMP           introspecter      inspect
 .!     ACCÉDER(INVERSER)        LOAD + NOT           supprimer champ   (futur)
```

### Liaison : : et =

```
COMBO   COMPOSITION              CPU                  RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 ::     LIER(LIER)               MOV→MOV (chaîné)    namespace/chemin  —
 :>     LIER(ENTRER)             MOV + CONVERT        cast/transformer  as
 :=     LIER(NOMMER)             MOV + STORE          définir constant  (futur)
 +=     AFFIRMER + NOMMER        ADD + STORE          incrémenter       —
 -=     NIER + NOMMER            SUB + STORE          décrémenter       —
 *=     GÉNÉRALISER + NOMMER     MUL + STORE          mul-assigner      —
```

### Spin : + - * # _

```
COMBO   COMPOSITION              CPU                  RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 +      AFFIRMER                 flag POSITIVE         vrai/succès      true
 -      NIER                     flag NEGATIVE         faux/échec       false
 *      GÉNÉRALISER              don't care            any/wildcard     any
 #      CORROMPRE                flag OVERFLOW         erreur           err
 _      IGNORER                  NOP                   void/rien        void
 ++     AFFIRMER(AFFIRMER)       ADD (accumuler)      concaténer        —
 ##     CORROMPRE(CORROMPRE)     double fault          panic (fatal)     panic
 !+     INVERSER(AFFIRMER)       NOT + flag            not true          —
 !-     INVERSER(NIER)           NOT + flag            not false         —
 !!     INVERSER(INVERSER)       NOT(NOT) = identity   double nég=oui   force
```

### Contrôle : ; et !

```
COMBO   COMPOSITION              CPU                  RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 ;      FINIR                    end cycle             fin instruction   —
 !|     INVERSER(CONNECTER)      MFENCE               barrière mémoire  fence
 !~     INVERSER(ONDULER)        cancel broadcast     annuler diffusion —
 !?     INVERSER(MESURER)        PUSH cleanup stack   defer (LIFO)      defer
```

### Yield : < et ~

```
COMBO   COMPOSITION              CPU                  RÉSULTAT          SUCRE
─────   ────────────────────     ───────────────────  ────────────────  ──────
 <~     SORTIR(ONDULER)          yield fiber          céder le contrôle yield
 >~     ENTRER(ONDULER)          resume fiber         reprendre         resume
 <|     SORTIR(CONNECTER)        close pipe           fermer connexion  —
 >|     ENTRER(CONNECTER)        open pipe            ouvrir connexion  —
```

---

## NIVEAU 2 — COMBINAISONS TRIPLES (3 quarks)

Même règle : `A B C = A(B(C))`. On compose les verbes du niveau 1.

### Direction + Spin (return/break typés)

```
COMBO     COMPOSITION                         RÉSULTAT                   SUCRE
───────   ──────────────────────────────      ────────────────────────   ──────
 << +     SORTIR(SORTIR(AFFIRMER))            return succès              return true
 << -     SORTIR(SORTIR(NIER))               return échec               return false
 << #     SORTIR(SORTIR(CORROMPRE))          return erreur              return err
 << _     SORTIR(SORTIR(IGNORER))            return void                return void
 << *     SORTIR(SORTIR(GÉNÉRALISER))        return any (wildcard)      —
 >< +     ENTRER(SORTIR(AFFIRMER))           break avec succès          break true
 >< #     ENTRER(SORTIR(CORROMPRE))          break avec erreur          break err
 >< -     ENTRER(SORTIR(NIER))              break avec échec           break false
 >> +     ENTRER(ENTRER(AFFIRMER))          continue si positif         continue true
 >> #     ENTRER(ENTRER(CORROMPRE))         continue sur erreur         continue err
```

### Direction + Corps (spawn/scope)

```
COMBO     COMPOSITION                         RÉSULTAT                   CPU
───────   ──────────────────────────────      ────────────────────────   ────────
 | {}     CONNECTER(CORPS)                    spawn green thread         create fiber
 ~ {}     ONDULER(CORPS)                      lancer kernel GPU          SIMT dispatch
 << {}    SORTIR(SORTIR(CORPS))               return d'un bloc           RET + block eval
 >< {}    ENTRER(SORTIR(CORPS))               break d'un bloc            JMP + block eval
 >> ;     ENTRER(ENTRER)                      continue (next iter)       JMP LOOP_START
 !? {}    INVERSER(MESURER(CORPS))            defer bloc (cleanup)       PUSH defer stack
```

### Mesure + Mesure + X (boucles avancées)

```
COMBO     COMPOSITION                         RÉSULTAT                   EXEMPLE
───────   ──────────────────────────────      ────────────────────────   ─────────────
 ?? {}    MESURER(MESURER(CORPS))              boucle avec body           while { }
 ?? +     MESURER(MESURER(AFFIRMER))           boucler tant que vrai      while true
 ?? ?     MESURER(MESURER(MESURER))            triple mesure (inutile?)   sur-observation
 ?? !     MESURER(MESURER(INVERSER))           boucler jusqu'à inversion  until
```

### Connexion composée (patterns concurrence)

```
COMBO     COMPOSITION                         RÉSULTAT                   PATTERN
───────   ──────────────────────────────      ────────────────────────   ───────────
 |~|      CONNECTER(ONDULER(CONNECTER))       pipe→broadcast→pipe        map-reduce
 ~|~      ONDULER(CONNECTER(ONDULER))         broadcast→pipe→broadcast   mesh/réseau
 ||~      CONNECTER(CONNECTER(ONDULER))       or → broadcast             diffuser si or
 |?|      CONNECTER(MESURER(CONNECTER))       pipe conditionnel          filter
 ~?~      ONDULER(MESURER(ONDULER))           sync barrier broadcast     sync mesh
 !|!      INVERSER(CONNECTER(INVERSER))       fence double               full barrier
```

### Accès composé (navigation profonde)

```
COMBO     COMPOSITION                         RÉSULTAT                   ÉQUIVALENT
───────   ──────────────────────────────      ────────────────────────   ──────────
 ..x      ACCÉDER(ACCÉDER(x))                parent.x                   cd ../x
 ..a.b    ACCÉDER(ACCÉDER(a)).ACCÉDER(b)     parent.a.b                 cd ../a/b
 .?.      ACCÉDER(MESURER(ACCÉDER))          introspecter puis accéder   reflect.get
 a::b::c  a LIER(LIER) b LIER(LIER) c       chemin profond             namespace::sub::item
```

### Mémoire composée

```
COMBO     COMPOSITION                         RÉSULTAT                   CPU
───────   ──────────────────────────────      ────────────────────────   ───────
 **@      GÉNÉRALISER(GÉNÉRALISER(EMPRUNT))   copier depuis une ref      memcpy(ptr)
 @.x      EMPRUNTER(ACCÉDER(x))              ref vers un champ          LEA [base+off]
 @@~      EMPRUNTER(EMPRUNTER(ONDULER))       ref exclusive mutable      lock + SIMT
```

---

## NIVEAU 3 — COMBINAISONS QUADRUPLES (4 quarks)

```
COMBO          COMPOSITION                                  RÉSULTAT
────────────   ──────────────────────────────────────       ──────────────────────
 << + expr ;   SORTIR(SORTIR(AFFIRMER(expr))) FINIR        return positif complet
 << # expr ;   SORTIR(SORTIR(CORROMPRE(expr))) FINIR       return erreur complet
 >< # expr ;   ENTRER(SORTIR(CORROMPRE(expr))) FINIR       break erreur complet
 << + {}   ;   SORTIR(SORTIR(AFFIRMER(CORPS))) FINIR       return bloc positif
 ?? + {}   ;   MESURER(MESURER(AFFIRMER(CORPS))) FINIR     boucle positive
 |~ [] ;       CONNECTER(ONDULER(COLLECTION)) FINIR        scatter sur une liste
 ~| [] ;       ONDULER(CONNECTER(COLLECTION)) FINIR        gather depuis une liste
 ? ! ? expr    MESURER(INVERSER(MESURER(expr)))             assert : expr DOIT être faux
 > < > expr    ENTRER(SORTIR(ENTRER(expr)))                traverser puis re-entrer = callback
 < > < expr    SORTIR(ENTRER(SORTIR(expr)))                swap-exit = yield conditionnel
 < ~ > expr    SORTIR(ONDULER(ENTRER(expr)))               yield le flux entrant
```

---

## NIVEAU N — LA RÈGLE DE PRÉDICTION

Pour **n'importe quelle** séquence de symboles jamais vue :

```
1. Lis de gauche à droite
2. Chaque symbole est un VERBE (action)
3. Il s'applique à tout ce qui est à sa droite
4. Le sens final = composition récursive des verbes

FORMULE :  sens(S₁ S₂ ... Sₙ) = S₁(S₂(...(Sₙ)...))
```

### Exemple : prédire `< ~ | > expr`

```
Étape 1 : < = SORTIR
Étape 2 : ~ = ONDULER
Étape 3 : | = CONNECTER
Étape 4 : > = ENTRER
Étape 5 : expr = la donnée

Composition : SORTIR(ONDULER(CONNECTER(ENTRER(expr))))
= sortir l'onde de la connexion qui entre expr
= yield le broadcast du pipe entrant
= céder le résultat d'un pipeline de réception
= pattern "sink with yield" en concurrence

CPU : STORE(SIMT(MOV(LOAD(data))))
= charger la donnée, la transférer, la broadcaster, la stocker
```

### Exemple : prédire `? @ ! ~ expr`

```
MESURER(EMPRUNTER(INVERSER(ONDULER(expr))))
= observer la référence de l'inverse de la diffusion de expr
= vérifier qu'une ref n'est PAS en cours de broadcast
= "is this reference safe from concurrent mutation?"
= race condition check émergent

CPU : CMP(LEA(NOT(SIMT(data))))
= tester si l'adresse pointe vers des données non-broadcastées
```

---

## TABLE DE LECTURE RAPIDE

### Les 10 patterns les plus courants

```
PATTERN          SENS              SUCRE           EXEMPLE
──────────────   ────────────────  ──────────────  ─────────────────────────
x = 42;          assigner          —               x = 42;
<< + expr;       return ok         return expr     << + result;
>< ;             break             break           >< ;
>> ;             continue          continue        >> ;
!? { }           defer (cleanup)   defer           !? { file.close(); };
expr ? { }       if/match          —               x ? { + : a; - : b; };
expr ?? { }      while/loop        loop            i ?? { (i<10) : i+=1; };
a | b            pipe (async)      —               data | parse | store;
a >< b           pass-through      —               data >< parse >< store;
** expr          copie profonde    clone           copy = ** original;
expr :> Type     cast              as              x :> f64;
..field          parent access     —               ..config;
'raw text'       texte brut        —               path = 'C:\Users';
```

### Spins (les 5 états d'une sphère)

```
SYMBOLE   SENS              SUCRE     CPU FLAG        EXEMPLE
───────   ────────────────  ────────  ──────────────  ────────
  +       positif/vrai      true      POSITIVE        flag = +;
  -       négatif/faux      false     NEGATIVE        flag = -;
  *       any/tout          any       DON'T CARE      match = *;
  #       erreur            err       OVERFLOW         err = #;
  _       void/rien         void      ZERO             _ = compute();
```

---

## COMMENT LIRE N'IMPORTE QUEL CODE 3.42

```
1. Repère les QUARKS (symboles de base)
2. Lis chaque quark comme un VERBE
3. Compose de gauche à droite : A(B(C(...)))
4. Le contexte (statement/expression) affine le sens
5. En cas de doute, le sucre syntaxique traduit
```

### Exercice : décoder `data |~ [a, b] ~| result;`

```
data               → la donnée source
|~                 → CONNECTER(ONDULER) = fan-out (scatter)
[a, b]             → vers la collection [a, b]
~|                 → ONDULER(CONNECTER) = fan-in (gather)
result             → dans result

= scatter data vers a et b, puis gather les résultats dans result
= map-reduce émergent en 1 ligne
```

### Exercice : décoder `<< + { x = f(); x.debug; x };`

```
<<                 → SORTIR(SORTIR) = return
+                  → AFFIRMER = positif
{ x = f(); ... }   → CORPS = bloc
x = f()            → appeler f, stocker dans x
x.debug            → ACCÉDER debug = bubble-up (MOVE la variable interne)
x                  → valeur finale du bloc

= return positif d'un bloc qui appelle f(), récupère le debug via bubble-up,
  et retourne x
```

---

## LA PYRAMIDE FRACTALE

```
NIVEAU 0 : 16 bosons + 5 gluons  → instructions CPU atomiques + conteneurs
NIVEAU 1 : ~60 doubles           → opérations composées (return, break, continue, if, pipe...)
NIVEAU 2 : ~200 triples          → patterns avancés (map-reduce, defer, yield...)
NIVEAU 3 : ~500 quadruples       → patterns émergents (race check, mesh, callback...)
NIVEAU N : ∞                      → le compilateur applique la même règle à chaque niveau

Chaque niveau est PRÉDICTIBLE depuis le niveau précédent.
Rien n'est hardcodé. Tout émerge de 16 bosons, 5 gluons, et 1 règle.
Le compilateur pattern-match les compositions connues → optimise en instructions CPU.
Les compositions inconnues → interprétation générique → toujours valide.
```

---

## GLOSSAIRE SUCRE ↔ SYMBOLE

```
SUCRE          SYMBOLE     NIVEAU    CATÉGORIE
─────────────  ──────────  ────────  ──────────
return         <<          1         direction
break          ><          1         direction
continue       >>          1         direction      ← v7.0
defer          !?          1         contrôle       ← v7.0
true           +           0         spin
false          -           0         spin
void           _           0         spin
any            *           0         spin
err            #           0         spin
loop           ??          1         mesure
clone          **          1         mémoire
as             :>          1         liaison
inspect        .?          1         accès
assert         ?!          1         mesure
fence          !|          1         contrôle
panic          ##          1         spin
or             ||          1         connexion
yield          <~          1         direction
resume         >~          1         direction
force          !!          1         contrôle
```
