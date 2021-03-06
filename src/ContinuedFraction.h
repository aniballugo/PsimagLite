
/*
// BEGIN LICENSE BLOCK
Copyright (c) 2009 , UT-Battelle, LLC
All rights reserved

[PsimagLite, Version 1.0.0]

*********************************************************
THE SOFTWARE IS SUPPLIED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. 

Please see full open source license included in file LICENSE.
*********************************************************

*/
/** \ingroup DMRG */
/*@{*/

/*! \file ContinuedFraction.h
 *
 * A continued fraction as explained in, e.g.,
 * E. Dagotto, Rev. Mod. Phys., 66, 763, (2004).
 */

#ifndef CONTINUED_FRACTION_H
#define CONTINUED_FRACTION_H
#include <iostream>
#include "Complex.h"
#include "TypeToString.h"
#include "ProgressIndicator.h"
#include "Random48.h"
#include "PlotParams.h"

namespace PsimagLite {
	template<
		typename RealType,
    	typename TridiagonalMatrixType_>
	class ContinuedFraction  {
	public:
		typedef TridiagonalMatrixType_ TridiagonalMatrixType;
		typedef typename std::complex<RealType> ComplexType;
		typedef typename TridiagonalMatrixType::value_type FieldType;
		typedef Matrix<FieldType> MatrixType;
		typedef std::vector<std::pair<RealType,ComplexType> > PlotDataType;
		typedef PlotParams<RealType> PlotParamsType;

		ContinuedFraction(
				const TridiagonalMatrixType& ab,
				const RealType& Eg,
				RealType weight,
			        int isign)
			: progress_("ContinuedFraction",0),ab_(ab),
			Eg_(Eg),weight_(weight),isign_(isign)
		{
			diagonalize();
		}

		ContinuedFraction() : progress_("ContinuedFraction",0),
			ab_(),Eg_(0),weight_(0),isign_(1) { }

		template<typename IoInputType>
		ContinuedFraction(IoInputType& io)
		: progress_("ContinuedFraction",0),ab_(io)
		{
			io.readline(weight_,"#CFWeight=");
			io.readline(Eg_,"#CFEnergy=");
			io.readline(isign_,"#CFIsign=");
			io.read(eigs_,"#CFEigs");
			io.read(intensity_,"#CFIntensities");
			diagonalize();
		}
		
		template<typename IoOutputType>
		void save(IoOutputType& io) const
		{
			ab_.save(io);

			std::string s = "#CFWeight=" + typeToString(weight_);
			io.printline(s);

			s = "#CFEnergy=" + typeToString(Eg_);
			io.printline(s);

			s="#CFIsign=" + typeToString(isign_);
			io.printline(s);

			io.printVector(eigs_,"#CFEigs");
			io.printVector(intensity_,"#CFIntensities");
		}

		void set(
			const TridiagonalMatrixType& ab,
			const RealType& Eg,
			RealType weight,
			int isign)
		{
			ab_ = ab;
			Eg_ = Eg;
			weight_ = weight;
			isign_ = isign;

			diagonalize();
		}

		void plot(
				PlotDataType& result,
				const PlotParamsType& params) const
		{
			size_t counter = 0;
			size_t n = size_t((params.omega2 - params.omega1)/params.deltaOmega); 
			if (result.size()==0) result.resize(n);
			for (RealType omega = params.omega1;omega <params.omega2;omega+=params.deltaOmega) {
				ComplexType z(omega,params.delta);
				ComplexType res = iOfOmega(z,Eg_,isign_);
				std::pair<RealType,ComplexType> p(omega,res);
				result[counter++] = p;
				if (counter>=result.size()) break;
				//std::cout<<omega<<" "<<real(res)<<" "<<imag(res)<<"\n";
			}
		} 

		//! Cases: 
		//! (1) <phi0|A (z+(E0-e_k))^{-1}|A^\dagger|phi0> and
		//! (2) <phi0|A^\dagger (z-(E0-e_k))^{-1}|A|phi0>
		//! (There are actually 4 cases for the off-diagonal gf because
		//! A has two cases:
		//! (1) A = c_i + c_j and
		//! (2) A = c_i - c_j
		ComplexType iOfOmega(const ComplexType& z,RealType offset,int isign) const

		{
			if (weight_==0) return ComplexType(0,0);

			ComplexType sum = 0;
			for (size_t l=0;l<intensity_.size();l++)
				sum +=intensity_[l]/(z-isign*(offset - eigs_[l]));

			return sum*weight_;
		}

	private:

		void diagonalize()
		{
			if (weight_==0) return;
			MatrixType T;
			ab_.buildDenseMatrix(T);
			eigs_.resize(T.n_row());
			diag(T,eigs_,'V');
			intensity_.resize(T.n_row());
			for (size_t i=0;i<T.n_row();i++) {
				intensity_[i]= T(0,i)*T(0,i);
			}
		}

		ProgressIndicator progress_;
		TridiagonalMatrixType ab_;
		RealType Eg_;
		RealType weight_;
		int isign_;
		std::vector<RealType> eigs_;
		std::vector<RealType> intensity_;
	}; // class ContinuedFraction
} // namespace PsimagLite 
/*@}*/
#endif  //CONTINUED_FRACTION_H
