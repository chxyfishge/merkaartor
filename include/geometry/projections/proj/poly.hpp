#ifndef _PROJECTIONS_POLY_HPP
#define _PROJECTIONS_POLY_HPP

// Generic Geometry Library - projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright Barend Gehrels (1995-2009), Geodan Holding B.V. Amsterdam, the Netherlands.
// Copyright Bruno Lalande (2008-2009)
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Geometry Library by Barend Gehrels (Geodan, Amsterdam)

// Original copyright notice:

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <geometry/projections/impl/base_static.hpp>
#include <geometry/projections/impl/base_dynamic.hpp>
#include <geometry/projections/impl/projects.hpp>
#include <geometry/projections/impl/factory_entry.hpp>
#include <geometry/projections/impl/pj_msfn.hpp>
#include <geometry/projections/impl/pj_mlfn.hpp>

namespace projection
{
	#ifndef DOXYGEN_NO_IMPL
	namespace impl
	{
		namespace poly
		{
			static const double TOL = 1e-10;
			static const double CONV = 1e-10;
			static const int N_ITER = 10;
			static const int I_ITER = 20;
			static const double ITOL = 1.e-12;

			struct par_poly
			{
				double ml0;
				double en[EN_SIZE];
			};

			// template class, using CRTP to implement forward/inverse
			template <typename LL, typename XY, typename PAR>
			struct base_poly_ellipsoid : public base_t_fi<base_poly_ellipsoid<LL, XY, PAR>, LL, XY, PAR>
			{

				typedef typename base_t_fi<base_poly_ellipsoid<LL, XY, PAR>, LL, XY, PAR>::LL_T LL_T;
				typedef typename base_t_fi<base_poly_ellipsoid<LL, XY, PAR>, LL, XY, PAR>::XY_T XY_T;

				par_poly m_proj_parm;

				inline base_poly_ellipsoid(const PAR& par)
					: base_t_fi<base_poly_ellipsoid<LL, XY, PAR>, LL, XY, PAR>(*this, par) {}

				inline void fwd(LL_T& lp_lon, LL_T& lp_lat, XY_T& xy_x, XY_T& xy_y) const
				{
					double  ms, sp, cp;

					if (fabs(lp_lat) <= TOL) { xy_x = lp_lon; xy_y = -this->m_proj_parm.ml0; }
					else {
						sp = sin(lp_lat);
						ms = fabs(cp = cos(lp_lat)) > TOL ? pj_msfn(sp, cp, this->m_par.es) / sp : 0.;
						xy_x = ms * sin(lp_lon *= sp);
						xy_y = (pj_mlfn(lp_lat, sp, cp, this->m_proj_parm.en) - this->m_proj_parm.ml0) + ms * (1. - cos(lp_lon));
					}
				}

				inline void inv(XY_T& xy_x, XY_T& xy_y, LL_T& lp_lon, LL_T& lp_lat) const
				{
					xy_y += this->m_proj_parm.ml0;
					if (fabs(xy_y) <= TOL) { lp_lon = xy_x; lp_lat = 0.; }
					else {
						double r, c, sp, cp, s2ph, ml, mlb, mlp, dPhi;
						int i;

						r = xy_y * xy_y + xy_x * xy_x;
						for (lp_lat = xy_y, i = I_ITER; i ; --i) {
							sp = sin(lp_lat);
							s2ph = sp * ( cp = cos(lp_lat));
							if (fabs(cp) < ITOL)
								throw proj_exception();;
							c = sp * (mlp = sqrt(1. - this->m_par.es * sp * sp)) / cp;
							ml = pj_mlfn(lp_lat, sp, cp, this->m_proj_parm.en);
							mlb = ml * ml + r;
							mlp = this->m_par.one_es / (mlp * mlp * mlp);
							lp_lat += ( dPhi =
								( ml + ml + c * mlb - 2. * xy_y * (c * ml + 1.) ) / (
								this->m_par.es * s2ph * (mlb - 2. * xy_y * ml) / c +
								2.* (xy_y - ml) * (c * mlp - 1. / s2ph) - mlp - mlp ));
							if (fabs(dPhi) <= ITOL)
								break;
						}
						if (!i)
							throw proj_exception();;
						c = sin(lp_lat);
						lp_lon = asin(xy_x * tan(lp_lat) * sqrt(1. - this->m_par.es * c * c)) / sin(lp_lat);
					}
				}
			};

			// template class, using CRTP to implement forward/inverse
			template <typename LL, typename XY, typename PAR>
			struct base_poly_spheroid : public base_t_fi<base_poly_spheroid<LL, XY, PAR>, LL, XY, PAR>
			{

				typedef typename base_t_fi<base_poly_spheroid<LL, XY, PAR>, LL, XY, PAR>::LL_T LL_T;
				typedef typename base_t_fi<base_poly_spheroid<LL, XY, PAR>, LL, XY, PAR>::XY_T XY_T;

