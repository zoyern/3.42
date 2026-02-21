/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_trit.c ::::::::::                  */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Implementation ternaire                        */
/*                                                                   */
/*    v2 : s3_word (24 trits packes dans uint64_t)                  */
/*         operations bitwise paralleles (neg, valid, pack/unpack)  */
/*         tagged values (tag + flags dans les 16 bits hauts)       */
/*         addition avec propagation de retenue                      */
/*                                                                   */
/* *************************************************************** */

#include "s3_trit.h"

/* ============================================================== */
/*                 NIVEAU 1 : TRIT UNITAIRE                        */
/* ============================================================== */

/*
** Multiplication ternaire = table d'interference.
** a * b : le compilateur genere 1 instruction imul.
** Pas de branchement, pas de lookup table.
*/
t_s3_trit	s3_trit_mul(t_s3_trit a, t_s3_trit b)
{
	return ((t_s3_trit)(a * b));
}

/*
** Addition ternaire equilibree avec retenue.
** sum = a + b peut valoir {-2, -1, 0, 1, 2}.
** En ternaire equilibre, on n'a que {-1, 0, +1}.
** Si sum > 1  → sum - 3, carry = +1
** Si sum < -1 → sum + 3, carry = -1
*/
t_s3_trit	s3_trit_add(t_s3_trit a, t_s3_trit b, t_s3_trit *carry)
{
	int	sum;

	sum = a + b;
	if (carry)
		*carry = S3_TRIT_ZERO;
	if (sum > 1)
	{
		if (carry)
			*carry = S3_TRIT_POS;
		return ((t_s3_trit)(sum - 3));
	}
	if (sum < -1)
	{
		if (carry)
			*carry = S3_TRIT_NEG;
		return ((t_s3_trit)(sum + 3));
	}
	return ((t_s3_trit)sum);
}

/*
** Negation = porte Pauli-X.
** neg(neg(x)) == x — involutoire.
** Base du "Undo natif" de 3.42.
*/
t_s3_trit	s3_trit_neg(t_s3_trit t)
{
	return ((t_s3_trit)(-t));
}

/*
** Comparaison ternaire.
** Retourne un TRIT, pas un flag. C'est une donnee de premiere classe.
** On peut stocker, combiner, passer en argument le resultat d'une cmp.
** En binaire, il faut 2 instructions (cmp + conditional move).
** En ternaire : 1 valeur = 3 cas.
*/
t_s3_trit	s3_trit_cmp(t_s3_trit a, t_s3_trit b)
{
	if (a < b)
		return (S3_TRIT_NEG);
	if (a > b)
		return (S3_TRIT_POS);
	return (S3_TRIT_ZERO);
}

/*
** Logique ternaire de Lukasiewicz.
** min = AND ternaire : le plus petit des deux.
** max = OR ternaire  : le plus grand des deux.
** En ternaire : min(-1, +1) = -1, max(-1, +1) = +1.
*/
t_s3_trit	s3_trit_min(t_s3_trit a, t_s3_trit b)
{
	if (a < b)
		return (a);
	return (b);
}

t_s3_trit	s3_trit_max(t_s3_trit a, t_s3_trit b)
{
	if (a > b)
		return (a);
	return (b);
}

/* ---- Conversions tryte ↔ int (inchangees, pour debug) ---- */

int	s3_tryte_to_int(t_s3_tryte *tryte)
{
	int	result;
	int	power;
	int	i;

	if (!tryte)
		return (0);
	result = 0;
	power = 1;
	i = S3_TRYTE_SIZE - 1;
	while (i >= 0)
	{
		result += tryte->trits[i] * power;
		power *= 3;
		i--;
	}
	return (result);
}

