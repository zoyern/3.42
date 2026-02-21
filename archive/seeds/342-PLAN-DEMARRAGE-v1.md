# 3.42 — Plan de démarrage

> Ce document est conçu pour être **autonome**. Même sans Internet, même sans Claude,
> tu peux suivre ces étapes, apprendre les concepts manquants, et avancer.
> Chaque étape explique le **pourquoi**, le **quoi**, et le **comment**.

---

## Structure du plan

Le plan est découpé en **sprints** (périodes de travail) de 2 semaines. Chaque sprint a :
- Un **objectif clair** — ce qu'on doit avoir à la fin
- Des **prérequis** — ce qu'il faut savoir avant de commencer (avec explications)
- Des **tâches** numérotées et ordonnées
- Un **livrable** — ce qui prouve que c'est terminé
- Des **concepts à apprendre** — expliqués dans ce document pour que tu puisses avancer offline

---

# PHASE 0 — PRÉPARATION (avant de coder)

---

## Sprint 0 : Installer l'environnement + apprendre Rust

**Durée** : 2 semaines
**Objectif** : être prêt à écrire du Rust propre et comprendre les concepts de base

### Tâches

1. **Installer Rust** via rustup : `curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh`
2. **Lire "The Rust Book"** : https://doc.rust-lang.org/book/ (chapitres 1-10 minimum)
3. **Faire les Rustlings** : https://github.com/rust-lang/rustlings — exercices pratiques
4. **Installer les outils** :
   - `cargo` (installé avec Rust — le gestionnaire de paquets)
   - `rust-analyzer` (plugin pour ton éditeur — complétion + diagnostic)
   - `git` (versioning — tu connais déjà via 42)
5. **Créer le repo GitHub** : `3.42` ou `szn-sphere`

### Concepts à apprendre offline

#### L'ownership (possession) en Rust

C'est LE concept clé de Rust, et c'est ce qui le différencie de C/C++.

En C, quand tu fais `char *s = malloc(10)`, tu dois te souvenir de faire `free(s)`. Si tu oublies → fuite mémoire. Si tu le fais deux fois → crash.

En Rust, chaque valeur a **un seul propriétaire**. Quand le propriétaire sort du scope (du bloc de code), la valeur est automatiquement libérée. Pas de `free`, pas de garbage collector (ramasse-miettes).

```rust
fn main() {
    let s = String::from("hello");  // s est propriétaire
    let t = s;                       // s transféré à t — s n'existe plus
    // println!("{}", s);            // ERREUR : s a été "moved" (déplacé)
    println!("{}", t);               // OK : t est le propriétaire
}  // t est libéré automatiquement ici
```

Pour "prêter" une valeur sans la donner : les **références** (comme des pointeurs mais sûrs) :

```rust
fn longueur(s: &String) -> usize {  // &String = référence (emprunt)
    s.len()
}  // s n'est pas libéré — c'est juste un emprunt

fn main() {
    let s = String::from("hello");
    let l = longueur(&s);  // on prête s, on ne le donne pas
    println!("{} a {} caractères", s, l);  // s est toujours valide
}
```

#### Les traits (interfaces) en Rust

Un trait, c'est comme une interface en Java ou un protocole en Swift : une liste de méthodes qu'un type doit implémenter.

```rust
// Définition du trait
trait Measurable {
    fn measure(&self) -> f32;
}

// Implémentation pour un type
struct Circle { radius: f32 }

impl Measurable for Circle {
    fn measure(&self) -> f32 {
        std::f32::consts::PI * self.radius * self.radius
    }
}
```

C'est crucial pour 3.42 : le trait `Sphere<T>` sera implémenté pour les couleurs, les booléens, les nombres, les particules...

#### Les generics (types génériques) en Rust

Écrire du code qui fonctionne pour **n'importe quel type** :

```rust
struct Sphere<T> {      // T = n'importe quel type
    value: T,
    amplitude: f32,
    phase: f32,
}

// Fonctionne pour des couleurs, des nombres, des particules...
let color_sphere: Sphere<Color> = Sphere { value: Color::red(), amplitude: 1.0, phase: 0.0 };
let number_sphere: Sphere<f32> = Sphere { value: 42.0, amplitude: 0.5, phase: 1.57 };
```

### Livrable

- [ ] `cargo new szn-sphere` → le projet compile et affiche "Hello, world!"
- [ ] Tu peux écrire une struct avec des generics et des traits
- [ ] Le repo est sur GitHub

---

# PHASE 1 — LA GRAINE

