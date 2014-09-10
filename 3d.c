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

// 3D.H : 3D graphics and vectors functions

/* === WARNING ==

	This is a work-in-progress, meaning that most of this code is unstable
	and it's subject to future changes.  Also, most of it is very hackish,
	not properly cleaned up nor tested.

   === WARNING == */

#include <stdlib.h>
#include <math.h>
#include "line.h"
#include "3d.h"

int *tcos, *tsin;

void create_lookup_tables() {
	double M_PI = 8.0 * atan(1.0);

	int c;
	double a;

	tcos = newa(int, 256);
	tsin = newa(int, 256);

	for (c = 0; c < 256; c++) {
		a = M_PI * c / 128;
		tcos[c] = (int)(cos(a) * 64);
		tsin[c] = (int)(sin(a) * 64);
	}
}

void destroy_lookup_tables() {
	free(tcos);
	free(tsin);
}

mesh_t* mesh_new(int pcount, int tcount) {
	mesh_t* r = new(mesh_t);
	r->pcount = pcount;
	r->tcount = tcount;
	r->points = newa(vector_t, pcount);
	r->triangles = newa(triangle_t, tcount);
	return r;
}

void mesh_delete(mesh_t* mesh) {
	free(mesh->points);
	free(mesh->triangles);
	free(mesh);
}

void vector_cross_product (vector_t* v1, vector_t* v2, vector_t* r) {
	int x, y, z;
	x = mulfx(v1->y, v2->z) - mulfx(v1->z, v2->y);
	y = mulfx(v1->z, v2->x) - mulfx(v1->x, v2->z);
	z = mulfx(v1->x, v2->y) - mulfx(v1->y, v2->x);
	r->x = x; r->y = y; r->z = z;
}

int vector_cross_product_z (vector_t* v1, vector_t* v2) {
	return mulfx(v1->x, v2->y) - mulfx(v1->y, v2->x);
}

int vector_dot_product (vector_t* v1, vector_t* v2) {
	return mulfx(v1->x, v2->x) + mulfx(v1->y, v2->y) + mulfx(v1->z, v2->z);
}

int vector_length_squared (vector_t *v) {
	return sqrfx(v->x) + sqrfx(v->y) + sqrfx(v->z);
}

int vector_length(vector_t *v) {
	return sqrtfx(vector_length_squared(v));
}

void vector_normalize(vector_t *p, vector_t *r) {
	int d = vector_length(p);
	if (d != 0) {
		r->x = divfx(p->x, d);
		r->y = divfx(p->y, d);
		r->z = divfx(p->z, d);
	}
}

void vector_scalar(vector_t *v, int s, vector_t* r) {
	r->x = mulfx(v->x, s);
	r->y = mulfx(v->y, s);
	r->z = mulfx(v->z, s);
}

void vector_add(vector_t *v1, vector_t *v2, vector_t *r) {
	r->x = v1->x + v2->x;
	r->y = v1->y + v2->y;
	r->z = v1->z + v2->z;
}

void vector_subtract (vector_t *v1, vector_t *v2, vector_t *r) {
	r->x = v1->x - v2->x;
	r->y = v1->y - v2->y;
	r->z = v1->z - v2->z;
}

int vector_distance (vector_t *v1, vector_t *v2) {
	vector_t r;
	vector_subtract(v1, v2, &r);
	return vector_length(&r);
}

void vector_rotate(vector_t* p, u_char angle_x, u_char angle_y, u_char angle_z, vector_t* r) {
	int x, y, z;
	int ca, sa;

	// z
	ca = tcos[angle_z]; sa = tsin[angle_z];
	x = p->x; y = p->y; z = p->z;
	r->x = (mulfx(ca, x) - mulfx(sa, y));
	r->y = (mulfx(sa, x) + mulfx(ca, y));
	r->z = z;

	// y
	ca = tcos[angle_y]; sa = tsin[angle_y];
	x = r->x; y = r->y; z = r->z;
	r->x = (mulfx(ca, x) - mulfx(sa, z));
	r->z = (mulfx(sa, x) + mulfx(ca, z));

	// x
	ca = tcos[angle_x]; sa = tsin[angle_x];
	x = r->x; y = r->y; z = r->z;
	r->y = (mulfx(ca, y) - mulfx(sa, z));
	r->z = (mulfx(sa, y) + mulfx(ca, z));
}

