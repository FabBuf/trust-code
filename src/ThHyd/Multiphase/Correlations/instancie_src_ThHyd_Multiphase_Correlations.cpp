//
// Warning : DO NOT EDIT !
// To update this file, run: make depend
//
#include <verifie_pere.h>
#include <Changement_phase_Silver_Simpson.h>
#include <Correlation.h>
#include <Flux_interfacial_Coef_Constant.h>
#include <Frottement_interfacial_Sonnenburg.h>
#include <Frottement_interfacial_Wallis.h>
#include <Frottement_interfacial_Weber.h>
#include <Frottement_interfacial_bulles.h>
#include <Masse_ajoutee_Coef_Constant.h>
#include <Multiplicateur_diphasique_Friedel.h>
#include <Multiplicateur_diphasique_Lottes_Flinn.h>
#include <Multiplicateur_diphasique_Muhler_Steinhagen.h>
#include <Multiplicateur_diphasique_homogene.h>
void instancie_src_ThHyd_Multiphase_Correlations() {
Cerr << "src_ThHyd_Multiphase_Correlations" << finl;
Changement_phase_Silver_Simpson inst1;verifie_pere(inst1);
Correlation inst2;verifie_pere(inst2);
Flux_interfacial_Coef_Constant inst3;verifie_pere(inst3);
Frottement_interfacial_Sonnenburg inst4;verifie_pere(inst4);
Frottement_interfacial_Wallis inst5;verifie_pere(inst5);
Frottement_interfacial_Weber inst6;verifie_pere(inst6);
Frottement_interfacial_bulles inst7;verifie_pere(inst7);
Masse_ajoutee_Coef_Constant inst8;verifie_pere(inst8);
Multiplicateur_diphasique_Friedel inst9;verifie_pere(inst9);
Multiplicateur_diphasique_Lottes_Flinn inst10;verifie_pere(inst10);
Multiplicateur_diphasique_Muhler_Steinhagen inst11;verifie_pere(inst11);
Multiplicateur_diphasique_homogene inst12;verifie_pere(inst12);
}
