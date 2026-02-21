/* *************************************************************** */
/*                                                                   */
/*                    :::::::: main.c ::::::::::                     */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Tests Arena v2 + Trit v2                       */
/*                                                                   */
/* *************************************************************** */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "arena/s3_arena.h"
#include "trit/s3_trit.h"

static int	g_ok = 0;
static int	g_fail = 0;

static void	check(int ok, const char *name)
{
	if (ok)
	{
		printf("  [OK]   %s\n", name);
		g_ok++;
	}
	else
	{
		printf("  [FAIL] %s\n", name);
		g_fail++;
	}
}

/* ======================== ARENA TESTS ======================== */

static void	test_arena(void)
{
	t_s3_arena		a;
	t_s3_arena_mark	m;
	int				*p;
	void			*p1;
	void			*p2;
	char			*big;
	int				i;
	int				ok;

	printf("\n--- Arena : init / alloc / destroy ---\n");
	check(s3_arena_init(&a, 0) == 0, "init OK");
	p = (int *)s3_arena_alloc(&a, sizeof(int));
	check(p != NULL, "alloc non-NULL");
	*p = 42;
	check(*p == 42, "ecriture/lecture");
	s3_arena_destroy(&a);
	check(a.head == NULL, "destroy clean");

	printf("\n--- Arena : alignement ---\n");
	s3_arena_init(&a, 0);
	p1 = s3_arena_alloc(&a, 1);
	p2 = s3_arena_alloc(&a, 3);
	check(((size_t)p1 % S3_ALIGN) == 0, "p1 aligne");
	check(((size_t)p2 % S3_ALIGN) == 0, "p2 aligne");
	s3_arena_destroy(&a);

	printf("\n--- Arena : overflow bloc ---\n");
	s3_arena_init(&a, 256);
	ok = 1;
	i = 0;
	while (i < 100)
	{
		if (!s3_arena_alloc(&a, 64))
			ok = 0;
		i++;
	}
	check(ok, "100 allocs de 64");
	check(a.head->next != NULL, "multi-blocs");
	s3_arena_destroy(&a);

	printf("\n--- Arena : reset ---\n");
	s3_arena_init(&a, 0);
	p1 = s3_arena_alloc(&a, 128);
	s3_arena_reset(&a);
	check(a.total == 0, "total = 0");
	p2 = s3_arena_alloc(&a, 128);
	check(p2 == p1, "meme adresse");
	s3_arena_destroy(&a);

	printf("\n--- Arena : mark/rollback ---\n");
	s3_arena_init(&a, 0);
	s3_arena_alloc(&a, 64);
	m = s3_arena_mark(&a);
	s3_arena_alloc(&a, 128);
	s3_arena_alloc(&a, 256);
	s3_arena_rollback(&a, m);
	check(a.total == m.total, "total restaure");
	p1 = s3_arena_alloc(&a, 128);
	s3_arena_rollback(&a, m);
	p2 = s3_arena_alloc(&a, 128);
	check(p1 == p2, "meme adresse apres rollback");
	s3_arena_destroy(&a);

	printf("\n--- Arena : edge cases ---\n");
	check(s3_arena_init(NULL, 0) == -1, "init(NULL)");
	s3_arena_init(&a, 0);
	check(s3_arena_alloc(NULL, 64) == NULL, "alloc(NULL)");
	check(s3_arena_alloc(&a, 0) == NULL, "alloc(0)");
	s3_arena_destroy(&a);
	s3_arena_destroy(NULL);
	check(1, "destroy(NULL) safe");

	printf("\n--- Arena : grosse alloc ---\n");
	s3_arena_init(&a, 256);
	big = (char *)s3_arena_alloc(&a, 1024);
	check(big != NULL, "alloc 1024 dans 256");
	memset(big, 'A', 1024);
	check(big[0] == 'A' && big[1023] == 'A', "ecriture 1024");
	s3_arena_destroy(&a);
}

/* ======================== TRIT UNITAIRE TESTS ======================== */

