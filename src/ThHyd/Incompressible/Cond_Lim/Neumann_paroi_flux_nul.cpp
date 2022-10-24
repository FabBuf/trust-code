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

#include <Neumann_paroi_flux_nul.h>
#include <Motcle.h>
#include <Equation_base.h>

Implemente_instanciable(Neumann_paroi_flux_nul,"Paroi",Neumann_paroi_adiabatique);


/*! @brief Ecrit le type de l'objet sur un flot de sortie.
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Neumann_paroi_flux_nul::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}

/*! @brief Simple appel a: Neumann_homogene::readOn(Entree& )
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree& s) le flot d'entree modifie
 */
Entree& Neumann_paroi_flux_nul::readOn(Entree& s )
{
  return Neumann_homogene::readOn(s) ;
}

/*! @brief Renvoie un booleen indiquant la compatibilite des conditions aux limites avec l'equation specifiee en parametre.
 *
 *     Des CL de type Neumann_paroi_flux_nul sont compatibles
 *     avec une equation dont le domaine est la Concentration, la Fraction_massique
 *     le Transport_Keps, le Transport_Keps_V2, le Transport_V2, le Diphasique_moyenne
 *     ou bien indetermine.
 *
 * @param (Equation_base& eqn) l'equation avec laquelle il faut verifier la compatibilite
 * @return (int) valeur booleenne, 1 si les CL sont compatibles avec l'equation 0 sinon
 */
int Neumann_paroi_flux_nul::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();

  Motcle Concentration      ="Concentration";
  Motcle Fraction_massique  ="Fraction_massique";
  Motcle K_Eps              ="Transport_Keps";
  Motcle K_Eps_V2           ="Transport_Keps_V2";
  Motcle K_Eps_Rea          ="Transport_Keps_Rea";
  Motcle V2                 ="Transport_V2";
  Motcle Diphasique         ="Diphasique_moyenne";
  Motcle Fraction_volumique ="Fraction_volumique";
  Motcle Aire_interfaciale  ="Interfacial_area";
  Motcle indetermine        ="indetermine";

  if ( (dom_app==Concentration) || (dom_app==K_Eps) || (dom_app==K_Eps_Rea) || (dom_app==K_Eps_V2) || (dom_app==V2) ||
       (dom_app==Diphasique) || (dom_app==indetermine) || (dom_app==Fraction_massique) || (dom_app==Fraction_volumique) || (dom_app==Aire_interfaciale) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}