void	s3_tryte_from_int(t_s3_tryte *tryte, int value)
{
	int	i;
	int	rem;

	if (!tryte)
		return ;
	i = S3_TRYTE_SIZE - 1;
	while (i >= 0)
	{
		tryte->trits[i] = S3_TRIT_ZERO;
		i--;
	}
	i = S3_TRYTE_SIZE - 1;
	while (i >= 0 && value != 0)
	{
		rem = value % 3;
		value = value / 3;
		if (rem == 2)
		{
			rem = -1;
			value++;
		}
		else if (rem == -2)
		{
			rem = 1;
			value--;
		}
		tryte->trits[i] = (t_s3_trit)rem;
		i--;
	}
}

/* ============================================================== */
/*                 NIVEAU 2 : WORD (24 trits packes)               */
/* ============================================================== */

/*
** ---- Helpers internes : encodage trit ↔ 2 bits ----
**
** Table de conversion :
**   trit -1 → 0b11 (S3_ENC_NEG)
**   trit  0 → 0b00 (S3_ENC_ZERO)
**   trit +1 → 0b01 (S3_ENC_POS)
**
** Astuce : on utilise (trit & 3) car en complement a 2 :
**   -1 = 0xFF → & 3 = 3 = 0b11 ✓
**    0 = 0x00 → & 3 = 0 = 0b00 ✓
**   +1 = 0x01 → & 3 = 1 = 0b01 ✓
*/
static uint64_t	s3_encode_trit(t_s3_trit t)
{
	return ((uint64_t)t & 3ULL);
}

/*
** Decodage 2 bits → trit.
** 00 → 0, 01 → +1, 11 → -1, 10 → invalide (retourne 0 + flag).
**
** Astuce pour 11 → -1 :
**   Les 2 bits sont identiques (11) → c'est un signed 2-bit = -1.
**   En pratique on fait : si bit1 est set, retourne -(bit0).
**   00 → 0, 01 → +1, 10 → invalide, 11 → -1.
*/
static t_s3_trit	s3_decode_trit(uint64_t enc)
{
	if (enc == S3_ENC_ZERO)
		return (S3_TRIT_ZERO);
	if (enc == S3_ENC_POS)
		return (S3_TRIT_POS);
	if (enc == S3_ENC_NEG)
		return (S3_TRIT_NEG);
	return (S3_TRIT_ZERO);
}

/* ---- Construction / extraction ---- */

/*
** s3_word_make : assemble tag + flags + data en un uint64_t.
**
** On shift le tag de 60 bits, les flags de 56 bits,
** et on masque la data aux 48 bits bas.
*/
t_s3_word	s3_word_make(uint64_t tag, uint64_t flags, uint64_t data)
{
	return ((tag << S3_TAG_SHIFT)
		| (flags << S3_FLAG_SHIFT)
		| (data & S3_DATA_MASK));
}

uint64_t	s3_word_tag(t_s3_word w)
{
	return ((w & S3_TAG_MASK) >> S3_TAG_SHIFT);
}

uint64_t	s3_word_flags(t_s3_word w)
{
	return ((w & S3_FLAG_MASK) >> S3_FLAG_SHIFT);
}

uint64_t	s3_word_data(t_s3_word w)
{
	return (w & S3_DATA_MASK);
}

/* ---- Pack / Unpack ---- */

/*
** s3_word_pack : encode 24 t_s3_trit dans un s3_word.
**
** Chaque trit est encode en 2 bits dans les positions 0-47.
** trit[0] → bits 1-0 (LSB), trit[23] → bits 47-46 (MSB).
** Le tag est fourni par l'appelant.
*/
t_s3_word	s3_word_pack(const t_s3_trit *trits, uint64_t tag)
{
	uint64_t	data;
	int			i;

	data = 0;
	i = 0;
	while (i < S3_WORD_TRITS)
	{
		data |= s3_encode_trit(trits[i]) << (i * S3_TRIT_BITS);
		i++;
	}
	return (s3_word_make(tag, S3_FLAG_NONE, data));
}