static void	test_trit_unit(void)
{
	t_s3_trit	carry;
	t_s3_trit	res;
	t_s3_tryte	ty;

	printf("\n--- Trit : multiplication ---\n");
	check(s3_trit_mul(S3_TRIT_POS, S3_TRIT_POS) == S3_TRIT_POS, "(+1)*(+1)=+1");
	check(s3_trit_mul(S3_TRIT_POS, S3_TRIT_NEG) == S3_TRIT_NEG, "(+1)*(-1)=-1");
	check(s3_trit_mul(S3_TRIT_NEG, S3_TRIT_NEG) == S3_TRIT_POS, "(-1)*(-1)=+1");
	check(s3_trit_mul(S3_TRIT_ZERO, S3_TRIT_POS) == S3_TRIT_ZERO, "0*(+1)=0");

	printf("\n--- Trit : addition ---\n");
	res = s3_trit_add(S3_TRIT_POS, S3_TRIT_POS, &carry);
	check(res == S3_TRIT_NEG && carry == S3_TRIT_POS, "(+1)+(+1)=-1 c=+1");
	res = s3_trit_add(S3_TRIT_POS, S3_TRIT_NEG, &carry);
	check(res == S3_TRIT_ZERO && carry == S3_TRIT_ZERO, "(+1)+(-1)=0 c=0");
	res = s3_trit_add(S3_TRIT_NEG, S3_TRIT_NEG, &carry);
	check(res == S3_TRIT_POS && carry == S3_TRIT_NEG, "(-1)+(-1)=+1 c=-1");

	printf("\n--- Trit : negation (Pauli-X) ---\n");
	check(s3_trit_neg(S3_TRIT_POS) == S3_TRIT_NEG, "neg(+1)=-1");
	check(s3_trit_neg(S3_TRIT_NEG) == S3_TRIT_POS, "neg(-1)=+1");
	check(s3_trit_neg(S3_TRIT_ZERO) == S3_TRIT_ZERO, "neg(0)=0");
	check(s3_trit_neg(s3_trit_neg(S3_TRIT_POS)) == S3_TRIT_POS, "involutoire");

	printf("\n--- Trit : cmp ---\n");
	check(s3_trit_cmp(S3_TRIT_POS, S3_TRIT_NEG) == S3_TRIT_POS, "+1 > -1");
	check(s3_trit_cmp(S3_TRIT_NEG, S3_TRIT_POS) == S3_TRIT_NEG, "-1 < +1");
	check(s3_trit_cmp(S3_TRIT_ZERO, S3_TRIT_ZERO) == S3_TRIT_ZERO, "0 == 0");

	printf("\n--- Trit : min/max (Lukasiewicz) ---\n");
	check(s3_trit_min(S3_TRIT_POS, S3_TRIT_NEG) == S3_TRIT_NEG, "min(+1,-1)=-1");
	check(s3_trit_max(S3_TRIT_POS, S3_TRIT_NEG) == S3_TRIT_POS, "max(+1,-1)=+1");
	check(s3_trit_min(S3_TRIT_ZERO, S3_TRIT_POS) == S3_TRIT_ZERO, "min(0,+1)=0");

	printf("\n--- Tryte : conversion ---\n");
	s3_tryte_from_int(&ty, 42);
	check(s3_tryte_to_int(&ty) == 42, "42 roundtrip");
	s3_tryte_from_int(&ty, -42);
	check(s3_tryte_to_int(&ty) == -42, "-42 roundtrip");
	s3_tryte_from_int(&ty, 0);
	check(s3_tryte_to_int(&ty) == 0, "0 roundtrip");
	s3_tryte_from_int(&ty, 364);
	check(s3_tryte_to_int(&ty) == 364, "364 (max) roundtrip");
	s3_tryte_from_int(&ty, -364);
	check(s3_tryte_to_int(&ty) == -364, "-364 (min) roundtrip");
}

/* ======================== WORD TESTS ======================== */