				par_poly m_proj_parm;

				inline base_poly_spheroid(const PAR& par)
					: base_t_fi<base_poly_spheroid<LL, XY, PAR>, LL, XY, PAR>(*this, par) {}

				inline void fwd(LL_T& lp_lon, LL_T& lp_lat, XY_T& xy_x, XY_T& xy_y) const
				{
					double  cot, E;

					if (fabs(lp_lat) <= TOL) { xy_x = lp_lon; xy_y = this->m_proj_parm.ml0; }
					else {
						cot = 1. / tan(lp_lat);
						xy_x = sin(E = lp_lon * sin(lp_lat)) * cot;
						xy_y = lp_lat - this->m_par.phi0 + cot * (1. - cos(E));
					}
				}

				inline void inv(XY_T& xy_x, XY_T& xy_y, LL_T& lp_lon, LL_T& lp_lat) const
				{
					double B, dphi, tp;
					int i;

					if (fabs(xy_y = this->m_par.phi0 + xy_y) <= TOL) { lp_lon = xy_x; lp_lat = 0.; }
					else {
						lp_lat = xy_y;
						B = xy_x * xy_x + xy_y * xy_y;
						i = N_ITER;
						do {
							tp = tan(lp_lat);
							lp_lat -= (dphi = (xy_y * (lp_lat * tp + 1.) - lp_lat -
								.5 * ( lp_lat * lp_lat + B) * tp) /
								((lp_lat - xy_y) / tp - 1.));
						} while (fabs(dphi) > CONV && --i);
						if (! i) throw proj_exception();;
						lp_lon = asin(xy_x * tan(lp_lat)) / sin(lp_lat);
					}
				}
			};

			// Polyconic (American)
			template <typename PAR>
			void setup_poly(PAR& par, par_poly& proj_parm)
			{
				if (par.es) {
					pj_enfn(par.es, proj_parm.en);
					proj_parm.ml0 = pj_mlfn(par.phi0, sin(par.phi0), cos(par.phi0), proj_parm.en);
				// par.inv = e_inverse;
				// par.fwd = e_forward;
				} else {
					proj_parm.ml0 = -par.phi0;
				// par.inv = s_inverse;
				// par.fwd = s_forward;
				}
			}

		} // namespace poly
	} //namespaces impl
	#endif // doxygen

	/*!
		\brief Polyconic (American) projection
		\ingroup projections
		\tparam LL latlong point type
		\tparam XY xy point type
		\tparam PAR parameter type
		\par Projection characteristics
		 - Conic
		 - Spheroid
		 - Ellipsoid
		\par Example
		\image html ex_poly.gif
	*/
	template <typename LL, typename XY, typename PAR = parameters>
	struct poly_ellipsoid : public impl::poly::base_poly_ellipsoid<LL, XY, PAR>
	{
		inline poly_ellipsoid(const PAR& par) : impl::poly::base_poly_ellipsoid<LL, XY, PAR>(par)
		{
			impl::poly::setup_poly(this->m_par, this->m_proj_parm);
		}
	};

	/*!
		\brief Polyconic (American) projection
		\ingroup projections
		\tparam LL latlong point type
		\tparam XY xy point type
		\tparam PAR parameter type
		\par Projection characteristics
		 - Conic
		 - Spheroid
		 - Ellipsoid
		\par Example
		\image html ex_poly.gif
	*/
	template <typename LL, typename XY, typename PAR = parameters>
	struct poly_spheroid : public impl::poly::base_poly_spheroid<LL, XY, PAR>
	{
		inline poly_spheroid(const PAR& par) : impl::poly::base_poly_spheroid<LL, XY, PAR>(par)
		{
			impl::poly::setup_poly(this->m_par, this->m_proj_parm);
		}
	};

	#ifndef DOXYGEN_NO_IMPL
	namespace impl
	{

		// Factory entry(s)
		template <typename LL, typename XY, typename PAR>
		class poly_entry : public impl::factory_entry<LL, XY, PAR>
		{
			public :
				virtual projection<LL, XY>* create_new(const PAR& par) const
				{
					if (par.es)
						return new base_v_fi<poly_ellipsoid<LL, XY, PAR>, LL, XY, PAR>(par);
					else
						return new base_v_fi<poly_spheroid<LL, XY, PAR>, LL, XY, PAR>(par);
				}
		};

		template <typename LL, typename XY, typename PAR>
		inline void poly_init(impl::base_factory<LL, XY, PAR>& factory)
		{
			factory.add_to_factory("poly", new poly_entry<LL, XY, PAR>);
		}

	} // namespace impl
	#endif // doxygen

}

#endif // _PROJECTIONS_POLY_HPP