void object_apply_transformations(object_t* obj, vector_t* pbuffer, int x, int y) {
	mesh_t* mesh = obj->mesh;
	vector_t* m = mesh->points;
	int c = mesh->pcount;
	while (c--) {
		vector_rotate(m, obj->rot_x, obj->rot_y, obj->rot_z, pbuffer);
		pbuffer->z += obj->trans_z;
		pbuffer->z /= 128;
		pbuffer->x = ((pbuffer->x + obj->trans_x) / pbuffer->z) + x;
		pbuffer->y = ((pbuffer->y + obj->trans_y) / pbuffer->z) + y;
		++pbuffer;
		++m;
	}
}

void object_render_wireframe(surface_t* s, object_t* obj, vector_t* pbuffer) {
	int c, diff;
	vector_t *i, *j, *k, q1, q2;
	mesh_t* mesh = obj->mesh;
	vector_t* m = mesh->points;
	triangle_t* t = mesh->triangles;

	object_apply_transformations(obj, pbuffer, 128, 96); // FIXME: must use surface center

	m = mesh->points;
	diff = (char*)pbuffer - (char*)m;
	for (c = 0; c < mesh->tcount; c++) {
		i = (vector_t*)((char*)t->vertexes[0] + diff);
		j = (vector_t*)((char*)t->vertexes[1] + diff);
		k = (vector_t*)((char*)t->vertexes[2] + diff);

		vector_subtract(j, i, &q1);
		vector_subtract(k, i, &q2);
		if (vector_cross_product_z(&q1, &q2) > 0) {
			surface_line(s, i->x, i->y, j->x, j->y);
			surface_line(s, j->x, j->y, k->x, k->y);
			surface_line(s, k->x, k->y, i->x, i->y);
		}
		t++;
	}	
}

void object_render_flatshading(surface_t* s, object_t* obj, vector_t* pbuffer, int* low, int* high, vector_t* light) {
	int c, diff;
	int l, top, bottom;
	vector_t *i, *j, *k, q1, q2, N;
	mesh_t* mesh = obj->mesh;
	vector_t* m = mesh->points;
	triangle_t* t = mesh->triangles;
	int li;

	object_apply_transformations(obj, pbuffer, 128, 96); // FIXME: must use surface center

	m = mesh->points;
	diff = (char*)pbuffer - (char*)m;
	for (c = 0; c < mesh->tcount; c++) {
		i = (vector_t*)((char*)t->vertexes[0] + diff);
		j = (vector_t*)((char*)t->vertexes[1] + diff);
		k = (vector_t*)((char*)t->vertexes[2] + diff);

		vector_subtract(j, i, &q1);
		vector_subtract(k, i, &q2);
		vector_cross_product(&q1, &q2, &N);

		if (N.z > 0) {
			vector_normalize(&N, &N);
			li = vector_dot_product(&N, light) * 5;
			li = f2i(li);
			li = (li < 0) ? 0 : ((li > 4) ? 4 : li);

			// FIXME: can optimize memset's by covering only "top" to "bottom" lines
			// in this case we should require clean buffers to start with.
			// and we should clean them after rendering.
			memset(low, MODE2_HEIGHT << 1, 64);	// FIXME: must use surface height
			memset(high, MODE2_HEIGHT << 1, 0);	// FIXME: must use surface height

			// calculate polygon
			calculate_side(i->x, i->y, j->x, j->y, low, high);
			calculate_side(j->x, j->y, k->x, k->y, low, high);
			calculate_side(k->x, k->y, i->x, i->y, low, high);

			top = (i->y < j->y) ? ((i->y < k->y) ? i->y : k->y) : ((j->y < k->y) ? j->y : k->y);
			bottom = (i->y > j->y) ? ((i->y > k->y) ? i->y : k->y) : ((j->y > k->y) ? j->y : k->y);

			for (l = top; l <= bottom; l++) {
				surface_hline(s, low[l], l, high[l], DITHER(li, l));
			}
		}
		t++;
	}	
}