---

## Sprint 1 : Sphere\<T\> et SphereColor

**Durée** : 2 semaines
**Objectif** : le conteneur sphérique fonctionne et les couleurs sont correctes

### Concepts à apprendre offline

#### Les matrices de rotation

Une matrice (un tableau de nombres 2D) peut représenter une rotation dans l'espace. Pour la sphère de Bloch, les rotations sont des matrices **2×2 complexes** du groupe SU(2).

**Pas de panique** — concrètement, une rotation sur la sphère c'est :

```
Nouveau point = Matrice de rotation × Ancien point

En coordonnées sphériques (θ, φ) :
- Rotation autour de l'axe Z : φ' = φ + angle
- Rotation autour de l'axe Y : θ' = θ + angle
- Rotation arbitraire : combinaison des deux
```

En Rust, ça donne :

```rust
struct SpherePoint {
    theta: f32,  // [0, π]
    phi: f32,    // [0, 2π]
    r: f32,      // [0, 1]
}

impl SpherePoint {
    fn rotate_z(&self, angle: f32) -> Self {
        Self {
            theta: self.theta,
            phi: (self.phi + angle) % (2.0 * std::f32::consts::PI),
            r: self.r,
        }
    }

    fn rotate_y(&self, angle: f32) -> Self {
        Self {
            theta: (self.theta + angle).clamp(0.0, std::f32::consts::PI),
            phi: self.phi,
            r: self.r,
        }
    }
}
```

Pour les matrices complètes (SU(2), quaternions), voir plus tard — pour le sprint 1, les rotations simples suffisent.

#### La conversion couleur : sphère → RGB

```
θ → Luminosité :
  L = θ / π                    (0 = noir, 1 = blanc)

φ → Teinte :
  H = φ / (2π) × 360           (0° = rouge, 120° = vert, 240° = bleu)

r → Saturation :
  C = r                        (0 = gris, 1 = saturé)

Puis conversion LCh → RGB via les formules OKLCH standard.
```

La bibliothèque Rust `palette` fait cette conversion. Pas besoin de la coder toi-même.

#### La distance géodésique (distance sur la sphère)

La distance entre deux points sur une sphère est la longueur du plus court chemin le long de la surface :

```
d = arccos(sin(θ₁)sin(θ₂) + cos(θ₁)cos(θ₂)cos(φ₁ - φ₂))
```

C'est la formule de Vincenty simplifiée. En Rust :

```rust
fn geodesic_distance(a: &SpherePoint, b: &SpherePoint) -> f32 {
    let inner = a.theta.sin() * b.theta.sin()
              + a.theta.cos() * b.theta.cos() * (a.phi - b.phi).cos();
    inner.clamp(-1.0, 1.0).acos()
}
```

### Tâches

1. Créer `src/sphere.rs` — le type `Sphere<T>` avec amplitude, phase, rayon
2. Créer `src/color.rs` — `SphereColor` qui implémente le trait `Sphere`
3. Implémenter `SphereColor::to_rgb()` et `SphereColor::from_rgb()`
4. Implémenter `SphereColor::interfere(other)` — mélange par interférence
5. Implémenter `SphereColor::distance(other)` — distance géodésique
6. Écrire des tests : comparer les résultats avec OKLCH (utiliser la crate `palette`)
7. Implémenter `Sphere<T>::measure()` — collapse probabiliste
8. Implémenter `Sphere<T>::nest()` — conteneur de conteneurs

### Livrable

- [ ] `cargo test` passe — les couleurs sphériques sont correctes
- [ ] Le mélange par interférence donne des résultats visuels corrects
- [ ] La distance géodésique corrèle avec la distance OKLCH (ΔE < 5%)

---

## Sprint 2 : Visualisation SDL3 + wgpu

**Durée** : 2 semaines
**Objectif** : une sphère de couleur interactive s'affiche à l'écran

### Concepts à apprendre offline

#### Le pipeline GPU (chaîne de traitement graphique)

Un GPU dessine en **3 étapes** :

1. **Vertex Shader** (shader de sommets) : transforme les coordonnées 3D en coordonnées 2D écran
2. **Rasterization** (rastérisation) : détermine quels pixels sont couverts par chaque triangle
3. **Fragment Shader** (shader de pixels) : calcule la couleur de chaque pixel

Pour afficher une sphère, on envoie au GPU un maillage (mesh) de triangles qui approximent une sphère, puis le fragment shader colore chaque pixel selon sa position (θ, φ).

#### wgpu : l'API GPU moderne

