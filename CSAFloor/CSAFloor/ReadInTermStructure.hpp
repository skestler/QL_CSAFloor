
#ifndef read_in_term_structure_hpp
#define read_in_term_strcture_hpp

#pragma once

#include <ql/quantlib.hpp>
#include <fstream>      // fstream
#include <vector>
#include <string>
#include <algorithm>    // copy
#include <iterator>     // ostream_operator
#include <boost/tokenizer.hpp>


using namespace QuantLib;

/*
This class reads in the quotes for market instruments to be used for bootstrapping of a standard EURIBOR curve
from an input file that provides the following format:

Type,Start/Tenor,End,Quote
OIS,1D,,0.0065
...
DEP,1W,,0.0085
...
FRA,1M,7M,0.01624
...
SWAP,2Y,,0.01555
...

At least one instrument needs to be present.
Further parameters that need to be passed are the following:
@_businessDayConvention: business day convention rule to be applied for interpretation of market instruments
@_calender: Calendar for business days
@_fixingDays: fixing days
*/

template<typename Traits, typename Interpolator>
class ReadInTermStructure
{
public:
	ReadInTermStructure(std::string& _filename, BusinessDayConvention _businessDayConvention,
		const Calendar& _calendar, Integer _fixingDays, const Date& _settlementDate,
		const DayCounter& _depositDayCounter, const DayCounter& _swapFixedLegDayCounter = Thirty360(Thirty360::European),
		const DayCounter& _termStructureDayCounter = Actual365Fixed(), const Frequency _swapFixedLegFrequency = Annual);

	std::string &filename;
	BusinessDayConvention businessDayConvention;
	const Calendar& calendar;
	Integer fixingDays;
	const Date& settlementDate;
	const DayCounter& depositDayCounter;
	const DayCounter& swapFixedLegDayCounter;
	const DayCounter& termStructureDayCounter;
	const Frequency swapFixedLegFrequency;

	// All quotes will be stored separately in order to allow specific shifts for sensitivity analyses
	std::map<std::string, boost::shared_ptr<SimpleQuote> > oisInstruments;
	std::map<std::string, boost::shared_ptr<SimpleQuote> > depositInstruments;
	std::map<std::string, boost::shared_ptr<SimpleQuote> > fraInstruments;
	std::map<std::string, boost::shared_ptr<SimpleQuote> > swapInstruments;

	// The final yield term structure
	boost::shared_ptr<YieldTermStructure> yieldTermStructure;

private:
	// Convert month into an integer, e.g. "3M" -> 3
	Integer getMonthTenorAsInteger(const std::string& tenor);
	// Convert years into an integer, e.g., "3Y" -> 3
	Integer getYearTenorAsInteger(const std::string& tenor);
};

template<class Interpolator>
class ReadInInterpolatedZeroCurve
{
public:
	ReadInInterpolatedZeroCurve(std::string& _filename, const Calendar& _calendar, 
								const DayCounter& _dayCounter = Actual365Fixed());

	std::string &filename;
	const Calendar& calendar;
	const DayCounter& dayCounter;

	// All quotes will be stored separately in order to allow specific shifts for sensitivity analyses
	std::vector<Date> dateVector;
	std::vector<Rate> rateVector;

	// The final yield term structure
	boost::shared_ptr<YieldTermStructure> zeroCurve;

private:
	void printZeroRateCurve();
	Month getMonthFromInteger(int month);
};

#endif

#include "ReadInTermStructure.tcc"

