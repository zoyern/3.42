/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_arena.c ::::::::::                 */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Arena Allocator — Implementation               */
/*                                                                   */
/*    Regles strictes :                                              */
/*    - L'allocation brute (malloc/mmap/static) est UNIQUEMENT ici  */
/*    - free/munmap est UNIQUEMENT dans s3_arena_destroy()          */
/*    - Tout le reste du projet passe par s3_arena_alloc()          */
/*    → Si cette regle est respectee : ZERO leak garanti.           */
/*                                                                   */
/*    v2 : taille fixe (real-time), mark/rollback (PEG),            */
/*         3 backends (malloc, mmap, static)                         */
/*                                                                   */
/* *************************************************************** */

#include "s3_arena.h"

/*
** ======================== BACKEND MEMOIRE ========================
**
** On abstrait l'allocation brute derriere 2 fonctions :
**   s3_raw_alloc(size) → pointeur vers `size` octets bruts
**   s3_raw_free(ptr, size) → libere ces octets
**
** Le reste de l'arena ne sait pas si c'est malloc, mmap, ou static.
** Pour changer de backend : -DS3_ARENA_BACKEND=1 a la compilation.
*/

/* --- Backend 0 : malloc/free (portable, par defaut) ------------- */
#if S3_ARENA_BACKEND == 0

# include <stdlib.h>

static void	*s3_raw_alloc(size_t size)
{
	return (malloc(size));
}

static void	s3_raw_free(void *ptr, size_t size)
{
	(void)size;
	free(ptr);
}

/* --- Backend 1 : mmap/munmap (Linux/Mac, zero metadata) --------- */
#elif S3_ARENA_BACKEND == 1

# include <sys/mman.h>

/*
** mmap avec MAP_ANONYMOUS : le kernel donne des pages vierges.
** Avantage sur malloc : pas de header de 16 octets par bloc.
** Le kernel gere directement les pages physiques.
*/
static void	*s3_raw_alloc(size_t size)
{
	void	*ptr;

	ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (ptr == MAP_FAILED)
		return (NULL);
	return (ptr);
}

static void	s3_raw_free(void *ptr, size_t size)
{
	munmap(ptr, size);
}

/* --- Backend 2 : buffer statique (embedded, pas de malloc) ------- */
#elif S3_ARENA_BACKEND == 2

/*
** Sur Arduino/STM32 sans OS, pas de malloc disponible.
** On utilise un tableau global. L'arene "alloue" dedans.
** Limitation : un seul bloc, taille fixe a la compilation.
*/
static char		g_s3_static_pool[S3_ARENA_STATIC_SIZE];
static int		g_s3_static_used = 0;

static void	*s3_raw_alloc(size_t size)
{
	if (size > S3_ARENA_STATIC_SIZE || g_s3_static_used)
		return (NULL);
	g_s3_static_used = 1;
	return ((void *)g_s3_static_pool);
}

static void	s3_raw_free(void *ptr, size_t size)
{
	(void)ptr;
	(void)size;
	g_s3_static_used = 0;
}

#else
# error "S3_ARENA_BACKEND invalide (0=malloc, 1=mmap, 2=static)"
#endif

/* ======================== ALIGNEMENT ======================== */

/*
** Arrondit `size` au prochain multiple de S3_ALIGN.
** Exemple avec S3_ALIGN=8 : 13 → 16, 8 → 8, 1 → 8
**
** Comment ca marche :
**   mask = 7 (0b0111)
**   (13 + 7) = 20 = 0b10100
**   20 & ~7  = 20 & 0b11000 = 16
*/
static size_t	s3_align(size_t size)
{
	return ((size + (S3_ALIGN - 1)) & ~((size_t)(S3_ALIGN - 1)));
}

/* ======================== BLOC ======================== */

/*
** Cree un nouveau bloc de memoire brute.
** Le bloc lui-meme (la struct) est alloue au debut du bloc memoire
** pour eviter une allocation separee (1 alloc au lieu de 2).
**
** Layout en memoire :
**   [t_s3_arena_block | padding | espace utilisable ........]
**   ^memory                     ^memory + header_size
**
** Le `used` commence apres le header pour ne pas ecraser la struct.
*/
static t_s3_arena_block	*s3_block_new(size_t cap)
{
	size_t				header;
	size_t				total;
	void				*raw;
	t_s3_arena_block	*block;

	header = s3_align(sizeof(t_s3_arena_block));
	total = header + cap;
	raw = s3_raw_alloc(total);
	if (!raw)
		return (NULL);
	block = (t_s3_arena_block *)raw;
	block->memory = (char *)raw;
	block->used = header;
	block->capacity = total;
	block->next = NULL;
	return (block);
}

/* ======================== API PUBLIQUE ======================== */

