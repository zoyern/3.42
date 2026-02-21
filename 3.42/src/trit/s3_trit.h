/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_trit.h ::::::::::                  */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Interface ternaire                             */
/*                                                                   */
/*    Deux niveaux d'API :                                           */
/*    1. s3_trit_* : operations sur 1 trit (debug, tests)          */
/*    2. s3_word_* : operations sur 24 trits packes (runtime, IR)  */
/*                                                                   */
/* *************************************************************** */

#ifndef S3_TRIT_H
# define S3_TRIT_H

# include "s3_trit_t.h"

/* ============================================================== */
/*                 NIVEAU 1 : TRIT UNITAIRE                        */
/*                 (debug, tests, comprehension)                   */
/* ============================================================== */

/*
** s3_trit_mul : multiplication ternaire (table d'interference).
**   (+1)*(+1) = +1, (+1)*(-1) = -1, 0*x = 0, (-1)*(-1) = +1
**   1 instruction CPU (imul).
*/
t_s3_trit		s3_trit_mul(t_s3_trit a, t_s3_trit b);

/*
** s3_trit_add : addition ternaire equilibree avec retenue.
**   (+1)+(+1) = -1 carry +1 (debordement → retenue)
**   (+1)+(-1) = 0, carry = 0
*/
t_s3_trit		s3_trit_add(t_s3_trit a, t_s3_trit b, t_s3_trit *carry);

/*
** s3_trit_neg : negation (porte Pauli-X).
**   neg(neg(x)) == x → involutoire, O(1).
*/
t_s3_trit		s3_trit_neg(t_s3_trit t);

/*
** s3_trit_cmp : comparaison ternaire.
**   Retourne -1 (a < b), 0 (a == b), +1 (a > b).
**   Avantage ternaire : le resultat EST une donnee, pas un flag.
*/
t_s3_trit		s3_trit_cmp(t_s3_trit a, t_s3_trit b);

/*
** s3_trit_min / s3_trit_max : logique ternaire de Lukasiewicz.
**   min = AND ternaire, max = OR ternaire.
*/
t_s3_trit		s3_trit_min(t_s3_trit a, t_s3_trit b);
t_s3_trit		s3_trit_max(t_s3_trit a, t_s3_trit b);

/* Conversions tryte ↔ int (debug) */
int				s3_tryte_to_int(t_s3_tryte *tryte);
void			s3_tryte_from_int(t_s3_tryte *tryte, int value);

/* ============================================================== */
/*                 NIVEAU 2 : WORD (24 trits packes)               */
/*                 (runtime, IR, performances)                      */
/* ============================================================== */

/*
** ---- Construction / extraction ----
*/

/*
** s3_word_make : cree un word avec tag + flags + data.
**   Assemble les champs dans un uint64_t.
*/
t_s3_word		s3_word_make(uint64_t tag, uint64_t flags, uint64_t data);

/*
** s3_word_tag / s3_word_flags / s3_word_data : extraction.
**   Chacune isole son champ en 1-2 instructions (shift + mask).
*/
uint64_t		s3_word_tag(t_s3_word w);
uint64_t		s3_word_flags(t_s3_word w);
uint64_t		s3_word_data(t_s3_word w);

/*
** ---- Pack / Unpack : conversion trit[] ↔ word ----
*/

/*
** s3_word_pack : 24 t_s3_trit → 1 t_s3_word.
**   Encode chaque trit en 2 bits, tag = S3_TAG_INT.
*/
t_s3_word		s3_word_pack(const t_s3_trit *trits, uint64_t tag);

/*
** s3_word_unpack : 1 t_s3_word → 24 t_s3_trit.
**   Decode chaque paire de bits. Detecte les patterns invalides (10).
**   Retourne 0 si OK, -1 si corruption detectee.
*/
int				s3_word_unpack(t_s3_word w, t_s3_trit *out);

/*
** ---- Conversion int ↔ word ----
*/

/*
** s3_word_from_int : int64_t → s3_word (ternaire equilibre).
**   Encode en balanced ternary dans les 48 bits data.
*/
t_s3_word		s3_word_from_int(int64_t value);

/*
** s3_word_to_int : s3_word → int64_t.
**   Decode les 24 trits en valeur decimale.
*/
int64_t			s3_word_to_int(t_s3_word w);

/*
** ---- Arithmetique sur word (24 trits en parallele) ----
*/

/*
** s3_word_neg : negation des 24 trits en parallele.
**   ~3 instructions bitwise. O(1) quel que soit le nombre de trits.
*/
t_s3_word		s3_word_neg(t_s3_word w);

/*
** s3_word_add : addition de deux words (24 trits + propagation carry).
**   Plus complexe que neg : necesssite propagation de retenue.
*/
t_s3_word		s3_word_add(t_s3_word a, t_s3_word b);

/*
** s3_word_cmp : comparaison de deux words.
**   Retourne un trit : -1 (a < b), 0 (a == b), +1 (a > b).
*/
t_s3_trit		s3_word_cmp(t_s3_word a, t_s3_word b);

/*
** ---- Validation ----
*/

/*
** s3_word_valid : verifie qu'aucun trit ne contient le pattern 10.
**   Retourne 1 si OK, 0 si corruption detectee.
**   Cout : ~4 instructions bitwise.
*/
int				s3_word_valid(t_s3_word w);

#endif
