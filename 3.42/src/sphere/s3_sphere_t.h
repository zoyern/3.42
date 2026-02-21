/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_sphere_t.h ::::::::::              */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Sphere : le conteneur universel               */
/*                                                                   */
/*    Chaque donnee dans 3.42 est une sphere avec 3 coordonnees :   */
/*    theta (angle vertical), phi (angle horizontal), r (rayon).    */
/*    C'est la brique atomique de tout le systeme.                  */
/*                                                                   */
/* *************************************************************** */

#ifndef S3_SPHERE_T_H
# define S3_SPHERE_T_H

/*
** Mode du conteneur spherique.
** Determine comment les donnees enfants sont interpretees.
*/
typedef enum e_s3_mode
{
	S3_MODE_CLASSIC,
	S3_MODE_QUANTUM,
	S3_MODE_PROBABILISTIC,
	S3_MODE_FACTORED,
	S3_MODE_VECTORIZED
}	t_s3_mode;

/*
** s_s3_sphere : le conteneur universel de 3.42.
**
** theta    : angle vertical [0, PI] — du pole nord au pole sud
**            En couleur : luminosite (0=noir, PI=blanc)
**            En ternaire : +1 (nord), 0 (equateur), -1 (sud)
**
** phi      : angle horizontal [0, 2*PI] — rotation autour de l'axe
**            En couleur : teinte (rouge → vert → bleu → rouge)
**            En booleen : contexte, direction semantique
**
** r        : rayon [0, 1] standard. Extensions :
**            r > 1 : amplifie (HDR)
**            r < 0 : anti-valeur (diff, annulation)
**            r = 0 : indetermine (variable non initialisee)
**
** mode     : comment interpreter les enfants de cette sphere
** children : pointeur vers le premier enfant (sphere emboitee)
** next     : pointeur vers la sphere suivante au meme niveau
** count    : nombre d'enfants directs
*/
typedef struct s_s3_sphere
{
	float					theta;
	float					phi;
	float					r;
	t_s3_mode				mode;
	struct s_s3_sphere		*children;
	struct s_s3_sphere		*next;
	int						count;
}	t_s3_sphere;

#endif
