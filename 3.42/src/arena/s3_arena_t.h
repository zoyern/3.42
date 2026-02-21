/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_arena_t.h ::::::::::               */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Arena Allocator — Types                        */
/*                                                                   */
/*    L'arene est le coeur de la gestion memoire de 3.42.           */
/*    Principe : on alloue UN gros bloc, on avance un curseur.      */
/*    Quand la phase est finie, on libere TOUT d'un coup.           */
/*    Zero leak possible par construction.                           */
/*                                                                   */
/*    v2 : taille fixe (real-time), mark/rollback (PEG parser),     */
/*         config portable (mmap/malloc/static)                      */
/*                                                                   */
/* *************************************************************** */

#ifndef S3_ARENA_T_H
# define S3_ARENA_T_H

# include <stddef.h>

/*
** ======================== CONFIGURATION ========================
**
** S3_ARENA_BLOCK_SIZE : taille d'un bloc en octets.
**   4096 = 1 page memoire sur Linux/Mac/Windows (x86-64, ARM64)
**   256  = embedded (Arduino, STM32 avec peu de RAM)
**   Regle : toujours un multiple de S3_ALIGN.
**   Configurable a la compilation : -DS3_ARENA_BLOCK_SIZE=256
*/
# ifndef S3_ARENA_BLOCK_SIZE
#  define S3_ARENA_BLOCK_SIZE 4096
# endif

/*
** S3_ALIGN : alignement de chaque allocation en octets.
**   8 = optimal pour 64-bit (pointeurs, double, uint64_t)
**   4 = suffisant pour 32-bit (ARM Cortex-M, embedded)
**   Configurable : -DS3_ALIGN=4
*/
# ifndef S3_ALIGN
#  define S3_ALIGN 8
# endif

/*
** S3_ARENA_BACKEND : comment on obtient la memoire brute.
**   0 = malloc/free  (portable, par defaut)
**   1 = mmap/munmap  (Linux/Mac, zero overhead metadata)
**   2 = buffer statique (embedded sans OS, pas de malloc)
**   Configurable : -DS3_ARENA_BACKEND=2
*/
# ifndef S3_ARENA_BACKEND
#  define S3_ARENA_BACKEND 0
# endif

/*
** S3_ARENA_STATIC_SIZE : taille du buffer statique (backend 2).
**   Utilise seulement si S3_ARENA_BACKEND == 2.
**   Sur Arduino Uno : 1024 (la moitie des 2Ko de RAM)
**   Sur STM32 : 8192-32768 selon le modele
*/
# ifndef S3_ARENA_STATIC_SIZE
#  define S3_ARENA_STATIC_SIZE 4096
# endif

/* ======================== TYPES ======================== */

/*
** t_s3_arena_block : un bloc de memoire contigu.
**
** memory   : debut du bloc alloue
** used     : octets deja utilises (= position du curseur)
** capacity : taille totale du bloc
** next     : bloc suivant (NULL si dernier)
**
** Invariant : used <= capacity (toujours)
** Invariant : capacity == S3_ARENA_BLOCK_SIZE (taille fixe,
**             sauf si une allocation > block_size force un bloc plus grand)
*/
typedef struct s_s3_arena_block
{
	char						*memory;
	size_t						used;
	size_t						capacity;
	struct s_s3_arena_block		*next;
}	t_s3_arena_block;

/*
** t_s3_arena_mark : position sauvegardee pour rollback.
**
** Utilise par le PEG parser : on tente un parsing, si ca echoue,
** on revient a la position du mark. Zero copie, zero free.
**
** block  : le bloc actif au moment du mark
** used   : la position du curseur dans ce bloc
** total  : le total d'octets alloues au moment du mark
*/
typedef struct s_s3_arena_mark
{
	t_s3_arena_block	*block;
	size_t				used;
	size_t				total;
}	t_s3_arena_mark;

/*
** t_s3_arena : l'arene elle-meme.
**
** head    : premier bloc (debut de la liste chainee)
** current : bloc actif pour les allocations
** total   : octets alloues (tous blocs confondus)
**
** Usage typique :
**   t_s3_arena phase;
**   s3_arena_init(&phase, 0);           // 1 bloc de 4096
**   node = s3_arena_alloc(&phase, 64);  // bump cursor de 64
**   s3_arena_reset(&phase);             // cursor = 0, zero free
**   s3_arena_destroy(&phase);           // fin du programme
*/
typedef struct s_s3_arena
{
	t_s3_arena_block	*head;
	t_s3_arena_block	*current;
	size_t				total;
}	t_s3_arena;

#endif
