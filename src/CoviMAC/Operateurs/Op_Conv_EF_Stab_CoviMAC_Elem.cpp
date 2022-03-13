/****************************************************************************
* Copyright (c) 2022, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Op_Conv_EF_Stab_CoviMAC_Elem.cpp
// Directory:   $TRUST_ROOT/src/CoviMAC/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_Stab_CoviMAC_Elem.h>
#include <Probleme_base.h>
#include <Convection_Diffusion_std.h>
#include <Milieu_base.h>
#include <Schema_Temps_base.h>
#include <Zone_CoviMAC.h>
#include <Zone_Cl_CoviMAC.h>
#include <TRUSTLists.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Array_tools.h>
#include <Matrix_tools.h>
#include <Champ_P0_CoviMAC.h>
#include <Champ_Face_CoviMAC.h>
#include <Masse_CoviMAC_Elem.h>
#include <Pb_Multiphase.h>
#include <Discretisation_base.h>

#include <Param.h>
#include <cmath>
#include <cfloat>
#include <vector>

Implemente_instanciable( Op_Conv_EF_Stab_CoviMAC_Elem, "Op_Conv_EF_Stab_CoviMAC_Elem_CoviMAC", Op_Conv_CoviMAC_base ) ;
Implemente_instanciable_sans_constructeur(Op_Conv_Amont_CoviMAC_Elem, "Op_Conv_Amont_CoviMAC_Elem_CoviMAC", Op_Conv_EF_Stab_CoviMAC_Elem);
Implemente_instanciable_sans_constructeur(Op_Conv_Centre_CoviMAC_Elem, "Op_Conv_Centre_CoviMAC_Elem_CoviMAC", Op_Conv_EF_Stab_CoviMAC_Elem);

Op_Conv_Amont_CoviMAC_Elem::Op_Conv_Amont_CoviMAC_Elem() { alpha = 1.0; }
Op_Conv_Centre_CoviMAC_Elem::Op_Conv_Centre_CoviMAC_Elem() { alpha = 0.0; }

// XD Op_Conv_EF_Stab_CoviMAC_Elem interprete Op_Conv_EF_Stab_CoviMAC_Elem 1 Class Op_Conv_EF_Stab_CoviMAC_Elem
Sortie& Op_Conv_EF_Stab_CoviMAC_Elem::printOn(Sortie& os) const { return Op_Conv_CoviMAC_base::printOn(os); }
Sortie& Op_Conv_Amont_CoviMAC_Elem::printOn(Sortie& os) const { return Op_Conv_CoviMAC_base::printOn(os); }
Sortie& Op_Conv_Centre_CoviMAC_Elem::printOn(Sortie& os) const { return Op_Conv_CoviMAC_base::printOn(os); }
Entree& Op_Conv_Amont_CoviMAC_Elem::readOn(Entree& is) { return Op_Conv_EF_Stab_CoviMAC_Elem::readOn(is); }
Entree& Op_Conv_Centre_CoviMAC_Elem::readOn(Entree& is) { return Op_Conv_EF_Stab_CoviMAC_Elem::readOn(is); }

Entree& Op_Conv_EF_Stab_CoviMAC_Elem::readOn( Entree& is )
{
  Op_Conv_CoviMAC_base::readOn( is );
  if (que_suis_je() == "Op_Conv_EF_Stab_CoviMAC_Elem_CoviMAC") //on n'est pas dans Op_Conv_Amont/Centre
    {
      Param param(que_suis_je());
      param.ajouter("alpha", &alpha);            // XD_ADD_P double parametre ajustant la stabilisation de 0 (schema centre) a 1 (schema amont)
      param.lire_avec_accolades_depuis(is);
    }

  if (sub_type(Masse_Multiphase, equation())) //convection dans Masse_Multiphase -> champs de debit / titre
    {
      const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, equation().probleme());
      noms_cc_phases_.dimensionner(pb.nb_phases()), cc_phases_.resize(pb.nb_phases());
      noms_vd_phases_.dimensionner(pb.nb_phases()), vd_phases_.resize(pb.nb_phases());
      noms_x_phases_.dimensionner(pb.nb_phases()),   x_phases_.resize(pb.nb_phases());
      for (int i = 0; i < pb.nb_phases(); i++)
        {
          champs_compris_.ajoute_nom_compris(noms_cc_phases_[i] = Nom("debit_") + pb.nom_phase(i));
          champs_compris_.ajoute_nom_compris(noms_vd_phases_[i] = Nom("vitesse_debitante_") + pb.nom_phase(i));
          champs_compris_.ajoute_nom_compris(noms_x_phases_[i] = Nom("titre_") + pb.nom_phase(i));
        }
    }

  return is;
}


void Op_Conv_EF_Stab_CoviMAC_Elem::preparer_calcul()
{
  Op_Conv_CoviMAC_base::preparer_calcul();

  /* au cas ou... */
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  zone.init_equiv(), equation().init_champ_convecte();
  flux_bords_.resize(zone.premiere_face_int(), (le_champ_inco.non_nul() ? le_champ_inco->valeurs() : equation().inconnue().valeurs()).line_size());

  if (zone.zone().nb_joints() && zone.zone().joint(0).epaisseur() < 2)
    Cerr << "Op_Conv_EF_Stab_CoviMAC_Elem : largeur de joint insuffisante (minimum 2)!" << finl, Process::exit();
}

