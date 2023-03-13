/****************************************************************************
 * Copyright (c) 2021, CEA
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

#include <EOS_to_TRUST.h>
#include <algorithm>
#include <Process.h>

using namespace NEPTUNE;

EOS_to_TRUST::EOS_to_TRUST()
{
#ifdef HAS_EOS
  fluide = nullptr;
  handler.set_exit_on_error(ok); // to deactivate use EOS_Std_Error_Handler::disable_feature
  handler.set_throw_on_error(ok);
  handler.set_dump_on_error(ok);
#endif
}

EOS_to_TRUST::~EOS_to_TRUST() { /* delete fluide; */ }

void EOS_to_TRUST::compute_eos_field(const SpanD P_ou_T, SpanD res,const char *const property_title, const char *const property_name, bool is_T) const
{
#ifdef HAS_EOS
  const char *const pp_ = is_T ? "Temperature" : "Pressure";
  const char *const tt_ = is_T ? "T" : "P";
  EOS_Field P_fld(pp_, tt_,  (int)P_ou_T.size(), (double*)P_ou_T.begin());
  EOS_Field z_fld(property_title,property_name, (int)res.size(), (double*)res.begin());
  ArrOfInt tmp((int)P_ou_T.size());
  EOS_Error_Field ferr(tmp);
  fluide->compute(P_fld, z_fld, ferr);
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST::compute_eos_field(const SpanD P, const SpanD T, SpanD res,const char *const property_title, const char *const property_name) const
{
#ifdef HAS_EOS
  EOS_Field T_fld("Temperature", "T", (int)T.size(),(double*)T.begin()), P_fld("Pressure", "P", (int)P.size(), (double*)P.begin());
  EOS_Field z_fld(property_title,property_name, (int)res.size(), (double*)res.begin());
  ArrOfInt tmp((int)P.size());
  EOS_Error_Field ferr(tmp);
  fluide->compute(P_fld, T_fld, z_fld, ferr);
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}

void EOS_to_TRUST::verify_model_fluid(Motcle& model_name, Motcle& fluid_name)
{
#ifdef HAS_EOS
  if (!(std::find(supp.AVAIL_MODELS.begin(), supp.AVAIL_MODELS.end(), model_name) != supp.AVAIL_MODELS.end()))
    {
      Cerr << "You define the : < " << model_name << " model which is not yet tested !" << finl;
      Cerr << "Please use one of the available models or contact the TRUST team." << finl;
      Cerr << finl;
      Cerr <<"Available Models : " << finl;
      for (const auto& itr : supp.AVAIL_MODELS) Cerr << itr << finl;

      Process::exit();
    }

  if(!(std::find(supp.AVAIL_FLUIDS.begin(), supp.AVAIL_FLUIDS.end(), fluid_name) != supp.AVAIL_FLUIDS.end()))
    {
      Cerr << "You define the : < " << fluid_name << " fluid which is not yet tested !" << finl;
      Cerr << "Please use one of the available fluids or contact the TRUST team." << finl;
      Cerr << finl;
      Cerr <<"Available Fluids : " << finl;
      for (const auto& itr : supp.AVAIL_FLUIDS) Cerr << itr << finl;

      Process::exit();
    }
#else
  Cerr << "EOS_to_TRUST::" <<  __func__ << " should not be called since TRUST is not compiled with the EOS library !!! " << finl;
  throw;
#endif
}