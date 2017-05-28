#include "nwol_log.h"

#if defined (__WINDOWS__)
#	include <windows.h>
#endif

::std::string								nwol::getWindowsErrorAsString		(uint64_t lastError)					{	// Get the error message, if any.
	if(0 == lastError) 
		return {};
	LPSTR										messageBuffer						= nullptr;
	const size_t								size								= FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, (DWORD)lastError, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
	retval_error_if({}, nullptr == messageBuffer, "FormatMessage() failed.")
	else {
		const ::std::string							message								(messageBuffer, size >= 2 ? size-2 : size);
		LocalFree(messageBuffer); 
		return message;
	}
}
// --------------------------------------------------- Library Comparison ----------------------------------------------------------------------------------------
// 							Boost 						 VC++ & STL (reference)		No Workflow Overhead Library	
// ---------------------------------------------------------------------------------------------------------------------------------------------------------------
// ---- Features:		<	100 %							100 %							>	300 %					- Standards compliance
//						>	500 % 							100 %							<	 20	%					- Code size
//						<	 10	% 							100 %							>	500	%					- Debugging capabilities
//						<	 10 %							100 %							>	500	%					- Error handling and reporting
//						< 	 10 %							100 %							>	500	%					- Code readability
//						< 	 10 %							100 %							>	500	%					- Ease of use
//						< 	 20 %							100 %							>	500	%					- Design
//						< 	100 %							100 %							>	500	%					- Reliability
//						< 	 10 %							100 %							>	500	%					- Beginner's-proof
//						< 	100 %							100 %							>	200	%					- Interop
//						< 	100 %							100 %							>	100	%					- Portability
// 
// ---- Times:			>  3000 % 							100 %							~	100	%					- Build 
//						>  2000	%							100 %							<	 50	%					- Development 
//						>	500 %							100 %							<	 10	%					- Maintenance 
//						>	500 %							100 %							<	 10	%					- Testing 
//						>  1000 %							100 %							<	 50	%					- Execution 
// 
// ---- Project management:
//							Poor							Low								   Ideal					- Level of communication between project members
//							Very Low						Low									High					- Development speed		
//							Research						Usability					Productivity					- Focus
//							OOP								OOP-procedural		  DOD-OOP-procedural					- Design
//							32								26									   2					- Project members
//							 2								 3.5								  10					- Average effective coding hours per day per member over 7 consecutive days
//
// ---- Estimated sizes:
//							  26131								830								  129					- Total Files
//							3908016							 556068								16468					- Total Lines 
//
//				( 8.89%)	 347697				(16.24%) 	  90359					( 5.51%)	  908					- Comment Lines			
//				(50.85%)	1987360				(51.95%) 	 288919					(60.68%)	 9993					- Code Lines				
//				( 9.14%)	 357515				( 5.07%) 	  28228					( 9.92%)	 1635					- Compiler directives Lines	
//				(31.10%)	1215444				(26.71%) 	 148562					(23.87%)	 3932					- Blank Lines				
//						  142412687 (136MiB)			   17759175	(17MiB)					   744481 (727KiB)			- Total bytes
//
//							 149.55							 669.96							   127.65					- Average lines per file							 
//							  13.30							 108.86							     7.03					- Average lines of comments per file				 
//							  76.05							 348.09							    77.46					- Average lines of code per file					 
//							  13.68							  34.00							    12.67					- Average lines of compiler directives per file		 
//							  46.51							 178.99							    30.48					- Average blank lines per file						 
//
// ---- Dates:				1999					1993 (VC++) & 1994 (STL)					2016					- First Release
//						2017-04-19							2017-04-05						2017-05-21					- Latest Update
// 
// ---- Accounting:		>  1000 %								100 %						<	 10 %					- Development costs
//							Low								  Medium 							 Top					- Quality
//							None				 				 N/A					  Invaluable					- Value
//							Free				 				Free					20000000 USD					- Price
//																	
// ----------------------------- About them:
//
// ---------------- VC++ & STL
//
// --- Description: 
//	The standard C++ libraries that come with the compiler.
//
// ---------------- Boost
//
// --- Description: 
//	Boost is a set of libraries for the C++ programming language that provide support for tasks and structures such as linear algebra, pseudorandom number generation, multithreading, image processing, regular expressions, and unit testing. It started as a research project and never got any further. 
//
// --- Libraries:
//	It contains over eighty individual libraries, with a large amount of them that doesn't do other than what the language primitives already do, with the only purpose that showing that such way of doing it is as possible as the simpler and straightforward way. 
// 
// ---------------- NWOL	
//
// --- Description: 
//	NWOL is a set of libraries for the C++ programming language that provide support for tasks and structures such as 3d math, pseudorandom number generation, image processing, unit testing, debugging, logging, type introspection, object reflection, networking, GUI, static RTTI. It contains only 2 individual libraries.
//
// --- Libraries:
//	NWOL is composed by two small libraries with minimal dependencies between them:
//	1. nwol: Contains lowest-level functions and types that improve safety and performance of common tasks, from assigning a value to every element in an array to a detailed error handling methodology and error reporting infrastructure.
//	2. gods: Contains not-so-lowest level functions and types that provide a second layer of static RTTI and garbage collection, along with a base for multilayered UDP communications and a data/functionality graph system.