static void	test_word(void)
{
	t_s3_word	w;
	t_s3_word	wa;
	t_s3_word	wb;
	t_s3_word	wsum;
	t_s3_trit	trits[S3_WORD_TRITS];
	int			i;

	printf("\n--- Word : construction ---\n");
	w = s3_word_make(S3_TAG_INT, S3_FLAG_NONE, 0x123);
	check(s3_word_tag(w) == S3_TAG_INT, "tag INT");
	check(s3_word_flags(w) == S3_FLAG_NONE, "flags NONE");
	check(s3_word_data(w) == 0x123, "data 0x123");

	w = s3_word_make(S3_TAG_CAP, S3_FLAG_OVERFLOW, 0xABCDEF);
	check(s3_word_tag(w) == S3_TAG_CAP, "tag CAP");
	check(s3_word_flags(w) == S3_FLAG_OVERFLOW, "flag OVERFLOW");
	check(s3_word_data(w) == 0xABCDEF, "data preserved");

	printf("\n--- Word : from_int / to_int ---\n");
	check(s3_word_to_int(s3_word_from_int(0)) == 0, "0 roundtrip");
	check(s3_word_to_int(s3_word_from_int(1)) == 1, "1 roundtrip");
	check(s3_word_to_int(s3_word_from_int(-1)) == -1, "-1 roundtrip");
	check(s3_word_to_int(s3_word_from_int(42)) == 42, "42 roundtrip");
	check(s3_word_to_int(s3_word_from_int(-42)) == -42, "-42 roundtrip");
	check(s3_word_to_int(s3_word_from_int(1000)) == 1000, "1000 roundtrip");
	check(s3_word_to_int(s3_word_from_int(-999999)) == -999999, "-999999 roundtrip");
	/* Max 24 trits = (3^24 - 1) / 2 = 141214768240 */
	check(s3_word_to_int(s3_word_from_int(141214768240LL)) == 141214768240LL,
		"max 24 trits roundtrip");
	check(s3_word_to_int(s3_word_from_int(-141214768240LL)) == -141214768240LL,
		"min 24 trits roundtrip");

	printf("\n--- Word : pack / unpack ---\n");
	i = 0;
	while (i < S3_WORD_TRITS)
	{
		trits[i] = (t_s3_trit)((i % 3) - 1);
		i++;
	}
	w = s3_word_pack(trits, S3_TAG_INT);
	check(s3_word_valid(w), "pack → valid");
	{
		t_s3_trit	out[S3_WORD_TRITS];
		int			ok;

		check(s3_word_unpack(w, out) == 0, "unpack OK");
		ok = 1;
		i = 0;
		while (i < S3_WORD_TRITS)
		{
			if (out[i] != trits[i])
				ok = 0;
			i++;
		}
		check(ok, "pack/unpack roundtrip");
	}

	printf("\n--- Word : neg (bitwise parallele) ---\n");
	w = s3_word_from_int(42);
	check(s3_word_to_int(s3_word_neg(w)) == -42, "neg(42) = -42");
	w = s3_word_from_int(-999);
	check(s3_word_to_int(s3_word_neg(w)) == 999, "neg(-999) = 999");
	w = s3_word_from_int(0);
	check(s3_word_to_int(s3_word_neg(w)) == 0, "neg(0) = 0");
	/* Involutoire : neg(neg(x)) == x */
	w = s3_word_from_int(12345);
	check(s3_word_to_int(s3_word_neg(s3_word_neg(w))) == 12345, "neg involutoire");

	printf("\n--- Word : add ---\n");
	wa = s3_word_from_int(100);
	wb = s3_word_from_int(200);
	wsum = s3_word_add(wa, wb);
	check(s3_word_to_int(wsum) == 300, "100 + 200 = 300");

	wa = s3_word_from_int(-50);
	wb = s3_word_from_int(50);
	wsum = s3_word_add(wa, wb);
	check(s3_word_to_int(wsum) == 0, "-50 + 50 = 0");

	wa = s3_word_from_int(-100);
	wb = s3_word_from_int(-200);
	wsum = s3_word_add(wa, wb);
	check(s3_word_to_int(wsum) == -300, "-100 + -200 = -300");

	wa = s3_word_from_int(0);
	wb = s3_word_from_int(42);
	wsum = s3_word_add(wa, wb);
	check(s3_word_to_int(wsum) == 42, "0 + 42 = 42");

	printf("\n--- Word : cmp ---\n");
	wa = s3_word_from_int(100);
	wb = s3_word_from_int(200);
	check(s3_word_cmp(wa, wb) == S3_TRIT_NEG, "100 < 200");
	check(s3_word_cmp(wb, wa) == S3_TRIT_POS, "200 > 100");
	check(s3_word_cmp(wa, wa) == S3_TRIT_ZERO, "100 == 100");

	wa = s3_word_from_int(-50);
	wb = s3_word_from_int(50);
	check(s3_word_cmp(wa, wb) == S3_TRIT_NEG, "-50 < 50");

	printf("\n--- Word : validation ---\n");
	w = s3_word_from_int(42);
	check(s3_word_valid(w), "42 valid");
	w = s3_word_from_int(0);
	check(s3_word_valid(w), "0 valid");
	/* Injection manuelle d'un pattern invalide (10) */
	w = s3_word_make(S3_TAG_INT, S3_FLAG_NONE, 0x2ULL);
	check(!s3_word_valid(w), "pattern 10 detecte");
}

/* ======================== MAIN ======================== */

int	main(void)
{
	printf("=== 3.42 — Tests Arena v2 + Trit v2 ===\n");
	test_arena();
	test_trit_unit();
	test_word();
	printf("\n=== Resultats : %d OK, %d FAIL ===\n", g_ok, g_fail);
	if (g_fail > 0)
		return (1);
	return (0);
}
