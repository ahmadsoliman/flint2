/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2011 Fredrik Johansson

******************************************************************************/

#include "flint.h"
#include "nmod_poly.h"
#include "nmod_poly_mat.h"

long
nmod_poly_mat_find_pivot_any(const nmod_poly_mat_t mat,
                                    long start_row, long end_row, long c)
{
    long r;

    for (r = start_row; r < end_row; r++)
    {
        if (!nmod_poly_is_zero(nmod_poly_mat_entry(mat, r, c)))
            return r;
    }

    return -1;
}
