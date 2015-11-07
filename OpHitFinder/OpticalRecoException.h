/**
 * \file OpticalRecoException.h
 *
 * \ingroup OpticalDetector
 * 
 * \brief Class def header for exception classes in OpticalDetector package
 *
 * @author kazuhiro
 */

/** \addtogroup OpticalDetector

    @{*/
#ifndef larana_OPTICALDETECTOR_OPTICALRECOEXCEPTION_H
#define larana_OPTICALDETECTOR_OPTICALRECOEXCEPTION_H

#include <iostream>
#include <exception>

namespace pmtana {
  /**
     \class OpticalRecoRException
     Generic (base) exception class
  */
  class OpticalRecoException : public std::exception{

  public:

    OpticalRecoException(const std::string& msg="");

    virtual ~OpticalRecoException() throw(){};

    virtual const char* what() const throw();

  private:

    std::string _msg;
  };

}
#endif
/** @} */ // end of doxygen group 

