/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3_sphere.c ::::::::::                */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Sphere : implementation                       */
/*                                                                   */
/* *************************************************************** */

#include "s3_sphere.h"
#include <math.h>

/*
** PI et TAU (2*PI) — constantes fondamentales pour les rotations.
** TAU = un tour complet. Toute rotation > TAU est equivalente
** a rotation modulo TAU (comme un cercle).
*/
# define S3_PI  3.14159265358979323846f
# define S3_TAU 6.28318530717958647692f

t_s3_sphere	*s3_sphere_new(t_s3_arena *arena, float theta, float phi,
	float r)
{
	t_s3_sphere	*s;

	if (!arena)
		return (NULL);
	s = (t_s3_sphere *)s3_arena_alloc(arena, sizeof(t_s3_sphere));
	if (!s)
		return (NULL);
	s->theta = theta;
	s->phi = phi;
	s->r = r;
	s->mode = S3_MODE_CLASSIC;
	s->children = NULL;
	s->next = NULL;
	s->count = 0;
	return (s);
}

void	s3_sphere_nest(t_s3_sphere *parent, t_s3_sphere *child)
{
	if (!parent || !child)
		return ;
	child->next = parent->children;
	parent->children = child;
	parent->count++;
}

/*
** Distance geodesique entre deux points sur la sphere.
** Formule : arccos(cos(t1)*cos(t2) + sin(t1)*sin(t2)*cos(p1-p2))
** ATTENTION : theta est une COLATITUDE (0=pole nord, PI=pole sud).
** Donc c'est cos*cos + sin*sin*cos(dphi), PAS l'inverse.
** Si on inverse sin/cos, la distance est 0 quand deux points
** sont a l'equateur (theta=PI/2) → bug silencieux.
** Le resultat est en radians [0, PI].
** On multiplie par la moyenne des rayons pour ponderer.
*/
float	s3_sphere_dist(t_s3_sphere *a, t_s3_sphere *b)
{
	float	cos_angle;
	float	avg_r;

	if (!a || !b)
		return (-1.0f);
	cos_angle = cosf(a->theta) * cosf(b->theta)
		+ sinf(a->theta) * sinf(b->theta) * cosf(a->phi - b->phi);
	if (cos_angle > 1.0f)
		cos_angle = 1.0f;
	if (cos_angle < -1.0f)
		cos_angle = -1.0f;
	avg_r = (a->r + b->r) / 2.0f;
	return (acosf(cos_angle) * avg_r);
}

/*
** Interference : melange de deux spheres.
** Mode rapide : multiplication des composantes.
** theta resultat = moyenne ponderee par les rayons
** phi resultat   = moyenne circulaire (gere le wrap-around 0/2PI)
** r resultat     = produit (table d'interference ternaire)
*/
t_s3_sphere	*s3_sphere_interf(t_s3_arena *arena, t_s3_sphere *a,
	t_s3_sphere *b)
{
	t_s3_sphere	*result;
	float		total_r;

	if (!arena || !a || !b)
		return (NULL);
	result = s3_sphere_new(arena, 0, 0, 0);
	if (!result)
		return (NULL);
	total_r = fabsf(a->r) + fabsf(b->r);
	if (total_r > 0.0001f)
		result->theta = (a->theta * fabsf(a->r)
				+ b->theta * fabsf(b->r)) / total_r;
	else
		result->theta = (a->theta + b->theta) / 2.0f;
	result->phi = atan2f(
			sinf(a->phi) + sinf(b->phi),
			cosf(a->phi) + cosf(b->phi));
	if (result->phi < 0.0f)
		result->phi += S3_TAU;
	result->r = a->r * b->r;
	return (result);
}

void	s3_sphere_rotate(t_s3_sphere *s, float d_theta, float d_phi)
{
	if (!s)
		return ;
	s->theta += d_theta;
	/* Clamp theta entre 0 et PI (pole nord → pole sud) */
	if (s->theta < 0.0f)
		s->theta = 0.0f;
	if (s->theta > S3_PI)
		s->theta = S3_PI;
	/* Phi est circulaire : wrap autour de [0, 2*PI] */
	s->phi += d_phi;
	while (s->phi < 0.0f)
		s->phi += S3_TAU;
	while (s->phi >= S3_TAU)
		s->phi -= S3_TAU;
}
