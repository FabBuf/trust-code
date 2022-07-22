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


#include <Solveur_Masse.h>
#include <Equation_base.h>
#include <Discretisation_base.h>


Implemente_deriv(Solveur_Masse_base);
Implemente_instanciable(Solveur_Masse,"Solveur_Masse",DERIV(Solveur_Masse_base));


/*! @brief Imprime le solveur de masse sur un flot de sortie.
 *
 * @param (Sortie& os) le flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Solveur_Masse::printOn(Sortie& os) const
{
  return DERIV(Solveur_Masse_base)::printOn(os);
}


/*! @brief Lit le solveur de masse sur un flot d'entree.
 *
 * @param (Entree& is) le flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Solveur_Masse::readOn(Entree& is)
{
  return DERIV(Solveur_Masse_base)::readOn(is);
}


/*! @brief Construit le bon type de Solveur de masse.
 *
 */
void Solveur_Masse::typer()
{
  Equation_base& eqn=mon_equation.valeur();
  Nom inut;
  Nom typ=eqn.discretisation().get_name_of_type_for(que_suis_je(),inut,eqn);
  DERIV(Solveur_Masse_base)::typer(typ);
  valeur().associer_eqn(eqn);
}