double Op_Conv_EF_Stab_CoviMAC_Elem::calculer_dt_stab() const
{
  double dt = 1e10;
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face(), &ve = zone.volumes(), &pe = zone.porosite_elem();
  const DoubleTab& vit = vitesse_->valeurs(),
                   *alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).eq_masse.inconnue().passe() : NULL;
  const IntTab& e_f = zone.elem_faces(), &f_e = zone.face_voisins();
  int i, e, f, n, N = std::min(vit.line_size(), equation().inconnue().valeurs().line_size());
  DoubleTrav flux(N); //somme des flux pf * |f| * vf

  for (e = 0; e < zone.nb_elem(); e++)
    {
      for (flux = 0, i = 0; i < e_f.dimension(1) && (f = e_f(e, i)) >= 0; i++) for (n = 0; n < N; n++)
          flux(n) += pf(f) * fs(f) * std::max((e == f_e(f, 1) ? 1 : -1) * vit(f, n), 0.); //seul le flux entrant dans e compte

      for (n = 0; n < N; n++) if ((!alp || (*alp)(e, n) > 1e-3) && flux(n)) dt = std::min(dt, pe(e) * ve(e) / flux(n));
    }

  return Process::mp_min(dt);
}

void Op_Conv_EF_Stab_CoviMAC_Elem::dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  int i, j, e, eb, f, ne_tot = zone.nb_elem_tot(), nf_tot = zone.nb_faces_tot(), n, N = equation().inconnue().valeurs().line_size();
  const Champ_Inc_base& cc = equation().champ_convecte();

  for (auto &&i_m : mats) if (i_m.first == "vitesse" || (cc.derivees().count(i_m.first) && !semi_impl.count(cc.le_nom().getString())))
      {
        Matrice_Morse mat;
        IntTrav stencil(0, 2);
        stencil.set_smart_resize(1);
        int m, M = equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size();
        if (i_m.first == "vitesse") /* vitesse */
          {
            for (f = 0; f < zone.nb_faces_tot(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem())
                  for (n = 0; n < N; n++) stencil.append_line(N * e + n, M * f + n * (M > 1));
          }
        else for (f = 0; f < zone.nb_faces_tot(); f++) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) /* inconnues scalaires */
                for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++) for (n = 0, m = 0; n < N; n++, m += (M > 1)) stencil.append_line(N * e + n, M * eb + m);

        tableau_trier_retirer_doublons(stencil);
        Matrix_tools::allocate_morse_matrix(N * ne_tot, M * (i_m.first == "vitesse" ? nf_tot + dimension * ne_tot : ne_tot), stencil, mat);
        i_m.second->nb_colonnes() ? *i_m.second += mat : *i_m.second = mat;
      }
}