wgpu est une abstraction au-dessus de Vulkan (Linux/Windows/Android), Metal (macOS/iOS), et DirectX 12 (Windows). Un seul code → fonctionne partout.

```rust
// Créer une surface de rendu
let surface = instance.create_surface(&window);

// Créer un pipeline de rendu
let pipeline = device.create_render_pipeline(&desc);

// Dessiner
encoder.begin_render_pass(&pass_desc);
pass.set_pipeline(&pipeline);
pass.draw(0..vertex_count, 0..1);
```

La crate `wgpu` a un tutoriel complet : https://sotrh.github.io/learn-wgpu/

#### SDL3 pour la fenêtre

SDL3 gère : création de fenêtre, événements clavier/souris, audio. En Rust : crate `sdl3` (ou `sdl2` si SDL3 pas encore dispo en Rust — la migration est simple).

### Tâches

1. Créer `src/renderer/mod.rs` — module de rendu
2. Initialiser SDL3 + wgpu (fenêtre + surface GPU)
3. Dessiner une sphère 3D avec les couleurs SphereColor sur chaque point
4. Gérer l'interaction souris : cliquer sur un point → afficher ses coordonnées (θ, φ, r)
5. Gérer la rotation : glisser la souris → tourner la sphère
6. Afficher en bas de l'écran : valeur RGB, valeur SphereColor, distance au dernier point cliqué

### Livrable

- [ ] Une fenêtre s'ouvre avec une sphère de couleur 3D interactive
- [ ] On peut la tourner et cliquer sur des points
- [ ] Les couleurs correspondent à OKLCH

---

## Sprint 3 : Le type Trit et l'arithmétique ternaire

**Durée** : 2 semaines
**Objectif** : le ternaire équilibré fonctionne nativement

### Concepts à apprendre offline

#### L'arithmétique ternaire équilibrée

En binaire : 5 = 101 (1×4 + 0×2 + 1×1)
En ternaire équilibré : 5 = +1, -1, -1 → (+1)×9 + (-1)×3 + (-1)×1 = 9 - 3 - 1 = 5

Addition ternaire :

```
(+1) + (+1) = (-1) avec retenue +1    (comme 1+1=10 en binaire)
(+1) + (0)  = (+1)
(+1) + (-1) = (0)
(0)  + (0)  = (0)
(-1) + (-1) = (+1) avec retenue -1
```

Multiplication = la table d'interférences du blueprint :

```
(+1) × (+1) = (+1)     constructif
(+1) × (-1) = (-1)     destructif
(+1) × (0)  = (0)      neutre
(-1) × (-1) = (+1)     constructif
```

### Tâches

1. Créer `src/trit.rs` — le type `Trit` = enum {Neg, Zero, Pos}
2. Implémenter Add, Sub, Mul, Neg pour Trit
3. Créer `src/tryte.rs` — `Tryte` = [Trit; 9] (9 trits = 19 683 valeurs)
4. Implémenter la conversion Tryte ↔ i32
5. Implémenter le hash ternaire basique (basé sur la multiplication de trits)
6. Écrire des tests exhaustifs (toutes les combinaisons de trits)
7. Implémenter `Sphere<Trit>` — un conteneur sphérique dont le spin est naturellement 1 (3 états)

### Livrable

- [ ] `cargo test` passe — arithmétique ternaire correcte
- [ ] Les conversions binaire ↔ ternaire sont exactes
- [ ] `Sphere<Trit>` fonctionne avec mesure probabiliste

---

# PHASE 2 — LE LANGAGE

---

## Sprint 4 : Grammaire Tree-sitter

**Durée** : 2 semaines
**Objectif** : le parser reconnaît la syntaxe de base du langage 3.42

### Concepts à apprendre offline

#### Qu'est-ce qu'un parser (analyseur syntaxique)

Un parser transforme du **texte** en un **arbre** (AST — Abstract Syntax Tree). Exemple :

```
Texte : "let x = 1 + 2"

AST :
  LetDeclaration
  ├── name: "x"
  └── value: BinaryExpr
      ├── op: "+"
      ├── left: Number(1)
      └── right: Number(2)
```

Tree-sitter fait ça de manière **incrémentale** : quand tu modifies un caractère, il ne re-parse que la partie modifiée → <1ms par frappe.

#### Comment écrire une grammaire Tree-sitter

La grammaire est un fichier JavaScript qui décrit les règles du langage :