/*
** s3_word_unpack : decode un s3_word en 24 t_s3_trit.
**
** Extrait chaque paire de bits et la convertit en trit.
** Verifie au passage qu'aucun pattern invalide (10) n'existe.
** Retourne 0 si OK, -1 si corruption.
*/
int	s3_word_unpack(t_s3_word w, t_s3_trit *out)
{
	uint64_t	data;
	uint64_t	enc;
	int			i;

	if (!out)
		return (-1);
	data = s3_word_data(w);
	i = 0;
	while (i < S3_WORD_TRITS)
	{
		enc = (data >> (i * S3_TRIT_BITS)) & 3ULL;
		if (enc == S3_ENC_INVALID)
			return (-1);
		out[i] = s3_decode_trit(enc);
		i++;
	}
	return (0);
}

/* ---- Conversion int ↔ word ---- */

/*
** s3_word_from_int : convertit un int64_t en s3_word (balanced ternary).
**
** Meme algorithme que s3_tryte_from_int, mais sur 24 trits.
** On encode chaque trit directement dans les bits data du word.
*/
t_s3_word	s3_word_from_int(int64_t value)
{
	uint64_t	data;
	int			i;
	int			rem;
	int64_t		v;

	data = 0;
	v = value;
	i = 0;
	while (i < S3_WORD_TRITS && v != 0)
	{
		rem = (int)(v % 3);
		v = v / 3;
		if (rem == 2)
		{
			rem = -1;
			v++;
		}
		else if (rem == -2)
		{
			rem = 1;
			v--;
		}
		data |= ((uint64_t)rem & 3ULL) << (i * S3_TRIT_BITS);
		i++;
	}
	return (s3_word_make(S3_TAG_INT, S3_FLAG_NONE, data));
}

/*
** s3_word_to_int : decode un s3_word en int64_t.
**
** Parcourt les 24 trits, chacun multiplie par 3^position.
** Utilise s3_decode_trit pour chaque paire de bits.
*/
int64_t	s3_word_to_int(t_s3_word w)
{
	uint64_t	data;
	uint64_t	enc;
	int64_t		result;
	int64_t		power;
	int			i;

	data = s3_word_data(w);
	result = 0;
	power = 1;
	i = 0;
	while (i < S3_WORD_TRITS)
	{
		enc = (data >> (i * S3_TRIT_BITS)) & 3ULL;
		result += (int64_t)s3_decode_trit(enc) * power;
		power *= 3;
		i++;
	}
	return (result);
}

/* ---- Arithmetique parallele ---- */

/*
** s3_word_neg : negation de 24 trits en parallele.
**
** Avec l'encodage 00=0, 01=+1, 11=-1 :
** neg(00) = 00 (0 reste 0)
** neg(01) = 11 (+1 → -1)
** neg(11) = 01 (-1 → +1)
**
** Astuce : pour chaque paire de bits (a, b) :
** - Si la paire est 00, on ne touche pas.
** - Si la paire est non-zero, on XOR avec 10 (flip le bit haut).
**   01 XOR 10 = 11 ✓
**   11 XOR 10 = 01 ✓
**   00 XOR 10 = 10 ✗ (casse les zeros !)
**
** Donc on a besoin d'un masque des trits non-zero.
**
** Construction du masque :
**   hi = bits hauts de chaque paire (bits 1,3,5,...47)
**   lo = bits bas de chaque paire (bits 0,2,4,...46)
**   non_zero = hi | lo (1 si le trit est non-zero)
**   mask = non_zero * 2 (decale vers le bit haut de chaque paire)
**   → XOR data avec mask → negation correcte.
**
** Tout ca en 5-6 instructions bitwise. Pas de boucle.
*/
t_s3_word	s3_word_neg(t_s3_word w)
{
	uint64_t	data;
	uint64_t	meta;
	uint64_t	lo;
	uint64_t	hi;
	uint64_t	non_zero;
	uint64_t	mask;

	meta = w & S3_META_MASK;
	data = s3_word_data(w);
	lo = data & 0x555555555555ULL;
	hi = (data >> 1) & 0x555555555555ULL;
	non_zero = hi | lo;
	mask = non_zero << 1;
	return (meta | ((data ^ mask) & S3_DATA_MASK));
}