// ajoute la contribution de la convection au second membre resu
// renvoie resu
void Op_Conv_EF_Stab_CoviMAC_Elem::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins(), &fcl = ref_cast(Champ_P0_CoviMAC, equation().inconnue().valeur()).fcl();
  const DoubleVect& fs = zone.face_surfaces(), &pf = zone.porosite_face();
  const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco->valeur() : equation().champ_convecte();
  const std::string& nom_cc = cc.le_nom().getString();
  const DoubleTab& vit = vitesse_->valeurs(), &vcc = semi_impl.count(nom_cc) ? semi_impl.at(nom_cc) : cc.valeurs(), bcc = cc.valeur_aux_bords();
  int i, j, e, eb, f, n, m, N = vcc.line_size(), Mv = vit.line_size(), M;

  Matrice_Morse *m_vit = mats.count("vitesse") ? mats.at("vitesse") : NULL;
  std::vector<std::tuple<const DoubleTab *, Matrice_Morse *, int>> d_cc; //liste des derivees de cc a renseigner : couples (derivee de cc, matrice, nb de compos de la variable)
  if (!semi_impl.count(nom_cc)) for (auto &i_m : mats) if (cc.derivees().count(i_m.first))
        d_cc.push_back(std::make_tuple(&cc.derivees().at(i_m.first), i_m.second, equation().probleme().get_champ(i_m.first.c_str()).valeurs().line_size()));

  DoubleTrav dv_flux(N), dc_flux(2, N); //derivees du flux convectif a la face par rapport a la vitesse / au champ convecte amont / aval

  /* convection aux faces internes (fcl(f, 0) == 0), de Neumann_val_ext ou de Dirichlet */
  for (f = 0; f < zone.nb_faces(); f++) if (!fcl(f, 0) || (fcl(f, 0) > 4 && fcl(f, 0) < 7))
      {
        for (dv_flux = 0, dc_flux = 0, i = 0; i < 2; i++) for (e = f_e(f, i), n = 0, m = 0; n < N; n++, m += (Mv > 1))
            {
              double v = vit(f, m) ? vit(f, m) : DBL_MIN, fac = pf(f) * fs(f) * (1. + (v * (i ? -1 : 1) > 0 ? 1. : -1) * alpha) / 2;
              dv_flux(n) += fac * (e >= 0 ? vcc(e, n) : bcc(f, n)); //f est reelle -> indice trivial dans bcc
              dc_flux(i, n) = e >= 0 ? fac * vit(f, m) : 0;
            }

        //second membre
        for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (n = 0, m = 0; n < N; n++, m += (Mv > 1))
              secmem(e, n) -= (i ? -1 : 1) * dv_flux(n) * vit(f, m);
        //derivees : vitesse
        if (m_vit) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (n = 0, m = 0; n < N; n++, m += (Mv > 1))
                (*m_vit)(N * e + n, Mv * f + m) += (i ? -1 : 1) * dv_flux(n);
        //derivees : champ convecte
        for (auto &&d_m_i : d_cc) for (i = 0; i < 2 && (e = f_e(f, i)) >= 0; i++) if (e < zone.nb_elem()) for (j = 0; j < 2 && (eb = f_e(f, j)) >= 0; j++)
                for (n = 0, m = 0, M = std::get<2>(d_m_i); n < N; n++, m += (M > 1))
                  (*std::get<1>(d_m_i))(N * e + n, M * eb + m) += (i ? -1 : 1) * dc_flux(j, n) * (*std::get<0>(d_m_i))(eb, m);
      }
}

void Op_Conv_EF_Stab_CoviMAC_Elem::creer_champ(const Motcle& motlu)
{
  Op_Conv_CoviMAC_base::creer_champ(motlu);
  int i = noms_cc_phases_.rang(motlu), j = noms_vd_phases_.rang(motlu), k = noms_x_phases_.rang(motlu);
  if (i >= 0 && !cc_phases_[i].non_nul())
    {
      equation().discretisation().discretiser_champ("vitesse", equation().zone_dis(), noms_cc_phases_[i], "kg/m2/s",dimension, 1, 0, cc_phases_[i]);
      champs_compris_.ajoute_champ(cc_phases_[i]);
    }
  if (j >= 0 && !vd_phases_[j].non_nul())
    {
      equation().discretisation().discretiser_champ("vitesse", equation().zone_dis(), noms_vd_phases_[j], "m/s",dimension, 1, 0, vd_phases_[j]);
      champs_compris_.ajoute_champ(vd_phases_[j]);
    }
  if (k >= 0 && !x_phases_[k].non_nul())
    {
      equation().discretisation().discretiser_champ("temperature", equation().zone_dis(), noms_x_phases_[k], "m/s",1, 1, 0, x_phases_[k]);
      champs_compris_.ajoute_champ(x_phases_[k]);
    }
}

