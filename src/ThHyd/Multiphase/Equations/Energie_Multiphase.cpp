/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Energie_Multiphase.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Equations
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#include <Energie_Multiphase.h>
#include <Pb_Multiphase.h>
#include <Discret_Thyd.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Avanc.h>
#include <Debog.h>
#include <Frontiere_dis_base.h>
#include <EcritureLectureSpecial.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>
#include <Navier_Stokes_std.h>
#include <DoubleTrav.h>
#include <Neumann_sortie_libre.h>
#include <Op_Conv_negligeable.h>
#include <Param.h>
#include <Schema_Implicite_base.h>
#include <SETS.h>
#include <EChaine.h>

#define old_forme

Implemente_instanciable(Energie_Multiphase,"Energie_Multiphase",Convection_Diffusion_std);

// Description:
//    Simple appel a: Convection_Diffusion_std::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& is
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Energie_Multiphase::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

// Description:
//    Verifie si l'equation a une inconnue et un fluide associe
//    et appelle Convection_Diffusion_std::readOn(Entree&).
// Precondition: l'objet a une inconnue associee
// Precondition: l'objet a un fluide associe
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Energie_Multiphase::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul());
  assert(le_fluide.non_nul());
  evanescence.associer_eqn(*this);
  Convection_Diffusion_std::readOn(is);
  //Nom unite;
  //if (dimension+bidim_axi==2) unite="[W/m]";
  //else unite="[W]";
  terme_convectif.set_fichier("Convection_chaleur");
  terme_convectif.set_description((Nom)"Convective heat transfer rate=Integral(-h*u*ndS) [W] if SI units used");
  terme_diffusif.set_fichier("Diffusion_chaleur");
  //terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) "+unite);
  terme_diffusif.set_description((Nom)"Conduction heat transfer rate=Integral(lambda*grad(T)*ndS) [W] if SI units used");

  if (!evanescence.non_nul())
    {
      EChaine eva("{ homogene { alpha_res 1e-6 } }");
      eva >> evanescence;
    }

  return is;
}

void Energie_Multiphase::set_param(Param& param)
{
  Convection_Diffusion_std::set_param(param);
  param.ajouter_non_std("evanescence|vanishing",(this));
}

int Energie_Multiphase::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="evanescence") is >> evanescence;
  else return Convection_Diffusion_std::lire_motcle_non_standard(mot, is);
  return 1;
}

// Description:
//    Associe un milieu physique a l'equation,
//    le milieu est en fait caste en Fluide_Incompressible ou en Fluide_Ostwald.
// Precondition:
// Parametre: Milieu_base& un_milieu
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//                 doit pourvoir etre force au type "Fluide_Incompressible"
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception: les proprietes physiques du fluide ne sont pas toutes specifiees
// Effets de bord:
// Postcondition:
void Energie_Multiphase::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_Incompressible& un_fluide = ref_cast(Fluide_Incompressible,un_milieu);
  associer_fluide(un_fluide);
}

const Champ_Don& Energie_Multiphase::diffusivite_pour_transport()
{
  return milieu().conductivite();
}

const Champ_base& Energie_Multiphase::diffusivite_pour_pas_de_temps()
{
  return milieu().diffusivite();
}

// Description:
//    Discretise l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation est discretisee
void Energie_Multiphase::discretiser()
{
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  Cerr << "Energy equation discretization " << finl;
  const Pb_Multiphase& pb = ref_cast(Pb_Multiphase, probleme());
  dis.temperature(schema_temps(), zone_dis(), l_inco_ch, pb.nb_phases());
  l_inco_ch.valeur().fixer_nature_du_champ(pb.nb_phases() == 1 ? scalaire : pb.nb_phases() == dimension ? vectoriel : multi_scalaire); //pfft
  for (int i = 0; i < pb.nb_phases(); i++)
    l_inco_ch.valeur().fixer_nom_compo(i, Nom("temperature_") + pb.nom_phase(i));
  champs_compris_.ajoute_champ(l_inco_ch);
  Equation_base::discretiser();
  Cerr << "Energie_Multiphase::discretiser() ok" << finl;
}


// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Incompressible upcaste en Milieu_base)
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Incompressible upcaste en Milieu_base
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Milieu_base& Energie_Multiphase::milieu() const
{
  return fluide();
}


// Description:
//    Renvoie le milieu physique de l'equation.
//    (un Fluide_Incompressible upcaste en Milieu_base)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Milieu_base&
//    Signification: le Fluide_Incompressible upcaste en Milieu_base
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Milieu_base& Energie_Multiphase::milieu()
{
  return fluide();
}


// Description:
//    Renvoie le fluide incompressible associe a l'equation.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Fluide_Incompressible&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes: reference constante
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Fluide_Incompressible& Energie_Multiphase::fluide() const
{
  if (!le_fluide.non_nul())
    {
      Cerr << "You forgot to associate the fluid to the problem named " << probleme().le_nom() << finl;
      Process::exit();
    }
  return le_fluide.valeur();
}


