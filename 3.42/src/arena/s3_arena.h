/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_arena.h ::::::::::                 */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Arena Allocator — Interface publique           */
/*                                                                   */
/*    5 fonctions + 1 mark/rollback. Pas de free() individuel.     */
/*    C'est volontaire : on ne peut PAS oublier de liberer.         */
/*                                                                   */
/*    Toute la memoire du projet passe par ici.                     */
/*    malloc() et free() n'existent nulle part ailleurs.            */
/*                                                                   */
/* *************************************************************** */

#ifndef S3_ARENA_H
# define S3_ARENA_H

# include "s3_arena_t.h"

/*
** s3_arena_init : cree une arene.
**   cap = 0 → utilise S3_ARENA_BLOCK_SIZE (4096 par defaut)
**   Retourne 0 si OK, -1 si l'allocation du bloc echoue.
*/
int				s3_arena_init(t_s3_arena *arena, size_t cap);

/*
** s3_arena_alloc : alloue `size` octets dans l'arene.
**   Alignement garanti a S3_ALIGN octets (8 par defaut).
**   Si le bloc actif est plein → nouveau bloc de MEME taille
**   (pas de doublement — temps borne pour le real-time).
**   Exception : si size > block_size, un bloc special est cree.
**   Retourne NULL seulement si plus de RAM disponible.
*/
void			*s3_arena_alloc(t_s3_arena *arena, size_t size);

/*
** s3_arena_reset : remet l'arene a zero SANS liberer la memoire.
**   Les blocs sont conserves, seuls les curseurs sont remis a 0.
**   O(nombre_de_blocs) — typiquement O(1) si un seul bloc suffit.
**   Ideal entre 2 frames VR (90fps) ou entre 2 passes du compilateur.
*/
void			s3_arena_reset(t_s3_arena *arena);

/*
** s3_arena_destroy : libere TOUTE la memoire de l'arene.
**   Tous les blocs sont liberes. L'arene est remise a zero.
**   Appeler une seule fois, a la fin du programme ou de la phase.
*/
void			s3_arena_destroy(t_s3_arena *arena);

/*
** s3_arena_mark : sauvegarde la position actuelle du curseur.
**   Retourne un t_s3_arena_mark qu'on passe a s3_arena_rollback.
**
**   Usage (PEG parser) :
**     mark = s3_arena_mark(&arena);
**     node = tente_parser_expression(&arena);
**     if (!node)
**         s3_arena_rollback(&arena, mark);  // annule, zero cout
*/
t_s3_arena_mark	s3_arena_mark(t_s3_arena *arena);

/*
** s3_arena_rollback : restaure le curseur a la position du mark.
**   Tout ce qui a ete alloue apres le mark est "oublie".
**   La memoire n'est pas liberee (elle sera reutilisee ou resetee).
**   O(1) si on est toujours dans le meme bloc.
*/
void			s3_arena_rollback(t_s3_arena *arena, t_s3_arena_mark mark);

#endif
