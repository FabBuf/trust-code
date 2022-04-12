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
// File:        Champ_Don_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Don_base_included
#define Champ_Don_base_included

#include <Champ_base.h>
#include <TRUSTTab.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_Don_base
//     classe de base des Champs donnes (non calcules)
//     les proprietes physiques sont des Champs donnes
//     mais peuvent etre variables dans le temps dans l'espace
//     fonctions d'autres champs ...
// .SECTION voir aussi
//     Champ_base Champ_Don Champ_Fonc
//////////////////////////////////////////////////////////////////////////////

template<bool B, typename T> using enable_if_t = typename std::enable_if<B, T>::type;

class Champ_Don_base : public Champ_base
{
  Declare_base(Champ_Don_base);
public:
  using Champ_Proto::valeurs;

  void mettre_a_jour(double temps) override;
  void dimensionner(int, int);
  int imprime(Sortie&, int) const override;
  int fixer_nb_valeurs_nodales(int nb_noeuds) override;
  int reprendre(Entree&) override;
  int sauvegarder(Sortie&) const override;

  DoubleTab& valeurs() override;
  const DoubleTab& valeurs() const override;

  Champ_base& affecter_(const Champ_base&) override;
  Champ_base& affecter_compo(const Champ_base&, int) override;

  virtual int initialiser(const double temps);

  // Description: Renvoie le nombre de degre de liberte par composante: le nombre de noeuds.
  inline int nb_valeurs_nodales() const override { return valeurs_.dimension(0); }

  // forbidden
  operator DoubleTab& () = delete;
  operator const DoubleTab& () const = delete;

protected :
  DoubleTab valeurs_;
};

inline void erreur_champ_(const char *nom_methode)
{
  Cerr << "TRUST Error !!! Verify the method " << nom_methode << " !!!" << finl;
  Process::exit();
}

template <typename _TYPE_RETURN_>
inline _TYPE_RETURN_ not_implemented_champ_(const char *nom_methode)
{
  Cerr << "TRUST Error !!! The method " << nom_methode << " is not implemented !!!" << finl;
  throw;
}

#endif /* Champ_Don_base_included */
