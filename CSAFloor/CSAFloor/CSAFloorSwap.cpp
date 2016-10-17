/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*!
 Copyright (C) 2000, 2001, 2002, 2003 RiskMap srl
 Copyright (C) 2003, 2004, 2005, 2006, 2007 StatPro Italia srl
 Copyright (C) 2004 Ferdinando Ametrano

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a

 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*  This example shows how to set up a Term Structure and then price a simple
    swap.
*/

// the only header you need to use QuantLib
#include <ql/quantlib.hpp>

#ifdef BOOST_MSVC
/* Uncomment the following lines to unmask floating-point
   exceptions. Warning: unpredictable results can arise...

   See http://www.wilmott.com/messageview.cfm?catid=10&threadid=9481
   Is there anyone with a definitive word about this?
*/
// #include <float.h>
// namespace { unsigned int u = _controlfp(_EM_INEXACT, _MCW_EM); }
#endif

#include <boost/timer.hpp>
#include <iostream>
#include <iomanip>
#include <boost/format.hpp>
#include "ReadInTermStructure.hpp"

using namespace QuantLib;

#if defined(QL_ENABLE_SESSIONS)
namespace QuantLib {

    Integer sessionId() { return 0; }

}
#endif

void testTermStructure(const Date & todaysDate, const DayCounter &mainDayCounter,
					const Handle<YieldTermStructure> &yieldTermStructure)
{
	Date testingDate1(23, November, 2011);
	Rate zerorate_testingDate1 = 0.006590158789;
	Real yearFraction1 = mainDayCounter.yearFraction(todaysDate, testingDate1);
	std::cout << testingDate1 << ", " << zerorate_testingDate1 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction1, Continuous) <<std::endl;

	Date testingDate2(30, November, 2011);
	Rate zerorate_testingDate2 = 0.008166857967;
	Real yearFraction2 = mainDayCounter.yearFraction(todaysDate, testingDate2);
	std::cout << testingDate2 << ", " << zerorate_testingDate2 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction2, Continuous) <<std::endl;

	Date testingDate3(23, May, 2012);
	Rate zerorate_testingDate3 = 0.0168486744875;
	Real yearFraction3 = mainDayCounter.yearFraction(todaysDate, testingDate3);
	std::cout << testingDate3 << ", " << zerorate_testingDate3 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction3, Continuous) <<std::endl;

	Date testingDate4(23, November, 2012);
	Rate zerorate_testingDate4 = 0.015717733752;
	Real yearFraction4 = mainDayCounter.yearFraction(todaysDate, testingDate4);
	std::cout << testingDate4 << ", " << zerorate_testingDate4 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction4, Continuous) <<std::endl;

	Date testingDate5(25, November, 2013);
	Rate zerorate_testingDate5 = 0.01538260621;
	Real yearFraction5 = mainDayCounter.yearFraction(todaysDate, testingDate5);
	std::cout << testingDate5 << ", " << zerorate_testingDate5 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction5, Continuous) <<std::endl;

	Date testingDate6(24, November, 2014);
	Rate zerorate_testingDate6 = 0.0165359;
	Real yearFraction6 = mainDayCounter.yearFraction(todaysDate, testingDate6);
	std::cout << testingDate6 << ", " << zerorate_testingDate6 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction6, Continuous) <<std::endl;

	Date testingDate7(23, November, 2016);
	Rate zerorate_testingDate7 = 0.0201702;
	Real yearFraction7 = mainDayCounter.yearFraction(todaysDate, testingDate7);
	std::cout << testingDate7 << ", " << zerorate_testingDate7 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction7, Continuous) <<std::endl;

	Date testingDate8(23, November, 2021);
	Rate zerorate_testingDate8 = 0.0262071;
	Real yearFraction8 = mainDayCounter.yearFraction(todaysDate, testingDate8);
	std::cout << testingDate8 << ", " << zerorate_testingDate8 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction8, Continuous) <<std::endl;

	Date testingDate9(24, November, 2036);
	Rate zerorate_testingDate9 = 0.0280392;
	Real yearFraction9 = mainDayCounter.yearFraction(todaysDate, testingDate9);
	std::cout << testingDate9 << ", " << zerorate_testingDate9 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction9, Continuous) <<std::endl;

	Date testingDate10(23, November, 2061);
	Rate zerorate_testingDate10 = 0.0273798;
	Real yearFraction10 = mainDayCounter.yearFraction(todaysDate, testingDate10);
	std::cout << testingDate10 << ", " << zerorate_testingDate10 << ", "
			  << (double) yieldTermStructure->zeroRate( yearFraction10, Continuous) <<std::endl << std::endl;
}

