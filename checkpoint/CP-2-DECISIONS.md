# 3.42 — DÉCISIONS TECHNIQUES — CP-2
## Log de toutes les décisions prises, avec rationale

---

## DÉCISIONS FONDAMENTALES (v1→v6)

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D1 | Tout est onde (sphère de Bloch) | Fourier prouve : tout signal = somme d'ondes. OKLCH le confirme pour les couleurs. Bloch sphere = représentation universelle des états. | ✓ SOLIDE |
| D2 | 1 règle : A B = A(B) | Simplicité maximale. Tout émerge de la composition. Profondeur infinie (N niveaux). | ✓ SOLIDE |
| D3 | Fermions / Bosons / Gluons | Calqué sur la physique des particules. Exclusion de Pauli pour fermions (2 fermions ≠ même position AST). | ✓ SOLIDE |
| D4 | {} = arène mémoire | Chaque scope crée une zone contiguë. Free en 1 op. Pas de GC. | ✓ SOLIDE |
| D5 | Types linéaires (MOVE default) | Pas de copie implicite. @ borrow, @@ exclusif, ** copie explicite. | ✓ SOLIDE |
| D6 | << return, >< break, >> continue | Émergent : STORE², LOAD+STORE, LOAD². Pas de keywords sauf sugar. | ✓ SOLIDE |
| D7 | ? = if, ?? = while | MESURER et MESURER² = CMP et CMP+JMP. Émergent du CPU. | ✓ SOLIDE |
| D8 | \| = pipe async, >< = pass-through sync | Deux modèles de flux. \| = SPSC buffer, >< = appel direct. | ✓ SOLIDE |

## DÉCISIONS v7.0

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D9 | '' = raw string (5ème gluon) | Comme bash single-quote. Utile pour regex, SQL, chemins. | ✓ SOLIDE |
| D10 | >> = continue (pas break) | v5.1 avait >> = break. v6.0 libéré. v7.0 : >> = LOAD² = ré-entrer = continue. | ✓ SOLIDE |
| D11 | !? = defer | INVERSER(MESURER) = évaluer plus tard. PUSH defer_stack. | ✓ SOLIDE |
| D12 | Sugar = plugin, pas hardcodé | Racket #lang prouve que ça marche. AST canonique = source de vérité. | ✓ SOLIDE |
| D13 | Conversion inter-dev = feature CORE | Dev A (C) et Dev B (Python) voient le même code. IDE affiche le sugar choisi. | ✓ CONCEPT |

## DÉCISIONS v8.0

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D14 | ^ = 17ème boson (QPU) | Hat operator (Â en quantique). Trio : \| CPU, ~ GPU, ^ QPU. XOR → bits::xor(). | ✓ SOLIDE |
| D15 | ^{} = quantum block | SUPERPOSER(CORPS) = circuit quantique. Simulé sur classique. | ✓ CONCEPT |
| D16 | ^^ = quantum measure | SUPERPOSER² = collapse. Observer = choisir un état. | ✓ CONCEPT |