```javascript
// grammar.js
module.exports = grammar({
  name: '342',
  rules: {
    // Un programme = une liste de déclarations
    program: $ => repeat($.declaration),

    // Une déclaration = let ou fn
    declaration: $ => choice(
      $.let_declaration,
      $.function_declaration,
    ),

    // let x = expression
    let_declaration: $ => seq('let', $.identifier, '=', $.expression),

    // fn name(args) -> type { body }
    function_declaration: $ => seq(
      'fn', $.identifier, '(', ')', '{', $.expression, '}'
    ),

    // Expression = nombre, identifiant, ou opération
    expression: $ => choice(
      $.number,
      $.identifier,
      $.binary_expression,
      $.math_expression,  // MODE MATH !
    ),

    // Mode math : f(x) = sin(x) * e^(-x²)
    math_expression: $ => seq(
      $.identifier, '(', $.identifier, ')', '=', $.math_formula
    ),

    // ... etc
  }
});
```

Installation : `npm install tree-sitter-cli` puis `tree-sitter generate`.

### Tâches

1. Installer tree-sitter-cli
2. Créer `tree-sitter-342/grammar.js` — grammaire minimale
3. Implémenter le mode CODE : `fn`, `let`, `if`, `while`, `return`, types de base
4. Implémenter le mode MATH : `f(x) = expression_math`
5. Tester que l'AST est correct pour les deux modes
6. Intégrer le parser dans le projet Rust (crate `tree-sitter`)

### Livrable

- [ ] `tree-sitter parse fichier.342` produit un AST correct
- [ ] Le mode CODE et le mode MATH coexistent dans le même fichier
- [ ] Parsing en <1ms pour des fichiers de <1000 lignes

---

## Sprint 5 : Le proto-compilateur Cranelift

**Durée** : 2 semaines
**Objectif** : le code 3.42 s'exécute

### Concepts à apprendre offline

#### Cranelift : compilation JIT (Just-In-Time — à la volée)

