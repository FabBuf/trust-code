////////////////////////////////////////////////////////////
//
// File:	Ecrire_fic_lml_2.h
// Directory:	$TRUST_ROOT/Geometrie/Decoupeur
//
////////////////////////////////////////////////////////////
								 

#ifndef Ecrire_fic_lml_2_included
#define Ecrire_fic_lml_2_included

#include <Ecrire_fic_lml.h>


//////////////////////////////////////////////////////////////////////////////
// 
// .DESCRIPTION 
//    Classe Ecrire_fic_lml_2
//    Lecture d'un fichier 
// .SECTION voir aussi 
//    Interprete
//////////////////////////////////////////////////////////////////////////////
class Ecrire_fic_lml_2 : public Ecrire_fic_lml
{
  Declare_instanciable(Ecrire_fic_lml_2);
    public :
    Entree& interpreter(Entree&);
 
}; 
#endif
