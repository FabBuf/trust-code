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

#ifndef Sous_domaine_dis_base_included
#define Sous_domaine_dis_base_included

#include <Ref_Domaine_dis_base.h>
#include <Ref_Sous_Domaine.h>

class Domaine_dis_base;

/*! @brief Cette classe est a la base de la hierarchie des sous-domaines discretisees.
 * Au moment de la discretisation du domaine, de meme qu'une
 * Domaine_dis est creee pour chaque domaine, une Sous_domaine_dis est
 * creee pour chaque Sous_Domaine et discretisee.
 *
 * Les classes qui heritent de Sous_domaine_dis_base doivent surcharger la methode discretiser();
 *
 * La Sous_Domaine definit un sous-ensemble des elements de la Domaine.
 * De meme, la Sous_domaine_dis definit un sous_ensemble des attributs
 * de la Domaine_dis, en particulier des faces.
 *
 * Voir aussi Domaine_dis, Sous_Domaine, Domaine_dis_base, Sous_Domaine_VF
*/
class Sous_domaine_dis_base : public Objet_U
{
  Declare_base(Sous_domaine_dis_base);

public:
  void associer_sous_domaine(const Sous_Domaine&);
  void associer_domaine_dis(const Domaine_dis_base&);

  // Methodes d'acces aux REFs
  const Sous_Domaine& sous_domaine() const;
  Sous_Domaine& sous_domaine();
  const Domaine_dis_base& domaine_dis() const;
  Domaine_dis_base& domaine_dis();

  // Methodes propres :
  virtual void discretiser() = 0;

protected:
  REF(Sous_Domaine) la_sous_domaine;
  REF(Domaine_dis_base) le_dom_dis;
};

#endif