Cranelift est un backend de compilation (moteur qui transforme l'AST en code machine). Il est rapide à compiler (40% plus rapide que LLVM) mais produit du code un peu moins optimisé (2% plus lent).

Le principe : AST → IR Cranelift (Intermediate Representation — représentation intermédiaire) → code machine natif.

```rust
use cranelift::prelude::*;

// Créer une fonction qui additionne deux nombres
let mut builder = FunctionBuilder::new(&mut func, &mut builder_context);
let a = builder.append_block_param(block, types::I64);
let b = builder.append_block_param(block, types::I64);
let result = builder.ins().iadd(a, b);
builder.ins().return_(&[result]);
```

#### La crate `cranelift-jit`

Elle permet d'exécuter le code compilé immédiatement — sans écrire de fichier exécutable. Parfait pour un REPL (Read-Eval-Print-Loop — boucle lire-évaluer-afficher) et un IDE interactif.

### Tâches

1. Créer `src/compiler/mod.rs` — module compilation
2. Implémenter AST → Cranelift IR pour les expressions numériques simples (1 + 2, etc.)
3. Implémenter les variables (`let x = 5`)
4. Implémenter les fonctions (`fn add(a, b) { a + b }`)
5. Créer un REPL basique : tu tapes une expression → elle s'exécute immédiatement
6. Implémenter le mode MATH → évaluation de fonctions mathématiques

### Livrable

- [ ] Le REPL fonctionne : `> 1 + 2` → `3`
- [ ] Les fonctions fonctionnent : `> fn add(a, b) { a + b }` puis `> add(3, 4)` → `7`
- [ ] Le mode math fonctionne : `> f(x) = x * x` puis `> f(5)` → `25`

---

## Sprint 6 : Visualisation temps réel des maths

**Durée** : 2 semaines
**Objectif** : tu tapes une fonction math → la courbe s'affiche en temps réel

### Tâches

1. Connecter le REPL au renderer wgpu
2. Quand le mode MATH détecte une fonction `f(x) = ...` :
   - Évaluer `f` pour x de -10 à 10 (1000 points)
   - Tracer la courbe en temps réel
3. Quand le code est modifié → la courbe se met à jour instantanément
4. Ajouter le zoom/pan (déplacement) sur le graphique
5. Ajouter les axes X et Y avec graduation

### Livrable

- [ ] Tu tapes `f(x) = sin(x) * exp(-x*x)` → la courbe s'affiche
- [ ] Tu modifies la formule → la courbe se met à jour en <16ms (60 FPS)
- [ ] **C'est la démo qui montre le paradigme à tout le monde**

---

# PHASE 3 — LE MOTEUR PHYSIQUE

---

## Sprint 7 : Lennard-Jones sur GPU

**Durée** : 2 semaines
**Objectif** : 10 000 particules simulées en temps réel

### Concepts à apprendre offline

#### Le potentiel de Lennard-Jones

C'est LA formule qui décrit comment deux atomes interagissent :

```
V(r) = 4ε × [(σ/r)¹² - (σ/r)⁶]

- r = distance entre les deux atomes
- ε = profondeur du puits (combien les atomes s'attirent)
- σ = diamètre effectif de l'atome

- À grande distance : quasi rien (les atomes s'ignorent)
- À moyenne distance : attraction (les atomes se rapprochent)
- À courte distance : répulsion forte (les atomes se repoussent)
```

Cette formule SEULE, appliquée entre toutes les paires de particules, fait émerger : les phases de la matière (solide, liquide, gaz), la viscosité, la pression, la tension de surface...

#### wgpu Compute Shaders (shaders de calcul)

Les compute shaders permettent d'utiliser le GPU pour des calculs généraux, pas seulement pour le rendu graphique. Parfait pour la physique : chaque particule est calculée en parallèle.

```wgsl
// Shader WGSL (langage de shader de WebGPU)
@compute @workgroup_size(256)
fn update_particles(@builtin(global_invocation_id) id: vec3<u32>) {
    let i = id.x;
    var force = vec3<f32>(0.0);

    for (var j = 0u; j < particle_count; j++) {
        if (i != j) {
            let r = particles[j].pos - particles[i].pos;
            let dist = length(r);
            // Lennard-Jones
            let s6 = pow(sigma / dist, 6.0);
            let f = 24.0 * epsilon * (2.0 * s6 * s6 - s6) / dist;
            force += normalize(r) * f;
        }
    }

    // Intégration (Verlet)
    particles[i].vel += force * dt;
    particles[i].pos += particles[i].vel * dt;
}
```

### Tâches

1. Créer `src/physics/mod.rs` — module physique
2. Implémenter Lennard-Jones en CPU d'abord (pour tester)
3. Porter sur GPU via wgpu compute shaders
4. Visualiser les particules (des sphères colorées selon leur vitesse)
5. Ajouter la température comme paramètre modifiable en temps réel
6. Observer : à température basse → les particules se figent (solide). À température haute → elles s'agitent (gaz). Entre les deux → liquide.

### Livrable

- [ ] 10 000 particules simulées à >30 FPS
- [ ] Les transitions de phase sont visibles (solide → liquide → gaz)
- [ ] **La viscosité émerge sans être programmée**

---

## Sprint 8 : Barnes-Hut et LOD

**Durée** : 2 semaines
**Objectif** : simuler 100 000+ particules

### Concepts à apprendre offline

#### L'algorithme de Barnes-Hut

Au lieu de calculer N² interactions, on regroupe les particules lointaines :

```
1. Diviser l'espace en octree (arbre où chaque nœud a 8 enfants)
2. Chaque nœud résume les particules qu'il contient (masse totale, centre de masse)
3. Pour une particule donnée :
   - Particules proches → calcul exact
   - Groupes lointains → traiter comme une seule grosse particule
4. Complexité : O(N log N) au lieu de O(N²)
```

Pour 100 000 particules : N² = 10 milliards de calculs. N log N = ~1,7 million. **~6000× moins.**

### Tâches

1. Implémenter l'octree (arbre octaire — division de l'espace en 8)
2. Implémenter Barnes-Hut pour Lennard-Jones
3. Combiner avec le compute shader GPU
4. Implémenter le LOD temporel Fibonacci (les groupes lointains sont recalculés moins souvent)
5. Visualiser avec un code couleur : vert = calcul exact, jaune = approximé, rouge = gelé

### Livrable

