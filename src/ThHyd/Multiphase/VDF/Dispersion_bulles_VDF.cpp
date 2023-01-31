/****************************************************************************
* Copyright (c) 2023, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <Dispersion_bulles_VDF.h>

Implemente_instanciable(Dispersion_bulles_VDF, "Dispersion_bulles_Face_VDF", Source_Dispersion_bulles_base);

Sortie& Dispersion_bulles_VDF::printOn(Sortie& os) const { return os; }

Entree& Dispersion_bulles_VDF::readOn(Entree& is)
{
  Source_Dispersion_bulles_base::readOn(is);
  // TODO : FIXME : pour coco
//  if sub_type(Op_Diff_Turbulent_PolyMAC_P0_Face, equation().operateur(0).l_op_base()) is_turb = 1;
  return is;
}

void Dispersion_bulles_VDF::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  throw;
//  const Champ_Face_VDF& ch = ref_cast(Champ_Face_VDF, equation().inconnue().valeur());
//  const Domaine_VDF& domaine = ref_cast(Domaine_VDF, equation().domaine_dis().valeur());
//  const IntTab& f_e = domaine.face_voisins(), &fcl = ch.fcl(), &e_f = domaine.elem_faces();
//  const DoubleVect& pe = equation().milieu().porosite_elem(), &pf = equation().milieu().porosite_face(), &ve = domaine.volumes(), &vf = domaine.volumes_entrelaces(), &fs = domaine.face_surfaces();
//  const DoubleTab& vf_dir = domaine.volumes_entrelaces_dir(), &xp = domaine.xp(), &xv = domaine.xv();
//  const DoubleTab& pvit = ch.passe(),
//                   &alpha = ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe(),
//                    &press = ref_cast(Pb_Multiphase, equation().probleme()).eq_qdm.pression().passe(),
//                     &temp  = ref_cast(Pb_Multiphase, equation().probleme()).eq_energie.inconnue().passe(),
//                      &rho   = equation().milieu().masse_volumique().passe(),
//                       &mu    = ref_cast(Fluide_base, equation().milieu()).viscosite_dynamique().passe();
//
//  const Milieu_composite& milc = ref_cast(Milieu_composite, equation().milieu());
//
//  DoubleTab const * d_bulles = (equation().probleme().has_champ("diametre_bulles")) ? &equation().probleme().get_champ("diametre_bulles").valeurs() : NULL ;
//  DoubleTab const * k_turb = (equation().probleme().has_champ("k")) ? &equation().probleme().get_champ("k").passe() : NULL ;
//
//  int N = pvit.line_size() , Np = press.line_size(), D = dimension, nf_tot = domaine.nb_faces_tot(), nf = domaine.nb_faces(), ne_tot = domaine.nb_elem_tot(),  cR = (rho.dimension_tot(0) == 1), cM = (mu.dimension_tot(0) == 1), Nk = (k_turb) ? (*k_turb).dimension(1) : 1;
//  DoubleTrav a_l(N), p_l(N), T_l(N), rho_l(N), mu_l(N), sigma_l(N,N), dv(N, N), nut_l(N), k_l(Nk), d_b_l(N), coeff(N, N, 2); //arguments pour coeff
//
//  // TODO : FIXME : pour coco
////  DoubleTrav nut(domaine.nb_elem_tot(), N); //viscosite turbulente
////  if (is_turb) ref_cast(Viscosite_turbulente_base, ref_cast(Op_Diff_Turbulent_PolyMAC_P0_Face, equation().operateur(0).l_op_base()).correlation().valeur()).eddy_viscosity(nut); //remplissage par la correlation
//
//  const Dispersion_bulles_base& correlation_db = ref_cast(Dispersion_bulles_base, correlation_.valeur());
//
//  /* calculaiton of the gradient of alpha at the face */
//  const Champ_P0_VDF& ch_a = ref_cast(Champ_P0_VDF, ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().valeur());
//  DoubleTrav grad_f_a(pvit);
//  ch_a.init_grad(0);
//  const IntTab& fg_d = ch_a.fgrad_d, &fg_e = ch_a.fgrad_e;  // Tables utilisees dans domaine_PolyMAC_P0::fgrad pour le calcul du gradient
//  const DoubleTab&  fg_w = ch_a.fgrad_w;
//  const Conds_lim& cls_a = ch_a.domaine_Cl_dis().les_conditions_limites();     // conditions aux limites du champ alpha
//  const IntTab&    fcl_a = ch_a.fcl();  // tableaux utilitaires sur les CLs : fcl(f, .) = (type de la CL, no de la CL, indice dans la CL)
//
//  for (int n = 0; n < N; n++)
//    for (int f = 0; f < nf_tot; f++)
//      {
//        grad_f_a(f, n) = 0;
//        for (int j = fg_d(f); j < fg_d(f+1) ; j++)
//          {
//            int e = fg_e(j);
//            int f_bord;
//            if ( (f_bord = e-ne_tot) < 0) //contribution d'un element
//              grad_f_a(f, n) += fg_w(j) * alpha(e, n);
//            else if (fcl_a(f_bord, 0) == 1 || fcl_a(f_bord, 0) == 2) //Echange_impose_base
//              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Echange_impose_base, cls_a[fcl_a(f_bord, 1)].valeur()).T_ext(fcl_a(f_bord, 2), n) : 0;
//            else if (fcl_a(f_bord, 0) == 4) //Neumann non homogene
//              grad_f_a(f, n) += fg_w(j) ? fg_w(j) * ref_cast(Neumann_paroi      , cls_a[fcl_a(f_bord, 1)].valeur()).flux_impose(fcl_a(f_bord, 2), n) : 0;
//            else if (fcl_a(f_bord, 0) == 6) // Dirichlet
//              grad_f_a(f, n) += fg_w(j) * ref_cast(Dirichlet, cls_a[fcl_a(f_bord, 1)].valeur()).val_imp(fcl_a(f_bord, 2), n);
//          }
//      }
//
//  /* faces */
//  for (int f = 0; f < nf; f++)
//    if (fcl(f, 0) < 2)
//      {
//        a_l = 0 ;
//        p_l = 0 ;
//        T_l = 0 ;
//        rho_l = 0;
//        mu_l = 0 ;
//        nut_l = 0 ;
//        k_l = 0 ;
//        d_b_l=0;
//        sigma_l=0;
//        dv = 0 ;
//        int e;
//        for (int c = 0; c < 2 && (e = f_e(f, c)) >= 0; c++)
//          {
//            for (int n = 0; n < N; n++)
//              {
//                a_l(n)   += vf_dir(f, c)/vf(f) * alpha(e, n);
//                p_l(n)   += vf_dir(f, c)/vf(f) * press(e, n * (Np > 1));
//                T_l(n)   += vf_dir(f, c)/vf(f) * temp(e, n);
//                rho_l(n) += vf_dir(f, c)/vf(f) * rho(!cR * e, n);
//                mu_l(n)  += vf_dir(f, c)/vf(f) * mu(!cM * e, n);
//                nut_l(n) += is_turb    ? vf_dir(f, c)/vf(f) * nut(e,n) : 0;
//                d_b_l(n) += (d_bulles) ? vf_dir(f, c)/vf(f) * (*d_bulles)(e,n) : 0;
//                for (int k = 0; k < N; k++)
//                  if (milc.has_interface(n,k))
//                    {
//                      Interface_base& sat = milc.get_interface(n, k);
//                      sigma_l(n,k) += vf_dir(f, c)/vf(f) * sat.sigma(temp(e,n),press(e,n * (Np > 1)));
//                    }
//                for (int k = 0; k < N; k++)
//                  dv(k, n) += vf_dir(f, c)/vf(f) * ch.v_norm(pvit, pvit, e, f, k, n, nullptr, nullptr);
//              }
//            for (int n = 0; n <Nk; n++) k_l(n)   += (k_turb)   ? vf_dir(f, c)/vf(f) * (*k_turb)(e,0) : 0;
//          }
//
//        correlation_db.coefficient(a_l, p_l, T_l, rho_l, mu_l, sigma_l, nut_l, k_l, d_b_l, dv, coeff); // correlation identifies the liquid phase
//
//        for (int k = 0; k < N; k++)
//          for (int l = 0; l < N; l++)
//            if (k != l)
//              {
//                double fac = beta_*pf(f) * vf(f);
//                secmem(f, k) += fac * ( - coeff(k, l, 0) * grad_f_a(f, k) + coeff(l, k, 0) * grad_f_a(f, l));
//              }
//      }
}