## DÉCISIONS CP-1

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D17 | % = 18ème boson PROPORTIONNER | CPU: DIV/IDIV (remainder). Printf ÉMERGE (%"fmt"). %? = probabilité. | ✓ SOLIDE |
| D18 | $ = interpolation dans strings | Plus explicite que {}. "$name" et "${expr}". PEG gère $ nu, $id, ${expr}, \$. | ✓ SOLIDE |
| D19 | 4 spins réels (2 bits) + * flag | + - # _ = 4 états hardware (2 bits). * = boson TRANSFORMER, PAS un 5ème spin. | ✓ SOLIDE |
| D20 | Stack : Rust → LLVM → SDL3/wgpu → self-host | Bootstrap en Rust. LLVM pour portabilité. Recherche confirme : aucun langage majeur compile directement. | ✓ SOLIDE |
| D21 | Bubble-up par défaut | Arène callee reste vivante tant que return value existe. .field = MOVE. Zero-cost si non utilisé. Escape analysis pour libérer tôt. | ✓ SOLIDE |
| D22 | Portes quantiques émergentes | ^! = X, ^~ = H, ^? = measure, ^| = CNOT. Compositions = portes. Paramétriques via ^(θ). | ✓ THÉORIE |
| D23 | Debug universel via sugar | Sugar parse C/C++/Rust → 342 AST → energy tracking → hotspots. Lossy OK (outil d'analyse, pas transpiler). | ✓ CONCEPT |
| D24 | Visualisation programme | AST + couleurs = vue 2D/3D. Temperature = énergie. Prototype : 342-sphere.html. | ✓ CONCEPT |
| D25 | ` = potentiel gluon CODE | Émerge de la règle gluon : entre 2 bornes identiques + expr intérieure. Comme "" et ''. | 🔍 DIFFÉRÉ |

## DÉCISIONS CP-2 (NOUVEAU)

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D26 | / = 19ème boson DIVISER | CPU: DIV (quotient). // = DIVISER² = commentaires ÉMERGENT. /* */ = DIVISER + gluon. Symétrique avec % (reste). | ✓ SOLIDE |
| D27 | Commentaires émergents | // et /* */ ne sont plus hardcodés conceptuellement. Ils émergent de / + / et / + gluon. Le PEG les implémente dans Spacing. | ✓ SOLIDE |
| D28 | Agnosticisme bit/trit/qbit | Le code 342 est identique. Le backend encode les spins selon la cible. Binaire(2 bits), ternaire(2 trits), quantique(1 qubit). | ✓ CONCEPT |
| D29 | Pipeline de traçabilité | 342 Source → AST → 342-IR → LLVM IR → Assembly → Binaire. Chaque niveau est un "sugar" du suivant. Debuggable à tous les niveaux. | ✓ SOLIDE |
| D30 | 342-IR custom (physique) | IR intermédiaire propre à 342, avant LLVM. Permet visualisation, debug, optimisations basées sur le modèle particules. Traduit vers LLVM IR pour portabilité. | ✓ CONCEPT |
| D31 | Compositions à profondeur N | Pas de limite à 4. !?~^x = 4 bosons composés. Compilateur résout récursivement. Exclusion de Pauli : pas 2 fermions adjacents sans boson. | ✓ SOLIDE |
| D32 | Bitwise émergent via module bits:: | &(AND) reste opérateur. \|(OR), ^(XOR), ~(NOT) réservés → :.bits.or(), :.bits.xor(), :.bits.not(). Syntaxe modernisée par D47 (:. = accès natif). | ✓ SOLIDE (maj D47) |
| D33 | Paradigme = computation universelle | Pas juste un langage. Unifie : shell, programmation, HDL, notation math, notation physique. Chaque domaine = un sugar. | ✓ VISION |
| D34 | Classes/structs = type + trait + impl | Pas de keyword class/struct. Héritage = trait composition (trait A : B + C). Virtual dispatch = ? sur trait. self = @ implicite dans impl. | ✓ SOLIDE |

## DÉCISIONS CP-2.1 (SESSION ACTUELLE)

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D35 | Zéro keywords — type/trait émergents de : | `Name : { valeurs }` = type. `Name : { .signatures }` = trait. Le contenu décide, pas un keyword. `type` et `trait` deviennent sugar optionnel. | ✓ SOLIDE |
| D36 | Copy émergent (taille + pas de @) | ≤ 64 bits ET pas de référence (@/@@) = COPY implicite. Sinon = MOVE. Le compilateur analyse la structure du type. i64 = copy, @u64 = move. Pas de trait Copy. | ✓ SOLIDE (corrigé audit) |
| D37 | @@ séquentiel, pas bloquant | @@ = écriture exclusive. Si ref1 prend @@ puis ref2 prend @@, ref2 obtient la main (séquentiel). Warning si ref1 n'a rien écrit → optimisation auto en @. Pas d'erreur compile. | ✓ CONCEPT (inspiré Vale) |
| D38 | Warnings → optimisations AST | Un @@ inutile = warning + réduction en @ dans l'IR. Dead code elimination émergente. Les warnings ne bloquent pas, ils optimisent. | ✓ SOLIDE |
| D39 | Typed holes via _ | `_` dans une expression = trou typé. Le compilateur infère le type attendu et propose. Code valide même incomplet (_ = placeholder runtime). Émergent de _ = IGNORER. | ✓ SOLIDE (Haskell/Idris prouvé) |
| D40 | ~~Gravitons = catégorie séparée~~ | REMPLACÉ par D46. Le graviton n'est PAS une catégorie séparée, il ÉMERGE de boson + gluon. | ⚠️ OBSOLÈTE → D46 |
| D41 | Parallélisme via gravitons + pipe | \|\| = OR logique (PEG existante). Thread spawn = \|{task} \| \|{task} (gravitons en parallèle via pipe). \|! = async (CONNECTER+INVERSER = fire-and-forget). MOVE des données = thread-safe. | ✓ CONCEPT (corrigé audit, inspiré Pony) |
| D42 | Arena-scoped access control (pas borrow checker) | PAS un borrow checker Rust (pas de lifetime annotations). C'est un contrôle d'accès scopé : @ = lecture illimitée, @@ = écriture séquentielle, scope {} = durée de vie. Arènes O(1) free. Warnings optimisants. Analyse Lobster (95% vérif compile-time). | ✓ CONCEPT (clarifié audit) |
| D43 | Cranelift/QBE pour prototype, LLVM pour prod | Cranelift = 40% plus rapide que LLVM, 86% perf, production-ready (Wasmtime), Rust natif. QBE = plus léger (73% perf, 5K LOC). LLVM = production. Cranelift recommandé 2026. | ✓ SOLIDE (maj fév 2026) |
| D44 | Sphère = organisation, pas mémoire | La sphère de Bloch pour organiser les particules et visualiser. PAS pour adresser la mémoire (coût trigo, perte cache). Algèbre géométrique conforme pour les transformations. | ✓ SOLIDE (CGA prouvé) |
| D45 | Symétrie CPU des bosons | < > (I/O), + - (ADD/SUB), . ? (read/test), : = (declare/assign), ! ~ (NOT/broadcast), \| ^ (CPU/QPU). Solitaires : % / * # _ @ ; | ✓ SOLIDE |
| D46 | Graviton = boson + gluon (émergent) | Un boson appliqué à un gluon {} = scope typé = graviton. :{} = type/trait, \|{} = CPU, ~{} = GPU, ^{} = QPU, ?{} = match, !{} = defer block. Pas une nouvelle catégorie, émerge de A(B). | ✓ SOLIDE |
| D47 | :. = accès natif sans keyword | :.module.fn() = accéder aux fonctions natives du langage. Remplace use/import/std. :.bits = portes logiques classiques, :.trits = ternaires, :.io = I/O. | ✓ CONCEPT |
| D48 | Portes logiques complètes | Classiques : :.bits.{and,or,xor,not,nand,nor}. Ternaires : :.trits.{min,max,inv}. Quantiques : ^!,^~,^\|,^(θ). Les 3 paradigmes couverts. | ✓ SOLIDE |
| D49 | .. = remonter au parent (ACCÉDER²) | . = descendre (enfant). .. = remonter (parent). Comme cd/cd .. Émergent de la composition. | ✓ CONCEPT |
| D50 | Mutex émergent de @@ + pipe | @@ = file d'attente FIFO séquentielle. \| = buffer SPSC. Pas de lock bidirectionnel → pas de deadlock. Lecture @ = non-bloquante. Écriture @@ = séquentielle. | ✓ CONCEPT (inspiré acteurs Erlang/Pony) |
| D51 | Sugar = dictionnaire boson→mot | Chaque sugar = un dictionnaire {boson→keyword}. C : ?→if, ??→while. Python : ?→if, ??→while (sans {}). Conversion X→342→Y = déterministe. Couches dissociées mais reliées par AST. | ✓ VISION |
| D52 | Const émergent (pas de keyword) | Si un fermion n'est jamais @@ dans son scope → implicitement const. Le compilateur détecte et optimise (read-only, inline). Pas de const/let/mut/var. | ✓ SOLIDE |
| D53 | 19 gravitons complets (3 tiers) | Chaque boson + {} = graviton. Tier 1 (8 fondamentaux) : :{} ?{} \|{} ~{} ^{} !{} ={} #{}. Tier 2 (6 puissants) : <{} >{} .{} +{} -{} *{}. Tier 3 (5 spécialisés) : %{} ;{} @{} /{} _{}. | ✓ SOLIDE |
| D54 | ~~Portes logiques classiques~~ → Transducteurs pipe | CORRIGÉ : \|! = async (D41 conservé). \|* = map, \|- = filter, \|+ = fold. Portes logiques classiques → :.bits (D48). Même suffixes sur ~/^ = GPU/QPU parallèle. | ✓ SOLIDE (corrigé conflit) |
| D55 | , reste séparateur syntaxique | Pas un boson (pas de CPU mapping). Lie des fermions dans un gluon. Ponctuation pure comme l'espace. | ✓ SOLIDE (confirmé) |
| D56 | Génériques via () pas <> | <> sont des bosons (SORTIR/ENTRER). () est un gluon = conteneur de paramètres de type. Map(K, V, Alloc) pas Map<K, V, Alloc>. Valeurs par défaut avec =. | ✓ SOLIDE |
| D57 | 19 gravitons = 3 tiers | Tier 1 = fondamentaux (:{} ?{} \|{} ~{} ^{} !{} ={} #{}). Tier 2 = fonctionnels (+{} -{} *{} <{} >{} .{}). Tier 3 = spécialisés (%{} ;{} @{} /{} _{}). Voir designs/GRAVITONS-COMPLETE-SPEC.md | ✓ SOLIDE |
| D58 | Stream fusion via gravitons transducteurs | *{} -{} +{} composent sans collections intermédiaires = 1 seule boucle. Inspiré Clojure transducers, Haskell stream fusion. Killer feature. | ✓ SOLIDE |
| D59 | Trio compute × suffixes = matrice symétrique | Même suffixe (!*+-?~\|) sur \|/~/^ donne CPU séquentiel / GPU parallèle / QPU quantique. \|* = map CPU, ~* = parallel map GPU, ^* = rotation QPU. FRACTAL. | ✓ SOLIDE |
| D60 | !{} defer vs !? lazy = coexistence | !{} = INVERSER+CORPS = defer block (exécuter à sortie scope). !? = INVERSER+MESURER = lazy eval (évaluer plus tard). Sémantiques différentes, pas de conflit. | ✓ SOLIDE |
| D61 | Borrow granularity = struct-level | Un @@ sur un champ verrouille le struct entier. Un @ sur un champ verrouille le struct en lecture entière. Conservateur comme Rust. Field-level = optimisation future (flag `#split`). | ✓ SOLIDE |

## DÉCISIONS REJETÉES

| Proposition | Pourquoi rejetée |
|-------------|------------------|
| / comme boson (ancienne) | RÉHABILITÉ en CP-2 : / = DIVISER, commentaires émergent. L'ancienne analyse était incorrecte. |
| & comme boson | Déjà pris (&& = AND logique). Ne compose pas (&&& = ?) |
| , comme boson | Séparateur structurel, pas une force. Pas d'instruction CPU directe. Potentiel gluon de liaison. |
| \ comme boson | Pas utile. Échappement en strings suffit. |
| α⁻¹ = 128 | FAUX. Oublié. La vraie constante de structure fine = 137.036. |
| QSEARCH O(√n) classique | IMPOSSIBLE. Grover est quantique uniquement. |
| 5 spins hardware | 5 nécessiterait 3 bits. 4 suffit (2 bits). * = flag logiciel. |
| Compilation directe sans IR | Recherche confirme : LLVM ou IR équivalent = nécessaire pour portabilité. Aucun langage majeur compile directement vers binaire. Mais QBE (mini-IR) acceptable pour prototype. |
| Borrow checker style Rust | Trop complexe, courbe d'apprentissage catastrophique. Remplacé par D37+D38+D42 (MOVE + arènes + @@ séquentiel + warnings). |
| SH pour diffing | Harmoniques sphériques = données continues. Diff = discret. Incompatible. Delta encoding + LZ4 gardé. |
| Sphère pour adressage mémoire | Coût trigonométrique, perte cache CPU. Sphère = visualisation/organisation uniquement (D44). |
| \|! = NOT gate (D54 original) | CONFLIT avec D41 (\|! = async). \| = pipe, pas logique. Portes classiques → :.bits. \|! reste async. |
| \|+ = AND gate | Idem. \|+ = fold (transducteur), pas AND. Portes → :.bits.and(). |

---

## PRINCIPES DIRECTEURS

1. **Tout émerge** — si on doit hardcoder, c'est qu'on n'a pas compris l'émergence
2. **CPU d'abord** — chaque boson doit avoir un ancrage hardware réel
3. **Composition fractale** — une seule règle, profondeur infinie, sans limite
4. **Zéro perte** — chaque checkpoint préserve toutes les données
5. **Pragmatique** — ce qui marche > théorie parfaite
6. **Sugar pour tous** — le langage est pour tout le monde, le sugar s'adapte
7. **Portable** — bit/trit/qbit agnostique, PC/mobile/embarqué/QPU
8. **Traçable** — chaque niveau de compilation est visible et debuggable

---

## SOURCES SCIENTIFIQUES

| Concept | Source | Statut |
|---------|--------|--------|
| Sphère de Bloch | Wikipedia, arXiv:1111.4427 | ✓ PROUVÉ pour qubits, généralisable SU(N) |
| Principe d'exclusion de Pauli | Nature Comm. Physics 2019 | ✓ PROUVÉ, vérifié sur QPU |
| Brevet ternaire Huawei | CN119652311A (2025) | ✓ BREVETÉ, -40% transistors, -60% énergie revendiqué |
| Vale (références générationnelles) | verdagon.dev | ✓ PROUVÉ, 2-10% overhead mesuré |
| Lobster (analyse lifetime) | aardappel.github.io | ✓ PROUVÉ, élimine 95% refcount ops |
| Pony (capabilities thread-safe) | ponylang.io | ✓ PROUVÉ mathématiquement thread-safe |
| QBE backend | c9x.me | ✓ PROUVÉ, 3.9× plus rapide que LLVM, 73% perf |
| Typed holes | GHC Haskell, Idris | ✓ PROUVÉ, code valide avec trous |
| Algèbre géométrique conforme | CGA, Clifford algebras | ✓ PROUVÉ pour transformations géométriques |
| TCMalloc arenas | ASPLOS 2024 | ✓ PROUVÉ à l'échelle Google |
| ISA ternaire viable | IJCA vol.83, Hackaday 2016 | ✓ PROUVÉ (Trillium, prototypes) |
| LLVM = standard compilateurs | Sources multiples (2013+) | ✓ PROUVÉ, perf = GCC depuis 2013 |
| Tables périodiques 3D | Chancourtois 1862, Benfey 1964 | ✓ HISTORIQUE, hélice/cylindre/spirale |
| Wavelets pour signaux | Nature Comp. Sci. 2021 (fCWT) | ✓ PROUVÉ pour signaux, spéculatif pour code |
| Horloge CPU = onde carrée | Électronique standard | ✓ PROUVÉ, 30% puissance CPU |
