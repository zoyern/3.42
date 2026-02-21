/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_trit_t.h ::::::::::                */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Types ternaires                                */
/*                                                                   */
/*    Deux niveaux :                                                 */
/*    - t_s3_trit  : 1 trit (signed char, debug/test)              */
/*    - t_s3_word  : 24 trits packes dans un uint64_t (runtime)    */
/*                                                                   */
/*    v2 : ajout s3_word (24 trits + 16 bits metadata),             */
/*         encodage 2-bit (00=0, 01=+1, 11=-1),                    */
/*         tagged values pour IR et runtime                          */
/*                                                                   */
/* *************************************************************** */

#ifndef S3_TRIT_T_H
# define S3_TRIT_T_H

# include <stdint.h>
# include <stddef.h>

/* ======================== TRIT UNITAIRE ======================== */

/*
** Un trit stocke dans un signed char (-1, 0, +1).
** Utilise pour les operations elementaires et le debug.
** Le compilateur optimise a*b en 1 instruction (imul).
*/
typedef signed char	t_s3_trit;

# define S3_TRIT_NEG  (-1)
# define S3_TRIT_ZERO (0)
# define S3_TRIT_POS  (1)

/* ======================== TRYTE (6 trits) ======================== */

/*
** Un tryte = 6 trits = plus petite unite adressable en ternaire.
** 3^6 = 729 valeurs — range : [-364, +364].
** Equivalent d'un octet (byte) dans le monde binaire.
*/
# define S3_TRYTE_SIZE 6

typedef struct s_s3_tryte
{
	t_s3_trit	trits[S3_TRYTE_SIZE];
}	t_s3_tryte;

/* ======================== WORD (24 trits packes) ======================== */

/*
** s3_word : le mot machine ternaire de 3.42.
**
** Layout dans un uint64_t :
**
**   bits 63-48 : METADATA (16 bits)
**     63-60 : tag (type de la valeur)
**     59-56 : flags (overflow, NaT, mode)
**     55-48 : reserve (region ID, capabilities)
**
**   bits 47-0 : DATA (48 bits = 24 trits × 2 bits chacun)
**     Trit 0 (LSB) : bits 1-0
**     Trit 1        : bits 3-2
**     ...
**     Trit 23 (MSB) : bits 47-46
**
** Encodage par trit (2 bits) :
**   00 = 0     (zero)
**   01 = +1    (positif)
**   11 = -1    (negatif)
**   10 = INVALIDE (detecte comme corruption)
**
** Pourquoi cet encodage :
**   - neg(trit) : on XOR avec un masque des trits NON-ZERO seulement.
**     01 XOR 10 = 11 (+1→-1), 11 XOR 10 = 01 (-1→+1), 00 reste 00.
**     Voir s3_word_neg() dans s3_trit.c pour l'algorithme complet.
**   - Le pattern 10 est invalide → detecte les corruptions memoire
**   - 24 trits → 3^24 = 282 429 536 481 valeurs (~38 bits effectifs)
*/
typedef uint64_t	t_s3_word;

/*
** ======================== TAGS (bits 63-60) ========================
**
** Le tag identifie le type de la valeur SANS consulter de table.
** Comme le NaN-boxing de V8/SpiderMonkey, mais en ternaire.
** Avantage : un switch sur le tag = 1 instruction, cache-friendly.
*/
# define S3_TAG_INT    0x0ULL   /* entier ternaire signe             */
# define S3_TAG_UINT   0x1ULL   /* entier ternaire non-signe         */
# define S3_TAG_PTR    0x2ULL   /* pointeur (index dans arena)       */
# define S3_TAG_TRIT   0x3ULL   /* trit logique (1 seul trit actif)  */
# define S3_TAG_FLOAT  0x4ULL   /* flottant (encodage custom)        */
# define S3_TAG_NODE   0x5ULL   /* noeud AST (index dans arena)      */
# define S3_TAG_CAP    0x6ULL   /* capacite (jeton de securite)      */

/*
** ======================== FLAGS (bits 59-56) ========================
*/
# define S3_FLAG_NONE     0x0ULL   /* normal                          */
# define S3_FLAG_OVERFLOW 0x1ULL   /* overflow lors du dernier calcul */
# define S3_FLAG_NAT      0x2ULL   /* Not a Trit (valeur invalide)   */
# define S3_FLAG_SAT      0x4ULL   /* mode saturant (clamp, pas wrap)*/
# define S3_FLAG_TRAP     0x8ULL   /* mode trap (erreur si overflow) */

/*
** ======================== MASQUES ========================
**
** Pour extraire ou modifier les champs du s3_word.
*/
# define S3_DATA_BITS    48
# define S3_DATA_MASK    0x0000FFFFFFFFFFFFULL  /* bits 47-0  */
# define S3_TAG_SHIFT    60
# define S3_TAG_MASK     0xF000000000000000ull  /* bits 63-60 */
# define S3_FLAG_SHIFT   56
# define S3_FLAG_MASK    0x0F00000000000000ull  /* bits 59-56 */
# define S3_META_MASK    0xFFFF000000000000ull  /* bits 63-48 */

/*
** ======================== CONSTANTES ========================
*/
# define S3_WORD_TRITS   24    /* nombre de trits dans un word      */
# define S3_TRIT_BITS    2     /* bits par trit dans l'encodage     */

/*
** Encodage 2-bit d'un trit dans un word.
** Utilise pour pack/unpack entre t_s3_trit et t_s3_word.
*/
# define S3_ENC_ZERO     0x0U  /* 00 */
# define S3_ENC_POS      0x1U  /* 01 */
# define S3_ENC_NEG      0x3U  /* 11 */
# define S3_ENC_INVALID  0x2U  /* 10 — corruption detectee */

#endif