- [ ] 100 000 particules à >30 FPS
- [ ] Le LOD Fibonacci fonctionne (pas d'artefacts visuels)
- [ ] On peut zoomer et voir la transition factorisé → exact

---

# PHASE 4 — PROCHAINES ÉTAPES

---

## Ce qui vient après (pour planifier avec 42)

### Sprint 9-10 : LLVM + WASM backends
- Ajouter LLVM pour les binaires optimisés
- Ajouter WASM pour la portabilité navigateur
- Le même code tourne en natif ET dans un navigateur

### Sprint 11-12 : IDE complet
- Éditeur texte avec coloration syntaxique Tree-sitter
- Panneau visualisation (courbes math, simulation physique, sphère de Bloch)
- Panneau debug : les variables sont des points sur la sphère

### Sprint 13-14 : Identité SBT
- Prototype de Soulbound Token local
- Signature des commits Git avec la clé d'identité
- Prototype de traçabilité des contributions

### Sprint 15-16 : Réseau P2P basique
- Communication pair-à-pair (libp2p)
- Partage de code/fichiers décentralisé
- Premiers tests de smart contracts (contrats intelligents)

---

## L'association à 42

### Pourquoi une association

- **Structure légale** pour un projet open source
- **Recruter** des étudiants de 42 motivés
- **Protéger** la propriété intellectuelle du projet
- **Financer** via des bourses, sponsors, Gitcoin

### Comment créer une asso à 42

1. Trouver **2+ membres** (président, trésorier minimum)
2. Rédiger les **statuts** (définir l'objet : "développer un écosystème numérique open source unifiant OS, langage et identité")
3. Déclarer en **préfecture** (ou en ligne via le-compte-asso.associations.gouv.fr)
4. Ouvrir un **compte bancaire** associatif
5. Demander un **local** à 42 (en parler avec le staff)

### Le pitch pour recruter à 42

> "On construit un langage de programmation qui unifie le code, les maths et la physique
> en temps réel. Si tu as envie de toucher au parsing, à la compilation, au rendu GPU,
> à la crypto, ou à l'assembleur — il y a une place pour toi."

Les profils intéressants :
- Quelqu'un qui s'intéresse au **kernel/assembleur** → module OS et arithmétique ternaire
- Quelqu'un qui aime le **graphisme** → renderer wgpu + SDF
- Quelqu'un qui aime la **crypto/réseau** → identité SBT + P2P
- Quelqu'un qui aime les **maths** → moteur physique émergent + harmoniques

---

## Concepts à maîtriser à 42 (en parallèle)

| Concept | Où l'apprendre | Pourquoi c'est utile pour 3.42 |
|---------|---------------|-------------------------------|
| **Matrices** | Cours d'algèbre linéaire, 3Blue1Brown (YouTube) | Rotations sur la sphère, transformations graphiques |
| **Quaternions** | "Visualizing quaternions" (YouTube, 3Blue1Brown) | Rotations 3D sans gimbal lock (blocage de cardan) |
| **Fourier** | "But what is the Fourier Transform?" (3Blue1Brown) | Décomposition en ondes, compression, harmoniques |
| **Graphes** | Projet ft_containers à 42 | Structure de la blockchain en toile |
| **Assembleur** | Projet libasm à 42, "Programming from the Ground Up" | Comprendre ce que le compilateur produit |
| **Réseau** | Projet ft_irc, webserv à 42 | Bases pour le P2P |
| **Parsing** | "Crafting Interpreters" (livre gratuit en ligne) | Comprendre comment un langage est construit |
| **GPU** | "Learn wgpu" tutorial, shaders sur shadertoy.com | Rendu et compute shaders |

---

## Pour avancer sans Internet

Ce document + le blueprint (`342-BLUEPRINT-UNIFIE-v1.md`) contiennent tout ce dont tu as besoin pour comprendre les concepts. Si tu es bloqué sur un concept spécifique :

1. **Matrices** : une matrice 2×2 transforme un vecteur 2D. `[a b; c d] × [x; y] = [ax+by; cx+dy]`. Les rotations sont des matrices où a²+c²=1 et b²+d²=1.

2. **Nombres complexes** : un nombre `a + bi` où `i² = -1`. Sur le plan, `a` = axe horizontal, `b` = axe vertical. Multiplier par `e^(iφ)` = tourner d'un angle φ. C'est LA base de la sphère de Bloch.

3. **Hash** : une fonction qui transforme n'importe quelle donnée en un nombre fixe. Comme une empreinte digitale : unique, irréversible, toujours la même taille.

4. **Blockchain** : une liste chaînée où chaque élément contient le hash du précédent. Si on modifie un élément, son hash change, donc celui du suivant aussi — toute la chaîne est invalidée. C'est ce qui rend la modification impossible.

5. **P2P** : chaque machine est client ET serveur. Quand tu télécharges un fichier, tu le redistribues en même temps. Pas de serveur central.

---

> Ce plan est conçu pour évoluer. Chaque sprint peut prendre plus ou moins de temps.
> L'important n'est pas le calendrier, c'est la direction.
> Commence par la graine. Le reste poussera.

---

> Plan v1.0 — Février 2026
> Conçu pour être autonome, même sans Internet
> Chaque sprint a un livrable concret et testable
