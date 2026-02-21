# 3.42 — Spécification Syntaxique v2
## "Tout est Sphère, ? est Mesure, le Spin détermine tout"

---

## 1. QUARKS (primitives fondamentales)

### Sphère
Toute valeur est une sphère : contenu + spin (état trit).

### Spin (4 états)
| Pattern | Trit | Signification | Sphère |
|---------|------|---------------|--------|
| `+` | 01 | positif, succès, vrai, actif | pôle nord |
| `-` | 11 | négatif, échec, faux, rejeté | pôle sud |
| `*` | 00 | neutre, vide, rien, en attente | centre/équateur |
| `#` | 10 | erreur, corrompu, invalide | décohérence |

### Wildcard
| Pattern | Signification |
|---------|---------------|
| `_` | attrape-tout (match n'importe quel spin) |

### Mesure
| Opérateur | Effet |
|-----------|-------|
| `?` | mesure unique (if/match/foreach selon le type) |
| `??` | mesure continue (while/loop) |

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
| `()` | groupement, paramètres |
| `[]` | liste, map, index, slice |
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

---

## 5. ANNOTATIONS

| Annotation | Effet |
|------------|-------|
| `@parallel` | exécution parallèle CPU |
| `@gpu` | compute shader GPU |
| `@async` | I/O non-bloquant |
| `@extend` | définir un nouveau domaine (macros hygiéniques) |
| `@comptime` | exécution à la compilation |
| `@public` | sphère accessible inter-processus |

**Règle** : `@` en PREFIX = annotation. `@`/`@@` en POSTFIX = emprunt.
**Règle d'or** : les annotations optimisent, elles ne changent pas le comportement.

---

## 6. ÉMERGENCE — CE QUI N'EST PAS DÉFINI MAIS EXISTE

| Construction | Émerge de | Syntaxe |
|-------------|-----------|---------|
| if/else | `?` sur valeur unique | `(cond) ? { + : a; - : b; }` |
| switch/match | `?` avec conditions | `val ? { (c1) : a; (c2) : b; _ : c; }` |
| foreach | `?` sur collection | `list ? { item? : action(item); }` |
| while | `??` sur condition mutable | `(cond) ?? { + : body; }` |
| loop | `??` sur true | `(true) ?? { + : body; }` |
| try/catch | `?` après opération faillible | `op() { result? : ok; - err? : handle; }` |
| scan | `?` multi-spin sur collection | `list ? { item? : a; - e? : b; * : c; }` |
| pipeline | chaînage UFCS + `{}` | `a.b().c() { r? : ok; - : err; }` |
| stream | `??` sur flux infini | `pi ?? { digit? : process(digit); }` |

---

## 7. TYPES SPÉCIAUX

### Streams infinis (constantes calculables)
```
// pi, e, phi = flux infinis de chiffres
pi ?? {
    digit? : process(digit);
    (digit_index > 100) ? { + : break; };
};

// Prendre les N premiers
first_100 = pi.take(100).collect();

// Précision adaptée au contexte
area = pi * r * r;     // compilateur choisit la précision nécessaire
```

### Probabilité (stdlib, pas keyword)
```
use math::probability { prob, uniform, normal };

coin = prob(0.5);
coin ? { + : "pile"; - : "face"; };

both = p1 && p2;        // intersection
either = p1 || p2;      // union

dice = uniform(1, 6);
height = normal(175.0, 10.0);
```

---

## 8. ARCHITECTURE COMPILATION

```
Code 3.42
    ↓
PEG Grammar (formalise la lecture du code)
    ↓ construit
Parser combiné (PEG = lexer+parser en une seule phase)
    ↓ produit
AST (unique nœud de contrôle : QueryNode)
    ↓
C11 freestanding (pas de libc, pas de syscalls)
    ↓
├── Mode natif (kernel 3.42) → scheduler intégré, zéro syscall
└── Mode hosted (Linux/Win/Mac) → HAL mince (~20 fonctions)
    ↓
SDL3 + WGPU (graphique/compute)
    ↓
Hardware (CPU/GPU/QPU)
```

---

## 9. RUNTIME INTÉGRÉ

- Green threads (2KB par thread, ~200ns context switch)
- Arena allocation (64MB chunks, libération en bloc)
- SLAB pools (objets taille fixe, O(1))
- Lock-free SPSC ring buffers (scheduler)
- io_uring/kqueue/IOCP (I/O batché)
- Scheduler adaptatif (décide CPU/GPU/séquentiel automatiquement)
- CAS atomique sur 2 bits pour les spins
- Move semantics (déplacement = copie du pointeur, ~1ns, NRVO élimine la majorité)

---

## 10. INVENTAIRE DES SYMBOLES

```
Spins :      + - * #                    (4)
Wildcard :   _                          (1)
Mesure :     ? ??                       (2)
Contrôle :   .? identifier?             (2)
Logique :    ! && ||                    (3)
Bitwise :    & | ^ << >>               (5)
Arithm :     + - * / %                  (5 — contexte distingue des spins)
Comparaison: == != < > <= >=            (6)
Structure :  { } ( ) [ ] ; : = . :: @   (12)
Emprunt :    @ @@                       (2 — postfix)
Réservé :    ~                          (1)
Mot-clés :   mut return break copy      (4)
             module type trait use as    (5)

Total : ~14 symboles uniques + 9 mot-clés
(vs C : 38 mots-clés + 45 opérateurs)
(vs Rust : 52 mots-clés + ~40 opérateurs)
```
