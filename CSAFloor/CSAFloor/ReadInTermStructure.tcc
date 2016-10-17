

template<typename Traits, typename Interpolator>
ReadInTermStructure<Traits, Interpolator>::ReadInTermStructure(
		std::string& _filename, BusinessDayConvention _businessDayConvention, const Calendar& _calendar,
		Integer _fixingDays, const Date& _settlementDate, const DayCounter& _depositDayCounter,
		const DayCounter& _swapFixedLegDayCounter = Thirty360(Thirty360::European),
		const DayCounter& _termStructureDayCounter = Actual365Fixed(), 
		const Frequency _swapFixedLegFrequency = Annual)
	: filename(_filename), businessDayConvention(_businessDayConvention), calendar(_calendar), fixingDays(_fixingDays),
	   settlementDate(_settlementDate), depositDayCounter(_depositDayCounter),
	   swapFixedLegDayCounter(_swapFixedLegDayCounter), termStructureDayCounter(_termStructureDayCounter),
	   swapFixedLegFrequency(_swapFixedLegFrequency)
{

	std::ifstream in(filename.c_str());
	if (!in.is_open()) {
		std::cout << "Stream is not open" << std::endl;
		exit(1);
	}

	typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
    std::vector< std::string > vec;
    std::string line;

	std::vector<boost::shared_ptr<RateHelper> > termStructureInstruments;

	while (std::getline(in,line)) {
        Tokenizer tok(line);
        vec.assign(tok.begin(),tok.end());

		if (std::strcmp(vec[0].c_str(),"OIS") == 0) {
			const char * tenor;
			tenor = vec[1].c_str();
			Rate oisQuote = std::atof(vec[3].c_str());			
			boost::shared_ptr<SimpleQuote> oisRate(new SimpleQuote(oisQuote));

			TimeUnit timeUnit;
			Integer timeFactor;
			bool validPeriod = true;
			if (strcmp(tenor,"1D")==0)		{ timeUnit = Days; timeFactor = 1;  }
			else if (strcmp(tenor,"2D")==0) { timeUnit = Days; timeFactor = 2;  }
			else {
				validPeriod = false;
				std::cout << "OIS instrument: Non-valid tenor " << tenor << std::endl;
			}
			if (validPeriod) {
				oisInstruments[vec[1]] = oisRate;
				boost::shared_ptr<RateHelper> ois(new DepositRateHelper(Handle<Quote>(oisRate),
									timeFactor*timeUnit, 0, calendar, businessDayConvention,
									true, depositDayCounter));
				termStructureInstruments.push_back(ois);
			}
		}
		else if (std::strcmp(vec[0].c_str(),"DEP") == 0) {
			const char * tenor;
			tenor = vec[1].c_str();
			Rate depQuote = std::atof(vec[3].c_str());			
			boost::shared_ptr<SimpleQuote> depRate(new SimpleQuote(depQuote));

			TimeUnit timeUnit;
			Integer timeFactor;
			bool validPeriod = true;
			if (strcmp(tenor,"1W")==0)		{ timeUnit = Weeks; timeFactor = 1;  }
			else if (strcmp(tenor,"2W")==0) { timeUnit = Weeks; timeFactor = 2;  }
			else if (strcmp(tenor,"3W")==0) { timeUnit = Weeks; timeFactor = 3;  }
			else if (strcmp(tenor,"1M")==0) { timeUnit = Months; timeFactor = 1; }
			else if (strcmp(tenor,"2M")==0) { timeUnit = Months; timeFactor = 2; }
			else if (strcmp(tenor,"3M")==0) { timeUnit = Months; timeFactor = 3; }
			else if (strcmp(tenor,"4M")==0) { timeUnit = Months; timeFactor = 4; }
			else if (strcmp(tenor,"5M")==0) { timeUnit = Months; timeFactor = 5; }
			else if (strcmp(tenor,"6M")==0) { timeUnit = Months; timeFactor = 6; }
			else if (strcmp(tenor,"7M")==0) { timeUnit = Months; timeFactor = 7; }
			else if (strcmp(tenor,"8M")==0) { timeUnit = Months; timeFactor = 8; }
			else if (strcmp(tenor,"9M")==0) { timeUnit = Months; timeFactor = 9; }
			else if (strcmp(tenor,"1Y")==0) { timeUnit = Years; timeFactor = 1;  }
			else {
				validPeriod = false;
				std::cout << "Deposit instrument: Non-valid tenor " << tenor << std::endl;
			}
			if (validPeriod) {
				depositInstruments[vec[1]] = depRate;
				boost::shared_ptr<RateHelper> dep(new DepositRateHelper(Handle<Quote>(depRate),
									timeFactor*timeUnit, fixingDays,calendar, businessDayConvention,
									true, depositDayCounter));
				termStructureInstruments.push_back(dep);
			}
			//Handle<Quote> tmp(mmRate);
			//std::cout << "Check: " << (*(tmp.currentLink()).get()).value() << std::endl;
		}
		else if (std::strcmp(vec[0].c_str(),"FRA") == 0) {
			Rate fraQuote = std::atof(vec[3].c_str());			
			boost::shared_ptr<SimpleQuote> fraRate(new SimpleQuote(fraQuote));
			Integer start = getMonthTenorAsInteger(vec[1]);
			Integer end   = getMonthTenorAsInteger(vec[2]);
			fraInstruments[vec[1]] = fraRate;
			boost::shared_ptr<RateHelper> fra(new FraRateHelper(Handle<Quote>(fraRate),
					start, end, fixingDays, calendar, businessDayConvention, true, depositDayCounter));
			termStructureInstruments.push_back(fra);
		}
		else if (std::strcmp(vec[0].c_str(),"SWAP") == 0) {
			Integer tenor = getYearTenorAsInteger(vec[1]);
			Rate swapQuote = std::atof(vec[3].c_str());			
			boost::shared_ptr<SimpleQuote> swapRate(new SimpleQuote(swapQuote));
			boost::shared_ptr<IborIndex> swFloatingLegIndex(new Euribor6M);
			swapInstruments[vec[1]] = swapRate;

			boost::shared_ptr<RateHelper> swap(new SwapRateHelper(Handle<Quote>(swapRate), tenor*Years,
				calendar, swapFixedLegFrequency,businessDayConvention, swapFixedLegDayCounter,swFloatingLegIndex));
			termStructureInstruments.push_back(swap);
		}
    }

	for (std::map<std::string, boost::shared_ptr<SimpleQuote> >::iterator it=depositInstruments.begin(); it!=depositInstruments.end(); ++it) {
		std::cout << (*it).first << ": " << (*it).second->value() << std::endl;
	}

	double tolerance = 1.0e-15;	
	boost::shared_ptr<YieldTermStructure> tmp(
		new PiecewiseYieldCurve<Traits,Interpolator>(settlementDate, termStructureInstruments,
		termStructureDayCounter, tolerance));
	std::cout << "Yield Curve constructed" << std::endl;
	yieldTermStructure = tmp;
}

