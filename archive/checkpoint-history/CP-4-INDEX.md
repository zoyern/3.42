# 3.42 — INDEX DE NAVIGATION — CP-4
## Document unifié + Visualisations + Architecture finale
### Date : 02/03/2026

---

## STRUCTURE DES FICHIERS

```
342/
├── checkpoint/
│   ├── CP-4-INDEX.md            ← CE FICHIER (navigation)
│   ├── CP-4-UNIFIED.md          ← DOCUMENT PRINCIPAL (16 sections, tout le projet)
│   ├── CP-2-DECISIONS.md        ← 83 décisions détaillées (D1-D83)
│   ├── CP-2-OPEN.md             ← 28 questions (TOUTES RÉSOLUES)
│   ├── CP-3-INDEX.md            ← Ancien index (archivé, lecture seule)
│   ├── CP-3-RESEARCH-2026.md    ← État de l'art 2025-2026
│   └── CP-3.9-CONVERGENCE.md    ← Dernière session convergence (types, décimaux, GA)
├── grammar/
│   └── 342.peg                  ← PEG formelle (à mettre à jour pour D74-D83)
├── designs/
│   ├── Q19-PARTICLEIR-SPEC.md   ← IR spécification + 5 exemples
│   ├── Q20-*.md                 ← Système Diffable (7 fichiers)
│   ├── GRAVITONS-COMPLETE-SPEC.md ← 18 gravitons (3 tiers) spécification
│   └── MEMORY_SAFETY_*.md       ← Sécurité mémoire formelle (Q21)
├── 342-vision.html              ← VISUALISATION GLOBALE (table périodique, symétrie, roadmap)
├── 342-language.html            ← RÉFÉRENCE LANGAGE (code, exemples, sugar toggle)
├── 342-emergence.html           ← Visualisation orbitale des bosons
├── 342-particles.html           ← Visualisation particules par niveaux
├── 342-sphere.html              ← Visualisation 3D sphère de Bloch
├── 342-octogones.html           ← Visualisation octogonale
├── archive/
│   ├── checkpoint-cp3/          ← CP-1.*, CP-2-INDEX, CP-3.2 à CP-3.8, AUDIT-GEMINI
│   ├── blueprints/              ← Anciennes versions
│   ├── grammar/                 ← Anciennes grammaires
│   ├── notes/                   ← Notes de travail
│   ├── seeds/                   ← Graines d'idées
│   └── specs/                   ← Anciennes spécifications
├── examples/                    ← Exemples de code
└── src/                         ← Code source (trit C11, 73 tests)
```

---

## LECTURE RECOMMANDÉE

### Pour comprendre le projet (dans l'ordre) :
1. **CP-4-UNIFIED.md** → le document complet (16 sections)
2. **342-vision.html** → vue interactive globale (ouvrir dans un navigateur)
3. **342-language.html** → référence langage avec exemples de code

### Pour les détails :
- **CP-2-DECISIONS.md** → 83 décisions avec rationale complet
- **designs/** → spécifications techniques (ParticleIR, Diffable, Gravitons, Mémoire)
- **CP-3-RESEARCH-2026.md** → état de l'art et recherche

---

## COMPTEURS

| Métrique | Valeur |
|----------|--------|
| Décisions | **83** (D1-D83, 1 obsolète D40, 3 corrigées D36/D41/D65) |
| Bosons | **18** (spin 1) |
| Gluons | **5** (spin 0 conteneurs) |
| Séparateurs | **3** (spin 0 ponctuation) |
| Gravitons | **18** (3 tiers : 8+6+4) |
| Questions | **28/28 RÉSOLUES** |
| Tiers orbitaux | **4** (s, p, d, f) |
| Paires symétriques | **9** + 1 axe central (|) |

---

## CLÉS DE LECTURE RAPIDE

**18 bosons** : `< > . ? ! | ~ ^ % / : = + - * # _ @`
**5 gluons** : `{} () [] "" ''`
**3 séparateurs** : `;` `,` ` `
**1 règle** : `A B = A(B)`
**0 keywords** : `:` suffit

### Noms des bosons (verbes français)
| Tier 0/s | Tier 1/p | Tier 2/d | Tier 3/f | Axe |
|----------|----------|----------|----------|-----|
| ~ ONDULER | : DÉFINIR | ? MESURER | % PROPORTIONNER | \| CONNECTER |
| ^ SUPERPOSER | = LIER | < ENTRER | @ RÉFÉRENCER | |
| | . INTÉRIEUR | > SORTIR | / DIVISER | |
| | ! INVERSER | + ACCUMULER | _ IGNORER | |
| | # CONFIGURER | - RÉDUIRE | | |
| | | * TRANSFORMER | | |

### Modes de calcul
- Séquentiel = défaut (pas de symbole)
- `~` = parallèle (GPU/SIMD)
- `^` = superposé (QPU)
- `|` = connexion (pipes/threads)

---

## PROCHAINES ÉTAPES

1. PEG grammar : ajouter D74-D83 (range types, autograd, CGA, #key)
2. Parser Pest : .peg → .pest + 50 tests
3. AST → ParticleIR : lowering en Rust (~500 lignes)
4. Cranelift POC : hello world compilé
5. 10 exemples : démontrer les 18 gravitons
6. Stream fusion : |* |- |+ → 1 boucle dans l'IR
7. Sugar universel : dictionnaires C/Python/COBOL/Haskell
8. Benchmarks : arena vs malloc, fusion vs boucle manuelle
9. Autograd POC : Diffable + math ops → gradients automatiques
10. WASM backend : Cranelift → WASM pour web

---

## AUTEUR

**Alexis Mounib** (almounib) — 42 Paris
GitHub : [github.com/zoyern](https://github.com/zoyern)
