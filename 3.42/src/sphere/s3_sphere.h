/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_sphere.h ::::::::::                */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Sphere : interface publique                    */
/*                                                                   */
/* *************************************************************** */

#ifndef S3_SPHERE_H
# define S3_SPHERE_H

# include "s3_sphere_t.h"
# include "../arena/s3_arena.h"

/*
** s3_sphere_new : cree une sphere dans l'arene donnee.
**   Toute sphere est allouee via l'arene — jamais de malloc direct.
**   theta, phi, r : coordonnees initiales.
**   Retourne NULL si l'arene refuse l'allocation.
*/
t_s3_sphere	*s3_sphere_new(t_s3_arena *arena, float theta, float phi,
				float r);

/*
** s3_sphere_nest : ajoute `child` comme enfant de `parent`.
**   Cree la structure de spheres emboitees.
**   L'enfant est ajoute en tete de la liste (O(1)).
*/
void		s3_sphere_nest(t_s3_sphere *parent, t_s3_sphere *child);

/*
** s3_sphere_dist : distance entre deux spheres.
**   Distance geodesique (plus court chemin sur la surface).
**   Utile pour : proximite de couleurs, similarite semantique,
**   compression (spheres proches = regroupables).
*/
float		s3_sphere_dist(t_s3_sphere *a, t_s3_sphere *b);

/*
** s3_sphere_interf : interference entre deux spheres.
**   Retourne une nouvelle sphere resultant du melange.
**   En mode rapide : multiplication (table ternaire).
**   Allouee dans l'arene.
*/
t_s3_sphere	*s3_sphere_interf(t_s3_arena *arena, t_s3_sphere *a,
				t_s3_sphere *b);

/*
** s3_sphere_rotate : applique une rotation (d_theta, d_phi).
**   Modifie la sphere en place.
**   C'est l'operation fondamentale : un calcul = une rotation.
*/
void		s3_sphere_rotate(t_s3_sphere *s, float d_theta, float d_phi);

#endif