int main(int, char* []) {

	// This program reads IR market data from a csv file and constructs a corresponding yield curve
	// To run it, you will need to enter the path to the input csv-file on your system below.

	Calendar calendar = TARGET();
	std::cout << "Calendar = " << calendar << std::endl;
	Integer fixingDays = 2;

	// Set up two main day counters
	//DayCounter depositDayCounter = Actual360();
	DayCounter curveDayCounter = Actual365Fixed();

	std::string filename1("C:\\Users\\SKestler\\Documents\\Programming\\TestingQuantlib\\20160607_EoniaCurve.csv");
	std::string filename2("C:\\Users\\SKestler\\Documents\\Programming\\TestingQuantlib\\20160607_EUR6M.csv");

		// Term structures that will be used for pricing:
    // the one used for discounting cash flows
	// RelinkableHandle can be "re-linked" to another smart-pointer (other term structure in this case)
	// RelinkableHandle is derived from Handle - where re-linking is not possible
	RelinkableHandle<YieldTermStructure> discountingTermStructure;
	RelinkableHandle<YieldTermStructure> forwardTermStructure;
	
	ReadInInterpolatedZeroCurve<Linear> readInInterpolatedZeroCurve1(filename1,calendar,curveDayCounter);
	ReadInInterpolatedZeroCurve<Linear> readInInterpolatedZeroCurve2(filename2,calendar,curveDayCounter);
    
	discountingTermStructure.linkTo(readInInterpolatedZeroCurve1.zeroCurve);
	forwardTermStructure.linkTo(readInInterpolatedZeroCurve2.zeroCurve);

	std::cout << "Generated Swap Curves" << std::endl;
	getchar();
	Date todaysDate(07, June, 2016);
    Settings::instance().evaluationDate() = todaysDate;

	// general swap infos
    Real	notional = 1000000.0;
	Integer lengthInYears = 10;
    Rate	fixedRate = 0.02;
	Spread  spread = 0.0;
    VanillaSwap::Type swapType = VanillaSwap::Payer;
	boost::shared_ptr<PricingEngine> swapEngine(
					new DiscountingSwapEngine(discountingTermStructure));

	// fixed leg info
    Frequency fixedLegFrequency = Annual;
    BusinessDayConvention fixedLegConvention = ModifiedFollowing;
	DayCounter fixedLegDayCounter = Thirty360(Thirty360::European);

	// floating leg
	BusinessDayConvention floatingLegConvention = ModifiedFollowing;
	DayCounter floatingLegDayCounter = Actual360();
    Frequency floatingLegFrequency = Semiannual;

	// EURIBOR Index
	boost::shared_ptr<IborIndex> euriborIndex(new Euribor6M(forwardTermStructure));
	
	for (int j=1; j<=10; ++j) {
		std::cout << "," << j << "Y";
	}
	std::cout << std::endl;

	for (int i=0; i<=10; ++i) {  // Forward starting in i years
		Date settlementDate = todaysDate + 2*Days + i*Years;
		std::cout << i << "Y";
		for (int j=1; j<=10; ++j) {
			Date maturity = settlementDate + j*Years;
			
			Schedule fixedSchedule(settlementDate, maturity, Period(fixedLegFrequency),
                        calendar, fixedLegConvention, fixedLegConvention, DateGeneration::Forward, false);
			//for (Schedule::const_iterator it=fixedSchedule.begin(); it!=fixedSchedule.end(); ++it) {
			//	std::cout << (*it) << std::endl;
			//}
			//getchar();

			Schedule floatSchedule(settlementDate, maturity,
                        Period(floatingLegFrequency),
                        calendar, floatingLegConvention,
                        floatingLegConvention,
                        DateGeneration::Forward, false);
			//for (Schedule::const_iterator it=floatSchedule.begin(); it!=floatSchedule.end(); ++it) {
			//	std::cout << (*it) << std::endl;
			//}
			//Set up the vanilla swap
			VanillaSwap swap(swapType, notional,
				fixedSchedule, fixedRate, fixedLegDayCounter,
				floatSchedule, euriborIndex, spread,
				floatingLegDayCounter);	
			swap.setPricingEngine(swapEngine);

			//getchar();

			//Real NPV;
			//NPV = swap.NPV();
			//std::cout << "NPV = " << NPV << std::endl;
			//Spread fairSpread;
			//fairSpread = swap.fairSpread();
			//std::cout << "fair spread = " << fairSpread << std::endl;
			Rate fairRate;
			fairRate = swap.fairRate();
			std::cout << "," << fairRate;
		}
		std::cout << std::endl;
	}
	getchar();
	return 0;
	

}

