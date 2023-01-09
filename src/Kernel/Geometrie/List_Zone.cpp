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

#include <List_Zone.h>

Implemente_instanciable(List_Zone, "List_Zone", liste);

Sortie& List_Zone::printOn(Sortie& os) const { return liste::printOn(os); }

Entree& List_Zone::readOn(Entree& is)
{
  if (!est_vide())
    if (0) Cerr << "list not empty, append to it" << finl;

  Nom accouverte = "{", accfermee = "}", virgule = ",";
  Motcle nom_;
  is >> nom_;
  if (nom_ == (const char*) "vide") return is;

  if (nom_ != accouverte) Cerr << "Error while reading a list. One expected an opened bracket { to start." << finl, Process::exit();
  Zone t;

  while (1)
    {
      is >> add(t);
      is >> nom_;
      if (nom_ == accfermee) return is;
      if (nom_ != virgule) Cerr << nom_ << " one expected a ',' or a '}'" << finl, Process::exit();
    }
}

Zone& List_Zone::add_if_not(const char *const t)
{
  Nom nom(t);
  if (est_vide())
    {
      Zone to_add;
      to_add.nommer(nom);
      return add(to_add);
    }
  liste_curseur curseur = *this;
  while (curseur)
    {
      if (curseur.valeur().le_nom() == nom)
        return static_cast<Zone&>(curseur.valeur());
      if (curseur.list().est_dernier())
        {
          Zone to_add;
          to_add.nommer(nom);
          return add(to_add);
        }
      ++curseur;
    }
  Cerr << "Error in a list for add_if_not " << finl;
  return static_cast<Zone&>(valeur());
}

