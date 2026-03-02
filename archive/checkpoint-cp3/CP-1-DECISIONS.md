# 3.42 — DÉCISIONS TECHNIQUES — CP-1
## Log de toutes les décisions prises, avec rationale

---

## DÉCISIONS FONDAMENTALES (v1→v6)

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D1 | Tout est onde (sphère de Bloch) | Fourier prouve : tout signal = somme d'ondes. OKLCH le confirme pour les couleurs. | ✓ SOLIDE |
| D2 | 1 règle : A B = A(B) | Simplicité maximale. Tout émerge de la composition. | ✓ SOLIDE |
| D3 | Fermions / Bosons / Gluons | Calqué sur la physique des particules. Exclusion de Pauli pour fermions. | ✓ SOLIDE |
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

## DÉCISIONS CP-1 (NOUVEAU)

| # | Décision | Rationale | Statut |
|---|----------|-----------|--------|
| D17 | % = 18ème boson PROPORTIONNER | CPU: DIV/IDIV (remainder). Printf ÉMERGE (%"fmt"). %? = probabilité. Compose fractalement. | ✓ THÉORIE |
| D18 | $ = interpolation dans strings | Plus explicite que {}. "$name" et "${expr}". Compositions : $! (escape), $? (conditionnel). | ✓ CONCEPT |
| D19 | 4 spins réels (2 bits) + * flag | + - # _ = 4 états hardware (2 bits). * = boson TRANSFORMER, PAS un 5ème spin. Wildcard en pattern = OR mask. | ✓ SOLIDE |
| D20 | Stack : Rust → LLVM → SDL3/wgpu → self-host | Bootstrap en Rust (comme Go depuis C). LLVM pour portabilité. SDL3+wgpu pour runtime. | ✓ SOLIDE |
| D21 | Bubble-up par défaut | Arène callee reste vivante tant que return value existe. .field = MOVE. Safe via types linéaires. Zero-cost si non utilisé. | ⚠️ RECHERCHE |
| D22 | Portes quantiques émergentes | ^! = X gate, ^~ = H gate, ^? = measure, ^| = CNOT. Pas hardcodé, émerge de la composition. | ✓ THÉORIE |
| D23 | Debug universel via sugar | Sugar parse C/C++/Rust → 342 AST → energy tracking → hotspots. Debugger portable. | ⚠️ CONCEPT |
| D24 | Visualisation programme | AST + couleurs = vue 2D/3D. Temperature = énergie (froid bleu → chaud rouge). | ⚠️ CONCEPT |
| D25 | ` = potentiel 6ème gluon | Noté mais DIFFÉRÉ. Pas sur tous les claviers. Pourrait être CODE gluon. | 🔍 DIFFÉRÉ |

## DÉCISIONS REJETÉES

| Proposition | Pourquoi rejetée |
|-------------|------------------|
| / comme boson | Ne compose pas fractalement (// = commentaire, pas de sens pour //) |
| & comme boson | Déjà pris (&& = AND logique). Ne compose pas (&&& = ?) |
| , comme boson | Séparateur structurel, pas une force. Pas d'instruction CPU. |
| \ comme boson | Pas utile. Échappement en strings suffit. |
| α⁻¹ = 128 | FAUX. La vraie constante de structure fine = 137.036. |
| QSEARCH O(√n) classique | IMPOSSIBLE. Grover est quantique uniquement. |
| 5 spins hardware | 5 nécessiterait 3 bits. 4 suffit (2 bits). * = flag logiciel. |

---

## PRINCIPES DIRECTEURS

1. **Tout émerge** — si on doit hardcoder, c'est qu'on n'a pas compris l'émergence
2. **CPU d'abord** — chaque boson doit avoir un ancrage hardware réel
3. **Composition fractale** — une seule règle, profondeur infinie
4. **Zéro perte** — chaque checkpoint préserve toutes les données
5. **Pragmatique** — ce qui marche > théorie parfaite
6. **Sugar pour tous** — le langage est pour tout le monde, le sugar s'adapte
