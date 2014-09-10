/*=========================================================================

GFX - a small graphics library 

Copyright (C) 2004  Rafael de Oliveira Jannone

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Contact the author:
	by e-mail : rafael AT jannone DOT org
	homepage  : http://jannone.org/gfxlib
	ICQ UIN   : 10115284

See the License at http://www.gnu.org/copyleft/lesser.txt

=========================================================================*/

/*! \file 3d.h
    \brief vector math and 3d graphics
*/
// 3D.H : 3D graphics and vectors functions (header)

/* === WARNING ==

	This is a work-in-progress, meaning that most of this code is unstable
	and it's subject to future changes.  Also, most of it is very hackish,
	not properly cleaned up nor tested.

   === WARNING == */

// note 1: this code uses fixed-point arithmetic
// note 2: every angle is one byte, to be mapped on the lookup tables
// note 3: you must create the look up tables before doing anything

#ifndef V3D_H
#define V3D_H

#include "gfx.h"

/// lookup cosine table
extern int *tcos;

/// lookup sine table
extern int *tsin;

/// represents a vector in 3 dimensions
typedef struct {
	int x;	///< value X
	int y;	///< value Y
	int z;	///< value Z
} vector_t;

/// a triangle made of 3 vertexes
typedef struct {
	vector_t* vertexes[3];	///< 3 vertexes
} triangle_t;

/// mesh: a set of points (vectors) refered by a set of triangles
typedef struct {
	int pcount;	///< number of points
	int tcount;	///< number of triangles

	vector_t* points;	///< points
	triangle_t* triangles;	///< triangles
} mesh_t;

/// an object is a solid in a scene, with translation, rotation and the solid's mesh
typedef struct {
	mesh_t* mesh;	///< mesh
	u_char rot_x;	///< angle of rotation X-axis
	u_char rot_y;	///< angle of rotation Y-axis
	u_char rot_z;	///< angle of rotation Z-axis
	int trans_x;	///< translation on X-axis
	int trans_y;	///< translation on Y-axis
	int trans_z;	///< translation on Z-axis
} object_t;

/// create the lookup tables for cos and sin. you MUST call this before operating on vectors.
void create_lookup_tables();

/// deallocate the lookup tables
void destroy_lookup_tables();

/// create a new mesh with \a pcount points and \a tcount triangles
mesh_t* mesh_new(int pcount, int tcount);

/// deallocate mesh
void mesh_delete(mesh_t* mesh);

/// cross product of \a v1 by \a v2, result into \a r
void vector_cross_product (vector_t* v1, vector_t* v2, vector_t* r);

/// cross product of the z axis of \a v1 by \a v2
int vector_cross_product_z (vector_t* v1, vector_t* v2);

/// dot product of \a v1 by \a v2
int vector_dot_product (vector_t* v1, vector_t* v2);

/// squared length of vector \a v
int vector_length_squared (vector_t *v);

/// pure length of vector \a v (it involves sqrt calculus, so it is expensive)
int vector_length(vector_t *v);

/// normalize vector \a p, result in \a r
void vector_normalize(vector_t *p, vector_t *r);

/// scale vector \a v by \a s, result in \a r
void vector_scalar(vector_t *v, int s, vector_t* r);

/// add vector \a v1 with \a v2, result in \a r
void vector_add(vector_t *v1, vector_t *v2, vector_t *r);

/// subtract vector \a v1 by \a v2, result in \a r
void vector_subtract (vector_t *v1, vector_t *v2, vector_t *r);

/// distance between vectors \a v1 and \a v2
int vector_distance (vector_t *v1, vector_t *v2);

/// rotate vector \a p by the given angles, result in \a r
void vector_rotate(vector_t* p, u_char angle_x, u_char angle_y, u_char angle_z, vector_t* r);

/// apply perspective transformations on object \a obj, centering the points around \a x and \a y
void object_apply_transformations(object_t* obj, vector_t* pbuffer, int x, int y);

/// render object \a obj with wireframes
void object_render_wireframe(surface_t* s, object_t* obj, vector_t* pbuffer);

/// render object \a obj with flat-shading, requires a normalized source of \a light
void object_render_flatshading(surface_t* s, object_t* obj, vector_t* pbuffer, int* low, int* high, vector_t* light);

#endif