template<typename Traits, typename Interpolator>
Integer
ReadInTermStructure<Traits, Interpolator>::getMonthTenorAsInteger(const std::string& tenor)
{
	if (std::strcmp(tenor.c_str(),"1M") == 0)		return 1;
	else if (std::strcmp(tenor.c_str(),"2M") == 0)	return 2;
	else if (std::strcmp(tenor.c_str(),"3M") == 0)	return 3;
	else if (std::strcmp(tenor.c_str(),"4M") == 0)	return 4;
	else if (std::strcmp(tenor.c_str(),"5M") == 0)	return 5;
	else if (std::strcmp(tenor.c_str(),"6M") == 0)	return 6;
	else if (std::strcmp(tenor.c_str(),"7M") == 0)	return 7;
	else if (std::strcmp(tenor.c_str(),"8M") == 0)	return 8;
	else if (std::strcmp(tenor.c_str(),"9M") == 0)	return 9;
	else if (std::strcmp(tenor.c_str(),"10M") == 0)	return 10;
	else if (std::strcmp(tenor.c_str(),"11M") == 0)	return 11;
	else if (std::strcmp(tenor.c_str(),"12M") == 0)	return 12;
	else if (std::strcmp(tenor.c_str(),"13M") == 0)	return 13;
	else if (std::strcmp(tenor.c_str(),"14M") == 0)	return 14;
	else if (std::strcmp(tenor.c_str(),"15M") == 0)	return 15;
	else if (std::strcmp(tenor.c_str(),"16M") == 0)	return 16;
	else if (std::strcmp(tenor.c_str(),"17M") == 0)	return 17;
	else if (std::strcmp(tenor.c_str(),"18M") == 0)	return 18;
	else {
		std::cout << "Unknown FRA tenor " << tenor << ". Exit." << std::endl;
		getchar();
		exit(1);
	}
}

