# 3.42 — QUESTIONS OUVERTES — CP-1
## Ce qui reste à explorer, rechercher, décider

---

## PRIORITÉ HAUTE (prochain checkpoint)

### Q1 : Bubble-up — explicite ou par défaut ?
**Contexte** : Quand une fonction retourne, ses variables internes sont-elles accessibles via `.field` par défaut, ou faut-il un marqueur explicite ?
**Position actuelle** : Par défaut (zero-cost si non utilisé)
**À rechercher** :
- Impact sur la taille des arènes (arène callee reste vivante = plus de mémoire ?)
- Comparaison avec Rust lifetimes (plus ou moins overhead ?)
- Cas pathologiques (chaîne de 100 appels avec bubble-up = 100 arènes vivantes ?)
**Action** : Benchmark théorique + prototype en C

### Q2 : % compositions — quelles sont utiles ?
**Contexte** : % est le 18ème boson. Quelles compositions sont réellement utiles vs théoriques ?
**À vérifier** :
- `%%` (normaliser) — CPU : `DIV` + `MUL 100` ? Ou juste scaling ?
- `%?` (probabilité) — comment ça se compile ? `RDRAND` + `CMP` ?
- `%""` (format) — le transpiler gère comment ? Phase sugar ou phase compile ?
**Action** : Écrire des exemples concrets et vérifier la compilation

### Q3 : $ interpolation — conflits possibles ?
**Contexte** : $ remplace {} pour l'interpolation dans les strings.
**Questions** :
- `"prix: $montant$"` — le 2ème $ est-il un problème ?
- Faut-il des délimiteurs obligatoires ? `$name` vs `${name}` toujours ?
- Quid des noms avec _ : `"valeur: $my_var_name"` — où finit le nom ?
**Action** : Définir les règles de parsing exactes dans le PEG

### Q4 : Classes/structs émergentes
**Contexte** : L'utilisateur veut que classes et structs ÉMERGENT de type + trait + impl.
**Questions** :
- `type` + `trait` + `impl` = suffisant pour tout ce que fait une classe ?
- Héritage = trait composition ? `trait A : B + C` ?
- Méthodes = fonctions dans impl avec `self` = `@` implicite ?
- Virtual dispatch = ? measure sur le trait ?
**Action** : Écrire un exemple complet (classe Animal avec héritage) en 342

---

## PRIORITÉ MOYENNE (CP-2 ou CP-3)

### Q5 : ` comme 6ème gluon
**Contexte** : Le backtick pourrait être un gluon CODE (conteneur de code brut).
**Pour** : Utile pour templates, macros, code generation
**Contre** : Pas sur tous les claviers facilement, confus avec ''
**Décision** : Différée

### Q6 : Debug universel via sugar
**Contexte** : Sugar parse C/Rust → 342 AST → energy tracking
**Questions** :
- Les AST de C et Rust sont TRÈS différents de 342. La conversion est-elle lossless ?
- Comment gérer les features spécifiques (templates C++, lifetimes Rust) ?
- Est-ce un transpiler complet ou juste un outil d'analyse statique ?
**Action** : Prototype avec un fichier C simple → 342 AST → analyse

### Q7 : Visualisation programme 2D/3D
**Contexte** : AST + couleurs = vue visuelle du programme en exécution
**Questions** :
- Quel format ? SVG ? WebGL ? Terminal ?
- Real-time ou post-mortem ?
- À quel point c'est scalable (programme de 100K lignes) ?
**Action** : Prototype simple en HTML/JS

### Q8 : Portes quantiques émergentes
**Contexte** : ^! = X, ^~ = H, ^| = CNOT. Théoriquement toutes les portes émergent.
**Questions** :
- Est-ce que les 15 portes universelles sont couvertes ?
- Comment faire les portes paramétriques (Rz(θ), Ry(θ)) ?
- Le simulateur classique est-il assez performant ?
**Action** : Lister les portes et vérifier la couverture

---

## PRIORITÉ BASSE (long terme)

### Q9 : Trit coprocesseur
- Quand le hardware ternaire sera disponible (5-10 ans)
- Comment mapper les spins {+, -, _} sur des trits hardware
- FPGA prototype possible ?

### Q10 : QPU backend réel
- Quand les QPU seront accessibles (IBM Qiskit, Google Cirq)
- Comment ^{} compilerait vers un circuit réel
- Gestion du bruit / décoherence

### Q11 : Blockchain pour code
- IP automatique via hash blockchain
- Comment éviter le bloat (chaque commit = transaction ?)
- Coût énergétique (Hedera = 0.000003 kWh/tx mais quand même)

### Q12 : Unification maths/physique
- Un physicien écrit des équations, un dev écrit du code, même AST
- Notation Dirac (|ψ⟩) via sugar scientifique
- Intégration numérique via sugar (∫ f dx)

---

## BUGS CONNUS

| Bug | Sévérité | Statut |
|-----|----------|--------|
| Aucun bug connu dans la spec | — | — |
| PEG non testé avec parser réel | ⚠️ | À faire en Phase 1 |
| $ parsing dans strings edge cases | ⚠️ | Q3 ci-dessus |

---

## PROCHAINES ÉTAPES CONCRÈTES

1. **Prototype parser** : Implémenter le PEG CP-1 dans un vrai parser (packrat ou PEG.js)
2. **Tests** : Écrire 50+ cas de test pour chaque disambiguation (15 cas)
3. **Exemples** : Écrire 10 programmes complets en 342 natif
4. **Sugar C** : Implémenter un sugar C minimal (if/while/return → ?/??/<<)
5. **Arena allocator** : Étendre le code C11 existant (src/trit/)