/*
** s3_arena_init : initialise l'arene avec un premier bloc.
**
** Si cap == 0, on utilise S3_ARENA_BLOCK_SIZE (4096).
** On ne met PAS la memoire a zero (pas de calloc) :
** le type system lineaire garantit qu'on n'accede jamais
** a de la memoire non initialisee. Economie : 4096 ecritures.
*/
int	s3_arena_init(t_s3_arena *arena, size_t cap)
{
	t_s3_arena_block	*block;

	if (!arena)
		return (-1);
	if (cap == 0)
		cap = S3_ARENA_BLOCK_SIZE;
	block = s3_block_new(cap);
	if (!block)
		return (-1);
	arena->head = block;
	arena->current = block;
	arena->total = 0;
	return (0);
}

/*
** s3_arena_alloc : le bump allocator.
**
** Cas normal (95% des appels) :
**   On avance le curseur de `aligned` octets. C'est tout.
**   Cout : 1 addition, 1 comparaison, 1 store. ~3 instructions CPU.
**
** Cas "bloc plein" :
**   On cree un nouveau bloc de MEME TAILLE (pas de doublement).
**   Pourquoi pas doubler : en real-time (VR 90fps), le temps
**   d'allocation doit etre borne. Si on double, le 10e bloc
**   fait 2Mo et l'allocation prend un temps imprevisible.
**
** Cas "trop gros" :
**   Si l'allocation demandee depasse la taille d'un bloc,
**   on cree un bloc special juste pour cette allocation.
**   Rare : un noeud AST fait ~64 octets, pas 4096.
*/
void	*s3_arena_alloc(t_s3_arena *arena, size_t size)
{
	size_t				aligned;
	void				*ptr;
	size_t				new_cap;
	t_s3_arena_block	*block;

	if (!arena || size == 0)
		return (NULL);
	aligned = s3_align(size);
	if (arena->current->used + aligned <= arena->current->capacity)
	{
		ptr = arena->current->memory + arena->current->used;
		arena->current->used += aligned;
		arena->total += aligned;
		return (ptr);
	}
	new_cap = arena->head->capacity - s3_align(sizeof(t_s3_arena_block));
	if (new_cap < aligned)
		new_cap = aligned;
	block = s3_block_new(new_cap);
	if (!block)
		return (NULL);
	arena->current->next = block;
	arena->current = block;
	ptr = block->memory + block->used;
	block->used += aligned;
	arena->total += aligned;
	return (ptr);
}

/*
** s3_arena_reset : remet tous les curseurs a zero.
**
** Les blocs NE SONT PAS liberes — ils seront reutilises.
** C'est ce qui rend le reset si rapide : on ne touche pas
** a la memoire, on dit juste "tout est libre".
**
** Le type system (Pass 6 : verification lineaire) a GARANTI
** que toutes les ressources dans cette arene ont ete consommees.
** Donc au runtime, ce reset est sur — la preuve est au compile-time.
*/
void	s3_arena_reset(t_s3_arena *arena)
{
	size_t				header;
	t_s3_arena_block	*block;

	if (!arena)
		return ;
	header = s3_align(sizeof(t_s3_arena_block));
	block = arena->head;
	while (block)
	{
		block->used = header;
		block = block->next;
	}
	arena->current = arena->head;
	arena->total = 0;
}

/*
** s3_arena_destroy : libere TOUTE la memoire.
**
** Parcourt la liste chainee, libere chaque bloc.
** Apres cet appel, l'arene est inutilisable (head = NULL).
** S'appelle une seule fois : a la fin du programme.
*/
void	s3_arena_destroy(t_s3_arena *arena)
{
	t_s3_arena_block	*block;
	t_s3_arena_block	*next;

	if (!arena)
		return ;
	block = arena->head;
	while (block)
	{
		next = block->next;
		s3_raw_free(block->memory, block->capacity);
		block = next;
	}
	arena->head = NULL;
	arena->current = NULL;
	arena->total = 0;
}

/*
** s3_arena_mark : photographie l'etat actuel de l'arene.
**
** Retourne un "snapshot" (instantane) : quel bloc, quelle position.
** Le PEG parser utilise ca pour le backtracking :
**
**   mark = s3_arena_mark(&arena);
**   resultat = tente_parser(&arena);   // alloue des noeuds
**   if (!resultat)
**       s3_arena_rollback(&arena, mark); // "annule" les allocations
**
** Cout : copie de 3 valeurs (1 pointeur + 2 size_t). ~24 octets.
*/
t_s3_arena_mark	s3_arena_mark(t_s3_arena *arena)
{
	t_s3_arena_mark	m;

	m.block = arena->current;
	m.used = arena->current->used;
	m.total = arena->total;
	return (m);
}

/*
** s3_arena_rollback : revient a la position sauvegardee.
**
** Tout ce qui a ete alloue entre mark et rollback est "oublie".
** La memoire n'est pas liberee ni mise a zero — elle sera
** reutilisee par les prochains alloc() ou ecrasee au reset().
**
** Si on a depasse dans un bloc suivant entre-temps, on revient
** quand meme au bloc du mark. Les blocs intermediaires restent
** dans la liste (ils seront reutilises ou liberes au destroy).
*/
void	s3_arena_rollback(t_s3_arena *arena, t_s3_arena_mark mark)
{
	if (!arena)
		return ;
	arena->current = mark.block;
	arena->current->used = mark.used;
	arena->total = mark.total;
}