template<typename Traits, typename Interpolator>
Integer
ReadInTermStructure<Traits, Interpolator>::getYearTenorAsInteger(const std::string& tenor)
{
	if (std::strcmp(tenor.c_str(),"1Y") == 0)		return 1;
	else if (std::strcmp(tenor.c_str(),"2Y") == 0)	return 2;
	else if (std::strcmp(tenor.c_str(),"3Y") == 0)	return 3;
	else if (std::strcmp(tenor.c_str(),"4Y") == 0)	return 4;
	else if (std::strcmp(tenor.c_str(),"5Y") == 0)	return 5;
	else if (std::strcmp(tenor.c_str(),"6Y") == 0)	return 6;
	else if (std::strcmp(tenor.c_str(),"7Y") == 0)	return 7;
	else if (std::strcmp(tenor.c_str(),"8Y") == 0)	return 8;
	else if (std::strcmp(tenor.c_str(),"9Y") == 0)	return 9;
	else if (std::strcmp(tenor.c_str(),"10Y") == 0)	return 10;
	else if (std::strcmp(tenor.c_str(),"11Y") == 0)	return 11;
	else if (std::strcmp(tenor.c_str(),"12Y") == 0)	return 12;
	else if (std::strcmp(tenor.c_str(),"13Y") == 0)	return 13;
	else if (std::strcmp(tenor.c_str(),"14Y") == 0)	return 14;
	else if (std::strcmp(tenor.c_str(),"15Y") == 0)	return 15;
	else if (std::strcmp(tenor.c_str(),"20Y") == 0)	return 20;
	else if (std::strcmp(tenor.c_str(),"25Y") == 0)	return 25;
	else if (std::strcmp(tenor.c_str(),"30Y") == 0)	return 30;
	else if (std::strcmp(tenor.c_str(),"40Y") == 0)	return 40;
	else if (std::strcmp(tenor.c_str(),"50Y") == 0)	return 50;
	else {
		std::cout << "Unknown swap tenor " << tenor << ". Exit." << std::endl;
		getchar();
		exit(1);
	}
}

template<class Interpolator>
ReadInInterpolatedZeroCurve<Interpolator>::ReadInInterpolatedZeroCurve(
								std::string& _filename, const Calendar& _calendar, 
								const DayCounter& _dayCounter)
	: filename(_filename), calendar(_calendar), dayCounter(_dayCounter)
{

	std::ifstream in(filename.c_str());
	if (!in.is_open()) {
		std::cout << "Stream is not open" << std::endl;
		getchar();
		exit(1);
	}
	
	typedef boost::tokenizer< boost::escaped_list_separator<char> > Tokenizer;
	boost::escaped_list_separator<char> line_separators('\\',',','\"');
	boost::escaped_list_separator<char> date_separators('\\','/','\"');

	std::vector< std::string > vec;
	std::vector< std::string > vec2;
    std::string line;

	while (std::getline(in,line)) {
        Tokenizer tok(line,line_separators);
        vec.assign(tok.begin(),tok.end());
		if (std::strcmp(vec[0].c_str(),"Date") == 0) {
			continue;
		}
		
		Rate readInRate = std::atof(vec[1].c_str())/100;	
		rateVector.push_back(readInRate);		

		Tokenizer tok2(vec[0],date_separators);
		vec2.assign(tok2.begin(),tok.end());
		Month month = getMonthFromInteger(atoi(vec2[1].c_str()));
		Date readInDate(atoi(vec2[0].c_str()), month, atoi(vec2[2].c_str()));
		dateVector.push_back(readInDate);
		//std::cout << vec2[0].c_str() << " " << month << " " << vec2[2].c_str() << " " << vec[0].c_str() << std::endl;
	}
	boost::shared_ptr<YieldTermStructure> tmp(new InterpolatedZeroCurve<Interpolator>(dateVector, rateVector, dayCounter, calendar));
	zeroCurve = tmp;
}

template<class Interpolator>
Month
ReadInInterpolatedZeroCurve<Interpolator>::getMonthFromInteger(int month)
{
	if (month == 1) return Jan;
	if (month == 2) return Feb;
	if (month == 3) return Mar;
	if (month == 4) return Apr;
	if (month == 5) return May;
	if (month == 6) return Jun;
	if (month == 7) return Jul;
	if (month == 8) return Aug;
	if (month == 9) return Sep;
	if (month == 10) return Oct;
	if (month == 11) return Nov;
	if (month == 12) return Dec;

}