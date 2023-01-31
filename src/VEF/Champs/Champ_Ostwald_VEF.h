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

#ifndef Champ_Ostwald_VEF_included
#define Champ_Ostwald_VEF_included

#include <Ref_Navier_Stokes_std.h>
#include <Ref_Champ_P1NC.h>
#include <Champ_Ostwald.h>
#include <Ref_Domaine_VEF.h>
#include <Champ_P1NC.h>

/*! @brief classe Champ_Ostwald_VEF Represente un champ en discretisation VEF qui varie en fonction
 *
 *         de la consistance et de l'indice de structure.
 *     Champ utilise pour le fluide d'Ostwald et la discretisation VEF.
 *     Fait reference a domaine_VEF pou utiliser la bonne domaine avec domaine_dis_base
 *                     Champ_P1NC pour calculer D:D
 *                     Navier_Stokes_std pour avoir l'equation thermo_hydraulique
 *                     et pouvoir utiliser une de ses inconnues: la vitesse.
 *
 * @sa Champ_Ostwald
 */
class Champ_Ostwald_VEF : public Champ_Ostwald
{
  Declare_instanciable(Champ_Ostwald_VEF);
public :
  void associer_eqn(const Navier_Stokes_std& );
  void mettre_a_jour(double temps) override;
  void me_calculer(double tps) override;
  int initialiser(const double temps) override;
  void init_mu(DoubleTab& );
  void calculer_dscald(DoubleTab&);

  inline const Champ_P1NC& mon_champs() const { return mon_champ_.valeur(); }
  inline void associer_champ(const Champ_P1NC& un_champ) { mon_champ_ = un_champ; }

protected :
  void calculer_mu(DoubleTab& );
  REF(Champ_P1NC) mon_champ_;
  REF(Navier_Stokes_std) eq_hydraulique;
};

#endif /* Champ_Ostwald_VEF_included */
