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

#ifndef Sortie_libre_Gradient_Pression_impose_included
#define Sortie_libre_Gradient_Pression_impose_included

/*! @brief Sortie_libre_Gradient_Pression_impose
 *
 *  Cette classe derive de la classe Neumann_sortie_libre
 *
 *     Elle represente une frontiere ouverte avec condition de gradient de pression impose.
 *     L'objet de type Champ_front le_champ_front contient le gradient impose. La fonction flux_impose() renvoie une valeur de pression
 *     a l'exterieur calculee a partir du gradient de pression impose et de la pression a l'interieur du domaine.
 *
 * @sa Milieu_base
 */

#include <Neumann_sortie_libre.h>
#include <Ref_Champ_P0_VDF.h>
#include <Ref_Zone_VDF.h>

class Sortie_libre_Gradient_Pression_impose: public Neumann_sortie_libre
{
  Declare_instanciable(Sortie_libre_Gradient_Pression_impose);
public:
  void completer() override;
  void mettre_a_jour(double) override;
  double flux_impose(int) const override;
  double flux_impose(int, int) const override;
  virtual double grad_P_imp(int) const;

protected:
  REF(Zone_VDF) le_dom_VDF;
  REF(Champ_P0_VDF) pression_interne;
  DoubleVect trace_pression_int;   // pression interne sur les mailles de bord
  DoubleVect coeff;
};

#endif
