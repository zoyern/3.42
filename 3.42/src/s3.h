/* *************************************************************** */
/*                                                                   */
/*                    :::::::: s3.h ::::::::::                       */
/*                                                                   */
/*    By: almounib <almounib@student.42.fr>                         */
/*    Projet: 3.42 — Include centralisé                             */
/*                                                                   */
/*    Un seul #include pour tout utiliser.                           */
/*    Les modules sont charges dans l'ordre des dependances :       */
/*    arena (0 dep) → trit (0 dep) → [futur: ast, type, ...]      */
/*                                                                   */
/* *************************************************************** */

#ifndef S3_H
# define S3_H

/* Couche 0 : memoire */
# include "arena/s3_arena.h"

/* Couche support : arithmetique ternaire */
# include "trit/s3_trit.h"

/*
** Modules futurs (a activer quand theorises et implementes) :
** # include "ast/s3_ast.h"       ← Couche 1 : PEG parser + AST type
** # include "type/s3_type.h"     ← Couche 2 : type system lineaire
** # include "ir/s3_ir.h"         ← Couche 3 : IR SSA ternaire
** # include "opt/s3_opt.h"       ← Couche 4 : e-graph optimization
** # include "emit/s3_emit.h"     ← Couche 5 : backend (Cranelift/natif)
*/

#endif
