# AUDIT PEG v6.0 — Résultats

## TESTS DE PARSING (22 cas)

| # | Input | Règle attendue | Parse OK ? | Notes |
|---|-------|---------------|------------|-------|
| 1 | `>< ;` | BreakStmt (no spin, no expr) | ✓ | `><` + Semicolon |
| 2 | `>< + result ;` | BreakStmt (spin +, expr result) | ✓ | SpinReturn = + !'=' |
| 3 | `>< # "error" ;` | BreakStmt (spin #, expr "error") | ✓ | |
| 4 | `>< expr ;` | BreakStmt (no spin, expr) | ✓ | Spin? = none, expr = ok |
| 5 | `>< + ;` | BreakStmt (spin +, no expr) | ✓ | Break positif sans valeur |
| 6 | `<< + result ;` | ReturnStmt (spin +, expr result) | ✓ | Inchangé depuis v5.1 |
| 7 | `data >< transform ;` | ExprStmt: PassThrough(data, transform) | ✓ | BreakStmt échoue (data avant ><) |
| 8 | `a >< b >< c ;` | ExprStmt: PassThrough(PassThrough(a,b), c) | ✓ | Left-associative |
| 9 | `a \| b >< c ;` | ExprStmt: Pipe(a, PassThrough(b, c)) | ✓ | Pipe > PassThrough en précédence |
| 10 | `a >< b \| c ;` | ExprStmt: Pipe(PassThrough(a,b), c) | ✓ | |
| 11 | `0..10` | RangeExpr(0, 10) | ✓ | Infixe entre BroadcastExpr |
| 12 | `a..*` | RangeExpr(a, SpinLiteral(*)) | ✓ | Open range vers any |
| 13 | `(0..10) ?? { ... }` | MeasureExpr: ContinuousMeasure sur GroupExpr(RangeExpr) | ✓ | |
| 14 | `..field ;` | ExprStmt: ParentAccessExpr(field) | ✓ | PrimaryExpr, prefix |
| 15 | `.. ;` | ExprStmt: ParentAccessExpr(none) | ✓ | Parent scope lui-même |
| 16 | `obj.field ;` | ExprStmt: PostfixExpr(obj, DotAccess(field)) | ✓ | '.' !'.' !'?' |
| 17 | `x = _ ;` | Assignment: x = SpinLiteral(_) | ✓ | _ ![a-zA-Z0-9_] |
| 18 | `x = void ;` | Assignment: x = SpinLiteral(void) | ✓ | Sugar for _ |
| 19 | `x = any ;` | Assignment: x = SpinLiteral(any) | ✓ | Sugar for * |
| 20 | `x = * ;` | Assignment: x = SpinLiteral(*) | ✓ | *!'*' matches |
| 21 | `_foo = 5 ;` | Assignment: _foo = 5 | ✓ | _ suivi de 'f' → Identifier |
| 22 | `_ : action ;` (dans ?{}) | WildcardBranch | ✓ | Contexte MeasureBranch |

**22/22 PASS**

## TESTS DE CONFLITS (10 cas)

| # | Conflit potentiel | Résolution | OK ? |
|---|-------------------|------------|------|
| 1 | `><` stmt vs expr | BreakStmt essayé AVANT ExprStmt. `>< ;` = break. `a >< b ;` = ExprStmt car `a` ne matche pas BreakStmt | ✓ |
| 2 | `<<` return vs shift | ReturnStmt AVANT ExprStmt. `<< + x;` = return. `a << b;` = ExprStmt(shift) | ✓ |
| 3 | `_` void vs wildcard | Contextes différents (PrimaryExpr vs MeasureBranch) | ✓ |
| 4 | `_` void vs identifier | `'_' ![a-zA-Z0-9_]` → `_` seul = void, `_foo` = identifier | ✓ |
| 5 | `..` parent vs range | ParentAccessExpr = PrimaryExpr (prefix). RangeExpr = infixe. Zéro overlap | ✓ |
| 6 | `.` access vs `..` parent | DotAccess a `!'.'` → rejette `..` | ✓ |
| 7 | `*` any vs mul vs copy | ** > * expr > a*b > * seul (longueur + ordered choice) | ✓ |
| 8 | `>=` cmp vs `><` break | `>=` = CmpOp, `><` = BreakStmt/PassThrough. Tokens différents | ✓ |
| 9 | `>>` shift vs ancien break | `>>` n'est plus dans aucun Statement. ShiftOp only | ✓ |
| 10 | `any`/`void` keyword vs ident | Keyword check via `![a-zA-Z0-9_]`. `anyone` = identifier OK | ✓ |

**10/10 PASS**

## EDGE CASES (6 cas)

| # | Case | Comportement | OK ? |
|---|------|-------------|------|
| 1 | `_ = compute();` | Assignment: `_` = Identifier (pas SpinLiteral car dans Assignment, pas PrimaryExpr). Semantique = "discard" | ✓ Design |
| 2 | `...rest` | `...` n'est PAS `..` + `.`. ParentAccessExpr matche `..` puis Identifier? cherche un ident. `r` commence un ident → ParentAccessExpr(`rest`). MAIS : dans ListDestructure, `'...' Identifier` est un token séparé. Conflit? Non : ListDestructure est dans Statement (DestructAssign), pas Expression. | ✓ |
| 3 | `a .. b` (espaces) | RangeExpr: BroadcastExpr `Spacing '..' Spacing` BroadcastExpr. `a` space `..` space `b` → range. Idem sans espaces : `a..b` → range car `.` est postfix DotAccess qui rejette `..` (!'.'). Donc l'infixe RangeExpr gagne | ✓ |
| 4 | `;;` (double semicolon) | Statement vide? Non : `Statement` ne matche pas `;` seul. Le second `;` serait un parsing error. Acceptable — FINIR(FINIR) = NOP conceptuel mais pas dans la PEG | ⚠️ Note |
| 5 | `>>` seul en statement | `>> ;` → pas BreakStmt (c'est `><`). Pas ReturnStmt (c'est `<<`). Assignment? Non. ExprStmt: Expression → `>>` n'est pas un PrimaryExpr ou UnaryOp → FAIL. Erreur de contexte. Correct (réservé futur) | ✓ |
| 6 | `><` suivi de `>=` | `>< >= ;` → BreakStmt: `><` puis SpinReturn? → SpinPattern essaie `>` mais `>` n'est PAS un SpinPattern (`+ - * #`). Donc SpinReturn = none. Expression? → `>=` n'est pas une expression valide seule → none. Semicolon → FAIL (`;` pas trouvé car `>=` est là). → BreakStmt FAIL. → ExprStmt: `><` n'est pas parsable comme expression primaire → FAIL. → Parsing error. Correct. | ✓ |

**5/6 PASS, 1 NOTE** (le `;;` est un cas cosmétique)

## COUVERTURE DES CONCEPTS v6.0

| Concept | Règle PEG | ✓ |
|---------|----------|---|
| return (`<<`) | ReturnStmt | ✓ |
| break (`><`) | BreakStmt | ✓ |
| pass-through (`a >< b`) | PassThroughExpr | ✓ |
| if (`? {}`) | SingleMeasure + MeasureBlock | ✓ |
| while/loop (`?? {}`) | ContinuousMeasure + MeasureBlock | ✓ |
| pipe (`\|`) | PipeExpr | ✓ |
| broadcast (`~`) | BroadcastExpr | ✓ |
| range (`a..b`) | RangeExpr | ✓ |
| copy (`**`) | CopyExpr | ✓ |
| cast (`:>`) | CastExpr | ✓ |
| mut (`~`) | MutDecl | ✓ |
| module (`::name {}`) | ModuleDecl | ✓ |
| type | TypeDecl | ✓ |
| trait | TraitDecl | ✓ |
| impl | ImplDecl | ✓ |
| thread (`\| {}`) | PipeExpr + BlockExpr | ✓ |
| GPU (`~ {}`) | BroadcastExpr + BlockExpr | ✓ |
| parent access (`..`) | ParentAccessExpr | ✓ |
| dot access (`.field`) | DotAccess | ✓ |
| introspect (`.?`) | DotMeasure | ✓ |
| void literal (`_`) | SpinLiteral | ✓ |
| any literal (`*`) | SpinLiteral | ✓ |
| sugar `true/false/void/any` | SpinLiteral | ✓ |
| struct literal | StructLiteral | ✓ |
| function literal | FunctionLiteral | ✓ |
| block (`{}`) | BlockExpr | ✓ |
| namespace (`::`) | ModulePath | ✓ |
| destructuring | DestructAssign | ✓ |
| annotations (`@`) | AnnotatedStatement | ✓ |
| borrow (`@`, `@@`) | BorrowSuffix | ✓ |
| slice (`list[a..b]`) | SliceAccess | ✓ |
| concat (`++`) | AddOp | ✓ |
| multi-spin return | SpinReturnType | ✓ |

**33/33 concepts couverts**

## RÈGLES DÉFINIES MAIS NON-RÉFÉRENCÉES

| Règle | Statut |
|-------|--------|
| `IdentList` | Définie, jamais utilisée | ⚠️ Réservée futur |
| `Block` (alias) | Utilisée par FunctionLiteral | ✓ |

## RÉSUMÉ

```
Parse tests     : 22/22 PASS
Conflict tests  : 10/10 PASS
Edge cases      : 5/6 PASS + 1 NOTE
Coverage        : 33/33 concepts
Dead rules      : 1 (IdentList — réservée)

VERDICT : PEG v6.0 VALIDÉE ✓
```

## CHANGEMENTS DEPUIS v5.1 (DIFF RÉSUMÉ)

1. `BreakStmt` : `'>>' Semicolon` → `'><' SpinReturn? Expression? Semicolon`
2. `PassThroughExpr` : NOUVEAU (a >< b, infixe)
3. `RangeExpr` : NOUVEAU (a..b, infixe — remplace RangeLiteral qui était dead code)
4. `ParentAccessExpr` : NOUVEAU (..field, prefix dans PrimaryExpr)
5. `SpinLiteral` : + `'_' ![a-zA-Z0-9_]` et `'any' ![a-zA-Z0-9_]`
6. `Keyword` : + `'any'`
7. `DotAccess` : + `!'.'` (rejette `..`)
8. `PipeExpr` : pointe vers PassThroughExpr (plus BroadcastExpr)
9. Supprimé : `RangeLiteral` de PrimaryExpr (left-recursion)
10. Supprimé : `ParentAccess` de Postfix (conflit range)