// Description:
//    Renvoie le fluide incompressible associe a l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Fluide_Incompressible&
//    Signification: le fluide incompressible associe a l'equation
//    Contraintes:
// Exception: pas de fluide associe a l'eqaution
// Effets de bord:
// Postcondition:
Fluide_Incompressible& Energie_Multiphase::fluide()
{
  assert(le_fluide.non_nul());
  return le_fluide.valeur();
}


// Description:
//    Impression des flux sur les bords sur un flot de sortie.
//    Appelle Equation_base::impr(Sortie&)
// Precondition: Sortie&
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: code de retour propage
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Energie_Multiphase::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}


// Description:
//    Renvoie le nom du domaine d'application de l'equation.
//    Ici "Thermique".
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Motcle&
//    Signification: le nom du domaine d'application de l'equation
//    Contraintes: toujours egal a "Thermique"
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Motcle& Energie_Multiphase::domaine_application() const
{
  static Motcle mot("Thermique");
  return mot;
}

// Description:
//    Associe un fluide incompressible a l'equation.
// Precondition:
// Parametre: Fluide_Incompressible& un_fluide
//    Signification: le milieu fluide incompressible a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un milieu associe
void Energie_Multiphase::associer_fluide(const Fluide_Incompressible& un_fluide)
{
  le_fluide = un_fluide;
}

/* l'evanescence passe en dernier */
void Energie_Multiphase::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  Convection_Diffusion_std::dimensionner_blocs(matrices, semi_impl);
  evanescence.valeur().dimensionner_blocs(matrices, semi_impl);
}

void Energie_Multiphase::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  Convection_Diffusion_std::assembler_blocs_avec_inertie(matrices, secmem, semi_impl);
  evanescence.valeur().ajouter_blocs(matrices, secmem, semi_impl);
}


void Energie_Multiphase::calculer_champ_conserve(const Champ_Inc_base& ch, double t, DoubleTab& val, DoubleTab& bval, tabs_t& deriv, int val_only)
{
  const Champ_base& ch_rho = ch.equation().milieu().masse_volumique();
  const Champ_Inc_base& ch_alpha = ref_cast(Pb_Multiphase, ch.equation().probleme()).eq_masse.inconnue(),
                        &ch_en = ref_cast(Champ_Inc_base, ch.equation().milieu().energie_interne()), //toujours un Champ_Inc
                         *pch_rho = sub_type(Champ_Inc_base, ch_rho) ? &ref_cast(Champ_Inc_base, ch_rho) : NULL; //pas toujours un Champ_Inc
  const DoubleTab& alpha = ch_alpha.valeurs(t), &rho = ch_rho.valeurs(t), &en = ch_en.valeurs(t);

  /* valeurs du champ */
  int i, j, n, N = val.line_size(), Nl = val.dimension_tot(0), cR = sub_type(Champ_Uniforme, ch_rho);
  for (i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++) val.addr()[j] = alpha.addr()[j] * rho.addr()[cR ? n : j] * en.addr()[j];
  if (val_only) return;

  /* on ne peut utiliser valeur_aux_bords que si ch_rho a une zone_dis_base */
  DoubleTab b_al = ch_alpha.valeur_aux_bords(), b_rho, b_en = ch_en.valeur_aux_bords();
  int Nb = b_al.dimension_tot(0);
  if (ch_rho.a_une_zone_dis_base()) b_rho = ch_rho.valeur_aux_bords();
  else b_rho.resize(Nb, N), ch_rho.valeur_aux(ref_cast(Zone_VF, ch.zone_dis_base()).xv_bord(), b_rho);
  for (j = 0; j < Nb * N; j++) bval.addr()[j] = b_al.addr()[j] * b_rho.addr()[j] * b_en.addr()[j];

  DoubleTab& d_a = deriv["alpha"];//derivee en alpha : rho * en
  for (d_a.resize(Nl, N), i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++) d_a.addr()[j] = rho.addr()[cR ? n : j] * en.addr()[j];

  /* derivees a travers rho et en */
  const tabs_t d_vide = {}, &d_rho = pch_rho ? pch_rho->derivees() : d_vide, &d_en = ch_en.derivees();
  std::set<std::string> vars; //liste de toutes les derivees possibles
  for (auto && d_c : d_rho) vars.insert(d_c.first);
  for (auto && d_c : d_en) vars.insert(d_c.first);

  for (auto && var : vars)
    {
      const DoubleTab *dr = d_rho.count(var) ? &d_rho.at(var) : NULL, *de = d_en.count(var) ? &d_en.at(var) : NULL;
      DoubleTab& d_v = deriv[var];
      for (d_v = alpha, i = j = 0; i < Nl; i++) for (n = 0; n < N; n++, j++)
          d_v.addr()[j] *= (dr ? dr->addr()[j] * en.addr()[j] : 0) + (de ? rho.addr()[cR ? n : j] * de->addr()[j] : 0);
    }
}

int Energie_Multiphase::equation_non_resolue() const
{
  return 1;
}