/*
** s3_word_add : addition de deux words, 24 trits avec carry.
**
** L'addition ternaire en parallele bitwise est complexe.
** On fait donc l'approche correcte trit-par-trit pour le moment.
** Optimisation SIMD possible plus tard (Pass 16).
**
** Pourquoi ne pas faire du bitwise parallele maintenant :
** La propagation de retenue ternaire est plus complexe que binaire.
** Un ripple-carry ternaire necessite 24 iterations sequentielles.
** L'optimisation (carry-lookahead ternaire) est un projet de
** recherche en soi — on l'implementera dans la pass d'optimisation.
*/
t_s3_word	s3_word_add(t_s3_word a, t_s3_word b)
{
	t_s3_trit	ta[S3_WORD_TRITS];
	t_s3_trit	tb[S3_WORD_TRITS];
	t_s3_trit	carry;
	t_s3_trit	trit_carry;
	int			i;
	int			sum;
	uint64_t	flags;

	s3_word_unpack(a, ta);
	s3_word_unpack(b, tb);
	carry = S3_TRIT_ZERO;
	i = 0;
	while (i < S3_WORD_TRITS)
	{
		sum = ta[i] + tb[i] + carry;
		carry = S3_TRIT_ZERO;
		if (sum > 1)
		{
			carry = S3_TRIT_POS;
			sum -= 3;
		}
		else if (sum < -1)
		{
			carry = S3_TRIT_NEG;
			sum += 3;
		}
		ta[i] = (t_s3_trit)sum;
		i++;
	}
	flags = S3_FLAG_NONE;
	if (carry != S3_TRIT_ZERO)
		flags = S3_FLAG_OVERFLOW;
	trit_carry = carry;
	(void)trit_carry;
	return (s3_word_pack(ta, S3_TAG_INT)
		| (flags << S3_FLAG_SHIFT));
}

/*
** s3_word_cmp : comparaison de deux words.
**
** Compare trit par trit du MSB vers le LSB.
** Le premier trit different determine le resultat.
** (Comme strcmp, mais en ternaire.)
*/
t_s3_trit	s3_word_cmp(t_s3_word a, t_s3_word b)
{
	uint64_t	da;
	uint64_t	db;
	uint64_t	enc_a;
	uint64_t	enc_b;
	t_s3_trit	ta;
	t_s3_trit	tb;
	int			i;

	da = s3_word_data(a);
	db = s3_word_data(b);
	if (da == db)
		return (S3_TRIT_ZERO);
	i = S3_WORD_TRITS - 1;
	while (i >= 0)
	{
		enc_a = (da >> (i * S3_TRIT_BITS)) & 3ULL;
		enc_b = (db >> (i * S3_TRIT_BITS)) & 3ULL;
		ta = s3_decode_trit(enc_a);
		tb = s3_decode_trit(enc_b);
		if (ta != tb)
			return (s3_trit_cmp(ta, tb));
		i--;
	}
	return (S3_TRIT_ZERO);
}

/* ---- Validation ---- */

/*
** s3_word_valid : detecte les patterns invalides (10) dans les 24 trits.
**
** Pour chaque paire de bits (hi, lo) :
**   Valide : 00, 01, 11
**   Invalide : 10 (hi=1, lo=0)
**
** Detection : hi AND (NOT lo) → 1 si pattern 10 existe.
** Si le resultat est non-zero, au moins un trit est corrompu.
*/
int	s3_word_valid(t_s3_word w)
{
	uint64_t	data;
	uint64_t	lo;
	uint64_t	hi;
	uint64_t	invalid;

	data = s3_word_data(w);
	lo = data & 0x555555555555ULL;
	hi = (data >> 1) & 0x555555555555ULL;
	invalid = hi & (~lo);
	return (invalid == 0);
}