void Op_Conv_EF_Stab_CoviMAC_Elem::mettre_a_jour(double temps)
{
  Op_Conv_CoviMAC_base::mettre_a_jour(temps);
  const Zone_CoviMAC& zone = la_zone_poly_.valeur();
  const IntTab& f_e = zone.face_voisins();
  const Champ_Inc_base& cc = le_champ_inco.non_nul() ? le_champ_inco.valeur() : equation().champ_convecte();
  const DoubleVect& pf = zone.porosite_face(), &pe = zone.porosite_elem(), &fs = zone.face_surfaces();
  const DoubleTab& vit = vitesse_->valeurs(), &vcc = cc.valeurs(), bcc = cc.valeur_aux_bords();
  DoubleTab balp;
  if (vd_phases_.size()) balp = equation().inconnue().valeur().valeur_aux_bords();


  int i, e, f, d, D = dimension, n, m, N = vcc.line_size(), nf_tot = zone.nb_faces_tot(), M = vit.line_size();
  DoubleTrav cc_f(N); //valeur du champ convecte aux faces
  /* flux aux bords */
  for (f = 0; f < zone.premiere_face_int(); f++)
    {
      for (cc_f = 0, i = 0; i < 2; i++) for (e = f_e(f, i), n = 0, m = 0; n < N; n++, m += (M > 1))
          cc_f(n) +=  (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2 * (e >= 0 ? vcc(e, n) : bcc(f, n));

      for (n = 0, m = 0; n < N; n++, m += (M > 1)) flux_bords_(f, n) = pf(f) * fs(f) * vit(f, m) * cc_f(n);
    }

  if (cc_phases_.size()) for (n = 0, m = 0; n < N; n++, m += (M > 1)) if (cc_phases_[n].non_nul()) /* mise a jour des champs de debit */
        {
          Champ_Face_CoviMAC& c_ph = ref_cast(Champ_Face_CoviMAC, cc_phases_[n].valeur());
          DoubleTab& v_ph = c_ph.valeurs();
          /* on remplit la partie aux faces, puis on demande au champ d'interpoler aux elements */
          for (f = 0; f < zone.nb_faces(); f++)
            {
              for (v_ph(f) = 0, i = 0; i < 2; i++)
                v_ph(f) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2 * ((e = f_e(f, i)) >= 0 ? vcc(e, n) : bcc(f, n));
              v_ph(f) *= vit(f, m);
            }
          c_ph.update_ve(v_ph); //interpolation face -> element d'ordre 1 (sans matrices)
          for (f = 0; f < zone.nb_faces(); f++) v_ph(f) *= pf(f);
          for (e = 0; e < zone.nb_elem(); e++) for (d = 0; d < D; d++) v_ph(nf_tot + D * e + d) *= pe(e); //pour repasser en debitant
          c_ph.changer_temps(temps);
        }

  if (vd_phases_.size()) for (n = 0, m = 0; n < N; n++, m += (M > 1)) if (vd_phases_[n].non_nul()) /* mise a jour des champs de vitesse debitante */
        {
          const DoubleTab& alp = equation().inconnue().valeurs();
          Champ_Face_CoviMAC& c_ph = ref_cast(Champ_Face_CoviMAC, vd_phases_[n].valeur());
          DoubleTab& v_ph = c_ph.valeurs();
          /* on remplit la partie aux faces, puis on demande au champ d'interpoler aux elements */
          for (f = 0; f < zone.nb_faces(); f++)
            {
              for (v_ph(f) = 0, i = 0; i < 2; i++)
                v_ph(f) += (1. + (vit(f, m) * (i ? -1 : 1) >= 0 ? 1. : -1.) * alpha) / 2 * ((e = f_e(f, i)) >= 0 ? alp(e, n) : balp(f, n));
              v_ph(f) *= vit(f, m);
            }
          c_ph.update_ve(v_ph); //interpolation face -> element d'ordre 1 (sans matrices)
          for (f = 0; f < zone.nb_faces(); f++) v_ph(f) *= pf(f);
          for (e = 0; e < zone.nb_elem(); e++) for (d = 0; d < D; d++) v_ph(nf_tot + D * e + d) *= pe(e); //pour repasser en debitant
          c_ph.changer_temps(temps);
        }

  DoubleTrav G(N), v2(N);
  double Gt;
  if (x_phases_.size()) for (e = 0; e < zone.nb_elem(); e++) //titre : aux elements
      {
        for (v2 = 0, d = 0; d < D; d++) for (n = 0; n < N; n++) v2(n) += std::pow(vit(nf_tot + D * e + d, n), 2);
        for (Gt = 0, n = 0; n < N; Gt += G(n), n++) G(n) = vcc(e, n) * sqrt(v2(n));
        for (n = 0; n < N; n++) if (x_phases_[n].non_nul()) x_phases_[n]->valeurs()(e) = Gt ? G(n) / Gt : 0;
      }
  if (x_phases_.size()) for (n = 0; n < N; n++) if (x_phases_[n].non_nul()) x_phases_[n]->changer_temps(temps);
}

